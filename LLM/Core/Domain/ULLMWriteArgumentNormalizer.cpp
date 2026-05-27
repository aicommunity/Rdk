#include "ULLMWriteArgumentNormalizer.h"

#include "../Context/ULinkPatternCatalog.h"
#include "../Orchestrator/ULLMLibraryScopeHint.h"
#include "../Orchestrator/ULLMLifecycleArgumentGate.h"
#include "URdkEntityResolver.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <cstdlib>
#include <mutex>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

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

void fillAddComponentDefaults(nlohmann::json& args)
{
    if(!args.contains("parent_long_name"))
        args["parent_long_name"] = "";
    if(!args.contains("channel_index"))
        args["channel_index"] = 0;
    if(!args.contains("class_name") || !args["class_name"].is_string())
        return;
    std::string cn = args["class_name"].get<std::string>();
    if(cn.empty())
        return;
    if(!args.contains("short_name") || !args["short_name"].is_string()
       || args["short_name"].get<std::string>().empty())
    {
        std::string sn = cn;
        if(!sn.empty() && sn[0] == 'N')
            sn.erase(sn.begin());
        if(sn.empty())
            sn = "Component1";
        args["short_name"] = sn;
    }
}

struct ClassCandidate {
    std::string class_name;
    double score = 0.0;
};

const ULinkPatternCatalog& linkPatternCatalog()
{
    static std::once_flag once;
    static ULinkPatternCatalog catalog;
    std::call_once(once, []() {
        std::filesystem::path root;
        if(const char* env = std::getenv("NMSDK_ROOT"))
            root = std::filesystem::path(env);
        if(root.empty() || !std::filesystem::exists(root / "CMakeLists.txt"))
        {
            root = std::filesystem::current_path();
            for(int i = 0; i < 8 && root.has_parent_path(); ++i)
            {
                if(std::filesystem::exists(root / "CMakeLists.txt"))
                    break;
                root = root.parent_path();
            }
        }
        if(!root.empty())
            catalog.loadFromFile(root / "Bin/LLM/index/link-patterns.json");
    });
    return catalog;
}

bool fillClassDisambiguationOut(WriteArgumentNormalizeResult& out, const std::string& query,
                                const std::vector<ClassCandidate>& candidates)
{
    out.ok = false;
    out.needs_clarification = true;
    out.error_code = "CLASS_AMBIGUOUS";
    out.message = "Which component class do you mean?";
    out.clarification = nlohmann::json::object();
    out.clarification["ambiguous"] = true;
    out.clarification["kind"] = "class";
    out.clarification["query"] = query;
    out.clarification["candidates"] = nlohmann::json::array();
    for(const ClassCandidate& c : candidates)
        out.clarification["candidates"].push_back(
            {{"class_name", c.class_name}, {"score", c.score}});
    if(candidates.empty())
    {
        out.error_code = "CLASS_NOT_REGISTERED";
        out.message = "Class \"" + query
                        + "\" is not registered. Use list_registered_classes and reply with "
                          "the exact class name.";
        out.clarification["kind"] = "class";
    }
    return false;
}

bool normalizeAddComponentArguments(nlohmann::json& args, URdkDomainAccess& domain,
                                    const std::string& user_text,
                                    WriteArgumentNormalizeResult& out)
{
    std::vector<std::string> registered;
    const DomainStatus list_st = domain.listRegisteredClassNames(registered);
    const bool have_registry = list_st.ok() && !registered.empty();

    if(!user_text.empty())
    {
        const std::string existing_class = args.value("class_name", "");
        const bool class_already_set =
            have_registry && isRegisteredClassName(registered, existing_class);
        if(!class_already_set)
        {
            PendingToolArguments pending;
            pending.tool_name = "add_component";
            pending.partial_arguments = args;
            args = mergeArgumentsFromUserText(pending, user_text, domain.application());
        }
    }

    if(have_registry)
    {
        const std::string class_name = args.value("class_name", "");
        const std::string query = extractClassNameQuery(class_name, user_text);
        if(!query.empty())
        {
            const RegisteredClassResolution resolved = resolveRegisteredClassName(query, registered);
            if(resolved.status == RegisteredClassResolution::Status::Resolved)
            {
                args["class_name"] = resolved.class_name;
            }
            else if(resolved.status == RegisteredClassResolution::Status::Ambiguous)
            {
                std::vector<ClassCandidate> candidates;
                candidates.reserve(resolved.candidates.size());
                for(const auto& [name, score] : resolved.candidates)
                    candidates.push_back({name, score});
                return fillClassDisambiguationOut(out, query, candidates);
            }
            else if(!isRegisteredClassName(registered, class_name))
            {
                return fillClassDisambiguationOut(out, query, {});
            }
        }
        else if(!isRegisteredClassName(registered, class_name))
        {
            return fillClassDisambiguationOut(out, class_name, {});
        }
        if(isRegisteredClassName(registered, args.value("class_name", "")))
            args["class_name"] =
                canonicalRegisteredClassName(registered, args["class_name"].get<std::string>());
    }
    else if(!user_text.empty())
    {
        const LibraryScopeHint scope = detectLibraryScopeFromUserText(user_text);
        if(args.contains("class_name") && args["class_name"].is_string())
            args["class_name"] = resolveComponentClassName(args["class_name"].get<std::string>(), scope);
        else
            args["class_name"] = resolveComponentClassName(user_text, scope);
    }

    fillAddComponentDefaults(args);
    return true;
}

bool fillEntityDisambiguationOut(WriteArgumentNormalizeResult& out, const std::string& field,
                                 const std::string& query,
                                 const ComponentEntityResolution& resolved,
                                 URdkEntityResolver& resolver)
{
    out.ok = false;
    out.needs_clarification = true;
    out.error_code = "ENTITY_AMBIGUOUS";
    out.message = "Ambiguous component reference for " + field;
    EntityResolutionResult er;
    er.status = EntityResolutionStatus::Ambiguous;
    er.candidates.reserve(resolved.candidates.size());
    for(const auto& [ln, score] : resolved.candidates)
        er.candidates.push_back({ln, "", "", score});
    out.clarification = resolver.toToolJson(er);
    out.clarification["kind"] = "component";
    out.clarification["field"] = field;
    out.clarification["query"] = query;
    return false;
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

    nlohmann::json snap;
    if(!domain.listNetSnapshot(snap, channel_index, 500).ok())
    {
        out.ok = false;
        out.error_code = "ENTITY_NOT_FOUND";
        out.message = "Component not found for " + field + ": " + value;
        return false;
    }

    URdkEntityResolver resolver(domain);
    const ComponentEntityResolution resolved =
        resolveComponentEntity(value, snap["components"]);
    (void)tool_name;

    if(resolved.status == ComponentEntityResolution::Status::Resolved)
    {
        arguments[field] = resolved.canonical_long_name;
        return true;
    }

    if(resolved.status == ComponentEntityResolution::Status::Ambiguous)
        return fillEntityDisambiguationOut(out, field, value, resolved, resolver);

    out.ok = false;
    out.error_code = "ENTITY_NOT_FOUND";
    out.message = "Component not found for " + field + ": " + value;
    EntityResolutionResult er;
    er.status = EntityResolutionStatus::NotFound;
    out.clarification = resolver.toToolJson(er);
    out.clarification["kind"] = "component";
    out.clarification["field"] = field;
    out.clarification["query"] = value;
    return false;
}

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

bool isGenericLinkPortName(const std::string& name)
{
    const std::string lower = toLowerAscii(name);
    return lower == "output" || lower == "input" || lower == "out" || lower == "in";
}

std::optional<std::string> findPortCaseInsensitive(const std::vector<std::string>& ports,
                                                   const std::string& query)
{
    if(query.empty())
        return std::nullopt;
    for(const std::string& port : ports)
    {
        if(port == query)
            return port;
    }
    const std::string q = toLowerAscii(query);
    for(const std::string& port : ports)
    {
        if(toLowerAscii(port) == q)
            return port;
    }
    return std::nullopt;
}

std::optional<std::string> pickPreferredOutputPort(const std::vector<std::string>& outputs)
{
    if(outputs.empty())
        return std::nullopt;
    if(outputs.size() == 1)
        return outputs.front();
    for(const std::string& port : outputs)
    {
        if(port == "Output")
            return port;
    }
    for(const std::string& port : outputs)
    {
        if(port.find("ExcSynapse1") != std::string::npos)
            return port;
    }
    for(const std::string& port : outputs)
    {
        if(port.find("LTZone") != std::string::npos)
            return port;
    }
    return std::nullopt;
}

std::optional<std::string> pickPreferredInputPort(const std::vector<std::string>& inputs)
{
    if(inputs.empty())
        return std::nullopt;
    if(inputs.size() == 1)
        return inputs.front();
    for(const std::string& port : inputs)
    {
        if(port == "Input")
            return port;
    }
    for(const std::string& port : inputs)
    {
        if(port.find("Soma1.ExcSynapse1") != std::string::npos)
            return port;
    }
    for(const std::string& port : inputs)
    {
        if(port.find("ExcSynapse1") != std::string::npos)
            return port;
    }
    return std::nullopt;
}

std::string formatPortListForMessage(const std::vector<std::string>& ports)
{
    if(ports.empty())
        return "(none)";
    std::ostringstream oss;
    for(size_t i = 0; i < ports.size(); ++i)
    {
        if(i > 0)
            oss << ", ";
        oss << ports[i];
        if(i >= 7)
        {
            oss << ", ...";
            break;
        }
    }
    return oss.str();
}

nlohmann::json portCandidatesJson(const std::vector<std::string>& ports)
{
    nlohmann::json arr = nlohmann::json::array();
    for(const std::string& port : ports)
        arr.push_back({{"port_name", port}});
    return arr;
}

bool resolveConnectPortField(const std::string& field, const std::string& component_long_name,
                             const std::vector<std::string>& catalog, bool is_output,
                             nlohmann::json& arguments, WriteArgumentNormalizeResult& out)
{
    if(!arguments.contains(field) || !arguments[field].is_string())
        return true;

    std::string value = arguments[field].get<std::string>();
    const bool generic = isGenericLinkPortName(value);

    if(!value.empty() && !generic)
    {
        if(const std::optional<std::string> exact = findPortCaseInsensitive(catalog, value))
        {
            arguments[field] = *exact;
            return true;
        }
    }

    if(!value.empty() && !generic)
    {
        out.ok = false;
        out.needs_clarification = true;
        out.error_code = "CONNECT_PORT_NOT_FOUND";
        out.message = "Unknown " + field + " \"" + value + "\" on component " + component_long_name
                        + ". Available " + (is_output ? "outputs" : "inputs") + ": "
                        + formatPortListForMessage(catalog) + ".";
        out.clarification = nlohmann::json::object();
        out.clarification["ambiguous"] = true;
        out.clarification["kind"] = "property";
        out.clarification["field"] = field;
        out.clarification["component_long_name"] = component_long_name;
        out.clarification["candidates"] = portCandidatesJson(catalog);
        return false;
    }

    const std::optional<std::string> picked =
        is_output ? pickPreferredOutputPort(catalog) : pickPreferredInputPort(catalog);
    if(picked)
    {
        arguments[field] = *picked;
        return true;
    }

    out.ok = false;
    out.needs_clarification = true;
    out.error_code = "CONNECT_PORTS_AMBIGUOUS";
    out.message =
        "Cannot infer " + field + " for " + component_long_name + ". Specify one of: "
        + formatPortListForMessage(catalog)
        + " (call get_component_properties for details).";
    out.clarification = nlohmann::json::object();
    out.clarification["ambiguous"] = true;
    out.clarification["kind"] = "property";
    out.clarification["field"] = field;
    out.clarification["component_long_name"] = component_long_name;
    out.clarification["candidates"] = portCandidatesJson(catalog);
    return false;
}

bool normalizeConnectComponentsArguments(nlohmann::json& arguments, URdkDomainAccess& domain,
                                         int channel_index, WriteArgumentNormalizeResult& out)
{
    const std::string from_ln = arguments.value("from_long_name", "");
    const std::string to_ln = arguments.value("to_long_name", "");
    if(from_ln.empty() || to_ln.empty())
        return true;

    std::vector<std::string> from_outputs;
    std::vector<std::string> from_inputs_unused;
    DomainStatus st_from =
        domain.listComponentPubPorts(from_ln, channel_index, from_outputs, from_inputs_unused);
    if(!st_from.ok())
    {
        out.ok = false;
        out.error_code = "ENTITY_NOT_FOUND";
        out.message = st_from.message;
        return false;
    }

    std::vector<std::string> to_outputs_unused;
    std::vector<std::string> to_inputs;
    DomainStatus st_to =
        domain.listComponentPubPorts(to_ln, channel_index, to_outputs_unused, to_inputs);
    if(!st_to.ok())
    {
        out.ok = false;
        out.error_code = "ENTITY_NOT_FOUND";
        out.message = st_to.message;
        return false;
    }

    auto tryCatalogFill = [&]() -> bool {
        std::string from_class;
        std::string to_class;
        const DomainStatus fs = domain.getComponentClassName(from_ln, channel_index, from_class);
        const DomainStatus ts = domain.getComponentClassName(to_ln, channel_index, to_class);
        if(!fs.ok() || !ts.ok())
            return false;

        auto candidates = linkPatternCatalog().suggest(from_class, to_class, 3);
        if(candidates.empty() && from_class.find("Neuron") != std::string::npos
           && to_class.find("Neuron") != std::string::npos)
            candidates = linkPatternCatalog().suggest(from_class, to_class, 5);
        if(candidates.empty())
            return false;

        const double top = candidates[0].score;
        const double second = candidates.size() > 1 ? candidates[1].score : 0.0;
        constexpr double kMinAutoFillScore = 0.6;
        constexpr double kMinScoreGap = 0.2;
        if(top < kMinAutoFillScore || (top - second) < kMinScoreGap)
            return false;

        arguments["from_property"] = candidates[0].from_port;
        arguments["to_property"] = candidates[0].to_port;
        out.ok = true;
        out.error_code.clear();
        out.message.clear();
        out.needs_clarification = false;
        out.clarification = nlohmann::json::object();
        return true;
    };

    if(!resolveConnectPortField("from_property", from_ln, from_outputs, true, arguments, out))
    {
        if(tryCatalogFill())
            return true;
        return false;
    }
    if(!resolveConnectPortField("to_property", to_ln, to_inputs, false, arguments, out))
    {
        if(tryCatalogFill())
            return true;
        return false;
    }
    return true;
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

    if(tool_name == "connect_components")
    {
        if(!normalizeConnectComponentsArguments(out.normalized_arguments, domain, ch, out))
            return out;
    }

    out.ok = true;
    return out;
}

} // namespace RDK::LLM
