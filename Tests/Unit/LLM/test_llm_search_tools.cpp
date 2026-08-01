#include <gtest/gtest.h>

#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMEmbeddingToolRouter.h"
#include "Tools/ULLMSearchTools.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"

using namespace RDK::LLM;

namespace {

bool containsTool(const SearchToolsResult& found, const std::string& name)
{
    for(const std::string& t : found.tools)
        if(t == name)
            return true;
    return false;
}

} // namespace

TEST(SearchTools, RanksByQueryTokens)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);
    RegisterAgentTools(registry);

    const SearchToolsResult found = searchToolsByQuery(registry, "connect components link", 5);
    EXPECT_FALSE(found.tools.empty());
    bool has_connect = false;
    for(const std::string& t : found.tools)
        if(t == "connect_components")
            has_connect = true;
    EXPECT_TRUE(has_connect);
}

TEST(SearchTools, HybridDiscoversLongTailTools)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);
    RegisterAgentTools(registry);

    // TD-146 / TD-163: deterministic miss-rate fixture for progressive disclosure.
    const SearchToolsResult links =
        searchToolsHybrid(registry, "list model links topology wiring", 12);
    EXPECT_TRUE(containsTool(links, "list_model_links"))
        << "list_model_links must appear for link/topology queries";

    const SearchToolsResult sub =
        searchToolsHybrid(registry, "spawn explore subagent read-only", 12);
    EXPECT_TRUE(containsTool(sub, "spawn_explore_subagent"))
        << "spawn_explore_subagent must appear for explore/subagent queries";
}
