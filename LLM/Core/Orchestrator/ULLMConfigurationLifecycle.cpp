#include "ULLMConfigurationLifecycle.h"

#include "../LlmTypes.h"

#include <cctype>
#include <unordered_set>

namespace RDK::LLM {

namespace {

std::string toLower(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool contains(const std::string& hay, const char* needle)
{
    return hay.find(needle) != std::string::npos;
}

} // namespace

ConfigurationLifecycleAction detectConfigurationLifecycleAction(const std::string& user_text)
{
    const std::string lower = toLower(user_text);

    if(contains(lower, "validate") || contains(lower, "валид")
       || (contains(lower, "провер") && contains(lower, "конфиг")))
        return ConfigurationLifecycleAction::Validate;

    if((contains(lower, "create") || contains(lower, "созда") || contains(lower, "new "))
       && (contains(lower, "config") || contains(lower, "configuration") || contains(lower, "конфиг")
           || contains(lower, "конфигурац") || contains(lower, "project")
           || contains(lower, "проект")))
        return ConfigurationLifecycleAction::Create;

    if((contains(lower, "load") || contains(lower, "open") || contains(lower, "открой")
        || contains(lower, "загруз"))
       && (contains(lower, "config") || contains(lower, "configuration") || contains(lower, "конфиг")
           || contains(lower, "конфигурац") || contains(lower, "project")
           || contains(lower, "проект")))
        return ConfigurationLifecycleAction::Load;

    if((contains(lower, "save") || contains(lower, "сохран"))
       && (contains(lower, "config") || contains(lower, "configuration") || contains(lower, "конфиг")
           || contains(lower, "конфигурац") || contains(lower, "project")
           || contains(lower, "проект")))
        return ConfigurationLifecycleAction::Save;

    if((contains(lower, "close") || contains(lower, "закрой"))
       && (contains(lower, "config") || contains(lower, "configuration") || contains(lower, "конфиг")
           || contains(lower, "конфигурац") || contains(lower, "project")
           || contains(lower, "проект")))
        return ConfigurationLifecycleAction::Close;

    return ConfigurationLifecycleAction::None;
}

bool isConfigurationLifecycleToolName(const std::string& tool_name)
{
    static const std::unordered_set<std::string> kNames = {
        "create_configuration",  "load_configuration",     "load_project",
        "save_configuration",    "save_project",         "save_project_metadata",
        "close_configuration",   "validate_configuration", "copy_configuration",
        "rename_configuration",  "reload_configuration_parameters", "update_configuration",
        "list_registered_classes", "describe_class",       "get_net_snapshot",
        "find_component",        "list_components"};
    return kNames.count(tool_name) > 0;
}

std::string configurationLifecycleSystemHint(ConfigurationLifecycleAction action,
                                             bool project_loaded)
{
    std::string hint =
        "Configuration lifecycle mode: use the registered function tools (native tool_calls), "
        "not markdown JSON blocks. ";
    switch(action)
    {
    case ConfigurationLifecycleAction::Create:
        hint += "To create a new configuration on disk (EN: create project/config; RU: создай "
                "проект/конфигурацию), call create_configuration exactly once with "
                "parent_directory when needed, or omit path for autocreate. Do NOT use "
                "add_component for a new on-disk project.";
        break;
    case ConfigurationLifecycleAction::Load:
        hint += "Call load_configuration with configuration_path (folder or project.ini).";
        break;
    case ConfigurationLifecycleAction::Save:
        hint += project_loaded ? "Call save_configuration or save_configuration with path."
                               : "A configuration must be open before save.";
        break;
    case ConfigurationLifecycleAction::Close:
        hint += "Call close_configuration.";
        break;
    case ConfigurationLifecycleAction::Validate:
        hint += "Call validate_configuration with configuration_path.";
        break;
    default:
        break;
    }
    return hint;
}

std::optional<std::string> forcedToolForLifecycle(ConfigurationLifecycleAction action,
                                                  bool project_loaded)
{
    switch(action)
    {
    case ConfigurationLifecycleAction::Create:
        if(!project_loaded)
            return std::string("create_configuration");
        break;
    case ConfigurationLifecycleAction::Load:
        return std::string("load_configuration");
    case ConfigurationLifecycleAction::Validate:
        return std::string("validate_configuration");
    case ConfigurationLifecycleAction::Save:
        if(project_loaded)
            return std::string("save_configuration");
        break;
    case ConfigurationLifecycleAction::Close:
        if(project_loaded)
            return std::string("close_configuration");
        break;
    default:
        break;
    }
    return std::nullopt;
}

bool isLifecycleWriteToolName(const std::string& tool_name)
{
    static const std::unordered_set<std::string> kWrite = {
        "create_configuration", "load_configuration",     "load_project",
        "save_configuration",   "save_project",           "save_project_metadata",
        "close_configuration",  "copy_configuration",     "rename_configuration",
        "update_configuration", "reload_configuration_parameters"};
    return kWrite.count(tool_name) > 0;
}

bool toolInvokeNeedsArgumentClarification(const std::string& tool_name,
                                          const ToolGatewayResult& result)
{
    if(result.ok)
        return false;
    if(result.error_code == "SchemaValidationFailed" || result.error_code == "ARGS_REQUIRED")
        return true;
    if(result.error_code == "PATH_NOT_ALLOWED"
       && (tool_name == "load_configuration" || tool_name == "load_project"
           || tool_name == "validate_configuration"))
        return true;
    return false;
}

std::string formatLifecycleToolUserMessage(const std::string& tool_name,
                                             const ToolGatewayResult& result)
{
    if(!result.ok)
    {
        if(!result.message.empty())
            return result.message;
        return "Operation failed: " + tool_name;
    }
    if(tool_name == "create_configuration")
        return "Configuration create prepared. Review and apply the change in the assistant panel.";
    if(tool_name == "load_configuration" || tool_name == "load_project")
        return "Configuration load prepared. Confirm to apply.";
    if(tool_name == "save_configuration" || tool_name == "save_project")
        return "Save prepared. Confirm to apply.";
    if(tool_name == "validate_configuration")
        return "Validation finished. See tool result in the conversation.";
    return "Operation completed: " + tool_name;
}

} // namespace RDK::LLM
