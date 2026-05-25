#ifndef RDK_ULLM_POLICY_ENGINE_H
#define RDK_ULLM_POLICY_ENGINE_H

#include "../Domain/URdkDomainAccess.h"
#include "../LlmTypes.h"
#include "../Orchestrator/ULLMExecutionPlan.h"

namespace RDK::LLM {

class ULLMToolRegistry;

class ULLMPolicyEngine {
public:
    PolicyDecision checkToolInvoke(const ToolInvokeRequest& req,
                                   const LLMToolDefinition& tool,
                                   const URdkDomainAccess& domain) const;
    PolicyDecision checkPlan(const ULLMExecutionPlan& plan, const LLMSessionContext& session,
                             const ULLMToolRegistry& registry) const;
};

} // namespace RDK::LLM

#endif
