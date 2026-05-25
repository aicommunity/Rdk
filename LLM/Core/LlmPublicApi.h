#ifndef RDK_LLM_PUBLIC_API_H
#define RDK_LLM_PUBLIC_API_H

#include <memory>
#include <string>

#include "LlmTypes.h"
#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Settings/ILLMProviderSettingsSource.h"
#include "Settings/ULLMProviderAuth.h"
#include "Settings/ULLMSettingsStore.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/RegisterCoreRdkTools.h"

namespace RDK {
class UApplication;
}

namespace RDK::LLM {

class LLMServices {
public:
    static LLMServices& instance();

    void initialize(RDK::UApplication* app, ILLMProjectContextProvider* project_context,
                    ILLMProviderSettingsSource* settings_source = nullptr);
    void applyActiveProvider();
    ProviderAccessCheck checkActiveProviderAccess(const LLMSessionContext& session) const;

    ULLMAgentOrchestrator& orchestrator();
    URdkDomainAccess& domain();
    ULLMSettingsStore& settings();
    const LLMProviderProfile& activeProviderProfile() const;

private:
    void rebuildProvider();

    std::unique_ptr<URdkDomainAccess> m_domain;
    std::unique_ptr<ILLMProvider> m_provider;
    std::unique_ptr<ULLMConversationStore> m_store;
    std::unique_ptr<ULLMToolArgumentValidator> m_validator;
    std::unique_ptr<ULLMIdempotencyStore> m_idempotency;
    std::unique_ptr<ULLMPolicyEngine> m_policy;
    std::unique_ptr<ULLMToolGateway> m_gateway;
    std::unique_ptr<ULLMAgentOrchestrator> m_orchestrator;
    std::unique_ptr<ULLMSettingsStore> m_settings;
    LLMProviderProfile m_active_profile;
    ILLMProjectContextProvider* m_project_context = nullptr;
};

} // namespace RDK::LLM

#endif
