#ifndef RDK_ULLM_TOOL_FILTER_EXPAND_H
#define RDK_ULLM_TOOL_FILTER_EXPAND_H

#include "../LlmTypes.h"
#include "../Tools/ULLMToolRegistry.h"

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_set>

namespace RDK::LLM {

/// Progressive disclosure: add discovered tool names into the allowlist for later ReAct rounds.
inline void expandToolFilterFromSearchResult(ToolFilter& filter, const nlohmann::json& search_result,
                                             const ULLMToolRegistry& registry)
{
    if(!search_result.contains("tools"))
        return;
    if(!filter.allowed_tool_names)
        filter.allowed_tool_names = std::unordered_set<std::string>{};
    const auto& tools = search_result["tools"];
    if(!tools.is_array())
        return;
    for(const auto& t : tools)
    {
        std::string name;
        if(t.is_string())
            name = t.get<std::string>();
        else if(t.is_object())
            name = t.value("name", "");
        if(name.empty())
            continue;
        if(registry.find(name))
            filter.allowed_tool_names->insert(name);
    }
}

} // namespace RDK::LLM

#endif
