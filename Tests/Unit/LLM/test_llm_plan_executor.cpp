#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Orchestrator/ULLMExecutionPlan.h"
#include "Orchestrator/ULLMPlanExecutor.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

namespace {

nlohmann::json snapshotReadSchema()
{
    return {{"type", "object"},
            {"properties",
             {{"channel_index", {{"type", "integer"}}},
              {"max_components", {{"type", "integer"}}}}},
            {"additionalProperties", false}};
}

nlohmann::json addComponentWriteSchema()
{
    return {{"type", "object"},
            {"properties",
             {{"class_name", {{"type", "string"}}},
              {"parent_long_name", {{"type", "string"}}},
              {"short_name", {{"type", "string"}}},
              {"channel_index", {{"type", "integer"}}}}},
            {"additionalProperties", false}};
}

} // namespace

TEST(LLMPlanExecutor, RunsStepsInDependencyOrder)
{
    ULLMToolRegistry registry;
    LLMToolDefinition read_def;
    read_def.name = "get_net_snapshot";
    read_def.kind = LLMToolKind::Read;
    read_def.input_schema = snapshotReadSchema();
    registry.registerTool(read_def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = true;
        r.result["ok"] = true;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMPlanExecutor executor(registry, gateway);

    ULLMExecutionPlan plan;
    plan.plan_id = "test";
    ExecutionPlanStep s1;
    s1.step_id = 1;
    s1.tool_name = "get_net_snapshot";
    plan.steps.push_back(s1);

    LLMSessionContext session;
    session.project_loaded = true;
    session.llm_write_enabled = true;
    const PlanExecutionResult result = executor.execute(plan, session, "trace");
    EXPECT_TRUE(result.ok);
    EXPECT_EQ(result.completed_step_ids.size(), 1u);
}

TEST(LLMPlanExecutor, RepeatCountInvokesAddComponentMultipleTimes)
{
    int add_invokes = 0;
    ULLMToolRegistry registry;
    LLMToolDefinition add_def;
    add_def.name = "add_component";
    add_def.kind = LLMToolKind::Write;
    add_def.requires_confirmation = true;
    add_def.input_schema = addComponentWriteSchema();
    registry.registerTool(add_def, [&add_invokes](const nlohmann::json&) {
        ++add_invokes;
        ToolGatewayResult r;
        r.ok = true;
        r.result["long_name"] = "/ch0/Neuron" + std::to_string(add_invokes);
        return r;
    });

    LLMToolDefinition read_def;
    read_def.name = "get_net_snapshot";
    read_def.kind = LLMToolKind::Read;
    read_def.input_schema = snapshotReadSchema();
    registry.registerTool(read_def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = true;
        r.result["ok"] = true;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMPlanExecutor executor(registry, gateway);

    ULLMExecutionPlan plan;
    plan.plan_id = "repeat-test";
    ExecutionPlanStep snap;
    snap.step_id = 1;
    snap.tool_name = "get_net_snapshot";
    ExecutionPlanStep add;
    add.step_id = 2;
    add.tool_name = "add_component";
    add.arguments = {{"class_name", "NSPNeuron"},
                     {"parent_long_name", ""},
                     {"short_name", "Neuron"},
                     {"channel_index", 0}};
    add.depends_on = {1};
    add.repeat_count = 2;
    plan.steps = {snap, add};

    LLMSessionContext session;
    session.project_loaded = true;
    session.llm_write_enabled = true;
    const PlanExecutionResult result = executor.execute(plan, session, "trace");
    EXPECT_TRUE(result.ok);
    EXPECT_EQ(add_invokes, 2);
    EXPECT_EQ(result.completed_step_ids.size(), 2u);
}

TEST(LLMPlanExecutor, RecordedPathWritesTranscript)
{
    ULLMToolRegistry registry;
    LLMToolDefinition read_def;
    read_def.name = "get_net_snapshot";
    read_def.kind = LLMToolKind::Read;
    read_def.input_schema = snapshotReadSchema();
    registry.registerTool(read_def, [](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = true;
        r.result["ok"] = true;
        return r;
    });

    ULLMPolicyEngine policy;
    URdkDomainAccess domain(nullptr);
    ULLMAuditLog audit;
    ULLMIdempotencyStore idem;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway(registry, policy, domain, audit, idem, validator);
    ULLMPlanExecutor executor(registry, gateway);
    ULLMConversationStore store;
    ConversationState& state = store.getOrCreate("plan-rec");

    ULLMExecutionPlan plan;
    plan.plan_id = "recorded";
    ExecutionPlanStep s1;
    s1.step_id = 1;
    s1.tool_name = "get_net_snapshot";
    plan.steps.push_back(s1);

    LLMSessionContext session;
    session.session_id = "plan-rec";
    session.project_loaded = true;
    session.llm_write_enabled = true;
    PlanExecuteOptions opts;
    opts.conversation_state = &state;
    opts.conversation_store = &store;
    const PlanExecutionResult result = executor.execute(plan, session, "trace", opts);
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(state.messages.empty());
    bool saw_tool = false;
    for(const LLMMessage& m : state.messages)
    {
        if(m.role == LLMMessage::Role::Tool && m.tool_name.value_or("") == "get_net_snapshot")
            saw_tool = true;
    }
    EXPECT_TRUE(saw_tool);
}
