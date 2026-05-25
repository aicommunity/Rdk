#include "RegisterApplicationTools.h"

#include "../Domain/URdkApplicationCommands.h"
#include "../LlmPublicApi.h"
#include "ApplicationToolHelpers.h"
#include "ULLMToolRegistry.h"

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
                                            {{"required", nlohmann::json::array({"project_ini_path"})}}})},
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
                   {{"type", "object"}, {"additionalProperties", true}}, true, true),
        [](const nlohmann::json& args) -> ToolGatewayResult {
            return invokeApplicationTool(activeSink(),
                                         [&]() { return commands().updateConfiguration(args); });
        });

    registry.registerTool(
        makeAppDef("validate_configuration", LLMToolKind::Read,
                   "Validate a configuration file on disk without opening it in the UI",
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
}

} // namespace RDK::LLM
