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
            "list_hardware_component_classes",
            "search_basic_docs",
            "list_basic_component_classes",
            "search_cvbasic_docs",
            "list_cvbasic_component_classes"};
}

std::unordered_set<std::string> kQueryTools()
{
    auto tools = kLibraryAssistTools();
    tools.insert({"get_net_snapshot",
                  "list_model_links",
                  "get_component_ports",
                  "list_registered_classes",
                  "describe_class",
                  "find_component",
                  "get_component_properties",
                  "list_recent_configurations",
                  "list_channels",
                  "list_ui_panels",
                  "list_watch_series",
                  "list_watch_mdi",
                  "search_project_docs",
                  "inspect_configuration",
                  "search_configuration_links",
                  "list_project_files",
                  "read_text_artifact",
                  "stat_project_file",
                  "validate_project",
                  "validate_configuration",
                  "get_system_log_policy",
                  "read_system_log"});
    return tools;
}

std::unordered_set<std::string> kMutateTools()
{
    return {"add_component",
            "set_property",
            "remove_component",
            "connect_components",
            "disconnect_components",
            "clone_component",
            "move_component",
            "rename_component",
            "reorder_component",
            "export_component",
            "import_component",
            "calculate_component",
            "reset_component",
            "default_component",
            "select_component",
            "create_configuration",
            "load_configuration",
            "load_project",
            "open_recent_configuration",
            "set_active_channel",
            "save_configuration",
            "save_configuration_as",
            "save_project",
            "close_configuration",
            "update_configuration",
            "copy_configuration",
            "rename_configuration",
            "reload_configuration_parameters",
            "save_project_metadata",
            "show_ui_panel",
            "open_component_gui_tab",
            "add_watch_series",
            "remove_watch_series",
            "clear_watch_series",
            "create_watch_mdi",
            "focus_watch_mdi",
            "close_watch_mdi",
            "start_channel_calculation",
            "pause_channel_calculation",
            "reset_channel_calculation",
            "step_channel_calculation",
            "run_n_steps",
            "add_channel",
            "delete_channel",
            "clone_channel"};
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
