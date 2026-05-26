#include "ULLMWriteArgumentNormalizer.h"

#include "../Orchestrator/ULLMLibraryScopeHint.h"
#include "../Orchestrator/ULLMLifecycleArgumentGate.h"
#include "URdkEntityResolver.h"

#include <algorithm>
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

bool isRegisteredClass(const std::vector<std::string>& registered, const std::string& name)
{
    return std::find(registered.begin(), registered.end(), name) != registered.end();
}

std::string closestRegisteredClassName(const std::string& query,
                                     const std::vector<std::string>& registered)
{
    if(query.empty() || registered.empty())
        return query;

    const LibraryScopeHint scope = detectLibraryScopeFromUserText(query);
    std::string resolved = resolveComponentClassName(query, scope);
    if(isRegisteredClass(registered, resolved))
        return resolved;

    std::string best = query;
    int best_dist = 999;
    const std::string qlower = query;
    for(const std::string& candidate : registered)
    {
        int d = 0;
        const size_t n = std::min(qlower.size(), candidate.size());
        for(size_t i = 0; i < n; ++i)
            if(qlower[i] != candidate[i])
                ++d;
        d += static_cast<int>(std::max(qlower.size(), candidate.size()) - n);
        if(d < best_dist)
        {
            best_dist = d;
            best = candidate;
        }
    }
    if(best_dist <= 4)
        return best;
    return query;
}

bool normalizeAddComponentArguments(nlohmann::json& args, URdkDomainAccess& domain,
                                    const std::string& user_text,
                                    WriteArgumentNormalizeResult& out)
{
    if(!user_text.empty())
    {
        PendingToolArguments pending;
        pending.tool_name = "add_component";
        pending.partial_arguments = args;
        args = mergeArgumentsFromUserText(pending, user_text, domain.application());
    }

    std::vector<std::string> registered;
    const DomainStatus list_st = domain.listRegisteredClassNames(registered);
    const bool have_registry = list_st.ok() && !registered.empty();

    if(have_registry)
    {
        if(const std::optional<std::string> inferred =
               inferAddComponentClassFromUserText(user_text, registered))
            args["class_name"] = *inferred;

        std::string class_name = args.value("class_name", "");
        if(!isRegisteredClass(registered, class_name))
        {
            class_name = closestRegisteredClassName(class_name, registered);
            if(!isRegisteredClass(registered, class_name))
            {
                out.ok = false;
                out.error_code = "CLASS_NOT_REGISTERED";
                out.message = "Class \"" + args.value("class_name", "")
                              + "\" is not registered. Use list_registered_classes or a valid "
                                "class name (e.g. NPulseNeuron for a pulse neuron).";
                return false;
            }
            args["class_name"] = class_name;
        }
    }
    else if(!user_text.empty())
    {
        const LibraryScopeHint scope = detectLibraryScopeFromUserText(user_text);
        if(const std::optional<std::string> inferred =
               inferAddComponentClassFromUserText(user_text, {}))
            args["class_name"] = *inferred;
        else if(args.contains("class_name") && args["class_name"].is_string())
            args["class_name"] = resolveComponentClassName(args["class_name"].get<std::string>(), scope);
        else
            args["class_name"] = resolveComponentClassName(user_text, scope);
    }

    if(!args.contains("parent_long_name"))
        args["parent_long_name"] = "";
    if(!args.contains("channel_index"))
        args["channel_index"] = 0;

    return true;
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
                                                         int channel_index,
                                                         const std::string& user_text)
{
    WriteArgumentNormalizeResult out;
    out.normalized_arguments = std::move(arguments);

    if(tool_name == "add_component")
    {
        if(!normalizeAddComponentArguments(out.normalized_arguments, domain, user_text, out))
            return out;
    }

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
