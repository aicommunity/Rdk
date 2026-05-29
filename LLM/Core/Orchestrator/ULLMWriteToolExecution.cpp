#include "ULLMWriteToolExecution.h"

#include "ULLMAgentOrchestrator.h"
#include "ULLMConfigurationLifecycle.h"
#include "ULLMLifecycleArgumentGate.h"
#include "../../../Core/Application/UApplication.h"
#include "ULLMStepVerifier.h"
#include "ULLMWriteToolRepair.h"
#include "ULLMWriteToolUserMessage.h"
#include "../Domain/ULLMWriteArgumentNormalizer.h"
#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "../Policy/ULLMPolicyLimits.h"
#include "../Policy/ULLMWriteToolPolicy.h"
#include "../Settings/ULLMUserMessages.h"
#include "../Session/ULLMConfirmationExpiry.h"
#include "../Session/ULLMSessionGraphMemory.h"

#include <QCryptographicHash>
#include <random>

namespace RDK::LLM {

namespace {

std::string pseudoSha256(const std::string& text)
{
    const QByteArray data(text.data(), static_cast<int>(text.size()));
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex().toStdString();
}

std::string stableArgumentsJson(const nlohmann::json& args)
{
    if(args.is_object())
        return args.dump();
    return args.dump();
}

std::string makeIdempotencyKey(const std::string& session_id, const std::string& trace_id,
                               const std::string& tool_name, const nlohmann::json& args,
                               const std::string& action_id)
{
    return pseudoSha256(session_id + "|" + trace_id + "|" + action_id + "|" + tool_name + "|"
                        + stableArgumentsJson(args));
}

std::string makeConfirmationId()
{
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist(0, 15);
    const char* hex = "0123456789abcdef";
    std::string id;
    for(int i = 0; i < 32; ++i)
        id += hex[dist(rng)];
    return id;
}

bool writeToolNeedsPreGatewayHitl(const ULLMToolRegistry& registry, const WriteToolExecutionRequest& req)
{
    const LLMToolDefinition* def = registry.find(req.tool_name);
    if(!def || !def->requires_confirmation)
        return false;
    if(req.session.auto_apply_writes)
        return false;
    if(req.skip_preview)
        return false;
    if(req.confirmed)
        return false;
    return !isConfirmationExemptWriteTool(req.tool_name, req.arguments);
}

std::string formatOutcomeForTool(const std::string& tool_name, const ToolGatewayResult& tr)
{
    if(tool_name == "add_component" || isNetGraphWriteTool(tool_name))
        return formatWriteToolUserMessage(tool_name, tr);
    if(isLifecycleWriteToolName(tool_name))
        return formatLifecycleToolUserMessage(tool_name, tr);
    if(tr.ok)
        return tr.message.empty() ? "Change applied." : tr.message;
    return tr.message.empty() ? "Operation failed: " + tool_name : tr.message;
}

bool writeToolSupportsPostVerify(const std::string& tool_name)
{
    return tool_name == "add_component" || tool_name == "connect_components";
}

void runPostVerifyRepairLoop(ULLMToolGateway& gateway, const WriteToolExecutionRequest& req,
                             ToolInvokeRequest& invoke, WriteToolExecutionResult& result)
{
    if(!result.gateway.ok || result.gateway.pending_confirmation
       || !writeToolSupportsPostVerify(req.tool_name))
        return;

    if(!LLMServices::instance().isInitialized())
    {
        result.verify_ok = true;
        result.verify_detail = "post_verify_domain_unavailable";
        return;
    }

    URdkDomainAccess& domain = LLMServices::instance().domain();
    const int channel_index = req.session.active_channel_index;
    const int max_repairs = defaultPolicyLimits().max_verify_repair_attempts;

    for(int repair = 0; repair <= max_repairs; ++repair)
    {
        const VerifyResult vr =
            runPostToolVerification(req.tool_name, invoke.arguments, result.gateway, domain,
                                    channel_index);
        GetAuditLog().append("post_tool_verified",
                             {{"tool", req.tool_name},
                              {"satisfied", vr.satisfied},
                              {"detail", vr.detail},
                              {"repair_attempt", repair}},
                             req.trace_id, req.session_id);

        if(vr.satisfied)
        {
            result.verify_ok = true;
            result.verify_detail = vr.detail;
            return;
        }

        if(repair >= max_repairs)
        {
            result.verify_ok = false;
            result.verify_detail = vr.detail;
            result.gateway.ok = false;
            result.gateway.error_code = "PostVerifyFailed";
            result.gateway.message = vr.detail;
            return;
        }

        const RepairAttemptResult repair_result =
            attemptRepair(req.tool_name, invoke.arguments, vr, repair);
        GetAuditLog().append("write_tool_repair",
                             {{"tool", req.tool_name},
                              {"retry", repair_result.retry},
                              {"attempt", repair},
                              {"detail", vr.detail}},
                             req.trace_id, req.session_id);

        if(!repair_result.retry)
        {
            result.verify_ok = false;
            result.verify_detail =
                repair_result.user_message.empty() ? vr.detail : repair_result.user_message;
            result.gateway.ok = false;
            result.gateway.error_code = "PostVerifyFailed";
            result.gateway.message = result.verify_detail;
            return;
        }

        invoke.arguments = repair_result.new_args;
        const std::string action_id =
            (req.idempotency_action_id.empty() ? "write" : req.idempotency_action_id) + ":repair:"
            + std::to_string(repair + 1);
        invoke.idempotency_key =
            makeIdempotencyKey(req.session_id, req.trace_id, req.tool_name, invoke.arguments,
                               action_id);
        result.gateway = gateway.invoke(invoke);
        if(!result.gateway.ok)
        {
            result.verify_ok = false;
            result.verify_detail = result.gateway.message.empty() ? "repair_invoke_failed"
                                                                  : result.gateway.message;
            return;
        }
    }
}

} // namespace

std::string formatHitlConfirmationText(const std::string& preview_text, const std::string& lang)
{
    const std::string hint = formatUserMessage("confirmation.apply_hint", lang);
    if(preview_text.empty())
        return hint;
    return preview_text + "\n\n" + hint;
}

std::string combinePreviewAndOutcome(const std::string& preview_text, const std::string& outcome_text)
{
    if(preview_text.empty())
        return outcome_text;
    if(outcome_text.empty())
        return preview_text;
    return preview_text + "\n\n" + outcome_text;
}

WriteToolExecutionResult executeWriteWithPreviewAndVerify(ULLMAgentOrchestrator& orch,
                                                          ConversationState& state,
                                                          const WriteToolExecutionRequest& req)
{
    WriteToolExecutionResult result;
    result.preview_text = formatActionIntentPreview(req.tool_name, req.arguments, req.gui, req.session,
                                                    req.user_lang, req.link_ctx);

    if(writeToolNeedsPreGatewayHitl(orch.m_registry, req))
    {
        bool defer_hitl_for_missing_lifecycle_args = false;
        if(isLifecycleWriteToolName(req.tool_name))
        {
            RDK::UApplication* app = nullptr;
            if(LLMServices::instance().isInitialized())
                app = LLMServices::instance().domain().application();
            const std::vector<ToolArgumentFieldSpec> lifecycle_missing =
                findMissingLifecycleFields(req.tool_name, req.arguments, app);
            const std::vector<ToolArgumentFieldSpec> schema_missing =
                findMissingArgumentsForTool(req.tool_name, req.arguments, app, orch.m_registry);
            defer_hitl_for_missing_lifecycle_args =
                !lifecycle_missing.empty() || !schema_missing.empty();
        }
        if(!defer_hitl_for_missing_lifecycle_args)
        {
            result.needs_hitl = true;
            result.gateway.ok = true;
            result.gateway.pending_confirmation = true;
            result.gateway.confirmation_id = makeConfirmationId();
            result.outcome_text = formatHitlConfirmationText(result.preview_text, req.user_lang);
            return result;
        }
    }

    ToolInvokeRequest invoke;
    invoke.trace_id = req.trace_id;
    invoke.tool_name = req.tool_name;
    invoke.arguments = req.arguments;
    invoke.idempotency_key =
        makeIdempotencyKey(req.session_id, req.trace_id, req.tool_name, req.arguments,
                           req.idempotency_action_id.empty() ? "write" : req.idempotency_action_id);
    invoke.session = req.session;
    invoke.session.session_id = req.session_id;
    invoke.user_text_hint = req.user_text_hint;
    invoke.confirmed = req.confirmed || req.skip_preview;

    result.gateway = orch.m_gateway.invoke(invoke);
    runPostVerifyRepairLoop(orch.m_gateway, req, invoke, result);
    result.outcome_text = formatOutcomeForTool(req.tool_name, result.gateway);

    if(result.gateway.pending_confirmation && !result.needs_hitl)
    {
        result.needs_hitl = true;
        result.outcome_text = formatHitlConfirmationText(result.preview_text, req.user_lang);
    }

    if(result.gateway.ok && !result.gateway.pending_confirmation && !result.needs_hitl
       && LLMServices::instance().isInitialized())
    {
        recordWriteToolOutcome(state, LLMServices::instance().domain(), req.tool_name,
                               result.gateway.result, req.session.active_channel_index);
    }

    return result;
}

} // namespace RDK::LLM
