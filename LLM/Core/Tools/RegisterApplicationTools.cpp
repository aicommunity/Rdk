#include "RegisterApplicationTools.h"

#include "../Domain/URdkApplicationCommands.h"
#include "../Domain/URdkDomainAccess.h"
#include "../Gui/ILLMPresentationSink.h"
#include "../LlmPublicApi.h"
#include "ApplicationToolHelpers.h"
#include "ULLMToolRegistry.h"

#include <unordered_map>

namespace RDK::LLM {

namespace {

LLMToolDefinition makeAppDef(const std::string& name, LLMToolKind kind, const std::string& desc,
                             nlohmann::json input, bool confirm, bool requires_project_loaded)
{
    LLMToolDefinition d;
    d.name = name;
    d.kind = kind;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = {{"type", "object"}};
    d.requires_confirmation = confirm;
    d.requires_project_loaded = requires_project_loaded;
    return d;
}

ILLMPresentationSink* activeSink()
{
    return LLMServices::instance().presentationSink();
}

URdkApplicationCommands& commands()
{
    return LLMServices::instance().applicationCommands();
}

RDK::LLM::LLMUiPanel panelFromId(const std::string& id)
{
    using R = RDK::LLM::LLMUiPanel;
    if(id == "components_list")
        return R::ComponentsList;
    if(id == "channels")
        return R::Channels;
    if(id == "logger")
        return R::Logger;
    if(id == "watch")
        return R::Watch;
    if(id == "images")
        return R::Images;
    if(id == "project_description")
        return R::ProjectDescription;
    if(id == "profiling")
        return R::Profiling;
    if(id == "diagram")
        return R::Diagram;
    if(id == "component_gui_tab_host")
        return R::ComponentGuiTabHost;
    return R::None;
}

nlohmann::json listUiPanelsMerged(ILLMPresentationSink* sink)
{
    struct CatalogItem {
        const char* id;
        const char* title;
        RDK::LLM::LLMUiPanel panel;
    };
    static const CatalogItem kCatalog[] = {
        {"components_list", "Components", RDK::LLM::LLMUiPanel::ComponentsList},
        {"channels", "Channels", RDK::LLM::LLMUiPanel::Channels},
        {"logger", "Logger", RDK::LLM::LLMUiPanel::Logger},
        {"watch", "Watch", RDK::LLM::LLMUiPanel::Watch},
        {"images", "Images", RDK::LLM::LLMUiPanel::Images},
        {"project_description", "Project Description",
         RDK::LLM::LLMUiPanel::ProjectDescription},
        {"profiling", "Profiling", RDK::LLM::LLMUiPanel::Profiling},
        {"diagram", "Diagram", RDK::LLM::LLMUiPanel::Diagram},
        {"component_gui_tab_host", "Component GUI Tab Host",
         RDK::LLM::LLMUiPanel::ComponentGuiTabHost}};

    std::unordered_map<std::string, bool> visible_by_id;
    if(sink)
    {
        const nlohmann::json host = sink->listLlmUiPanelsState();
        for(const nlohmann::json& item : host.value("items", nlohmann::json::array()))
        {
            if(item.contains("id") && item.contains("visible") && item["id"].is_string()
               && item["visible"].is_boolean())
                visible_by_id[item["id"].get<std::string>()] = item["visible"].get<bool>();
        }
    }

    nlohmann::json out;
    out["items"] = nlohmann::json::array();
    for(const CatalogItem& item : kCatalog)
    {
        const bool visible =
            visible_by_id.count(item.id) ? visible_by_id[item.id] : false;
        out["items"].push_back({{"id", item.id}, {"title", item.title}, {"visible", visible}});
    }
    return out;
}

} // namespace

void RegisterApplicationTools(ULLMToolRegistry& registry)
{
    registry.registerTool(
        makeAppDef("create_configuration", LLMToolKind::Write,
                   "Create a new configuration (project) on disk and open it",
                   {{"type", "object"},
                    {"properties",
                     {{"parent_directory", {{"type", "string"}}},
                      {"autocreate_subdirectory", {{"type", "boolean"}, {"default", true}}},
                      {"project_ini_path", {{"type", "string"}}},
                      {"project_name", {{"type", "string"}}},
                      {"project_description", {{"type", "string"}}},
                      {"project_type", {{"type", "integer"}}},
                      {"num_channels", {{"type", "integer"}, {"minimum", 1}, {"maximum", 32}}},
                      {"channels",
                       {{"type", "array"},
                        {"items",
                         {{"type", "object"},
                          {"properties",
                           {{"class_name", {{"type", "string"}}},
                            {"model_mode", {{"type", "integer"}}},
                            {"calculation_mode", {{"type", "integer"}}},
                            {"global_time_step", {{"type", "integer"}}},
                            {"default_time_step", {{"type", "integer"}}},
                            {"init_after_load", {{"type", "boolean"}}},
                            {"reset_after_load", {{"type", "boolean"}}},
                            {"predefined_structure", {{"type", "integer"}}}}},
                          {"additionalProperties", false}}}}},
                      {"if_open_project",
                       {{"type", "string"},
                        {"enum", nlohmann::json::array({"deny", "close", "save_and_close"})},
                        {"default", "deny"}}},
                      {"add_to_recent", {{"type", "boolean"}, {"default", true}}}}},
                    {"anyOf",
                     nlohmann::json::array({{{"required", nlohmann::json::array({"parent_directory"})}},
                                            {{"required", nlohmann::json::array({"project_ini_path"})}},
                                            {{"required",
                                              nlohmann::json::array({"autocreate_subdirectory"})}}})},
                    {"additionalProperties", false}},
                   true, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().createConfiguration(args); });
        });

    auto registerLoad = [&](const std::string& tool_name, const std::string& desc) {
        registry.registerTool(
            makeAppDef(tool_name, LLMToolKind::Write, desc,
                       {{"type", "object"},
                        {"properties",
                         {{"configuration_path", {{"type", "string"}}},
                          {"project_path", {{"type", "string"}}},
                          {"if_open_project",
                           {{"type", "string"},
                            {"enum", nlohmann::json::array({"deny", "close", "save_and_close"})},
                            {"default", "close"}}}}},
                        {"additionalProperties", false}},
                       true, false),
            [tool_name](const nlohmann::json& args) -> ToolGatewayResult {
                (void)tool_name;
                std::string err;
                const std::string path =
                    URdkApplicationCommands::resolveConfigurationPathFromArgs(args, err);
                if(path.empty())
                {
                    ToolGatewayResult r;
                    r.ok = false;
                    r.error_code = "DomainError";
                    r.message = err;
                    return r;
                }
                const std::string if_open = args.value("if_open_project", std::string("close"));
                return invokeApplicationTool(activeSink(), [&]() {
                    return commands().loadConfiguration(path, if_open);
                });
            });
    };

    registerLoad("load_configuration", "Open a configuration file (replaces current configuration)");
    registerLoad("load_project",
                 "Deprecated alias for load_configuration — open a project/configuration file");

    registry.registerTool(
        makeAppDef("save_configuration", LLMToolKind::Write, "Save the current configuration",
                   {{"type", "object"}, {"additionalProperties", false}}, true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().saveConfiguration(); });
        });

    auto registerSave = [&](const std::string& tool_name, const std::string& desc) {
        registry.registerTool(
            makeAppDef(tool_name, LLMToolKind::Write, desc,
                       {{"type", "object"},
                        {"properties",
                         {{"configuration_path", {{"type", "string"}}},
                          {"project_path", {{"type", "string"}}}}},
                        {"additionalProperties", false}},
                       true, true),
            [](const nlohmann::json& args) -> ToolGatewayResult {
                std::string err;
                const std::string path =
                    URdkApplicationCommands::resolveConfigurationPathFromArgs(args, err);
                return invokeApplicationTool(activeSink(), [&]() {
                    if(path.empty())
                        return commands().saveConfiguration();
                    return commands().saveConfigurationAs(path);
                });
            });
    };

    registerSave("save_configuration_as", "Save the current configuration to a new path");
    registerSave("save_project", "Deprecated alias for save_configuration / save_configuration_as");

    registry.registerTool(
        makeAppDef("close_configuration", LLMToolKind::Write, "Close the current configuration",
                   {{"type", "object"}, {"additionalProperties", false}}, true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().closeConfiguration(); });
        });

    registry.registerTool(
        makeAppDef("update_configuration", LLMToolKind::Write,
                   "Update metadata/settings of the open configuration",
                   {{"type", "object"},
                    {"properties",
                     {{"project_name", {{"type", "string"}}},
                      {"project_description", {{"type", "string"}}},
                      {"description_file_name", {{"type", "string"}}},
                      {"project_type", {{"type", "integer"}}},
                      {"project_mode", {{"type", "integer"}}},
                      {"multi_threading_mode", {{"type", "integer"}}},
                      {"calc_source_time_mode", {{"type", "integer"}}},
                      {"events_log_flag", {{"type", "boolean"}}},
                      {"debug_mode", {{"type", "boolean"}}},
                      {"num_channels", {{"type", "integer"}, {"minimum", 1}, {"maximum", 32}}},
                      {"channels",
                       {{"type", "array"},
                        {"items",
                         {{"type", "object"},
                          {"properties",
                           {{"class_name", {{"type", "string"}}},
                            {"model_mode", {{"type", "integer"}}},
                            {"calculation_mode", {{"type", "integer"}}},
                            {"global_time_step", {{"type", "integer"}}},
                            {"default_time_step", {{"type", "integer"}}},
                            {"init_after_load", {{"type", "boolean"}}},
                            {"reset_after_load", {{"type", "boolean"}}},
                            {"predefined_structure", {{"type", "integer"}}}}},
                          {"additionalProperties", false}}}}}}},
                    {"additionalProperties", false}},
                   true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().updateConfiguration(args); });
        });

    registry.registerTool(
        makeAppDef("validate_configuration", LLMToolKind::Read,
                   "Use when validating a configuration directory/file on disk without opening it. "
                   "Requires configuration_path (or resolvable path args). "
                   "Do not use for the already-open project — use validate_project.",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"configuration_path"})},
                    {"properties",
                     {{"configuration_path", {{"type", "string"}}},
                      {"project_path", {{"type", "string"}}}}},
                    {"additionalProperties", false}},
                   false, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            std::string err;
            const std::string path =
                URdkApplicationCommands::resolveConfigurationPathFromArgs(args, err);
            if(path.empty())
            {
                ToolGatewayResult r;
                r.ok = false;
                r.error_code = "DomainError";
                r.message = err;
                return r;
            }
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().validateConfigurationFile(path); });
        });

    registry.registerTool(
        makeAppDef("list_recent_configurations", LLMToolKind::Read,
                   "List recent configurations (deduped). Index is 1-based.",
                   {{"type", "object"}, {"additionalProperties", false}}, false, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(), [&]() {
                ApplicationCommandResult r;
                r.status = {};
                r.payload = commands().listRecentConfigurations();
                r.presentation = LLMPresentationEffect::None;
                return r;
            });
        });

    const nlohmann::json openRecentInputSchema = []() {
        nlohmann::json schema;
        schema["type"] = "object";
        schema["additionalProperties"] = false;
        schema["properties"] = {
            {"index", {{"type", "integer"}, {"minimum", 1}}},
            {"configuration_path", {{"type", "string"}}},
            {"if_open_project",
             {{"type", "string"},
              {"enum", nlohmann::json::array({"deny", "close", "save_and_close"})},
              {"default", "close"}}}};
        schema["anyOf"] = nlohmann::json::array({
            nlohmann::json{{"required", nlohmann::json::array({"index"})}},
            nlohmann::json{{"required",
                            nlohmann::json::array({"configuration_path"})}}
        });
        return schema;
    }();

    registry.registerTool(
        makeAppDef("open_recent_configuration", LLMToolKind::Write,
                   "Open a recent configuration by 1-based index or by path",
                   openRecentInputSchema, true, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            const std::string if_open = args.value("if_open_project", std::string("close"));
            if(args.contains("index"))
            {
                const int idx = args.at("index").get<int>();
                return invokeApplicationTool(activeSink(),
                                             [&]() { return commands().openRecentConfiguration(idx, if_open); });
            }
            if(args.contains("configuration_path") && args["configuration_path"].is_string()
               && !args["configuration_path"].get<std::string>().empty())
            {
                const std::string path = args["configuration_path"].get<std::string>();
                return invokeApplicationTool(
                    activeSink(),
                    [&]() { return commands().openRecentConfigurationByPath(path, if_open); });
            }
            ToolGatewayResult r;
            r.ok = false;
            r.error_code = "ARGS_REQUIRED";
            r.message =
                "open_recent_configuration requires index (1 = most recent) or configuration_path.";
            return r;
        });

    registry.registerTool(
        makeAppDef("list_ui_panels", LLMToolKind::Read,
                   "List available LLM UI panels and their current visibility",
                   {{"type", "object"}, {"additionalProperties", false}}, false, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(),
                                         [&]() {
                                             ApplicationCommandResult r;
                                             r.status = {};
                                             r.payload = listUiPanelsMerged(activeSink());
                                             r.presentation = LLMPresentationEffect::None;
                                             return r;
                                         });
        });

    const nlohmann::json showUiPanelInputSchema = {
        {"type", "object"},
        {"properties",
         {{"panel",
           {{"type", "string"},
            {"enum",
             nlohmann::json::array({"components_list",
                                    "channels",
                                    "logger",
                                    "watch",
                                    "images",
                                    "project_description",
                                    "profiling",
                                    "diagram",
                                    "component_gui_tab_host"})}}}}},
        {"required", nlohmann::json::array({"panel"})},
        {"additionalProperties", false}};

    registry.registerTool(
        makeAppDef("show_ui_panel", LLMToolKind::Write,
                   "Show a specific LLM UI panel (e.g. logger, components list)",
                   showUiPanelInputSchema, false, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            const std::string panel_id = args.at("panel").get<std::string>();
            const RDK::LLM::LLMUiPanel panel = panelFromId(panel_id);
            if(panel == RDK::LLM::LLMUiPanel::None)
            {
                ToolGatewayResult r;
                r.ok = false;
                r.error_code = "DomainError";
                r.message = "Unknown panel id: " + panel_id;
                return r;
            }

            return invokeApplicationTool(activeSink(), [&]() {
                ApplicationCommandResult r;
                r.status = {};
                r.presentation = LLMPresentationEffect::None;
                r.show_panel = panel;
                r.show_panel_visible = true;
                return r;
            });
        });

    registry.registerTool(
        makeAppDef("open_component_gui_tab", LLMToolKind::Write,
                   "Open component GUI tab host (may require user interaction)",
                   {{"type", "object"}, {"additionalProperties", false}}, true, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(), [&]() {
                ApplicationCommandResult r;
                r.status = {};
                r.presentation = LLMPresentationEffect::None;
                r.show_panel = RDK::LLM::LLMUiPanel::ComponentGuiTabHost;
                r.show_panel_visible = true;
                return r;
            });
        });

    registry.registerTool(
        makeAppDef("save_project_metadata", LLMToolKind::Write,
                   "Save only project.ini metadata for the open configuration",
                   {{"type", "object"}, {"additionalProperties", false}}, true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().saveConfigurationMetadata(); });
        });

    registry.registerTool(
        makeAppDef("reload_configuration_parameters", LLMToolKind::Write,
                   "Reload parameters.xml for the active channel from disk",
                   {{"type", "object"}, {"additionalProperties", false}}, true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(), [&]() {
                return commands().reloadConfigurationParameters();
            });
        });

    registry.registerTool(
        makeAppDef("copy_configuration", LLMToolKind::Write, "Copy the open configuration directory",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"destination_directory"})},
                    {"properties",
                     {{"destination_directory", {{"type", "string"}}},
                      {"switch_to_copy", {{"type", "boolean"}, {"default", false}}}}},
                    {"additionalProperties", false}},
                   true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            const std::string dest = args.at("destination_directory").get<std::string>();
            const bool sw = args.value("switch_to_copy", false);
            return invokeApplicationTool(activeSink(), [&]() {
                return commands().copyConfiguration(dest, sw);
            });
        });

    registry.registerTool(
        makeAppDef("rename_configuration", LLMToolKind::Write,
                   "Rename the configuration directory of the open project",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"new_directory_path"})},
                    {"properties", {{"new_directory_path", {{"type", "string"}}}}},
                    {"additionalProperties", false}},
                   true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            return invokeApplicationTool(activeSink(), [&]() {
                return commands().renameConfiguration(
                    args.at("new_directory_path").get<std::string>());
            });
        });

    auto registerChannel = [&](const std::string& name, const std::string& desc,
                               auto fn) {
        registry.registerTool(
            makeAppDef(name, LLMToolKind::Write, desc,
                       {{"type", "object"},
                        {"properties",
                         {{"channel_index",
                           {{"type", "integer"}, {"minimum", -1}, {"default", -1}}}}},
                        {"additionalProperties", false}},
                       true, true),
            [fn](const nlohmann::json& args) -> ToolGatewayResult {
                const int ch = args.value("channel_index", -1);
                return invokeApplicationTool(activeSink(), [&]() { return fn(ch); });
            });
    };

    registerChannel("start_channel_calculation", "Start channel calculation",
                    [](int ch) { return commands().startChannelCalculation(ch); });
    registerChannel("pause_channel_calculation", "Pause channel calculation",
                    [](int ch) { return commands().pauseChannelCalculation(ch); });
    registerChannel("reset_channel_calculation", "Reset channel calculation",
                    [](int ch) { return commands().resetChannelCalculation(ch); });
    registerChannel("step_channel_calculation", "Single step channel calculation",
                    [](int ch) { return commands().stepChannelCalculation(ch); });

    registry.registerTool(
        makeAppDef("run_n_steps", LLMToolKind::Write,
                   "Run N calculation steps (Calculate → Run N Steps). channel_index=-1 means all",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"steps"})},
                    {"properties",
                     {{"steps", {{"type", "integer"}, {"minimum", 1}, {"maximum", 10000}}},
                      {"channel_index",
                       {{"type", "integer"}, {"minimum", -1}, {"default", -1}}}}},
                    {"additionalProperties", false}},
                   true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            const int steps = args.at("steps").get<int>();
            const int ch = args.value("channel_index", -1);
            return invokeApplicationTool(activeSink(), [&]() {
                return commands().runNStepsChannelCalculation(ch, steps);
            });
        });

    registry.registerTool(
        makeAppDef("add_channel", LLMToolKind::Write, "Append a new calculation channel",
                   {{"type", "object"}, {"additionalProperties", false}}, true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(), [&]() { return commands().addChannel(); });
        });

    registry.registerTool(
        makeAppDef("delete_channel", LLMToolKind::Write,
                   "Delete a calculation channel (channel 0 cannot be deleted)",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"channel_index"})},
                    {"properties",
                     {{"channel_index", {{"type", "integer"}, {"minimum", 1}}}}},
                    {"additionalProperties", false}},
                   true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            return invokeApplicationTool(activeSink(), [&]() {
                return commands().deleteChannel(args.at("channel_index").get<int>());
            });
        });

    registry.registerTool(
        makeAppDef("clone_channel", LLMToolKind::Write,
                   "Clone a calculation channel into a new channel at the end",
                   {{"type", "object"},
                    {"properties",
                     {{"source_channel_index",
                       {{"type", "integer"}, {"minimum", 0}, {"default", 0}}}}},
                    {"additionalProperties", false}},
                   true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            const int src = args.value("source_channel_index", 0);
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().cloneChannel(src); });
        });

    registry.registerTool(
        makeAppDef("list_channels", LLMToolKind::Read, "List calculation channels and selected index",
                   {{"type", "object"}, {"additionalProperties", false}}, false, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            ToolGatewayResult r;
            r.ok = true;
            r.result = commands().listChannels();
            return r;
        });

    registry.registerTool(
        makeAppDef("set_active_channel", LLMToolKind::Write,
                   "Select active calculation channel for GUI and engine context",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"channel_index"})},
                    {"properties",
                     {{"channel_index", {{"type", "integer"}, {"minimum", 0}}}}},
                    {"additionalProperties", false}},
                   false, false),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            const int ch = args.at("channel_index").get<int>();
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().setActiveChannel(ch); });
        });

    const nlohmann::json watchSurfaceProps = {
        {"surface", {{"type", "string"}, {"enum", nlohmann::json::array({"window", "mdi"})},
                     {"default", "window"}}},
        {"mdi_id", {{"type", "integer"}, {"default", -1}}},
        {"tab_index", {{"type", "integer"}, {"minimum", 0}, {"default", 0}}},
        {"chart_index", {{"type", "integer"}, {"default", 0}}}};

    auto parseWatchArgs = [](const nlohmann::json& args) {
        RDK::LLM::LLMWatchSeriesArgs a;
        a.surface = args.value("surface", std::string("window"));
        a.mdi_id = args.value("mdi_id", -1);
        a.tab_index = args.value("tab_index", 0);
        a.chart_index = args.value("chart_index", 0);
        a.channel_index = args.value("channel_index", 0);
        a.long_name = args.value("long_name", std::string());
        a.property_name = args.value("property_name", std::string());
        a.jx = args.value("jx", 0);
        a.jy = args.value("jy", 0);
        a.serie_index = args.value("serie_index", -1);
        return a;
    };

    auto watchPayloadToCommand = [](const nlohmann::json& payload, bool show_watch) {
        ApplicationCommandResult r;
        r.payload = payload;
        if(!payload.value("ok", false))
        {
            r.status.code = DomainStatusCode::InvalidPropertyValue;
            r.status.message = payload.value("error", "Watch command failed");
            return r;
        }
        if(show_watch)
        {
            r.show_panel = LLMUiPanel::Watch;
            r.show_panel_visible = true;
        }
        return r;
    };

    registry.registerTool(
        makeAppDef("add_watch_series", LLMToolKind::Write,
                   "Add a property series to Watch window or MDI Watches_N chart",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"long_name", "property_name"})},
                    {"properties",
                     [&]() {
                         nlohmann::json props = watchSurfaceProps;
                         props["long_name"] = {{"type", "string"}};
                         props["property_name"] = {{"type", "string"}};
                         props["jx"] = {{"type", "integer"}, {"default", 0}};
                         props["jy"] = {{"type", "integer"}, {"default", 0}};
                         props["channel_index"] = {{"type", "integer"}, {"default", 0}};
                         return props;
                     }()},
                    {"additionalProperties", false}},
                   false, true),
        [parseWatchArgs, watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            const auto wa = parseWatchArgs(args);
            // Domain validate before Qt createSerie (DD-WATCH-001).
            if(LLMServices::instance().isInitialized())
            {
                const DomainStatus vst =
                    LLMServices::instance().domain().validateWatchProperty(
                        wa.long_name, wa.property_name, wa.channel_index, wa.jx, wa.jy);
                if(!vst.ok())
                {
                    ToolGatewayResult err;
                    err.ok = false;
                    err.error_code = "DomainError";
                    err.message = vst.message;
                    err.result = {{"ok", false},
                                  {"error", vst.message},
                                  {"status_code", static_cast<int>(vst.code)},
                                  {"long_name", wa.long_name},
                                  {"property_name", wa.property_name}};
                    return err;
                }
            }
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchAddSeries(wa)
                         : nlohmann::json{{"ok", false}, {"error", "Watch host unavailable"}};
                return watchPayloadToCommand(payload, wa.surface == "window");
            });
        });

    registry.registerTool(
        makeAppDef("list_watch_series", LLMToolKind::Read, "List Watch series on window or MDI",
                   {{"type", "object"},
                    {"properties", watchSurfaceProps},
                    {"additionalProperties", false}},
                   false, true),
        [parseWatchArgs, watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            const auto wa = parseWatchArgs(args);
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchListSeries(wa)
                         : nlohmann::json{{"ok", false},
                                          {"error", "Watch host unavailable"},
                                          {"items", nlohmann::json::array()}};
                return watchPayloadToCommand(payload, false);
            });
        });

    registry.registerTool(
        makeAppDef("remove_watch_series", LLMToolKind::Write,
                   "Remove a Watch series by index or long_name+property_name",
                   {{"type", "object"},
                    {"properties",
                     [&]() {
                         nlohmann::json props = watchSurfaceProps;
                         props["serie_index"] = {{"type", "integer"}, {"default", -1}};
                         props["long_name"] = {{"type", "string"}};
                         props["property_name"] = {{"type", "string"}};
                         return props;
                     }()},
                    {"additionalProperties", false}},
                   false, true),
        [parseWatchArgs, watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            const auto wa = parseWatchArgs(args);
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchRemoveSeries(wa)
                         : nlohmann::json{{"ok", false}, {"error", "Watch host unavailable"}};
                return watchPayloadToCommand(payload, false);
            });
        });

    registry.registerTool(
        makeAppDef("clear_watch_series", LLMToolKind::Write, "Clear Watch series on a chart/surface",
                   {{"type", "object"},
                    {"properties", watchSurfaceProps},
                    {"additionalProperties", false}},
                   false, true),
        [parseWatchArgs, watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            const auto wa = parseWatchArgs(args);
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchClearSeries(wa)
                         : nlohmann::json{{"ok", false}, {"error", "Watch host unavailable"}};
                return watchPayloadToCommand(payload, false);
            });
        });

    registry.registerTool(
        makeAppDef("list_watch_mdi", LLMToolKind::Read, "List MDI Watches_N windows",
                   {{"type", "object"}, {"additionalProperties", false}}, false, true),
        [watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            (void)args;
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchMdiList()
                         : nlohmann::json{{"ok", true}, {"items", nlohmann::json::array()}};
                return watchPayloadToCommand(payload, false);
            });
        });

    registry.registerTool(
        makeAppDef("create_watch_mdi", LLMToolKind::Write,
                   "Create a new MDI Watches_N window with optional grid layout",
                   {{"type", "object"},
                    {"properties",
                     {{"grid_rows", {{"type", "integer"}, {"minimum", 1}, {"default", 1}}},
                      {"grid_cols", {{"type", "integer"}, {"minimum", 1}, {"default", 1}}},
                      {"title", {{"type", "string"}}}}},
                    {"additionalProperties", false}},
                   false, true),
        [watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            const int rows = args.value("grid_rows", 1);
            const int cols = args.value("grid_cols", 1);
            const std::string title = args.value("title", std::string());
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchMdiCreate(rows, cols, title)
                         : nlohmann::json{{"ok", false}, {"error", "Watch host unavailable"}};
                return watchPayloadToCommand(payload, false);
            });
        });

    registry.registerTool(
        makeAppDef("focus_watch_mdi", LLMToolKind::Write, "Focus an existing Watches_N MDI window",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"mdi_id"})},
                    {"properties", {{"mdi_id", {{"type", "integer"}}}}},
                    {"additionalProperties", false}},
                   false, true),
        [watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            const int mdi_id = args.at("mdi_id").get<int>();
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchMdiFocus(mdi_id)
                         : nlohmann::json{{"ok", false}, {"error", "Watch host unavailable"}};
                return watchPayloadToCommand(payload, false);
            });
        });

    registry.registerTool(
        makeAppDef("close_watch_mdi", LLMToolKind::Write, "Close an existing Watches_N MDI window",
                   {{"type", "object"},
                    {"required", nlohmann::json::array({"mdi_id"})},
                    {"properties", {{"mdi_id", {{"type", "integer"}}}}},
                    {"additionalProperties", false}},
                   false, true),
        [watchPayloadToCommand](const nlohmann::json& args) -> ToolGatewayResult {
            const int mdi_id = args.at("mdi_id").get<int>();
            return invokeApplicationTool(activeSink(), [&]() {
                ILLMPresentationSink* sink = activeSink();
                nlohmann::json payload =
                    sink ? sink->watchMdiClose(mdi_id)
                         : nlohmann::json{{"ok", false}, {"error", "Watch host unavailable"}};
                return watchPayloadToCommand(payload, false);
            });
        });
}

} // namespace RDK::LLM
