#include "ULLMDynamicToolRouter.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <map>
#include <optional>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace RDK::LLM {

namespace {

int envInt(const char* name, const int fallback)
{
    const char* v = std::getenv(name);
    if(!v || !*v)
        return fallback;
    try
    {
        return std::max(0, std::stoi(v));
    }
    catch(...)
    {
        return fallback;
    }
}

std::string toLowerAscii(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

std::vector<std::string> tokenizeAscii(const std::string& text)
{
    std::vector<std::string> out;
    std::string cur;
    for(unsigned char c : text)
    {
        if(std::isalnum(c) || c == '_' || c >= 0x80)
            cur.push_back(static_cast<char>(std::tolower(c)));
        else if(!cur.empty())
        {
            if(cur.size() >= 2)
                out.push_back(cur);
            cur.clear();
        }
    }
    if(!cur.empty() && cur.size() >= 2)
        out.push_back(cur);
    return out;
}

bool containsAny(const std::string& text, const std::initializer_list<const char*> needles)
{
    for(const char* needle : needles)
    {
        if(text.find(needle) != std::string::npos)
            return true;
    }
    return false;
}

double overlapScore(const std::vector<std::string>& q, const std::vector<std::string>& doc)
{
    if(q.empty() || doc.empty())
        return 0.0;
    std::unordered_set<std::string> dset(doc.begin(), doc.end());
    int hit = 0;
    for(const std::string& t : q)
        hit += dset.count(t) ? 1 : 0;
    return static_cast<double>(hit) / static_cast<double>(std::max<size_t>(q.size(), 1));
}

std::optional<std::unordered_set<std::string>> preferredToolsForText(const std::string& user_text)
{
    const std::string lower = toLowerAscii(user_text);

    if(containsAny(lower, {"create config", "create configuration", "create project", "new config",
                           "созда", "новый проект", "новую конфигурац"}))
        return std::unordered_set<std::string>{"create_configuration"};

    if(containsAny(lower, {"load config", "open config", "open project", "открой", "загруз"}))
        return std::unordered_set<std::string>{
            "load_configuration", "load_project", "open_recent_configuration"};

    if(containsAny(lower, {"validate", "валид", "провер"}))
        return std::unordered_set<std::string>{"validate_configuration"};

    if(containsAny(lower, {"save config", "save project", "сохран"}))
        return std::unordered_set<std::string>{
            "save_configuration", "save_project", "save_project_metadata"};

    if(containsAny(lower, {"close config", "close project", "закрой"}))
        return std::unordered_set<std::string>{"close_configuration"};

    if(containsAny(lower, {"add component", "add ", "добав", "connect", "link", "соедин",
                           "set property", "свойств"}))
        return std::unordered_set<std::string>{"add_component",          "connect_components",
                                               "set_property",           "set_active_channel",
                                               "describe_class",         "list_registered_classes",
                                               "find_component",         "get_component_properties",
                                               "get_net_snapshot",       "list_channels"};

    return std::nullopt;
}

std::optional<std::unordered_set<std::string>> intersectAllowed(
    const std::optional<std::unordered_set<std::string>>& base,
    const std::unordered_set<std::string>& preferred)
{
    if(!base || preferred.empty())
        return std::nullopt;

    std::unordered_set<std::string> narrowed;
    for(const std::string& name : *base)
    {
        if(preferred.count(name))
            narrowed.insert(name);
    }
    if(narrowed.empty())
        return std::nullopt;
    return narrowed;
}

std::unordered_set<std::string> alwaysKeepTools(const ToolFilter& base)
{
    std::unordered_set<std::string> keep;
    if(!base.allowed_tool_names)
        return keep;
    // Keep core read helpers to avoid "no suitable tool" spirals.
    static const char* k[] = {"search_project_docs",
                              "list_registered_classes",
                              "describe_class",
                              "get_net_snapshot",
                              "find_component",
                              "get_component_properties",
                              "list_recent_configurations",
                              nullptr};
    for(const char** p = k; *p; ++p)
    {
        if(base.allowed_tool_names->count(*p))
            keep.insert(*p);
    }
    return keep;
}

std::optional<std::unordered_set<std::string>> scoreSubset(const ToolFilter& base,
                                                           const std::string& user_text)
{
    if(!base.allowed_tool_names || base.allowed_tool_names->empty())
        return std::nullopt;

    const int top_k = envInt("NMSDK_LLM_DYNAMIC_TOOL_ROUTING_TOP_K", 18);
    if(top_k <= 0)
        return std::nullopt;

    const std::vector<std::string> q = tokenizeAscii(user_text);
    if(q.empty())
        return std::nullopt;

    // Lightweight score: token overlap between user text and tool name.
    std::vector<std::pair<std::string, double>> scored;
    scored.reserve(base.allowed_tool_names->size());
    for(const std::string& tool : *base.allowed_tool_names)
    {
        const std::vector<std::string> doc = tokenizeAscii(tool);
        const double s = overlapScore(q, doc);
        scored.push_back({tool, s});
    }

    std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) {
        if(a.second != b.second)
            return a.second > b.second;
        return a.first < b.first;
    });

    std::unordered_set<std::string> picked = alwaysKeepTools(base);
    for(const auto& it : scored)
    {
        if(static_cast<int>(picked.size()) >= top_k)
            break;
        // Only pick scored tools, but allow zero-score if we still have very few tools.
        if(it.second > 0.0 || picked.size() < 6)
            picked.insert(it.first);
    }

    if(picked.empty())
        return std::nullopt;
    return picked;
}

} // namespace

bool ULLMDynamicToolRouter::isEnabled()
{
    const char* v = std::getenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING");
    return v && v[0] == '1';
}

ToolFilter ULLMDynamicToolRouter::apply(const ToolFilter& base, const std::string& user_text)
{
    if(!isEnabled())
        return base;

    ToolFilter out = base;
    if(const auto preferred = preferredToolsForText(user_text))
    {
        if(const auto narrowed = intersectAllowed(out.allowed_tool_names, *preferred))
            out.allowed_tool_names = *narrowed;
        return out;
    }

    if(const auto subset = scoreSubset(out, user_text))
        out.allowed_tool_names = *subset;
    return out;
}

} // namespace RDK::LLM
