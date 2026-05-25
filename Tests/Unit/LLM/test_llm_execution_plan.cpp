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
