#include "LlmPublicApi.h"

#include <cstdlib>
#include <filesystem>
#include <thread>

#include "Context/ILLMProjectContextProvider.h"
#include "Session/ULLMConversationStore.h"
#include "LlmModuleInit.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Packs/RegisterBuiltinPacks.h"
#include "Packs/ULLMCapabilityPackRegistry.h"
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
    RegisterAgentTools(GetToolRegistry());
    if(project_context)
        project_context->registerExtraTools(GetToolRegistry(), *m_domain);

    m_gateway = std::make_unique<ULLMToolGateway>(GetToolRegistry(), *m_policy, *m_domain,
                                                  GetAuditLog(), *m_idempotency, *m_validator);
    m_context_retriever =
        std::make_unique<URdkContextRetriever>(*m_domain, project_context);

    m_packs = std::make_unique<ULLMCapabilityPackRegistry>();
    RegisterBuiltinCapabilityPacks(*m_packs);

    const std::filesystem::path repository_root =
        project_context ? project_context->paths().repository_root : std::filesystem::path(".");
    const std::string fingerprint = m_catalog->catalogFingerprint();
    const std::filesystem::path prebuilt = m_catalog->prebuiltIndexDirectory();
    const std::filesystem::path dev_cache = repository_root / "LLM/index";

    auto ensure_index = [&]() {
        // Startup must stay off the UI-thread rebuild path: full catalog build/sync can take
        // minutes and freezes the launch progress bar at ~20% (NmsdkRegisterLlm).
        // Prefer an exact fingerprint match; otherwise accept a stale prebuilt. Rebuild async.
        const char* sync_env = std::getenv("NMSDK_LLM_INDEX_SYNC_ON_START");
        const bool sync_on_start = sync_env && sync_env[0] == '1';

        bool exact = false;
        auto try_load = [&](const std::filesystem::path& dir, const std::string& fp) -> bool {
            if(dir.empty())
                return false;
            return m_search_index->loadPrebuilt(dir, fp);
        };

        exact = try_load(prebuilt, fingerprint) || try_load(dev_cache, fingerprint);
        if(!exact)
        {
            // Empty fingerprint = ignore manifest match (stale index is fine for first paint).
            (void)(try_load(prebuilt, "") || try_load(dev_cache, ""));
        }

        if(sync_on_start && !m_search_index->empty())
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

        if(exact)
            return;

        // Refresh cache in the background for the next launch (and eventual warm search).
        ILLMKnowledgeCatalog* catalog = m_catalog.get();
        if(!catalog || dev_cache.empty())
            return;
        std::thread([catalog, repository_root, dev_cache, fingerprint]() {
            try
            {
                UDocSearchIndex idx;
                idx.buildFromCatalog(*catalog, repository_root);
                idx.savePrebuilt(dev_cache, fingerprint);
            }
            catch(...)
            {
            }
        }).detach();
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
    bindSpawnExploreSubagent(*m_provider, GetToolRegistry(), *m_gateway);
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

URdkContextRetriever* LLMServices::contextRetriever()
{
    return m_context_retriever.get();
}

ILLMCapabilityPackRegistry& LLMServices::packs()
{
    static ULLMCapabilityPackRegistry s_fallback;
    return m_packs ? *m_packs : s_fallback;
}

bool LLMServices::loadConversationSession(const std::string& session_id)
{
    return m_store && m_store->loadFromDisk(session_id);
}

const ConversationState* LLMServices::conversationState(const std::string& session_id) const
{
    return m_store ? m_store->findSession(session_id) : nullptr;
}

ConversationState* LLMServices::mutableConversationState(const std::string& session_id)
{
    if(!m_store || session_id.empty())
        return nullptr;
    return &m_store->getOrCreate(session_id);
}

} // namespace RDK::LLM
