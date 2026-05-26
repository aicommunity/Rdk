#include "ULLMLifecycleArgumentGate.h"

#include "../Domain/ULLMNameResolution.h"
#include "../Domain/URdkApplicationCommands.h"
#include "../Tools/ULLMToolRegistry.h"
#include "ULLMLibraryScopeHint.h"

#include <cctype>
#include <ctime>
#include <algorithm>
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

bool jsonStringFieldEmpty(const nlohmann::json& args, const char* key)
{
    if(!args.contains(key))
        return true;
    if(!args[key].is_string())
        return false;
    return trim(args[key].get<std::string>()).empty();
}

std::string toLowerAsciiLocal(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool isUnsignedListIndex(const std::string& s)
{
    if(s.empty())
        return false;
    for(char c : s)
    {
        if(!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

bool looksLikeClassIdentifier(const std::string& token)
{
    if(token.empty())
        return false;
    const unsigned char first = static_cast<unsigned char>(token[0]);
    if(!std::isalpha(first))
        return false;
    for(char c : token)
    {
        if(std::isalnum(static_cast<unsigned char>(c)) || c == '_')
            continue;
        return false;
    }
    return true;
}

std::optional<std::string>
resolveClassNameFromDisambiguationListImpl(const std::string& user_text,
                                           const nlohmann::json& candidates)
{
    return pickFromNumberedList(user_text, candidates, "class_name");
}

std::string defaultShortNameFromClass(const std::string& class_name)
{
    if(class_name.empty())
        return "Component1";
    std::string sn = class_name;
    if(!sn.empty() && sn[0] == 'N')
        sn.erase(sn.begin());
    if(sn.empty())
        sn = "Component1";
    return sn;
}

void mergeAddComponentArguments(nlohmann::json& args, const std::string& user_text,
                                const nlohmann::json& class_candidates)
{
    const std::string trimmed = trim(user_text);
    if(trimmed.empty())
        return;

    bool class_set_from_list = false;
    if(class_candidates.is_array() && !class_candidates.empty())
    {
        if(const std::optional<std::string> picked =
               resolveClassNameFromDisambiguationListImpl(trimmed, class_candidates))
        {
            args["class_name"] = *picked;
            args["short_name"] = defaultShortNameFromClass(*picked);
            class_set_from_list = true;
        }
        else if(isUnsignedListIndex(trimmed))
            return;
    }

    const LibraryScopeHint scope = detectLibraryScopeFromUserText(user_text);

    if(!class_set_from_list && trimmed.find_first_of(" \t\n\r") == std::string::npos)
    {
        if(looksLikeClassIdentifier(trimmed))
            args["class_name"] = trimmed;
        else
        {
            const std::string picked = resolveKnownClassAlias(trimmed);
            args["class_name"] = picked;
            args["short_name"] = defaultShortNameFromClass(picked);
        }
    }
    else if(jsonStringFieldEmpty(args, "class_name"))
    {
        std::string class_query = trimmed;
        const size_t last_space = trimmed.find_last_of(" \t\n\r");
        if(last_space != std::string::npos && last_space + 1 < trimmed.size())
            class_query = trim(trimmed.substr(last_space + 1));
        if(looksLikeClassIdentifier(class_query))
            args["class_name"] = class_query;
        else
            args["class_name"] = resolveComponentClassName(class_query, scope);
    }

    if(jsonStringFieldEmpty(args, "parent_long_name"))
        args["parent_long_name"] = "";

    if(jsonStringFieldEmpty(args, "short_name") && args.contains("class_name")
       && args["class_name"].is_string())
        args["short_name"] = defaultShortNameFromClass(args["class_name"].get<std::string>());
    else if(jsonStringFieldEmpty(args, "short_name") && trimmed.find_first_of(" \t\n\r") == std::string::npos)
        args["short_name"] = defaultShortNameFromClass(trimmed);

    if(!args.contains("channel_index"))
        args["channel_index"] = 0;
}

} // namespace

std::optional<std::string>
resolveClassNameFromDisambiguationList(const std::string& user_text,
                                       const nlohmann::json& candidates)
{
    return resolveClassNameFromDisambiguationListImpl(user_text, candidates);
}

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
        return args;
    }

    if(pending.tool_name == "add_component")
    {
        mergeAddComponentArguments(args, user_text, pending.class_disambiguation_candidates);
        return args;
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

    if(tool_name == "add_component")
    {
        prompt += "\nExample: reply `NPulseNeuron` or a list number like `1` (short name and model "
                  "root parent are filled automatically).";
        prompt += "\nOr: `class_name NPulseNeuron short_name Neuron1 parent_long_name \"\"`.";
        prompt += "\nUse list_registered_classes or list_*_component_classes for valid class names.";
    }

    prompt += "\n\nReply with the missing value(s) in your next message.";
    return prompt;
}

bool isGraphAddComponentTool(const std::string& tool_name)
{
    return tool_name == "add_component";
}

std::vector<ToolArgumentFieldSpec> findMissingFieldsFromToolSchema(const LLMToolDefinition& def,
                                                                   const nlohmann::json& args)
{
    std::vector<ToolArgumentFieldSpec> missing;
    if(!def.input_schema.is_object() || !def.input_schema.contains("required"))
        return missing;

    const nlohmann::json& properties =
        def.input_schema.value("properties", nlohmann::json::object());

    for(const nlohmann::json& req : def.input_schema["required"])
    {
        if(!req.is_string())
            continue;
        const std::string key = req.get<std::string>();
        bool is_missing = !args.contains(key);
        if(!is_missing && args[key].is_string() && trim(args[key].get<std::string>()).empty()
           && key != "parent_long_name")
            is_missing = true;
        if(is_missing)
        {
            ToolArgumentFieldSpec spec;
            spec.name = key;
            spec.type = "string";
            spec.required = true;
            if(properties.contains(key) && properties[key].is_object())
                spec.description = properties[key].value("description", key);
            else
                spec.description = key;
            missing.push_back(std::move(spec));
        }
    }
    return missing;
}

std::vector<ToolArgumentFieldSpec> findMissingToolArguments(const std::string& tool_name,
                                                            const nlohmann::json& args,
                                                            const ULLMToolRegistry& registry)
{
    const LLMToolDefinition* def = registry.find(tool_name);
    if(!def)
        return {};
    return findMissingFieldsFromToolSchema(*def, args);
}

std::vector<ToolArgumentFieldSpec> findMissingArgumentsForTool(const std::string& tool_name,
                                                              const nlohmann::json& args,
                                                              RDK::UApplication* app,
                                                              const ULLMToolRegistry& registry)
{
    std::vector<ToolArgumentFieldSpec> missing = findMissingLifecycleFields(tool_name, args, app);
    if(!missing.empty())
        return missing;
    return findMissingToolArguments(tool_name, args, registry);
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
