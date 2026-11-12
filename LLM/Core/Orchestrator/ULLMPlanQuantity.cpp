#include "ULLMPlanQuantity.h"

#include "ULLMPlanRepeatPolicy.h"
#include "ULLMQuantityParser.h"

namespace RDK::LLM {

void applyGoalQuantityToExecutionPlan(ULLMExecutionPlan& plan,
                                      std::optional<int> session_primary_quantity)
{
    int add_steps = 0;
    for(const ExecutionPlanStep& step : plan.steps)
    {
        if(step.tool_name == "add_component")
            ++add_steps;
    }
    if(add_steps != 1)
        return;

    int count = 0;
    const ParsedQuantity from_goal = extractQuantityHeuristic(plan.goal_en);
    if(from_goal.valid && from_goal.count > 1)
        count = from_goal.count;
    if(count <= 1 && session_primary_quantity && *session_primary_quantity > 1)
        count = *session_primary_quantity;
    if(count <= 1)
        return;

    for(ExecutionPlanStep& step : plan.steps)
    {
        if(toolSupportsPlanStepRepeat(step.tool_name))
            step.repeat_count = std::max(step.repeat_count, count);
    }
}

std::string uniqueShortNameForAddRepeat(const std::string& base_short_name, int repeat_index)
{
    if(repeat_index <= 0)
        return base_short_name;
    return base_short_name + std::to_string(repeat_index + 1);
}

} // namespace RDK::LLM
