#ifndef RDK_LLM_PUBLIC_API_H
#define RDK_LLM_PUBLIC_API_H

#include <memory>
#include <string>

#include "Context/ILLMKnowledgeCatalog.h"
#include "Context/UDocSearchIndex.h"
#include "LlmTypes.h"
#include "Domain/URdkApplicationCommands.h"
#include "Domain/URdkDomainAccess.h"
#include "Gui/ILLMPresentationSink.h"
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
                    ILLMProviderSettingsSource* settings_source = nullptr,
                    std::unique_ptr<ILLMKnowledgeCatalog> catalog = nullptr);
    void applyActiveProvider();
    bool isInitialized() const { return m_settings != nullptr; }
    ProviderAccessCheck checkActiveProviderAccess(const LLMSessionContext& session) const;

    ULLMAgentOrchestrator& orchestrator();
    URdkDomainAccess& domain();
    URdkApplicationCommands& applicationCommands();
    ULLMSettingsStore& settings();
    const LLMProviderProfile& activeProviderProfile() const;
    ILLMKnowledgeCatalog* catalog() const;
    UDocSearchIndex& searchIndex();

    void setPresentationSink(std::unique_ptr<ILLMPresentationSink> sink);
    ILLMPresentationSink* presentationSink() const;

private:
    void rebuildProvider();

    std::unique_ptr<URdkApplicationCommands> m_commands;
    std::unique_ptr<URdkDomainAccess> m_domain;
    std::unique_ptr<ILLMPresentationSink> m_presentation_sink;
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
    std::unique_ptr<ILLMKnowledgeCatalog> m_catalog;
    std::unique_ptr<UDocSearchIndex> m_search_index;
};

} // namespace RDK::LLM

#endif
