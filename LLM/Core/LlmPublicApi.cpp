#include "LlmPublicApi.h"

#include <cstdlib>

#include "Context/ILLMProjectContextProvider.h"
#include "LlmModuleInit.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Providers/ULLMProviderFactory.h"

namespace RDK::LLM {

LLMServices& LLMServices::instance()
{
    static LLMServices s;
    return s;
}

void LLMServices::initialize(RDK::UApplication* app, ILLMProjectContextProvider* project_context)
{
    m_domain = std::make_unique<URdkDomainAccess>(app);
    GetContextRegistry().setPrimary(project_context);

    LLMProviderProfile profile;
    if(const char* mock = std::getenv("NMSDK_LLM_MOCK"); mock && mock[0] == '1')
        profile.kind = LLMProviderKind::Mock;
    else
    {
        profile.kind = LLMProviderKind::OllamaOpenAICompat;
        profile.base_url = "http://127.0.0.1:11434/v1";
        profile.model = "qwen2.5:7b";
    }
    m_provider = ULLMProviderFactory::create(profile);

    m_store = std::make_unique<ULLMConversationStore>();
    m_validator = std::make_unique<ULLMToolArgumentValidator>();
    m_idempotency = std::make_unique<ULLMIdempotencyStore>();
    m_policy = std::make_unique<ULLMPolicyEngine>();

    RegisterCoreRdkTools(GetToolRegistry(), *m_domain, project_context);

    m_gateway = std::make_unique<ULLMToolGateway>(GetToolRegistry(), *m_policy, *m_domain,
                                                  GetAuditLog(), *m_idempotency, *m_validator);
    m_orchestrator =
        std::make_unique<ULLMAgentOrchestrator>(*m_provider, GetToolRegistry(), *m_gateway, *m_store);

    GetAuditLog().setLogDirectory((project_context ? project_context->paths().repository_root.string()
                                                   : ".") +
                                 "/LLM/audit");
}

ULLMAgentOrchestrator& LLMServices::orchestrator()
{
    return *m_orchestrator;
}

URdkDomainAccess& LLMServices::domain()
{
    return *m_domain;
}

} // namespace RDK::LLM
