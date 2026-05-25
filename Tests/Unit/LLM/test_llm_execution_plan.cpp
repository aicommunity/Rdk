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
