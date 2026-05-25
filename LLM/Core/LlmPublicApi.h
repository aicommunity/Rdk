#ifndef RDK_LLM_PUBLIC_API_H
#define RDK_LLM_PUBLIC_API_H

#include <memory>

#include "LlmTypes.h"
#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/RegisterCoreRdkTools.h"

namespace RDK {
class UApplication;
}

namespace RDK::LLM {

class LLMServices {
public:
    static LLMServices& instance();

    void initialize(RDK::UApplication* app, ILLMProjectContextProvider* project_context);
    ULLMAgentOrchestrator& orchestrator();
    URdkDomainAccess& domain();

private:
    LLMServices() = default;
    std::unique_ptr<URdkDomainAccess> m_domain;
    std::unique_ptr<ILLMProvider> m_provider;
    std::unique_ptr<ULLMConversationStore> m_store;
    std::unique_ptr<ULLMToolArgumentValidator> m_validator;
    std::unique_ptr<ULLMIdempotencyStore> m_idempotency;
    std::unique_ptr<ULLMPolicyEngine> m_policy;
    std::unique_ptr<ULLMToolGateway> m_gateway;
    std::unique_ptr<ULLMAgentOrchestrator> m_orchestrator;
};

} // namespace RDK::LLM

#endif
