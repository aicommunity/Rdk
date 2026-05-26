#include "ULLMWriteArgumentNormalizer.h"

#include "URdkEntityResolver.h"

#include <unordered_map>

namespace RDK::LLM {

namespace {

const std::unordered_map<std::string, std::vector<std::string>>& entityFieldsByTool()
{
    static const std::unordered_map<std::string, std::vector<std::string>> kMap = {
        {"set_property", {"long_name"}},
        {"remove_component", {"long_name"}},
        {"get_component_properties", {"long_name"}},
        {"connect_components", {"from_long_name", "to_long_name"}},
        {"disconnect_components", {"from_long_name", "to_long_name"}},
        {"add_component", {"parent_long_name"}},
    };
    return kMap;
}

bool resolveField(const std::string& tool_name, const std::string& field,
                  nlohmann::json& arguments, URdkDomainAccess& domain, int channel_index,
                  WriteArgumentNormalizeResult& out)
{
    if(!arguments.contains(field) || !arguments[field].is_string())
        return true;

    const std::string value = arguments[field].get<std::string>();
    if(value.empty())
        return true;

    nlohmann::json found;
    if(domain.findComponentByLongName(value, found, channel_index).ok())
        return true;

    URdkEntityResolver resolver(domain);
    const EntityResolutionResult resolved = resolver.resolveComponent(value, channel_index);
    (void)tool_name;

    if(resolved.status == EntityResolutionStatus::Resolved)
    {
        arguments[field] = resolved.canonical_long_name;
        return true;
    }

    if(resolved.status == EntityResolutionStatus::Ambiguous)
    {
        out.ok = false;
        out.needs_clarification = true;
        out.error_code = "ENTITY_AMBIGUOUS";
        out.message = "Ambiguous component reference for " + field;
        out.clarification = resolver.toToolJson(resolved);
        out.clarification["field"] = field;
        out.clarification["query"] = value;
        return false;
    }

    out.ok = false;
    out.error_code = "ENTITY_NOT_FOUND";
    out.message = "Component not found for " + field + ": " + value;
    out.clarification = resolver.toToolJson(resolved);
    out.clarification["field"] = field;
    out.clarification["query"] = value;
    return false;
}

} // namespace

bool writeToolNeedsEntityResolution(const std::string& tool_name)
{
    return entityFieldsByTool().count(tool_name) > 0;
}

bool isNetGraphWriteTool(const std::string& tool_name)
{
    static const std::unordered_set<std::string> kTools = {
        "add_component",
        "set_property",
        "remove_component",
        "connect_components",
        "disconnect_components",
    };
    return kTools.count(tool_name) > 0;
}

WriteArgumentNormalizeResult normalizeWriteToolArguments(const std::string& tool_name,
                                                         nlohmann::json arguments,
                                                         URdkDomainAccess& domain,
                                                         int channel_index)
{
    WriteArgumentNormalizeResult out;
    out.normalized_arguments = std::move(arguments);

    const auto it = entityFieldsByTool().find(tool_name);
    if(it == entityFieldsByTool().end())
    {
        out.ok = true;
        return out;
    }

    const int ch = out.normalized_arguments.value("channel_index", channel_index);
    for(const std::string& field : it->second)
    {
        if(!resolveField(tool_name, field, out.normalized_arguments, domain, ch, out))
            return out;
    }

    out.ok = true;
    return out;
}

} // namespace RDK::LLM
