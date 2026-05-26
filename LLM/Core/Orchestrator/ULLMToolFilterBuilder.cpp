#include "ULLMToolFilterBuilder.h"

namespace RDK::LLM {

namespace {

std::unordered_set<std::string> kLibraryAssistTools()
{
    return {"search_pulse_docs",
            "list_pulse_component_classes",
            "search_motion_control_docs",
            "list_motion_control_component_classes",
            "search_hardware_docs",
            "list_hardware_component_classes"};
}

std::unordered_set<std::string> kQueryTools()
{
    auto tools = kLibraryAssistTools();
    tools.insert({"get_net_snapshot",
                  "list_registered_classes",
                  "describe_class",
                  "find_component",
                  "get_component_properties",
                  "list_recent_configurations",
                  "list_channels",
                  "list_ui_panels",
                  "search_project_docs",
                  "validate_project",
                  "validate_configuration"});
    return tools;
}

std::unordered_set<std::string> kMutateTools()
{
    return {"add_component",
            "set_property",
            "remove_component",
            "connect_components",
            "disconnect_components",
            "create_configuration",
            "load_configuration",
            "load_project",
            "open_recent_configuration",
            "set_active_channel",
            "save_configuration",
            "save_project",
            "close_configuration",
            "update_configuration",
            "copy_configuration",
            "rename_configuration",
            "reload_configuration_parameters",
            "save_project_metadata",
            "show_ui_panel",
            "open_component_gui_tab",
            "start_channel_calculation",
            "pause_channel_calculation",
            "reset_channel_calculation",
            "step_channel_calculation"};
}

} // namespace

ToolFilter buildToolFilter(const LLMIntentKind intent, const bool write_enabled,
                           const ConfigurationLifecycleAction lifecycle_action)
{
    ToolFilter filter;
    filter.intent = intent;
    filter.include_write = (intent == LLMIntentKind::Mutate) && write_enabled;
    if(intent == LLMIntentKind::Query || intent == LLMIntentKind::Explain
       || intent == LLMIntentKind::Plan)
    {
        filter.include_write = false;
        filter.allowed_tool_names = kQueryTools();
    }
    else if(filter.include_write)
    {
        std::unordered_set<std::string> allowed = kQueryTools();
        for(const std::string& tool : kMutateTools())
            allowed.insert(tool);
        filter.allowed_tool_names = std::move(allowed);
    }
    (void)lifecycle_action;
    return filter;
}

} // namespace RDK::LLM
