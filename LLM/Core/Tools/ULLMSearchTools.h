#ifndef RDK_ULLM_SEARCH_TOOLS_H
#define RDK_ULLM_SEARCH_TOOLS_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

class ULLMToolRegistry;

struct SearchToolsResult {
    std::string query;
    std::vector<std::string> tools;
    int index_version = 1;
};

SearchToolsResult searchToolsByQuery(ULLMToolRegistry& registry, const std::string& query, int top_k);

} // namespace RDK::LLM

#endif
