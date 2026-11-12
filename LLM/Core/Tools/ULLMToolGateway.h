#ifndef RDK_ULLM_TOOL_GATEWAY_H
#define RDK_ULLM_TOOL_GATEWAY_H

#include "../Domain/URdkDomainAccess.h"
#include "../LlmTypes.h"
#include "../Observability/ULLMAuditLog.h"
#include "../Observability/ULLMIdempotencyStore.h"
#include "../Policy/ULLMPolicyEngine.h"
#include "ULLMToolArgumentValidator.h"
#include "ULLMToolRegistry.h"

namespace RDK::LLM {

class ULLMToolGateway {
public:
    ULLMToolGateway(ULLMToolRegistry& registry, ULLMPolicyEngine& policy,
                    URdkDomainAccess& domain, ULLMAuditLog& audit,
                    ULLMIdempotencyStore& idempotency, ULLMToolArgumentValidator& validator);

    ToolGatewayResult invoke(const ToolInvokeRequest& req);

private:
    ULLMToolRegistry& m_registry;
    ULLMPolicyEngine& m_policy;
    URdkDomainAccess& m_domain;
    ULLMAuditLog& m_audit;
    ULLMIdempotencyStore& m_idempotency;
    ULLMToolArgumentValidator& m_validator;
};

/// Active `ToolInvokeRequest` while a registry handler runs (set by gateway).
const ToolInvokeRequest* activeToolInvokeRequest();

} // namespace RDK::LLM

#endif
