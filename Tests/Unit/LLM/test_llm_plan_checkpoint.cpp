#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Observability/ULLMAuditLog.h"
#include "Observability/ULLMIdempotencyStore.h"
#include "Orchestrator/ULLMExecutionPlan.h"
#include "Orchestrator/ULLMPlanExecutor.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Tools/ULLMToolArgumentValidator.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

TEST(LLMPlanCheckpoint, PausesOnFailureThenResumes)
{
    ULLMToolRegistry registry;
    int step1_calls = 0;
    int step2_calls = 0;

    LLMToolDefinition ok_def;
    ok_def.name = "step_one";
    ok_def.kind = LLMToolKind::Read;
    ok_def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(ok_def, [&](const nlohmann::json&) {
        ++step1_calls;
        ToolGatewayResult r;
        r.ok = true;
        r.result["marker"] = "one";
        return r;
    });

    LLMToolDefinition fail_def;
    fail_def.name = "step_two";
    fail_def.kind = LLMToolKind::Read;
    fail_def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(fail_def, [&](const nlohmann::json&) {
        ++step2_calls;
        ToolGatewayResult r;
        r.ok = (step2_calls > 1);
        if(!r.ok)
            r.message = "first attempt fails";
        r.result["marker"] = "two";
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
    plan.plan_id = "chk-1";
    ExecutionPlanStep s1;
    s1.step_id = 1;
    s1.tool_name = "step_one";
    plan.steps.push_back(s1);
    ExecutionPlanStep s2;
    s2.step_id = 2;
    s2.tool_name = "step_two";
    plan.steps.push_back(s2);

    LLMSessionContext session;
    session.project_loaded = true;

    PlanExecutionResult first =
        executor.execute(plan, session, "trace", planExecuteWithCheckpointOnFailure());
    EXPECT_FALSE(first.ok);
    EXPECT_TRUE(first.paused_for_resume);
    EXPECT_EQ(plan.steps[0].status, "done");
    EXPECT_EQ(plan.steps[1].status, "failed");
    EXPECT_EQ(step1_calls, 1);
    EXPECT_EQ(step2_calls, 1);

    PlanExecutionResult second = executor.execute(plan, session, "trace2", planExecuteResume());
    EXPECT_TRUE(second.ok);
    EXPECT_EQ(plan.steps[0].status, "done");
    EXPECT_EQ(plan.steps[1].status, "done");
    EXPECT_EQ(step1_calls, 1);
    EXPECT_EQ(step2_calls, 2);
}

TEST(LLMPlanCheckpoint, RollbackUsesStoredResults)
{
    ULLMToolRegistry registry;
    int remove_calls = 0;

    LLMToolDefinition add_def;
    add_def.name = "add_component";
    add_def.kind = LLMToolKind::Write;
    add_def.requires_confirmation = true;
    add_def.input_schema = {{"type", "object"},
                            {"required", {"class_name", "parent_long_name", "short_name"}},
                            {"properties",
                             {{"class_name", {{"type", "string"}}},
                              {"parent_long_name", {{"type", "string"}}},
                              {"short_name", {{"type", "string"}}}}},
                            {"additionalProperties", false}};
    registry.registerTool(add_def, [&](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = true;
        r.result["long_name"] = "Net.Node";
        return r;
    });

    LLMToolDefinition remove_def;
    remove_def.name = "remove_component";
    remove_def.kind = LLMToolKind::Write;
    remove_def.requires_confirmation = true;
    remove_def.input_schema = {{"type", "object"},
                               {"required", {"long_name"}},
                               {"properties", {{"long_name", {{"type", "string"}}}}},
                               {"additionalProperties", false}};
    registry.registerTool(remove_def, [&](const nlohmann::json&) {
        ++remove_calls;
        ToolGatewayResult r;
        r.ok = true;
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
    plan.plan_id = "rb-1";
    ExecutionPlanStep s1;
    s1.step_id = 1;
    s1.tool_name = "add_component";
    s1.arguments = {{"class_name", "X"}, {"parent_long_name", "Net"}, {"short_name", "Node"}};
    plan.steps.push_back(s1);
    ExecutionPlanStep s2;
    s2.step_id = 2;
    s2.tool_name = "step_two";
    s2.status = "pending";
    plan.steps.push_back(s2);

    LLMSessionContext session;
    session.project_loaded = true;
    session.llm_write_enabled = true;

    plan.steps[0].status = "done";
    plan.steps[0].last_result = {{"long_name", "Net.Node"}};
    plan.paused = true;

    std::string note;
    const int applied = executor.compensateCompletedWrites(plan, session, "trace", note);
    EXPECT_EQ(applied, 1);
    EXPECT_EQ(remove_calls, 1);
}

TEST(LLMExecutionPlan, PersistPausedPlanRoundTrip)
{
    ULLMExecutionPlan plan;
    plan.plan_id = "persist-1";
    plan.paused = true;
    plan.checkpoint_after_step_id = 2;
    ExecutionPlanStep s1;
    s1.step_id = 1;
    s1.tool_name = "get_net_snapshot";
    s1.status = "done";
    s1.last_result = {{"ok", true}};
    plan.steps.push_back(s1);

    const nlohmann::json j = executionPlanToJson(plan);
    const auto restored = executionPlanFromJson(j);
    ASSERT_TRUE(restored.has_value());
    EXPECT_TRUE(restored->paused);
    EXPECT_EQ(restored->checkpoint_after_step_id, 2);
    EXPECT_EQ(restored->steps[0].status, "done");
    EXPECT_TRUE(restored->steps[0].last_result.value("ok", false));
}
