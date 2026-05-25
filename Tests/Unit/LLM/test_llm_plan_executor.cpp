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

TEST(LLMPlanExecutor, RunsStepsInDependencyOrder)
{
    ULLMToolRegistry registry;
    LLMToolDefinition read_def;
    read_def.name = "get_net_snapshot";
    read_def.kind = LLMToolKind::Read;
    read_def.input_schema = {{"type", "object"}, {"additionalProperties", false}};
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
