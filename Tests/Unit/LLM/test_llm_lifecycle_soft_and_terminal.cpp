#include <gtest/gtest.h>

#include <cstdlib>

#include "Orchestrator/ULLMConfigurationLifecycle.h"
#include "Orchestrator/ULLMTurnTerminalHelpers.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Orchestrator/ULLMExecutionPlan.h"
#include "Providers/ULLMMockProvider.h"
#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Tools/ULLMToolArgumentValidator.h"

using namespace RDK::LLM;

TEST(LifecycleSoftHints, DirectInvokeOffByDefault)
{
    unsetenv("NMSDK_LLM_LIFECYCLE_DIRECT");
    EXPECT_FALSE(lifecycleDirectInvokeEnabled());
}

TEST(LifecycleSoftHints, ForceToolChoiceOffByDefault)
{
    unsetenv("NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE");
    EXPECT_FALSE(lifecycleForceToolChoiceEnabled());
    EXPECT_FALSE(shouldForceLifecycleToolChoice(0.99f));
}

TEST(LifecycleSoftHints, ForceToolChoiceWhenEnvSet)
{
    setenv("NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE", "1", 1);
    EXPECT_TRUE(lifecycleForceToolChoiceEnabled());
    EXPECT_TRUE(shouldForceLifecycleToolChoice(0.96f));
    EXPECT_FALSE(shouldForceLifecycleToolChoice(0.5f));
    unsetenv("NMSDK_LLM_LIFECYCLE_FORCE_TOOL_CHOICE");
}

TEST(ExecutionPlanPropose, FromProposePlanArguments)
{
    nlohmann::json args = {{"goal", "wire neurons"},
                           {"steps",
                            nlohmann::json::array({{{"tool_name", "connect_components"},
                                                    {"arguments", nlohmann::json::object()}}})}};
    const std::optional<ULLMExecutionPlan> plan = executionPlanFromProposePlanArguments(args);
    ASSERT_TRUE(plan.has_value());
    EXPECT_EQ(plan->goal_en, "wire neurons");
    ASSERT_EQ(plan->steps.size(), 1u);
    EXPECT_EQ(plan->steps[0].tool_name, "connect_components");
}

TEST(ExecutionPlanPropose, PolicyAllowsReadOnlySnapshotPlan)
{
    nlohmann::json args = {{"goal", "inspect"},
                           {"steps",
                            nlohmann::json::array({{{"tool_name", "get_net_snapshot"},
                                                    {"arguments", nlohmann::json::object()}}})}};
    const std::optional<ULLMExecutionPlan> plan = executionPlanFromProposePlanArguments(args);
    ASSERT_TRUE(plan.has_value());

    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);
    ULLMPolicyEngine policy;
    LLMSessionContext session;
    session.project_loaded = true;
    session.llm_write_enabled = true;
    const PolicyDecision decision = policy.checkPlan(*plan, session, registry);
    EXPECT_TRUE(decision.allowed) << decision.deny_message;
}

TEST(TurnTerminalHelpers, AppendCancelledToolResults)
{
    ULLMConversationStore store;
    const std::vector<LLMToolCall> calls = {{"id-1", "ask_user", nlohmann::json::object()}};
    appendCancelledToolResults(store, "sess-cancel", calls);

    const ConversationState& state = store.getOrCreate("sess-cancel");
    ASSERT_EQ(state.messages.size(), 1u);
    EXPECT_EQ(state.messages[0].role, LLMMessage::Role::Tool);
    EXPECT_EQ(state.messages[0].tool_call_id, "id-1");
    EXPECT_NE(state.messages[0].content.find("cancelled"), std::string::npos);
}

TEST(MockProvider, PreservesProposePlanToolCalls)
{
    const nlohmann::json plan_args = {
        {"goal", "inspect"},
        {"steps",
         nlohmann::json::array({{{"tool_name", "get_net_snapshot"},
                                 {"arguments", nlohmann::json::object()}}})}};
    LLMCompletionResult plan_call;
    plan_call.ok = true;
    plan_call.tool_calls.push_back({"tc-plan", "propose_plan", plan_args});

    ULLMMockProvider provider;
    provider.enqueue(plan_call);
    const LLMCompletionResult out = provider.chat({}, {});
    ASSERT_EQ(out.tool_calls.size(), 1u);
    EXPECT_EQ(out.tool_calls[0].name, "propose_plan");
    EXPECT_TRUE(executionPlanFromProposePlanArguments(out.tool_calls[0].arguments).has_value());
}
