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

TEST(LLMPlanExecutor, CompensatesAddComponentOnLaterFailure)
{
    ULLMToolRegistry registry;
    int add_calls = 0;

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
        ++add_calls;
        ToolGatewayResult r;
        r.ok = true;
        r.result["long_name"] = "Net.TestNode";
        return r;
    });

    LLMToolDefinition fail_def;
    fail_def.name = "set_property";
    fail_def.kind = LLMToolKind::Write;
    fail_def.requires_confirmation = true;
    fail_def.input_schema = {{"type", "object"},
                             {"required", {"long_name", "property_name", "value"}},
                             {"properties",
                              {{"long_name", {{"type", "string"}}},
                               {"property_name", {{"type", "string"}}},
                               {"value", {{"type", "string"}}}}},
                             {"additionalProperties", false}};
    registry.registerTool(fail_def, [&](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = false;
        r.message = "simulated failure";
        return r;
    });

    int remove_calls = 0;
    LLMToolDefinition remove_def;
    remove_def.name = "remove_component";
    remove_def.kind = LLMToolKind::Write;
    remove_def.requires_confirmation = true;
    remove_def.input_schema = {{"type", "object"},
                               {"required", {"long_name"}},
                               {"properties", {{"long_name", {{"type", "string"}}}}},
                               {"additionalProperties", false}};
    registry.registerTool(remove_def, [&](const nlohmann::json& args) {
        ++remove_calls;
        ToolGatewayResult r;
        r.ok = true;
        r.result["removed"] = args.at("long_name").get<std::string>();
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
    plan.plan_id = "comp-test";
    ExecutionPlanStep s1;
    s1.step_id = 1;
    s1.tool_name = "add_component";
    s1.arguments = {{"class_name", "NPulseNeuron"},
                    {"parent_long_name", "Net"},
                    {"short_name", "TestNode"}};
    plan.steps.push_back(s1);
    ExecutionPlanStep s2;
    s2.step_id = 2;
    s2.tool_name = "set_property";
    s2.arguments = {{"long_name", "Net.TestNode"},
                    {"property_name", "Name"},
                    {"value", "x"}};
    plan.steps.push_back(s2);

    LLMSessionContext session;
    session.project_loaded = true;
    session.llm_write_enabled = true;
    const PlanExecutionResult result = executor.execute(plan, session, "trace");
    EXPECT_FALSE(result.ok);
    EXPECT_EQ(add_calls, 1);
    EXPECT_EQ(remove_calls, 1);
    EXPECT_EQ(result.compensation_steps_applied, 1);
}

TEST(LLMPlanExecutor, CompensatesSetPropertyWhenPreviousValueKnown)
{
    ULLMToolRegistry registry;

    LLMToolDefinition set_def;
    set_def.name = "set_property";
    set_def.kind = LLMToolKind::Write;
    set_def.requires_confirmation = true;
    set_def.input_schema = {{"type", "object"},
                            {"required", {"long_name", "property_name", "value"}},
                            {"properties",
                             {{"long_name", {{"type", "string"}}},
                              {"property_name", {{"type", "string"}}},
                              {"value", {{"type", "string"}}}}},
                            {"additionalProperties", false}};

    int set_calls = 0;
    registry.registerTool(set_def, [&](const nlohmann::json& args) {
        ++set_calls;
        ToolGatewayResult r;
        if(set_calls == 1)
        {
            r.ok = true;
            r.result["long_name"] = args.at("long_name");
            r.result["property_name"] = args.at("property_name");
            r.result["had_previous"] = true;
            r.result["previous_value"] = "old";
        }
        else
        {
            r.ok = true;
            EXPECT_EQ(args.at("value").get<std::string>(), "old");
        }
        return r;
    });

    LLMToolDefinition fail_def;
    fail_def.name = "fail_step";
    fail_def.kind = LLMToolKind::Read;
    fail_def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
    registry.registerTool(fail_def, [&](const nlohmann::json&) {
        ToolGatewayResult r;
        r.ok = false;
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
    plan.plan_id = "prop-comp";
    ExecutionPlanStep s1;
    s1.step_id = 1;
    s1.tool_name = "set_property";
    s1.arguments = {{"long_name", "Net.X"}, {"property_name", "Gain"}, {"value", "9"}};
    plan.steps.push_back(s1);
    ExecutionPlanStep s2;
    s2.step_id = 2;
    s2.tool_name = "fail_step";
    plan.steps.push_back(s2);

    LLMSessionContext session;
    session.project_loaded = true;
    session.llm_write_enabled = true;
    const PlanExecutionResult result = executor.execute(plan, session, "trace");
    EXPECT_FALSE(result.ok);
    EXPECT_EQ(set_calls, 2);
    EXPECT_EQ(result.compensation_steps_applied, 1);
}
