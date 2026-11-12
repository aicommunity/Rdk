#include "ULLMEmbeddingToolRouter.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace RDK::LLM {

namespace {

std::filesystem::path defaultToolsEmbeddingIndexPath()
{
    for(int i = 0; i < 8; ++i)
    {
        std::filesystem::path root = std::filesystem::current_path();
        for(int up = 0; up < i; ++up)
        {
            if(root.has_parent_path())
                root = root.parent_path();
        }
        const std::filesystem::path candidate = root / "LLM/index/tools-embeddings.jsonl";
        if(std::filesystem::is_regular_file(candidate))
            return candidate;
    }
    return {};
}

bool indexHasToolName(const std::filesystem::path& index_path, const std::string& tool_name)
{
    std::ifstream in(index_path);
    std::string line;
    while(std::getline(in, line))
    {
        if(line.empty())
            continue;
        try
        {
            const nlohmann::json j = nlohmann::json::parse(line);
            if(j.value("name", std::string()) == tool_name)
                return true;
        }
        catch(...)
        {
        }
    }
    return false;
}

} // namespace

SearchToolsResult searchToolsHybrid(ULLMToolRegistry& registry, const std::string& query, int top_k)
{
    SearchToolsResult out = searchToolsByQuery(registry, query, top_k);

    const char* path_env = std::getenv("NMSDK_LLM_TOOL_EMBED_INDEX");
    std::filesystem::path index_path;
    if(path_env && path_env[0])
        index_path = path_env;
    else
        index_path = defaultToolsEmbeddingIndexPath();

    if(!index_path.empty() && std::filesystem::is_regular_file(index_path))
    {
        out.index_version = 2;
        std::vector<std::string> boosted;
        for(const std::string& name : out.tools)
        {
            if(indexHasToolName(index_path, name))
                boosted.push_back(name);
        }
        for(const std::string& name : out.tools)
        {
            if(!indexHasToolName(index_path, name))
                boosted.push_back(name);
        }
        if(boosted.size() > static_cast<size_t>(top_k))
            boosted.resize(static_cast<size_t>(top_k));
        out.tools = std::move(boosted);
    }

    return out;
}

} // namespace RDK::LLM
