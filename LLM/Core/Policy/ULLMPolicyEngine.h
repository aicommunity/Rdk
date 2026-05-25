#ifndef RDK_ULLM_POLICY_ENGINE_H
#define RDK_ULLM_POLICY_ENGINE_H

#include "../Domain/URdkDomainAccess.h"
#include "../LlmTypes.h"

namespace RDK::LLM {

class ULLMPolicyEngine {
public:
    PolicyDecision checkToolInvoke(const ToolInvokeRequest& req,
                                   const LLMToolDefinition& tool,
                                   const URdkDomainAccess& domain) const;
};

} // namespace RDK::LLM

#endif
