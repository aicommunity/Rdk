#include "ULLMRecordedToolInvoke.h"

#include "../Domain/ULLMWriteArgumentNormalizer.h"
#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "../Observability/ULLMSystemLogExcerpt.h"
#include "../Observability/ULLMSystemLogReader.h"
#include "../Session/ULLMSessionGraphMemory.h"
#include "../TrustBoundary/ULLMTrustBoundary.h"
#include "ULLMConfigurationLifecycle.h"
#include "ULLMTurnTerminalHelpers.h"
#include "ULLMWriteToolUserMessage.h"

#include <QByteArray>
#include <QCryptographicHash>

namespace RDK::LLM {

namespace {

std::string shaHex(const std::string& text)
{
    const QByteArray data(text.data(), static_cast<int>(text.size()));
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex().toStdString();
}

std::string makeRecordedCallId(const RecordedToolInvokeRequest& req)
{
    if(!req.tool_call_id.empty())
        return req.tool_call_id;
    const std::string args_dump =
        req.arguments.is_null() ? std::string("{}") : req.arguments.dump();
    return "rec-" + shaHex(req.trace_id + "|" + req.tool_name + "|" + args_dump).substr(0, 20);
}

std::string toolMessageContent(const ToolGatewayResult& tr, ULLMSystemLogReader* reader,
                               int channel_index)
{
    std::string excerpt;
    if(reader)
    {
        const SystemLogPolicySnapshot pol = reader->policy(channel_index);
        if(!pol.summary_for_model.empty())
            excerpt = pol.summary_for_model;
    }
    return sanitizeUntrustedToolContent(toolJsonWithSystemLogExcerpt(tr, excerpt).dump());
}

std::string outcomeForTool(const std::string& tool_name, const ToolGatewayResult& tr)
{
    if(tool_name == "add_component" || isNetGraphWriteTool(tool_name))
        return formatWriteToolUserMessage(tool_name, tr);
    if(isLifecycleWriteToolName(tool_name))
        return formatLifecycleToolUserMessage(tool_name, tr);
    if(tr.ok)
        return tr.message.empty() ? "Change applied." : tr.message;
    return tr.message.empty() ? "Operation failed: " + tool_name : tr.message;
}

std::string makeIdempotencyKeyLocal(const std::string& session_id, const std::string& trace_id,
                                    const std::string& tool_name, const nlohmann::json& arguments,
                                    const std::string& action_id)
{
    return shaHex(session_id + "|" + trace_id + "|" + action_id + "|" + tool_name + "|"
                  + (arguments.is_null() ? "{}" : arguments.dump()));
}

} // namespace

RecordedToolInvokeResult recordedToolInvoke(ConversationState& state,
                                            const RecordedToolInvokeDeps& deps,
                                            const RecordedToolInvokeRequest& req)
{
    RecordedToolInvokeResult out;
    nlohmann::json args =
        req.arguments.is_object() ? req.arguments : nlohmann::json::object();
    out.tool_call_id = makeRecordedCallId(req);

    const LLMToolDefinition* def = deps.registry.find(req.tool_name);
    const bool is_write = def && def->kind == LLMToolKind::Write;

    if(is_write && deps.write_exec)
    {
        WriteToolExecutionRequest wreq;
        wreq.session_id = req.session_id;
        wreq.trace_id = req.trace_id;
        wreq.tool_name = req.tool_name;
        wreq.arguments = args;
        wreq.session = req.session;
        wreq.session.session_id = req.session_id;
        wreq.gui = req.gui;
        wreq.user_lang = req.user_lang;
        wreq.user_text_hint = req.user_text_hint;
        wreq.idempotency_action_id =
            req.idempotency_action_id.empty() ? "recorded" : req.idempotency_action_id;
        wreq.skip_preview = req.skip_preview;
        wreq.confirmed = req.confirmed;
        WriteToolExecutionResult wres = deps.write_exec(state, wreq);
        out.gateway = std::move(wres.gateway);
        out.preview_text = std::move(wres.preview_text);
        if(out.gateway.pending_confirmation || wres.needs_hitl)
        {
            out.needs_hitl = true;
            out.outcome_text = combinePreviewAndOutcome(out.preview_text, wres.outcome_text);
            appendAssistantToolCallsOnly(deps.store, req.session_id, out.tool_call_id, req.tool_name,
                                         args);
            out.wrote_assistant_tool_calls = true;
            if(out.outcome_text.empty())
                out.outcome_text = wres.outcome_text;
            return out;
        }
        // Successful auto-apply: show outcome only (Tools block already has args).
        out.outcome_text = wres.outcome_text;
    }
    else
    {
        ToolInvokeRequest invoke;
        invoke.trace_id = req.trace_id;
        invoke.tool_name = req.tool_name;
        invoke.arguments = args;
        invoke.session = req.session;
        invoke.session.session_id = req.session_id;
        invoke.user_text_hint = req.user_text_hint;
        invoke.confirmed = req.confirmed || req.force_confirmed
                           || (def && def->requires_confirmation && (req.confirmed || req.force_confirmed));
        if(def && def->requires_confirmation && (req.confirmed || req.force_confirmed || req.skip_preview))
            invoke.confirmed = true;
        invoke.skip_turn_tool_trace = req.skip_turn_tool_trace;
        invoke.idempotency_key =
            makeIdempotencyKeyLocal(req.session_id, req.trace_id, req.tool_name, args,
                                    req.idempotency_action_id.empty() ? out.tool_call_id
                                                                     : req.idempotency_action_id);
        out.gateway = deps.gateway.invoke(invoke);
        out.outcome_text = outcomeForTool(req.tool_name, out.gateway);
        if(out.gateway.pending_confirmation)
        {
            out.needs_hitl = true;
            appendAssistantToolCallsOnly(deps.store, req.session_id, out.tool_call_id, req.tool_name,
                                         args);
            out.wrote_assistant_tool_calls = true;
            return out;
        }
        if(out.gateway.ok && is_write && LLMServices::instance().isInitialized())
        {
            recordWriteToolOutcome(state, LLMServices::instance().domain(), req.tool_name,
                                   out.gateway.result, req.session.active_channel_index, &args);
        }
    }

    const std::string content =
        toolMessageContent(out.gateway, deps.log_reader, req.session.active_channel_index);
    const std::string outcome =
        req.append_outcome_assistant ? out.outcome_text : std::string{};
    appendDirectToolTranscript(deps.store, req.session_id, out.tool_call_id, req.tool_name, args,
                               content, outcome);
    out.wrote_assistant_tool_calls = true;
    out.wrote_tool_result = true;
    return out;
}

} // namespace RDK::LLM
