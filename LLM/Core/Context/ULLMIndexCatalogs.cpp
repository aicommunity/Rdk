#include "ULLMIndexCatalogs.h"

#include <mutex>

namespace RDK::LLM {

namespace {

std::filesystem::path resolveRepositoryRoot()
{
    if(const char* env = std::getenv("NMSDK_ROOT"))
    {
        const std::filesystem::path root(env);
        if(!root.empty() && std::filesystem::exists(root / "CMakeLists.txt"))
            return root;
    }
    std::filesystem::path root = std::filesystem::current_path();
    for(int i = 0; i < 8 && root.has_parent_path(); ++i)
    {
        if(std::filesystem::exists(root / "CMakeLists.txt"))
            return root;
        root = root.parent_path();
    }
    return {};
}

} // namespace

std::filesystem::path defaultLlmIndexRoot()
{
    const std::filesystem::path root = resolveRepositoryRoot();
    if(root.empty())
        return {};
    return root / "Bin/LLM/index";
}

const ULinkPatternCatalog& defaultLinkPatternCatalog()
{
    static std::once_flag once;
    static ULinkPatternCatalog catalog;
    std::call_once(once, []() {
        const std::filesystem::path index = defaultLlmIndexRoot();
        if(!index.empty())
            catalog.loadFromFile(index / "link-patterns.json");
    });
    return catalog;
}

} // namespace RDK::LLM
