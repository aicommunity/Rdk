#include "ULLMToolGateway.h"

#include "ApplicationToolAudit.h"
#include "../Domain/ULLMWriteArgumentNormalizer.h"
#include "../Policy/ULLMUserRole.h"
#include "../Policy/ULLMWriteToolPolicy.h"

#include <random>

namespace RDK::LLM {

static std::string makeConfirmationId()
{
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist(0, 15);
    const char* hex = "0123456789abcdef";
    std::string id;
    for(int i = 0; i < 32; ++i)
        id += hex[dist(rng)];
    return id;
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
        return result;
    }

    ToolInvokeRequest working_req = req;
    bool add_component_pre_normalized = false;
    if(req.tool_name == "add_component")
    {
        const WriteArgumentNormalizeResult pre = normalizeWriteToolArguments(
            req.tool_name, req.arguments, m_domain, req.session.active_channel_index,
            req.user_text_hint);
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
            return result;
        }
        working_req.arguments = pre.normalized_arguments;
        add_component_pre_normalized = true;
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
        return result;
    }

    if(def->idempotent && !req.idempotency_key.empty())
    {
        if(auto cached = m_idempotency.find(req.idempotency_key))
            return *cached;
    }

    ToolInvokeRequest invoke_req = working_req;
    if(writeToolNeedsEntityResolution(req.tool_name) && !add_component_pre_normalized)
    {
        const WriteArgumentNormalizeResult normalized = normalizeWriteToolArguments(
            working_req.tool_name, working_req.arguments, m_domain,
            req.session.active_channel_index, req.user_text_hint);
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
            return result;
        }
        invoke_req.arguments = normalized.normalized_arguments;
    }

    m_audit.append("tool_invoke_start",
                   {{"tool_name", req.tool_name},
                    {"user_role", userRoleName(resolveUserRole(req.session.user_id))}},
                   req.trace_id, req.session.session_id);

    result = m_registry.invokeHandler(invoke_req.tool_name, invoke_req.arguments);

    nlohmann::json finish = {{"tool_name", req.tool_name},
                             {"ok", result.ok},
                             {"error", result.message}};
    if(result.result.contains(kAuditConfigurationPathKey))
        finish["configuration_path"] = result.result[kAuditConfigurationPathKey];
    if(result.result.contains(kAuditPresentationEffectKey))
        finish["presentation_effect"] = result.result[kAuditPresentationEffectKey];

    m_audit.append("tool_invoke_finish", finish, req.trace_id, req.session.session_id);

    if(def->idempotent && !req.idempotency_key.empty() && result.ok)
        m_idempotency.put(req.idempotency_key, result);

    return result;
}

} // namespace RDK::LLM
