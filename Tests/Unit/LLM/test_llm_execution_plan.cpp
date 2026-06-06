#include <gtest/gtest.h>

#include "Orchestrator/ULLMExecutionPlan.h"

TEST(LLMExecutionPlan, ParsesJsonBlock)
{
    const std::string text = R"(
Here is the plan:
```json
{
  "plan_id": "p1",
  "steps": [
    {"step_id": 1, "tool_name": "get_net_snapshot", "arguments": {"channel_index": 0}},
    {"step_id": 2, "tool_name": "list_registered_classes", "arguments": {}, "depends_on": [1]}
  ]
}
```
)";
    const auto plan = RDK::LLM::parseExecutionPlanFromAssistantText(text);
    ASSERT_TRUE(plan.has_value());
    EXPECT_EQ(plan->steps.size(), 2u);
    EXPECT_EQ(plan->steps[1].depends_on.front(), 1);
}

TEST(LLMExecutionPlan, StrictResponseFormatShape)
{
    const nlohmann::json fmt = RDK::LLM::executionPlanOpenAiResponseFormat();
    EXPECT_EQ(fmt["type"], "json_schema");
    EXPECT_TRUE(fmt["json_schema"]["strict"].get<bool>());
    EXPECT_EQ(fmt["json_schema"]["name"], "execution_plan");
    EXPECT_TRUE(fmt["json_schema"]["schema"].contains("properties"));
}

TEST(LLMExecutionPlan, ParsesRawStrictJsonObject)
{
    const std::string text = R"({
  "plan_id": "strict-1",
  "steps": [{"step_id": 1, "tool_name": "get_net_snapshot", "arguments": {}}]
})";
    const auto plan = RDK::LLM::parseExecutionPlanFromAssistantText(text);
    ASSERT_TRUE(plan.has_value());
    EXPECT_EQ(plan->plan_id, "strict-1");
}

TEST(LLMExecutionPlan, FormatsPreviewText)
{
    RDK::LLM::ULLMExecutionPlan plan;
    plan.plan_id = "preview-1";
    RDK::LLM::ExecutionPlanStep step;
    step.step_id = 1;
    step.tool_name = "get_net_snapshot";
    plan.steps.push_back(step);
    const std::string preview = RDK::LLM::formatExecutionPlanPreview(plan);
    EXPECT_NE(preview.find("preview-1"), std::string::npos);
    EXPECT_NE(preview.find("get_net_snapshot"), std::string::npos);
}

TEST(LLMExecutionPlan, ParsesExtendedFields)
{
    const std::string text = R"({
  "plan_id": "extended-1",
  "goal_en": "Add two NSPNeuron and connect",
  "confidence": 0.82,
  "goal_success": { "type": "link_exists", "params": { "from_long_name": "PNeuron", "to_long_name": "PNeuron2" } },
  "steps": [
    {"step_id": 1, "tool_name": "add_component", "arguments": {"class_name": "NSPNeuron"},
     "success": {"type": "component_count", "params": {"class_name": "NSPNeuron", "min_count": 2}},
     "repeat_count": 2}
  ]
})";
    const auto plan = RDK::LLM::parseExecutionPlanFromAssistantText(text);
    ASSERT_TRUE(plan.has_value());
    EXPECT_EQ(plan->goal_en, "Add two NSPNeuron and connect");
    EXPECT_NEAR(plan->confidence, 0.82f, 1e-6f);
    ASSERT_TRUE(plan->goal_success.has_value());
    EXPECT_EQ(plan->goal_success->type, "link_exists");
    ASSERT_EQ(plan->steps.size(), 1u);
    ASSERT_TRUE(plan->steps[0].success.has_value());
    EXPECT_EQ(plan->steps[0].success->type, "component_count");
    EXPECT_EQ(plan->steps[0].repeat_count, 2);
}
