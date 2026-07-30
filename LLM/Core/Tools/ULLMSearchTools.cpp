#include "ULLMSearchTools.h"

#include "../Orchestrator/ULLMConfigurationLifecycle.h"
#include "../Orchestrator/ULLMToolFilterBuilder.h"
#include "ULLMToolRegistry.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>
#include <vector>

namespace RDK::LLM {

namespace {

std::vector<std::string> tokenize(const std::string& text)
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

double overlap(const std::vector<std::string>& q, const std::vector<std::string>& doc)
{
    if(q.empty() || doc.empty())
        return 0.0;
    std::unordered_set<std::string> dset(doc.begin(), doc.end());
    int hit = 0;
    for(const std::string& t : q)
        hit += dset.count(t) ? 1 : 0;
    return static_cast<double>(hit) / static_cast<double>(q.size());
}

} // namespace

SearchToolsResult searchToolsByQuery(ULLMToolRegistry& registry, const std::string& query,
                                     const int top_k)
{
    SearchToolsResult out;
    out.query = query;
    out.index_version = 1;

    std::unordered_set<std::string> all;
    const ToolFilter mutate =
        buildToolFilter(LLMIntentKind::Mutate, true, ConfigurationLifecycleAction::None);
    for(const LLMToolDefinition& def : registry.listForLlmApi(mutate))
        all.insert(def.name);

    const std::vector<std::string> q = tokenize(query);
    std::vector<std::pair<std::string, double>> scored;
    scored.reserve(all.size());
    for(const std::string& tool : all)
        scored.push_back({tool, overlap(q, tokenize(tool))});

    std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) {
        if(a.second != b.second)
            return a.second > b.second;
        return a.first < b.first;
    });

    const int limit = top_k > 0 ? top_k : 12;
    for(const auto& it : scored)
    {
        if(static_cast<int>(out.tools.size()) >= limit)
            break;
        if(it.second > 0.0 || out.tools.size() < 3)
            out.tools.push_back(it.first);
    }
    return out;
}

nlohmann::json enrichSearchToolsPayload(const SearchToolsResult& found,
                                        const ULLMToolRegistry& registry)
{
    nlohmann::json tools = nlohmann::json::array();
    for(const std::string& name : found.tools)
    {
        nlohmann::json entry = {{"name", name}};
        if(const LLMToolDefinition* def = registry.find(name))
        {
            entry["description"] = def->description;
            entry["kind"] = def->kind == LLMToolKind::Write ? "write" : "read";
            entry["requires_confirmation"] = def->requires_confirmation;
        }
        tools.push_back(std::move(entry));
    }
    return {{"query", found.query},
            {"index_version", found.index_version},
            {"tools", std::move(tools)}};
}

} // namespace RDK::LLM
