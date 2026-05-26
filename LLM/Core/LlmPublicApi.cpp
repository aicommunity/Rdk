#include "LlmPublicApi.h"

#include <cstdlib>
#include <filesystem>

#include "Context/ILLMProjectContextProvider.h"
#include "LlmModuleInit.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Providers/ULLMProviderFactory.h"
#include "Settings/ULLMProviderAuth.h"

namespace RDK::LLM {

namespace {

class EmptyKnowledgeCatalog : public ILLMKnowledgeCatalog {
public:
    std::vector<LLMKnowledgeSource> sources() const override { return {}; }
    std::filesystem::path prebuiltIndexDirectory() const override { return {}; }
    std::string catalogFingerprint() const override { return "empty"; }
};

} // namespace

LLMServices& LLMServices::instance()
{
    static LLMServices s;
    return s;
}

void LLMServices::initialize(RDK::UApplication* app, ILLMProjectContextProvider* project_context,
                             ILLMProviderSettingsSource* settings_source,
                             std::unique_ptr<ILLMKnowledgeCatalog> catalog)
{
    m_project_context = project_context;
    m_catalog = catalog ? std::move(catalog) : std::make_unique<EmptyKnowledgeCatalog>();
    m_search_index = std::make_unique<UDocSearchIndex>();
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

    const std::filesystem::path repository_root =
        project_context ? project_context->paths().repository_root : std::filesystem::path(".");
    const std::string fingerprint = m_catalog->catalogFingerprint();
    const std::filesystem::path prebuilt = m_catalog->prebuiltIndexDirectory();
    const std::filesystem::path dev_cache = repository_root / "LLM/index";

    auto ensure_index = [&]() {
        bool loaded = false;
        if(!prebuilt.empty())
            loaded = m_search_index->loadPrebuilt(prebuilt, fingerprint);
        if(!loaded && !dev_cache.empty())
            loaded = m_search_index->loadPrebuilt(dev_cache, fingerprint);

        if(loaded)
        {
            const IndexSyncResult sync =
                m_search_index->syncFromCatalog(*m_catalog, repository_root);
            if(sync == IndexSyncResult::NeedsFullRebuild)
            {
                m_search_index->buildFromCatalog(*m_catalog, repository_root);
                m_search_index->savePrebuilt(dev_cache, fingerprint);
            }
            else if(sync == IndexSyncResult::IncrementalUpdated)
                m_search_index->savePrebuilt(dev_cache, fingerprint);
            return;
        }

        m_search_index->buildFromCatalog(*m_catalog, repository_root);
        if(!dev_cache.empty())
            m_search_index->savePrebuilt(dev_cache, fingerprint);
    };
    ensure_index();

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

ILLMKnowledgeCatalog* LLMServices::catalog() const
{
    return m_catalog.get();
}

UDocSearchIndex& LLMServices::searchIndex()
{
    static UDocSearchIndex s_fallback;
    return m_search_index ? *m_search_index : s_fallback;
}

} // namespace RDK::LLM
