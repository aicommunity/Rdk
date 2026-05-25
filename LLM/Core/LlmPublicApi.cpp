#include "LlmPublicApi.h"

#include <cstdlib>

#include "Context/ILLMProjectContextProvider.h"
#include "LlmModuleInit.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Providers/ULLMProviderFactory.h"
#include "Settings/ULLMProviderAuth.h"

namespace RDK::LLM {

LLMServices& LLMServices::instance()
{
    static LLMServices s;
    return s;
}

void LLMServices::initialize(RDK::UApplication* app, ILLMProjectContextProvider* project_context,
                             ILLMProviderSettingsSource* settings_source)
{
    m_project_context = project_context;
    m_commands = std::make_unique<URdkApplicationCommands>(app);
    m_domain = std::make_unique<URdkDomainAccess>(app);
    m_domain->setApplicationCommands(m_commands.get());
    GetContextRegistry().setPrimary(project_context);

    m_settings = std::make_unique<ULLMSettingsStore>(settings_source);
    m_store = std::make_unique<ULLMConversationStore>();
    m_validator = std::make_unique<ULLMToolArgumentValidator>();
    m_idempotency = std::make_unique<ULLMIdempotencyStore>();
    m_policy = std::make_unique<ULLMPolicyEngine>();

    RegisterCoreRdkTools(GetToolRegistry(), *m_domain, project_context);
    if(project_context)
        project_context->registerExtraTools(GetToolRegistry());

    m_gateway = std::make_unique<ULLMToolGateway>(GetToolRegistry(), *m_policy, *m_domain,
                                                  GetAuditLog(), *m_idempotency, *m_validator);

    rebuildProvider();

    const std::string llm_root =
        (project_context ? project_context->paths().repository_root.string() : ".") + "/LLM";
    GetAuditLog().setLogDirectory(llm_root + "/audit");
    m_store->setStorageDirectory(llm_root + "/sessions");
}

void LLMServices::rebuildProvider()
{
    if(const char* mock = std::getenv("NMSDK_LLM_MOCK"); mock && mock[0] == '1')
    {
        LLMProviderProfile mock_profile;
        mock_profile.kind = LLMProviderKind::Mock;
        mock_profile.profile_id = "mock";
        m_active_profile = mock_profile;
        m_provider = ULLMProviderFactory::create(mock_profile);
    }
    else
    {
        m_active_profile = m_settings->activeProfile();
        m_provider = ULLMProviderFactory::create(m_active_profile);
    }

    m_orchestrator =
        std::make_unique<ULLMAgentOrchestrator>(*m_provider, GetToolRegistry(), *m_gateway, *m_store);
}

void LLMServices::applyActiveProvider()
{
    m_settings->save();
    rebuildProvider();
}

ProviderAccessCheck LLMServices::checkActiveProviderAccess(const LLMSessionContext& session) const
{
    if(m_active_profile.kind == LLMProviderKind::Mock)
        return {};
    return ULLMProviderAuth::checkProviderAccess(m_active_profile, session, m_settings->runtime());
}

ULLMAgentOrchestrator& LLMServices::orchestrator()
{
    return *m_orchestrator;
}

URdkDomainAccess& LLMServices::domain()
{
    return *m_domain;
}

URdkApplicationCommands& LLMServices::applicationCommands()
{
    return *m_commands;
}

void LLMServices::setPresentationSink(std::unique_ptr<ILLMPresentationSink> sink)
{
    m_presentation_sink = std::move(sink);
    if(m_domain)
        m_domain->setPresentationSink(m_presentation_sink.get());
}

ILLMPresentationSink* LLMServices::presentationSink() const
{
    return m_presentation_sink.get();
}

ULLMSettingsStore& LLMServices::settings()
{
    static ULLMSettingsStore s_fallback;
    return m_settings ? *m_settings : s_fallback;
}

const LLMProviderProfile& LLMServices::activeProviderProfile() const
{
    return m_active_profile;
}

} // namespace RDK::LLM
