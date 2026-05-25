#include "ULLMLifecycleArgumentGate.h"

#include "../Domain/URdkApplicationCommands.h"
#include "../Policy/ULLMPathPolicy.h"

#include <cctype>
#include <ctime>
#include <regex>

namespace RDK::LLM {

namespace {

std::string trim(const std::string& s)
{
    size_t b = 0;
    while(b < s.size() && std::isspace(static_cast<unsigned char>(s[b])))
        ++b;
    size_t e = s.size();
    while(e > b && std::isspace(static_cast<unsigned char>(s[e - 1])))
        --e;
    return s.substr(b, e - b);
}

bool looksLikePathChar(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) || c == '/' || c == '\\' || c == '_'
           || c == '-' || c == '.' || c == ':';
}

} // namespace

std::string extractPathFromUserText(const std::string& user_text)
{
    const std::string text = trim(user_text);
    if(text.empty())
        return {};

    try
    {
        static const std::regex win_re(R"(([A-Za-z]:\\[^\s\"'<>]+))");
        std::smatch m;
        if(std::regex_search(text, m, win_re) && m.size() > 1)
            return trim(m[1].str());

        static const std::regex unix_re(R"((/(?:[A-Za-z0-9._-]+/)*[A-Za-z0-9._-]+))");
        if(std::regex_search(text, m, unix_re) && m.size() > 1)
            return trim(m[1].str());
    }
    catch(...)
    {
    }

    const std::string marker = "project.ini";
    const size_t pos = text.find(marker);
    if(pos != std::string::npos)
    {
        size_t start = pos;
        while(start > 0 && looksLikePathChar(text[start - 1]))
            --start;
        return trim(text.substr(start, pos - start + marker.size()));
    }

    return {};
}

std::string toolNameForLifecycleAction(ConfigurationLifecycleAction action)
{
    switch(action)
    {
    case ConfigurationLifecycleAction::Create:
        return "create_configuration";
    case ConfigurationLifecycleAction::Load:
        return "load_configuration";
    case ConfigurationLifecycleAction::Save:
        return "save_configuration";
    case ConfigurationLifecycleAction::Close:
        return "close_configuration";
    case ConfigurationLifecycleAction::Validate:
        return "validate_configuration";
    default:
        return {};
    }
}

ConfigurationLifecycleAction lifecycleActionFromToolName(const std::string& tool_name)
{
    if(tool_name == "create_configuration")
        return ConfigurationLifecycleAction::Create;
    if(tool_name == "load_configuration" || tool_name == "load_project")
        return ConfigurationLifecycleAction::Load;
    if(tool_name == "save_configuration")
        return ConfigurationLifecycleAction::Save;
    if(tool_name == "close_configuration")
        return ConfigurationLifecycleAction::Close;
    if(tool_name == "validate_configuration")
        return ConfigurationLifecycleAction::Validate;
    return ConfigurationLifecycleAction::None;
}

std::vector<ToolArgumentFieldSpec> argumentFieldsForLifecycle(ConfigurationLifecycleAction action)
{
    std::vector<ToolArgumentFieldSpec> fields;
    switch(action)
    {
    case ConfigurationLifecycleAction::Create:
        fields.push_back({"parent_directory",
                          "string",
                          "Folder where the new configuration directory will be created",
                          true});
        fields.push_back(
            {"project_name", "string", "Optional display name for the new configuration", false});
        break;
    case ConfigurationLifecycleAction::Load:
    case ConfigurationLifecycleAction::Validate:
        fields.push_back({"configuration_path",
                          "string",
                          "Path to the configuration folder or project.ini file",
                          true});
        break;
    case ConfigurationLifecycleAction::Save:
        fields.push_back({"configuration_path",
                          "string",
                          "Optional path for Save As; leave empty to save the open configuration",
                          false});
        break;
    case ConfigurationLifecycleAction::Close:
        break;
    default:
        break;
    }
    return fields;
}

std::vector<ToolArgumentFieldSpec> findMissingLifecycleFields(const std::string& tool_name,
                                                              const nlohmann::json& args,
                                                              RDK::UApplication* app)
{
    std::vector<ToolArgumentFieldSpec> missing;
    if(tool_name == "create_configuration")
    {
        std::string err;
        if(URdkApplicationCommands::resolveProjectIniPath(args, app, err).empty())
            missing.push_back(argumentFieldsForLifecycle(ConfigurationLifecycleAction::Create)[0]);
        return missing;
    }

    if(tool_name == "load_configuration" || tool_name == "load_project"
       || tool_name == "validate_configuration")
    {
        std::string err;
        const std::string path = URdkApplicationCommands::resolveConfigurationPathFromArgs(args, err);
        if(path.empty())
            missing.push_back(
                argumentFieldsForLifecycle(tool_name == "validate_configuration"
                                               ? ConfigurationLifecycleAction::Validate
                                               : ConfigurationLifecycleAction::Load)[0]);
    }
    return missing;
}

nlohmann::json mergeArgumentsFromUserText(const PendingToolArguments& pending,
                                          const std::string& user_text,
                                          RDK::UApplication* app)
{
    (void)app;
    nlohmann::json args = pending.partial_arguments;
    const std::string path = extractPathFromUserText(user_text);
    const std::string trimmed = trim(user_text);

    if(pending.tool_name == "create_configuration")
    {
        if(!path.empty())
            args["parent_directory"] = path;
        else if(!trimmed.empty() && !args.contains("parent_directory"))
            args["parent_directory"] = trimmed;

        if(args.contains("project_name") || trimmed.empty())
            return args;
        if(!path.empty() && trimmed != path)
            args["project_name"] = trimmed;
        return args;
    }

    if(pending.tool_name == "load_configuration" || pending.tool_name == "load_project"
       || pending.tool_name == "validate_configuration")
    {
        if(!path.empty())
            args["configuration_path"] = path;
        else if(!trimmed.empty())
            args["configuration_path"] = trimmed;
        if(!args.contains("if_open_project") && pending.tool_name.find("load") != std::string::npos)
            args["if_open_project"] = "close";
    }

    return args;
}

std::string formatArgumentRequestPrompt(const std::string& tool_name,
                                        ConfigurationLifecycleAction action,
                                        const std::vector<ToolArgumentFieldSpec>& missing,
                                        RDK::UApplication* app)
{
    std::string prompt = "To run **" + tool_name + "**, I need additional information:\n";
    for(const ToolArgumentFieldSpec& f : missing)
        prompt += "- **" + f.name + "**: " + f.description + "\n";

    switch(action)
    {
    case ConfigurationLifecycleAction::Load:
        prompt += "\nExample: `load config from /home/user/MyProject` or paste the folder that "
                  "contains `project.ini`.";
        if(app)
        {
            const std::string root = URdkApplicationCommands::resolveDefaultConfigsRoot(app);
            if(!root.empty())
                prompt += "\nDefault configs folder on this machine: `" + root + "`.";
        }
        break;
    case ConfigurationLifecycleAction::Create:
        prompt += "\nExample: `create config in /tmp/MyProject name DemoCfg`, or say "
                  "`create new config` to autocreate under the default configs folder.";
        if(app)
        {
            const std::string root = URdkApplicationCommands::resolveDefaultConfigsRoot(app);
            if(!root.empty())
                prompt += "\nDefault configs folder: `" + root + "`.";
        }
        break;
    case ConfigurationLifecycleAction::Validate:
        prompt += "\nExample: `validate /path/to/project/project.ini`.";
        break;
    default:
        break;
    }
    prompt += "\n\nReply with the missing value(s) in your next message.";
    return prompt;
}

LifecycleArgumentPreflight preflightLifecycleArguments(ConfigurationLifecycleAction action,
                                                       const std::string& user_text,
                                                       RDK::UApplication* app)
{
    LifecycleArgumentPreflight out;
    out.tool_name = toolNameForLifecycleAction(action);
    if(out.tool_name.empty())
        return out;

    nlohmann::json args = nlohmann::json::object();
    const std::string path = extractPathFromUserText(user_text);

    switch(action)
    {
    case ConfigurationLifecycleAction::Create:
        if(!path.empty())
        {
            args["parent_directory"] = path;
            args["autocreate_subdirectory"] = false;
        }
        else
            args["autocreate_subdirectory"] = true;
        break;
    case ConfigurationLifecycleAction::Load:
        if(!path.empty())
        {
            args["configuration_path"] = path;
            args["if_open_project"] = "close";
        }
        break;
    case ConfigurationLifecycleAction::Validate:
        if(!path.empty())
            args["configuration_path"] = path;
        break;
    case ConfigurationLifecycleAction::Save:
    case ConfigurationLifecycleAction::Close:
        out.ready = true;
        out.arguments = args;
        return out;
    default:
        return out;
    }

    out.missing_fields = findMissingLifecycleFields(out.tool_name, args, app);
    if(out.missing_fields.empty())
    {
        out.ready = true;
        out.arguments = std::move(args);
    }
    else
    {
        out.arguments = std::move(args);
    }
    return out;
}

} // namespace RDK::LLM
