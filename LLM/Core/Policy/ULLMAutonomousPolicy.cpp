#include "ULLMAutonomousPolicy.h"

#include "ULLMPolicyLimits.h"

#include <unordered_set>

namespace RDK::LLM {

namespace {

const std::unordered_set<std::string>& autonomousReadTools()
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
        "list_channels",
    };
    return k;
}

const std::unordered_set<std::string>& autonomousWriteTools()
{
    static const std::unordered_set<std::string> k = {
        "add_component",
        "connect_components",
        "disconnect_components",
        "set_property",
        "remove_component",
        "clone_component",
        "move_component",
        "rename_component",
        "reorder_component",
        "calculate_component",
        "reset_component",
        "default_component",
        "select_component",
        "start_channel_calculation",
        "pause_channel_calculation",
        "reset_channel_calculation",
        "step_channel_calculation",
        "run_n_steps",
        "set_active_channel",
        "add_channel",
        "clone_channel",
    };
    return k;
}

const std::unordered_set<std::string>& autonomousLifecycleTools()
{
    static const std::unordered_set<std::string> k = {
        "create_configuration",
        "load_configuration",
        "load_project",
        "open_recent_configuration",
        "save_configuration",
        "save_project",
        "save_project_metadata",
        "close_configuration",
        "copy_configuration",
        "rename_configuration",
        "update_configuration",
        "reload_configuration_parameters",
    };
    return k;
}

bool isAutonomousMetaTool(const std::string& tool_name)
{
    // Clarification protocol must work under SemiAuto/Strict (Act-or-Clarify).
    return tool_name == "ask_user" || tool_name == "propose_plan";
}

bool isAutonomousUiOrWatchTool(const std::string& tool_name)
{
    return tool_name == "show_ui_panel" || tool_name == "open_component_gui_tab"
           || tool_name == "list_ui_panels" || tool_name == "add_watch_series"
           || tool_name == "list_watch_series" || tool_name == "remove_watch_series"
           || tool_name == "clear_watch_series" || tool_name == "list_watch_mdi"
           || tool_name == "create_watch_mdi" || tool_name == "focus_watch_mdi"
           || tool_name == "close_watch_mdi";
}

bool isChannelCalcWriteTool(const std::string& tool_name)
{
    return tool_name == "start_channel_calculation" || tool_name == "pause_channel_calculation"
           || tool_name == "reset_channel_calculation" || tool_name == "step_channel_calculation"
           || tool_name == "run_n_steps" || tool_name == "set_active_channel"
           || tool_name == "select_component";
}

} // namespace

bool ULLMAutonomousPolicy::isAutonomousReadTool(const std::string& tool_name)
{
    return autonomousReadTools().count(tool_name) > 0;
}

bool ULLMAutonomousPolicy::isAutonomousWriteTool(const std::string& tool_name)
{
    return autonomousWriteTools().count(tool_name) > 0;
}

bool ULLMAutonomousPolicy::isAutonomousLifecycleTool(const std::string& tool_name)
{
    return autonomousLifecycleTools().count(tool_name) > 0;
}

bool ULLMAutonomousPolicy::isToolWhitelisted(const std::string& tool_name, LLMAutonomousMode mode)
{
    if(mode == LLMAutonomousMode::Off)
        return true;
    return isAutonomousReadTool(tool_name) || isAutonomousWriteTool(tool_name)
           || isAutonomousLifecycleTool(tool_name) || isAutonomousMetaTool(tool_name)
           || isAutonomousUiOrWatchTool(tool_name);
}

AutonomousStepDecision ULLMAutonomousPolicy::checkStep(const std::string& tool_name,
                                                       LLMAutonomousMode mode, int steps_taken,
                                                       int max_steps)
{
    AutonomousStepDecision out;
    if(mode == LLMAutonomousMode::Off)
        return out;

    if(!isToolWhitelisted(tool_name, mode))
    {
        out.allowed = false;
        out.deny_code = "AUTONOMOUS_TOOL_NOT_WHITELISTED";
        out.deny_message = "Tool not allowed in autonomous mode: " + tool_name;
        return out;
    }

    if(isAutonomousReadTool(tool_name) || isAutonomousLifecycleTool(tool_name)
       || isAutonomousMetaTool(tool_name) || isAutonomousUiOrWatchTool(tool_name))
        return out;

    // Channel calc is a single control action — do not burn the write step budget.
    if(isChannelCalcWriteTool(tool_name))
        return out;

    if(steps_taken >= max_steps)
    {
        out.allowed = false;
        out.deny_code = "AUTONOMOUS_STEP_LIMIT";
        out.deny_message = "Autonomous step limit reached (" + std::to_string(max_steps) + ")";
        return out;
    }

    return out;
}

} // namespace RDK::LLM
