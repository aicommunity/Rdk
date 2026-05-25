#include <gtest/gtest.h>

#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ULLMMockProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

TEST(LLMOrchestrator, TextResponseFromMock)
{
    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "Hello from mock";
    provider.enqueue(mock);

    ULLMToolRegistry registry;
    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    LLMRequestEnvelope req;
    req.session_id = "test";
    req.trace_id = "t1";
    req.user_text = "hello";
    req.session.project_loaded = false;

    LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok);
    EXPECT_EQ(resp.text, "Hello from mock");
}
