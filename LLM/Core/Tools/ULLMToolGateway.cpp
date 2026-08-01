#include "ULLMToolGateway.h"

#include "ApplicationToolAudit.h"
#include "../Domain/ULLMEntityPathCanonicalizer.h"
#include "../Domain/ULLMWriteArgumentNormalizer.h"
#include "../Gui/ULLMPresentationScopeGuard.h"
#include "../LlmPublicApi.h"
#include "../Observability/ULLMToolTrace.h"
#include "../Session/ULLMGuiTurnPin.h"
#include "../Policy/ULLMUserRole.h"
#include "../Policy/ULLMWriteToolPolicy.h"

#include <chrono>
#include <optional>
#include <random>

namespace RDK::LLM {

namespace {

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

thread_local const ToolInvokeRequest* g_active_tool_invoke = nullptr;

struct ActiveToolInvokeGuard {
    explicit ActiveToolInvokeGuard(const ToolInvokeRequest* req)
    {
        g_active_tool_invoke = req;
    }
    ~ActiveToolInvokeGuard() { g_active_tool_invoke = nullptr; }
    ActiveToolInvokeGuard(const ActiveToolInvokeGuard&) = delete;
    ActiveToolInvokeGuard& operator=(const ActiveToolInvokeGuard&) = delete;
};

void maybeRecordTurnToolTrace(const ToolInvokeRequest& req, const LLMToolDefinition* def,
                              const nlohmann::json& display_arguments,
                              const ToolGatewayResult& result, int duration_ms = 0)
{
    if(req.skip_turn_tool_trace)
        return;
    if(!LLMServices::instance().isInitialized())
        return;
    const std::string session_id = req.session.session_id;
    if(session_id.empty())
        return;
    ConversationState* state = LLMServices::instance().mutableConversationState(session_id);
    if(!state)
        return;
    recordTurnToolInvocation(*state, req.tool_name, display_arguments, result, duration_ms,
                             def ? def->input_schema : nlohmann::json::object());
}

} // namespace

const ToolInvokeRequest* activeToolInvokeRequest()
{
    return g_active_tool_invoke;
}
ULLMToolGateway::ULLMToolGateway(ULLMToolRegistry& registry, ULLMPolicyEngine& policy,
                                 URdkDomainAccess& domain, ULLMAuditLog& audit,
                                 ULLMIdempotencyStore& idempotency,
                                 ULLMToolArgumentValidator& validator)
    : m_registry(registry)
    , m_policy(policy)
    , m_domain(domain)
    , m_audit(audit)
    , m_idempotency(idempotency)
    , m_validator(validator)
{
}

ToolGatewayResult ULLMToolGateway::invoke(const ToolInvokeRequest& req)
{
    ToolGatewayResult result;
    const LLMToolDefinition* def = m_registry.find(req.tool_name);
    if(!def)
    {
        result.ok = false;
        result.error_code = "ToolNotFound";
        result.message = req.tool_name;
        maybeRecordTurnToolTrace(req, nullptr, req.arguments, result);
        return result;
    }

    const ConversationState* conversation = nullptr;
    if(LLMServices::instance().isInitialized())
        conversation = LLMServices::instance().conversationState(req.session.session_id);

    ToolInvokeRequest working_req = req;
    bool write_pre_normalized = false;
    if(writeToolNeedsEntityResolution(req.tool_name) && !req.confirmed
       && !req.user_text_hint.empty())
    {
        const WriteArgumentNormalizeResult pre = normalizeWriteToolArguments(
            req.tool_name, req.arguments, m_domain, req.session.active_channel_index,
            req.user_text_hint, conversation);
        if(!pre.ok)
        {
            result.ok = false;
            result.error_code = pre.error_code;
            result.message = pre.message;
            if(pre.needs_clarification)
                result.result = pre.clarification;
            m_audit.append("tool_invoke_finish",
                           {{"tool_name", req.tool_name},
                            {"ok", false},
                            {"error", pre.error_code},
                            {"pre_normalize", true}},
                           req.trace_id, req.session.session_id);
            maybeRecordTurnToolTrace(req, def, working_req.arguments, result);
            return result;
        }
        working_req.arguments = pre.normalized_arguments;
        write_pre_normalized = true;
    }

    working_req.arguments =
        m_validator.normalizeForSchema(working_req.arguments, def->input_schema);
    if(def->kind == LLMToolKind::Read)
    {
        if(working_req.tool_name == "get_net_snapshot"
           && !working_req.arguments.contains("channel_index"))
            working_req.arguments["channel_index"] = working_req.session.active_channel_index;
        if(working_req.tool_name == "get_net_snapshot"
           && !working_req.arguments.contains("max_components"))
            working_req.arguments["max_components"] = 200;
    }

    std::string validation_error;
    if(!m_validator.validate(working_req.arguments, def->input_schema, validation_error))
    {
        result.ok = false;
        result.error_code = "SchemaValidationFailed";
        result.message = validation_error;
        m_audit.append("tool_invoke_finish",
                       {{"tool_name", req.tool_name}, {"ok", false}, {"error", validation_error}},
                       req.trace_id, req.session.session_id);
        maybeRecordTurnToolTrace(req, def, working_req.arguments, result);
        return result;
    }

    PolicyDecision pol = m_policy.checkToolInvoke(working_req, *def, m_domain);
    if(!pol.allowed)
    {
        result.ok = false;
        result.error_code = pol.deny_code;
        result.message = pol.deny_message;
        m_audit.append("policy_deny",
                       {{"tool_name", req.tool_name}, {"code", pol.deny_code}},
                       req.trace_id, req.session.session_id);
        maybeRecordTurnToolTrace(req, def, working_req.arguments, result);
        return result;
    }

    const bool skip_confirmation =
        req.confirmed || req.session.auto_apply_writes;
    if(def->requires_confirmation && !skip_confirmation
       && !isConfirmationExemptWriteTool(req.tool_name, req.arguments))
    {
        result.ok = true;
        result.pending_confirmation = true;
        result.confirmation_id = makeConfirmationId();
        m_audit.append("confirmation_requested",
                       {{"tool_name", req.tool_name}, {"confirmation_id", result.confirmation_id}},
                       req.trace_id, req.session.session_id);
        maybeRecordTurnToolTrace(req, def, working_req.arguments, result);
        return result;
    }

    if(def->idempotent && !req.idempotency_key.empty())
    {
        if(auto cached = m_idempotency.find(req.idempotency_key))
        {
            m_audit.append("tool_idempotency_cache_hit",
                           {{"tool_name", req.tool_name},
                            {"idempotency_key", req.idempotency_key}},
                           req.trace_id, req.session.session_id);
            maybeRecordTurnToolTrace(req, def, working_req.arguments, *cached);
            return *cached;
        }
        m_audit.append("tool_idempotency_cache_miss",
                       {{"tool_name", req.tool_name}, {"idempotency_key", req.idempotency_key}},
                       req.trace_id, req.session.session_id);
    }

    ToolInvokeRequest invoke_req = working_req;
    if(writeToolNeedsEntityResolution(req.tool_name) && !req.confirmed && !write_pre_normalized)
    {
        const WriteArgumentNormalizeResult normalized = normalizeWriteToolArguments(
            working_req.tool_name, working_req.arguments, m_domain,
            req.session.active_channel_index, req.user_text_hint, conversation);
        if(!normalized.ok)
        {
            result.ok = false;
            result.error_code = normalized.error_code;
            result.message = normalized.message;
            if(normalized.needs_clarification)
                result.result = normalized.clarification;
            m_audit.append("tool_invoke_finish",
                           {{"tool_name", req.tool_name},
                            {"ok", false},
                            {"error", normalized.error_code},
                            {"entity_resolution", true}},
                           req.trace_id, req.session.session_id);
            maybeRecordTurnToolTrace(req, def, working_req.arguments, result);
            return result;
        }
        invoke_req.arguments = normalized.normalized_arguments;
    }

    if(invoke_req.tool_name == "connect_components")
    {
        const WriteArgumentNormalizeResult port_norm = normalizeWriteToolArguments(
            invoke_req.tool_name, invoke_req.arguments, m_domain,
            invoke_req.session.active_channel_index, invoke_req.user_text_hint, conversation);
        if(!port_norm.ok)
        {
            result.ok = false;
            result.error_code = port_norm.error_code;
            result.message = port_norm.message;
            if(port_norm.needs_clarification)
                result.result = port_norm.clarification;
            m_audit.append("tool_invoke_finish",
                           {{"tool_name", req.tool_name},
                            {"ok", false},
                            {"error", port_norm.error_code},
                            {"connect_port_normalize", true}},
                           req.trace_id, req.session.session_id);
            maybeRecordTurnToolTrace(req, def, invoke_req.arguments, result);
            return result;
        }
        invoke_req.arguments = port_norm.normalized_arguments;
    }

    m_audit.append("tool_invoke_start",
                   {{"tool_name", req.tool_name},
                    {"user_role", userRoleName(resolveUserRole(req.session.user_id))}},
                   req.trace_id, req.session.session_id);

    ConversationState* mutable_state = nullptr;
    const LLMGuiContextSnapshot* pin_gui = nullptr;
    if(LLMServices::instance().isInitialized() && !req.session.session_id.empty())
    {
        mutable_state = LLMServices::instance().mutableConversationState(req.session.session_id);
        if(mutable_state)
            pin_gui = guiContextForWrite(*mutable_state);
    }

    if(pin_gui && def->kind == LLMToolKind::Write)
    {
        canonicalizeEntityPaths(invoke_req.tool_name, invoke_req.arguments, m_domain,
                                invoke_req.session.active_channel_index, *pin_gui,
                                def->input_schema);
    }

    bool pin_diagram = true;
    if(LLMServices::instance().isInitialized())
        pin_diagram = LLMServices::instance().settings().runtime().pin_diagram_for_writes;

    ILLMPresentationSink* sink =
        LLMServices::instance().isInitialized() ? LLMServices::instance().presentationSink()
                                                  : nullptr;
    std::optional<ULLMPresentationScopeGuard> presentation_guard;
    if(pin_gui && def->kind == LLMToolKind::Write && sink)
        presentation_guard.emplace(sink, *pin_gui, invoke_req.session.active_channel_index,
                                   pin_diagram);

    const auto started = std::chrono::steady_clock::now();
    try
    {
        ActiveToolInvokeGuard guard(&invoke_req);
        result = m_registry.invokeHandler(invoke_req.tool_name, invoke_req.arguments);
    }
    catch(const std::exception& ex)
    {
        result.ok = false;
        result.error_code = "ToolInvokeException";
        result.message = ex.what() ? ex.what() : "tool handler threw";
    }
    const int duration_ms = static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
                                                              - started)
            .count());

    nlohmann::json finish = {{"tool_name", req.tool_name},
                             {"ok", result.ok},
                             {"error", result.message},
                             {"duration_ms", duration_ms}};
    if(req.tool_name == "connect_components" && result.result.value("already_existed", false))
        finish["connect_components_skipped_existing"] = true;
    if(result.result.contains(kAuditConfigurationPathKey))
        finish["configuration_path"] = result.result[kAuditConfigurationPathKey];
    if(result.result.contains(kAuditPresentationEffectKey))
        finish["presentation_effect"] = result.result[kAuditPresentationEffectKey];
    finish["arguments_preview"] = sanitizeToolArgumentsForDisplay(invoke_req.arguments, def->input_schema);

    m_audit.append("tool_invoke_finish", finish, req.trace_id, req.session.session_id);

    maybeRecordTurnToolTrace(req, def, invoke_req.arguments, result, duration_ms);

    if(def->idempotent && !req.idempotency_key.empty() && result.ok)
    {
        m_idempotency.put(req.idempotency_key, result);
        m_audit.append("tool_idempotency_cached",
                       {{"tool_name", req.tool_name}, {"idempotency_key", req.idempotency_key}},
                       req.trace_id, req.session.session_id);
    }

    return result;
}

} // namespace RDK::LLM
