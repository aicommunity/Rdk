#include "ULLMWriteArgumentNormalizer.h"

#include "../Orchestrator/ULLMLibraryScopeHint.h"
#include "../Orchestrator/ULLMLifecycleArgumentGate.h"
#include "URdkEntityResolver.h"

#include <algorithm>
#include <cctype>
#include <cmath>
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

std::string toLowerAscii(std::string s)
{
    for(char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

int levenshteinDistance(const std::string& a, const std::string& b)
{
    const size_t n = a.size();
    const size_t m = b.size();
    if(n == 0)
        return static_cast<int>(m);
    if(m == 0)
        return static_cast<int>(n);
    std::vector<int> prev(m + 1);
    std::vector<int> cur(m + 1);
    for(size_t j = 0; j <= m; ++j)
        prev[j] = static_cast<int>(j);
    for(size_t i = 1; i <= n; ++i)
    {
        cur[0] = static_cast<int>(i);
        for(size_t j = 1; j <= m; ++j)
        {
            const int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            cur[j] = std::min({cur[j - 1] + 1, prev[j] + 1, prev[j - 1] + cost});
        }
        prev.swap(cur);
    }
    return prev[m];
}

struct ClassCandidate {
    std::string class_name;
    double score = 0.0; // higher is better
};

std::vector<ClassCandidate> findSimilarRegisteredClasses(const std::string& query,
                                                         const std::vector<std::string>& registered,
                                                         size_t max_candidates = 8)
{
    std::vector<ClassCandidate> out;
    if(query.empty() || registered.empty())
        return out;

    const std::string q = toLowerAscii(query);
    out.reserve(std::min(max_candidates, registered.size()));
    for(const std::string& c : registered)
    {
        const std::string cl = toLowerAscii(c);
        const int dist = levenshteinDistance(q, cl);
        const int maxlen = static_cast<int>(std::max(q.size(), cl.size()));
        const double norm = maxlen > 0 ? (static_cast<double>(dist) / static_cast<double>(maxlen)) : 1.0;
        double score = 1.0 - norm; // 1 is exact
        if(!q.empty() && cl.find(q) != std::string::npos)
            score += 0.25;
        if(!q.empty() && cl.rfind(q, 0) == 0) // prefix
            score += 0.15;
        if(score < 0.35)
            continue;
        out.push_back({c, score});
    }

    std::sort(out.begin(), out.end(), [](const ClassCandidate& a, const ClassCandidate& b) {
        if(std::fabs(a.score - b.score) > 1e-9)
            return a.score > b.score;
        return a.class_name < b.class_name;
    });
    if(out.size() > max_candidates)
        out.resize(max_candidates);
    return out;
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
        std::string class_name = args.value("class_name", "");
        if(!isRegisteredClass(registered, class_name))
        {
            const std::string query = !class_name.empty() ? class_name : user_text;
            const std::vector<ClassCandidate> candidates =
                findSimilarRegisteredClasses(query, registered, 8);

            if(candidates.size() == 1 && candidates[0].score >= 0.78)
            {
                args["class_name"] = candidates[0].class_name;
            }
            else if(!candidates.empty()
                    && candidates[0].score >= 0.75
                    && (candidates.size() == 1 || (candidates[0].score - candidates[1].score) >= 0.18))
            {
                args["class_name"] = candidates[0].class_name;
            }
            else
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
