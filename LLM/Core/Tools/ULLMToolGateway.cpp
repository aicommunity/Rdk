#include "ULLMToolGateway.h"

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

    std::string validation_error;
    if(!m_validator.validate(req.arguments, def->input_schema, validation_error))
    {
        result.ok = false;
        result.error_code = "SchemaValidationFailed";
        result.message = validation_error;
        m_audit.append("tool_invoke_finish",
                       {{"tool_name", req.tool_name}, {"ok", false}, {"error", validation_error}},
                       req.trace_id, req.session.session_id);
        return result;
    }

    PolicyDecision pol = m_policy.checkToolInvoke(req, *def, m_domain);
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

    if(def->requires_confirmation && !req.confirmed)
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

    m_audit.append("tool_invoke_start", {{"tool_name", req.tool_name}}, req.trace_id,
                   req.session.session_id);

    result = m_registry.invokeHandler(req.tool_name, req.arguments);

    m_audit.append("tool_invoke_finish",
                   {{"tool_name", req.tool_name}, {"ok", result.ok}, {"error", result.message}},
                   req.trace_id, req.session.session_id);

    if(def->idempotent && !req.idempotency_key.empty() && result.ok)
        m_idempotency.put(req.idempotency_key, result);

    return result;
}

} // namespace RDK::LLM
