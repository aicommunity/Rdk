#include <gtest/gtest.h>

#include "Tools/RegisterAgentTools.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMSearchTools.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"

using namespace RDK::LLM;

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
