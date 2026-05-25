#include <gtest/gtest.h>

#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ULLMMockProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/RegisterApplicationTools.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

TEST(LLMOrchestratorLifecycleArgs, CreateNewConfigWithoutPathSkipsLlm)
{
    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "LLM should not run for bare create new config";
    provider.enqueue(mock);

    ULLMToolRegistry registry;
    RegisterApplicationTools(registry);
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "lifecycle-create";
    req.trace_id = "t-create";
    req.user_text = "create new config";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_FALSE(resp.needs_argument_clarification);
    EXPECT_TRUE(resp.pending_confirmation || resp.ok);
    EXPECT_NE(resp.text, "LLM should not run for bare create new config");
}

TEST(LLMOrchestratorLifecycleArgs, LoadConfigWithoutPathSkipsLlm)
{
    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "LLM should not run for bare load config";
    provider.enqueue(mock);

    ULLMToolRegistry registry;
    RegisterApplicationTools(registry);
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "lifecycle-args";
    req.trace_id = "t-load";
    req.user_text = "load config";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok);
    EXPECT_TRUE(resp.needs_argument_clarification);
    EXPECT_NE(resp.text.find("load_configuration"), std::string::npos);
    EXPECT_NE(resp.text, "LLM should not run for bare load config");
}
