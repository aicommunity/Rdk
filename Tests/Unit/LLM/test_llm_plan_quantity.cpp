#include <gtest/gtest.h>

#include "Orchestrator/ULLMPlanQuantity.h"
#include "Orchestrator/ULLMQuantityParser.h"

#include <optional>

using namespace RDK::LLM;

TEST(LLMPlanQuantity, ApplyGoalQuantitySetsRepeatOnAddSteps)
{
    ULLMExecutionPlan plan;
    plan.goal_en = "add two NSPNeuron components";
    ExecutionPlanStep snap;
    snap.step_id = 1;
    snap.tool_name = "get_net_snapshot";
    ExecutionPlanStep add;
    add.step_id = 2;
    add.tool_name = "add_component";
    add.repeat_count = 1;
    plan.steps = {snap, add};

    applyGoalQuantityToExecutionPlan(plan, std::nullopt);
    ASSERT_EQ(plan.steps.size(), 2u);
    EXPECT_EQ(plan.steps[1].repeat_count, 2);
}

TEST(LLMPlanQuantity, SkipsSessionOverrideWhenMultipleAddSteps)
{
    ULLMExecutionPlan plan;
    plan.goal_en = "add 2 Foo and 3 Bar";
    ExecutionPlanStep a;
    a.tool_name = "add_component";
    a.repeat_count = 2;
    ExecutionPlanStep b;
    b.tool_name = "add_component";
    b.repeat_count = 3;
    plan.steps = {a, b};

    applyGoalQuantityToExecutionPlan(plan, 99);
    EXPECT_EQ(plan.steps[0].repeat_count, 2);
    EXPECT_EQ(plan.steps[1].repeat_count, 3);
}

TEST(LLMPlanQuantity, ApplyUsesSessionOverrideWhenGoalHasNoCount)
{
    ULLMExecutionPlan plan;
    plan.goal_en = "add NSPNeuron components";
    ExecutionPlanStep add;
    add.step_id = 2;
    add.tool_name = "add_component";
    add.repeat_count = 1;
    plan.steps.push_back(add);

    applyGoalQuantityToExecutionPlan(plan, 6);
    ASSERT_EQ(plan.steps.size(), 1u);
    EXPECT_EQ(plan.steps[0].repeat_count, 6);
}

TEST(LLMPlanQuantity, UniqueShortNamesForRepeats)
{
    EXPECT_EQ(uniqueShortNameForAddRepeat("Neuron", 0), "Neuron");
    EXPECT_EQ(uniqueShortNameForAddRepeat("Neuron", 1), "Neuron2");
    EXPECT_EQ(uniqueShortNameForAddRepeat("Neuron", 2), "Neuron3");
}

TEST(LLMQuantityParser, ParsesRussianThree)
{
    const ParsedQuantity q = extractQuantity("добавь три нейрона NLPNeuron");
    ASSERT_TRUE(q.valid);
    EXPECT_EQ(q.count, 3);
}

TEST(LLMPlanQuantity, PlanPreviewShowsRepeatMultiplier)
{
    ULLMExecutionPlan plan;
    plan.plan_id = "task_demo";
    ExecutionPlanStep add;
    add.step_id = 2;
    add.tool_name = "add_component";
    add.repeat_count = 3;
    plan.steps.push_back(add);
    const std::string preview = formatExecutionPlanPreview(plan);
    EXPECT_NE(preview.find("add_component x3"), std::string::npos);
}
