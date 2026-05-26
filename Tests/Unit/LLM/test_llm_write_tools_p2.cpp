#include <gtest/gtest.h>

#include "Domain/ULLMWriteArgumentNormalizer.h"
#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMToolFilterBuilder.h"
#include "Policy/ULLMPolicyEngine.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolRegistry.h"

#include "RegisterMotionControlLibLlmTools.h"
#include "RegisterPulseLibLlmTools.h"

using namespace RDK::LLM;

namespace {

bool filterAllows(const ToolFilter& filter, const char* tool)
{
    if(!filter.allowed_tool_names)
        return false;
    return filter.allowed_tool_names->count(tool) > 0;
}

} // namespace

TEST(LLMWriteToolsP2, EntityResolutionFlags)
{
    EXPECT_TRUE(writeToolNeedsEntityResolution("set_property"));
    EXPECT_TRUE(writeToolNeedsEntityResolution("add_pulse_component"));
    EXPECT_FALSE(writeToolNeedsEntityResolution("get_net_snapshot"));
    EXPECT_TRUE(isNetGraphWriteTool("set_motion_property"));
    EXPECT_FALSE(isNetGraphWriteTool("save_project"));
}

TEST(LLMWriteToolsP2, NormalizeWithoutEngineReturnsNotFound)
{
    URdkDomainAccess domain(nullptr);
    nlohmann::json args = {{"long_name", "Neuron1"}, {"channel_index", 0}};
    const WriteArgumentNormalizeResult out =
        normalizeWriteToolArguments("set_property", args, domain, 0);
    EXPECT_FALSE(out.ok);
    EXPECT_EQ(out.error_code, "ENTITY_NOT_FOUND");
}

TEST(LLMWriteToolsP2, PlanRequiresSnapshotBeforeGraphWrite)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);

    ULLMPolicyEngine policy;
    LLMSessionContext session;
    session.llm_write_enabled = true;

    ULLMExecutionPlan plan;
    plan.plan_id = "p2";
    ExecutionPlanStep write_step;
    write_step.step_id = 1;
    write_step.tool_name = "set_property";
    write_step.arguments = {{"long_name", "X"}, {"property_name", "Comment"}, {"value", "v"}};
    plan.steps.push_back(write_step);

    const PolicyDecision denied = policy.checkPlan(plan, session, registry);
    EXPECT_FALSE(denied.allowed);
    EXPECT_EQ(denied.deny_code, "PLAN_NEEDS_SNAPSHOT");

    ExecutionPlanStep snap_step;
    snap_step.step_id = 0;
    snap_step.tool_name = "get_net_snapshot";
    plan.steps.insert(plan.steps.begin(), snap_step);
    const PolicyDecision allowed = policy.checkPlan(plan, session, registry);
    EXPECT_TRUE(allowed.allowed);
}

TEST(LLMWriteToolsP2, LibraryWriteToolsRegistered)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterPulseLibLlmTools(registry, nullptr, domain);
    RegisterMotionControlLibLlmTools(registry, nullptr, domain);

    EXPECT_NE(registry.find("add_pulse_component"), nullptr);
    EXPECT_NE(registry.find("set_pulse_property"), nullptr);
    EXPECT_NE(registry.find("add_motion_component"), nullptr);
    EXPECT_NE(registry.find("set_motion_property"), nullptr);
    EXPECT_EQ(registry.find("add_pulse_component")->kind, LLMToolKind::Write);
}

TEST(LLMWriteToolsP2, MutateFilterIncludesLibraryWrites)
{
    const ToolFilter filter =
        buildToolFilter(LLMIntentKind::Mutate, true, ConfigurationLifecycleAction::None);
    EXPECT_TRUE(filterAllows(filter, "add_pulse_component"));
    EXPECT_TRUE(filterAllows(filter, "set_motion_property"));
    EXPECT_TRUE(filterAllows(filter, "search_pulse_docs"));
}
