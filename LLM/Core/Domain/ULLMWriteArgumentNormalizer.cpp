#include "ULLMWriteArgumentNormalizer.h"

#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "../Context/ULLMConnectSemanticsCatalog.h"
#include "../Context/ULinkPatternCatalog.h"
#include "../Orchestrator/ULLMLibraryScopeHint.h"
#include "../Orchestrator/ULLMLifecycleArgumentGate.h"
#include "../Session/ULLMConversationStore.h"
#include "../Session/ULLMSessionGraphMemory.h"
#include "ULLMConnectPortHeuristics.h"
#include "ULLMConnectPortInference.h"
#include "ULLMAddParentResolution.h"
#include "ULLMCurrentComponentScope.h"
#include "../Orchestrator/ULLMQuantityParser.h"
#include "../Session/ULLMGuiTurnPin.h"
#include "ULLMNameResolution.h"
#include "../Orchestrator/ULLMConnectPlanParsing.h"
#include "../Orchestrator/ULLMLifecycleArgumentGate.h"
#include "ULLMResolvedEntityStore.h"
#include "URdkEntityResolver.h"
#include "../Session/ULLMConversationStore.h"

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

const LLMGuiContextSnapshot* resolveGuiForWrite(const ConversationState* conversation)
{
    if(!conversation)
        return nullptr;
    return guiContextForWrite(*conversation);
}

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

void fillAddComponentDefaults(nlohmann::json& args,
                              const LLMGuiContextSnapshot* gui_fallback = nullptr)
{
    if(!args.contains("parent_long_name"))
        args["parent_long_name"] = "";
    std::string& parent = args["parent_long_name"].get_ref<std::string&>();
    if(parent.empty() || isModelRootContainerToken(parent, gui_fallback))
    {
        const std::string diagram_scope = readDiagramScopeLongName(gui_fallback);
        if(!diagram_scope.empty())
            parent = diagram_scope;
        else
            parent.clear(); // model root; never use focused leaf as add parent (TD-111 revert)
    }
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
        std::string sn;
        if(cn.size() > 2 && cn.compare(0, 2, "NS") == 0)
            sn = cn.substr(2);
        else
        {
            sn = cn;
            if(!sn.empty() && sn[0] == 'N')
                sn.erase(sn.begin());
        }
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
                                    WriteArgumentNormalizeResult& out,
                                    const LLMGuiContextSnapshot* gui_fallback)
{
    if(isConnectGoalText(user_text))
    {
        out.ok = false;
        out.error_code = "WRONG_TOOL_FOR_CONNECT";
        out.message =
            "User asked to link components; use connect_components, not add_component.";
        return false;
    }

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
        if(!user_text.empty())
        {
            if(const std::optional<std::string> embedded =
                   findExplicitRegisteredClassInUserText(user_text, registered))
            {
                args["class_name"] = *embedded;
                fillAddComponentDefaults(args, gui_fallback);
                return true;
            }
            const std::string explicit_class = extractClassNameTokenFromUserText(user_text);
            if(!explicit_class.empty() && isRegisteredClassName(registered, explicit_class))
            {
                args["class_name"] = canonicalRegisteredClassName(registered, explicit_class);
                fillAddComponentDefaults(args, gui_fallback);
                return true;
            }
        }

        const std::string class_name = args.value("class_name", "");
        // Continuity: a resolved registered class must not be overwritten by fuzzy nouns
        // from user_text ("нейрона", "компонент"). Only an explicit class token may override
        // (handled above).
        if(isRegisteredClassName(registered, class_name))
        {
            args["class_name"] = canonicalRegisteredClassName(registered, class_name);
        }
        else
        {
            const std::string query = extractClassNameQuery(class_name, user_text);
            if(!query.empty())
            {
                const RegisteredClassResolution resolved =
                    resolveRegisteredClassName(query, registered);
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
                else
                {
                    return fillClassDisambiguationOut(out, query, {});
                }
            }
            else if(!class_name.empty())
            {
                return fillClassDisambiguationOut(out, class_name, {});
            }
        }
    }
    else if(!user_text.empty())
    {
        const LibraryScopeHint scope = detectLibraryScopeFromUserText(user_text);
        if(args.contains("class_name") && args["class_name"].is_string())
            args["class_name"] = resolveComponentClassName(args["class_name"].get<std::string>(), scope);
        else
            args["class_name"] = resolveComponentClassName(user_text, scope);
    }

    fillAddComponentDefaults(args, gui_fallback);

    if(gui_fallback)
    {
        const std::string class_name = args.value("class_name", "");
        const std::string parent_hint = args.value("parent_long_name", "");
        const AddParentResolution parent_res =
            resolveValidAddParent(domain, parent_hint, class_name, args.value("channel_index", 0),
                                  *gui_fallback);
        if(parent_res.needs_clarification)
        {
            out.ok = false;
            out.needs_clarification = true;
            out.error_code = "PARENT_AMBIGUOUS";
            out.message = parent_res.message;
            out.clarification = nlohmann::json::object();
            out.clarification["ambiguous"] = true;
            out.clarification["kind"] = "component";
            out.clarification["field"] = "parent_long_name";
            out.clarification["query"] = parent_hint;
            out.clarification["candidates"] = nlohmann::json::array();
            for(const AddParentCandidate& c : parent_res.candidates)
            {
                out.clarification["candidates"].push_back(
                    {{"long_name", c.long_name}, {"class_name", c.class_name}});
            }
            return false;
        }
        if(parent_res.ok)
            args["parent_long_name"] = parent_res.parent_long_name;
    }

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

nlohmann::json snapshotComponentsUnderScope(const nlohmann::json& snap,
                                            const std::string& scope_long_name)
{
    if(scope_long_name.empty() || !snap.contains("components") || !snap["components"].is_array())
        return snap;

    const std::string prefix = scope_long_name + ".";
    nlohmann::json scoped = snap;
    nlohmann::json filtered = nlohmann::json::array();
    for(const nlohmann::json& comp : snap["components"])
    {
        if(!comp.is_object())
            continue;
        const std::string ln = comp.value("long_name", "");
        if(ln == scope_long_name || (ln.size() > prefix.size() && ln.compare(0, prefix.size(), prefix) == 0))
            filtered.push_back(comp);
    }
    if(!filtered.empty())
        scoped["components"] = std::move(filtered);
    return scoped;
}

bool resolveField(const std::string& tool_name, const std::string& field,
                  nlohmann::json& arguments, URdkDomainAccess& domain, int channel_index,
                  WriteArgumentNormalizeResult& out, const ConversationState* conversation)
{
    if(!arguments.contains(field) || !arguments[field].is_string())
        return true;

    const std::string value = arguments[field].get<std::string>();
    if(value.empty())
        return true;

    // Soft root sentinel "Model" is not a component long_name at root view.
    if(field == "parent_long_name")
    {
        const LLMGuiContextSnapshot* gui = resolveGuiForWrite(conversation);
        if(!gui && conversation && conversation->last_gui_context)
            gui = &*conversation->last_gui_context;
        if(isModelRootContainerToken(value, gui) || (value == "Model" && isAtRootDiagramView(gui)))
        {
            arguments[field] = "";
            return true;
        }
    }

    if(conversation)
    {
        if(const auto cached =
               lookupResolvedEntity(*conversation, "component", value, channel_index))
        {
            arguments[field] = *cached;
            if(LLMServices::instance().isInitialized())
            {
                GetAuditLog().append(
                    "entity_resolved_cache_hit",
                    {{"kind", "component"}, {"query_key", value}, {"canonical", *cached}},
                    "", conversation->session_id);
            }
            return true;
        }
    }

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

    const LLMGuiContextSnapshot* gui = resolveGuiForWrite(conversation);
    if(!gui && conversation && conversation->last_gui_context)
        gui = &*conversation->last_gui_context;
    const CurrentComponentScope scope = readCurrentComponentScope(gui);
    nlohmann::json scoped_snap =
        scope.valid ? snapshotComponentsUnderScope(snap, scope.long_name) : snap;

    URdkEntityResolver resolver(domain);
    auto try_resolve = [&](const nlohmann::json& components_json) -> ComponentEntityResolution {
        return resolveComponentEntity(value, components_json);
    };

    ComponentEntityResolution resolved = try_resolve(scoped_snap["components"]);
    if(resolved.status != ComponentEntityResolution::Status::Resolved
       && resolved.status != ComponentEntityResolution::Status::Ambiguous && scope.valid)
        resolved = try_resolve(snap["components"]);
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
    ULinkPatternCatalog catalog = linkPatternCatalog();
    const ULLMConnectSemanticsCatalog& semantics = defaultConnectSemanticsCatalog();
    ConnectPortInferenceResult inf = inferConnectPorts(
        arguments, domain, catalog, channel_index, semantics.empty() ? nullptr : &semantics);
    out.ok = inf.ok;
    out.needs_clarification = inf.needs_clarification;
    out.error_code = inf.error_code;
    out.message = inf.message;
    out.clarification = inf.clarification;
    return inf.ok;
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
                                                         const std::string& user_text,
                                                         const ConversationState* conversation)
{
    WriteArgumentNormalizeResult out;
    out.normalized_arguments = std::move(arguments);

    const LLMGuiContextSnapshot* gui = resolveGuiForWrite(conversation);
    if(!gui && conversation && conversation->last_gui_context)
        gui = &*conversation->last_gui_context;

    if(tool_name == "add_component")
    {
        if(!normalizeAddComponentArguments(out.normalized_arguments, domain, user_text, out, gui))
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
        if(!resolveField(tool_name, field, out.normalized_arguments, domain, ch, out, conversation))
            return out;
    }

    if(tool_name == "set_property")
    {
        const std::string long_name = out.normalized_arguments.value("long_name", std::string());
        std::string property_name = out.normalized_arguments.value("property_name", std::string());
        if(!long_name.empty() && !property_name.empty())
        {
            nlohmann::json props_out;
            const DomainStatus pst = domain.getComponentProperties(long_name, props_out, ch);
            if(pst.ok() && props_out.contains("properties") && props_out["properties"].is_array())
            {
                std::vector<std::string> catalog;
                catalog.reserve(props_out["properties"].size());
                for(const nlohmann::json& item : props_out["properties"])
                {
                    if(item.is_object() && item.contains("name") && item["name"].is_string())
                        catalog.push_back(item["name"].get<std::string>());
                }
                const RegisteredClassResolution prop_res =
                    resolvePropertyNameFromCatalog(property_name, catalog);
                if(prop_res.status == RegisteredClassResolution::Status::Resolved)
                {
                    out.normalized_arguments["property_name"] = prop_res.class_name;
                }
                else if(prop_res.status == RegisteredClassResolution::Status::Ambiguous)
                {
                    out.ok = false;
                    out.needs_clarification = true;
                    out.error_code = "PROPERTY_AMBIGUOUS";
                    out.message = "Ambiguous property_name for set_property";
                    nlohmann::json candidates = nlohmann::json::array();
                    for(const auto& [name, score] : prop_res.candidates)
                        candidates.push_back({{"name", name}, {"score", score}});
                    out.clarification = {{"kind", "property"},
                                         {"field", "property_name"},
                                         {"query", property_name},
                                         {"candidates", candidates}};
                    return out;
                }
            }
        }
    }

    if(tool_name == "connect_components")
    {
        if(!normalizeConnectComponentsArguments(out.normalized_arguments, domain, ch, out))
            return out;
    }

    out.ok = true;
    return out;
}

namespace {

bool isAddComponentGoalUserText(const std::string& user_text)
{
    if(user_text.empty() || isConnectGoalText(user_text) || isValidateConfigurationGoalText(user_text))
        return false;
    const std::string lower = user_text;
    std::string lc;
    lc.reserve(lower.size());
    for(char c : lower)
        lc += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return lc.find("add ") != std::string::npos || lc.find("добав") != std::string::npos
           || lc.find("созда") != std::string::npos || lc.find("create ") != std::string::npos;
}

bool isRepeatSameAddCue(const std::string& user_text)
{
    std::string lc;
    lc.reserve(user_text.size());
    for(char c : user_text)
        lc += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return lc.find("таких") != std::string::npos || lc.find("такой же") != std::string::npos
           || lc.find("такое же") != std::string::npos || lc.find("same") != std::string::npos
           || lc.find("another") != std::string::npos || lc.find("еще") != std::string::npos
           || lc.find("ещё") != std::string::npos || lc.find("more") != std::string::npos
           || lc.find("again") != std::string::npos;
}

/// Generic / type continuer nouns: not a new class query when last_add is set.
bool looksLikeContinuerNoun(const std::string& user_text)
{
    std::string lc;
    lc.reserve(user_text.size());
    for(char c : user_text)
        lc += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return lc.find("нейрон") != std::string::npos || lc.find("neuron") != std::string::npos
           || lc.find("компонент") != std::string::npos || lc.find("component") != std::string::npos
           || lc.find("блок") != std::string::npos || lc.find("block") != std::string::npos
           || lc.find("модул") != std::string::npos || lc.find("module") != std::string::npos;
}

bool looksLikeAddContinuityCueLocal(const std::string& user_text)
{
    return isRepeatSameAddCue(user_text) || looksLikeContinuerNoun(user_text);
}

} // namespace

bool looksLikeRepeatSameAddCue(const std::string& user_text)
{
    return isRepeatSameAddCue(user_text);
}

bool looksLikeAddContinuityCue(const std::string& user_text)
{
    return looksLikeAddContinuityCueLocal(user_text);
}

std::optional<PreparedAddComponentInvoke> tryPrepareAddComponentDirect(
    const std::string& user_text, const LLMGuiContextSnapshot& gui, URdkDomainAccess& domain,
    int channel_index, int repeat_count, const SessionGraphMemory* session_graph)
{
    if(!isAddComponentGoalUserText(user_text))
        return std::nullopt;

    std::vector<std::string> registered;
    const DomainStatus list_st = domain.listRegisteredClassNames(registered);
    if(!list_st.ok() || registered.empty())
        return std::nullopt;

    std::optional<std::string> explicit_class =
        findExplicitRegisteredClassInUserText(user_text, registered);
    std::string class_from_last;
    std::string parent_from_last;
    std::string short_from_last;
    if(!explicit_class && session_graph && session_graph->last_add
       && !session_graph->last_add->class_name.empty()
       && looksLikeAddContinuityCueLocal(user_text))
    {
        class_from_last = session_graph->last_add->class_name;
        parent_from_last = session_graph->last_add->parent_long_name;
        short_from_last = session_graph->last_add->short_name_base;
        explicit_class = class_from_last;
    }
    if(!explicit_class)
        return std::nullopt;

    nlohmann::json args = nlohmann::json::object();
    args["class_name"] = *explicit_class;
    if(!parent_from_last.empty())
        args["parent_long_name"] = parent_from_last;
    if(!short_from_last.empty())
        args["short_name"] = short_from_last;

    WriteArgumentNormalizeResult norm =
        normalizeWriteToolArguments("add_component", args, domain, channel_index, user_text, nullptr);
    PreparedAddComponentInvoke prepared;
    int effective_repeat = std::max(1, repeat_count);
    const ParsedQuantity from_text = extractQuantityHeuristic(user_text);
    if(from_text.valid && from_text.count > effective_repeat)
        effective_repeat = std::min(from_text.count, 32);
    prepared.repeat_count = effective_repeat;
    prepared.arguments = norm.normalized_arguments;

    if(norm.needs_clarification)
    {
        prepared.needs_clarification = true;
        prepared.clarification = norm.clarification;
        return prepared;
    }
    if(!norm.ok)
        return std::nullopt;

    fillAddComponentDefaults(prepared.arguments, &gui);
    if(!parent_from_last.empty()
       && (!prepared.arguments.contains("parent_long_name")
           || prepared.arguments.value("parent_long_name", "").empty()
           || prepared.arguments.value("parent_long_name", "") == "Model"))
        prepared.arguments["parent_long_name"] = parent_from_last;
    if(!short_from_last.empty()
       && (!prepared.arguments.contains("short_name")
           || prepared.arguments.value("short_name", "").empty()))
        prepared.arguments["short_name"] = short_from_last;

    const AddParentResolution parent_res = resolveValidAddParent(
        domain, prepared.arguments.value("parent_long_name", ""), *explicit_class, channel_index,
        gui);
    if(parent_res.needs_clarification)
    {
        prepared.needs_clarification = true;
        prepared.clarification = nlohmann::json::object();
        prepared.clarification["ambiguous"] = true;
        prepared.clarification["kind"] = "component";
        prepared.clarification["field"] = "parent_long_name";
        prepared.clarification["candidates"] = nlohmann::json::array();
        for(const AddParentCandidate& c : parent_res.candidates)
        {
            prepared.clarification["candidates"].push_back(
                {{"long_name", c.long_name}, {"class_name", c.class_name}});
        }
        return prepared;
    }
    if(parent_res.ok)
        prepared.arguments["parent_long_name"] = parent_res.parent_long_name;
    if(!prepared.arguments.contains("class_name")
       || !prepared.arguments.contains("parent_long_name")
       || !prepared.arguments.contains("short_name"))
        return std::nullopt;
    if(findMissingLifecycleFields("add_component", prepared.arguments, domain.application())
           .empty())
        return prepared;
    return std::nullopt;
}

} // namespace RDK::LLM
