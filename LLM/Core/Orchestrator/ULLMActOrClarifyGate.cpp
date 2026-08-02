#include "ULLMActOrClarifyGate.h"

#include "ULLMChannelCalcCommand.h"
#include "ULLMComponentStructureGoal.h"
#include "ULLMConnectPlanParsing.h"
#include "ULLMTaskPlanParsing.h"
#include "ULLMWatchPlotGoal.h"

namespace RDK::LLM {

bool isActionableGoalForActOrClarify(const std::string& planning_text, LLMIntentKind intent,
                                     ConfigurationLifecycleAction lifecycle_action,
                                     bool filter_include_write)
{
    if(intent == LLMIntentKind::Mutate && filter_include_write)
        return true;
    if(intent == LLMIntentKind::Plan && filter_include_write)
        return true;
    // Query/Explain: force tool use (search_tools / explore / docs) instead of prose essays.
    if(intent == LLMIntentKind::Query || intent == LLMIntentKind::Explain)
        return true;
    if(lifecycle_action != ConfigurationLifecycleAction::None)
        return true;
    if(isConnectGoalText(planning_text) || isDisconnectGoalText(planning_text))
        return true;
    if(isAddComponentGoal(planning_text))
        return true;
    if(isChannelCalcGoalText(planning_text))
        return true;
    if(isComponentStructureGoal(planning_text))
        return true;
    if(isWatchPlotGoal(planning_text))
        return true;
    return false;
}

bool shouldRequireActOrClarify(bool provider_tools_offered, bool tool_calls_empty,
                               const std::string& planning_text, LLMIntentKind intent,
                               ConfigurationLifecycleAction lifecycle_action,
                               bool filter_include_write, bool has_pending_tool_arguments,
                               bool in_understanding_phase)
{
    if(!provider_tools_offered || !tool_calls_empty)
        return false;
    if(has_pending_tool_arguments || in_understanding_phase)
        return false;
    return isActionableGoalForActOrClarify(planning_text, intent, lifecycle_action,
                                           filter_include_write);
}

} // namespace RDK::LLM
