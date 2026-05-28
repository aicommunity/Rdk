#include <gtest/gtest.h>
#include <cstdlib>

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
#include "Orchestrator/ULLMExecutionPlan.h"

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

TEST(LLMOrchestrator, MutateNoSuitableToolAfterRecovery)
{
    ::unsetenv("NMSDK_LLM_INTENT_LLM");

    ULLMMockProvider provider;
    LLMCompletionResult first;
    first.ok = true;
    first.text = "I need more context.";
    provider.enqueue(first);
    LLMCompletionResult second;
    second.ok = true;
    second.text = "Still no tool call.";
    provider.enqueue(second);

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
    req.session_id = "mutate-no-tool";
    req.trace_id = "t2";
    req.user_text = "create configuration";
    req.session.project_loaded = false;
    req.session.llm_write_enabled = true;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok);
    EXPECT_TRUE(resp.no_suitable_tool);
    EXPECT_EQ(resp.text, "Cannot find a suitable action.");
}

TEST(LLMOrchestrator, RollbackReportsFailureWhenCompensationFails)
{
    ULLMMockProvider provider;
    ULLMToolRegistry registry;

    LLMToolDefinition disconnect_def;
    disconnect_def.name = "disconnect_components";
    disconnect_def.kind = LLMToolKind::Write;
    disconnect_def.requires_confirmation = false;
    disconnect_def.input_schema = {{"type", "object"}};
    registry.registerTool(disconnect_def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = false;
        r.error_code = "UNDO_FAIL";
        r.message = "cannot disconnect";
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMConversationStore store;
    ULLMAgentOrchestrator orch(provider, registry, gateway, store);

    ConversationState& state = store.getOrCreate("rollback-session");
    state.session_id = "rollback-session";
    ULLMExecutionPlan plan;
    plan.plan_id = "plan-1";
    ExecutionPlanStep step;
    step.step_id = 1;
    step.tool_name = "connect_components";
    step.status = "done";
    step.last_result = {{"from_long_name", "A"},
                        {"from_property", "p1"},
                        {"to_long_name", "B"},
                        {"to_property", "p2"}};
    plan.steps.push_back(step);
    state.pending_plan = plan;

    LLMSessionContext session;
    session.session_id = "rollback-session";
    session.project_loaded = true;
    session.llm_write_enabled = true;

    const LLMFinalResponse resp = orch.rollbackPlanExecution("rollback-session", "trace-rb", session);
    EXPECT_FALSE(resp.ok);
    EXPECT_EQ(resp.text, "Plan rollback failed.");
    EXPECT_FALSE(resp.error.empty());
}

TEST(LLMOrchestrator, CancelSessionDoesNotAffectOtherSessions)
{
    ULLMMockProvider provider;
    LLMCompletionResult mock;
    mock.ok = true;
    mock.text = "ok";
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

    orch.cancelSession("session-a");

    LLMRequestEnvelope req;
    req.session_id = "session-b";
    req.trace_id = "trace-session-b";
    req.user_text = "hello";
    req.session.project_loaded = false;

    const LLMFinalResponse resp = orch.handleUserMessage(req);
    EXPECT_TRUE(resp.ok);
    EXPECT_EQ(resp.text, "ok");
}
