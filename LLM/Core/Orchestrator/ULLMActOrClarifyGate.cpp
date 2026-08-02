#include "ULLMActOrClarifyGate.h"

#include "ULLMChannelCalcCommand.h"
#include "ULLMComponentStructureGoal.h"
#include "ULLMConnectPlanParsing.h"
#include "ULLMTaskPlanParsing.h"
#include "ULLMWatchPlotGoal.h"

#include <unordered_set>

namespace RDK::LLM {

namespace {

bool isInformationalReadToolName(const std::string& tool_name)
{
    static const std::unordered_set<std::string> k = {
        "get_net_snapshot",
        "list_model_links",
        "get_component_ports",
        "find_component",
        "get_component_properties",
        "list_registered_classes",
        "describe_class",
        "validate_project",
        "validate_configuration",
        "list_channels",
        "inspect_configuration",
        "search_configuration_links",
        "search_project_docs",
        "list_project_files",
        "stat_project_file",
        "read_text_artifact",
        "search_tools",
        "list_recent_configurations",
        "list_ui_panels",
        "list_watch_series",
        "list_watch_mdi",
        "spawn_explore_subagent",
    };
    return k.count(tool_name) > 0;
}

} // namespace

bool turnHasSuccessfulReadOnlyEvidence(const std::vector<TurnToolInvocationView>& trace)
{
    if(trace.empty())
        return false;
    bool any_ok_read = false;
    for(const TurnToolInvocationView& inv : trace)
    {
        if(!isInformationalReadToolName(inv.tool_name))
            return false;
        if(inv.ok)
            any_ok_read = true;
    }
    return any_ok_read;
}

bool isActionableGoalForActOrClarify(const std::string& planning_text, LLMIntentKind intent,
                                     ConfigurationLifecycleAction lifecycle_action,
                                     bool filter_include_write)
{
    if(intent == LLMIntentKind::Mutate && filter_include_write)
        return true;
    if(intent == LLMIntentKind::Plan && filter_include_write)
        return true;
    // Query/Explain: force an initial read tool instead of inventing topology/docs.
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
                               bool in_understanding_phase, bool has_turn_tool_evidence,
                               bool has_successful_read_only_evidence)
{
    if(!provider_tools_offered || !tool_calls_empty)
        return false;
    if(has_pending_tool_arguments || in_understanding_phase)
        return false;
    (void)has_turn_tool_evidence;
    // Prose only after successful read-only evidence (chat 17-25-18: failed inspect must not
    // unlock an error essay). Covers Query/Explain and Mutate false-positives alike.
    if(has_successful_read_only_evidence)
        return false;
    return isActionableGoalForActOrClarify(planning_text, intent, lifecycle_action,
                                           filter_include_write);
}

} // namespace RDK::LLM
