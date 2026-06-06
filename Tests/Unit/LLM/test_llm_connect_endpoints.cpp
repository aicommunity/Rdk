#include <gtest/gtest.h>

#include "Domain/ULLMConnectEndpoints.h"
#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMConnectPlanParsing.h"
#include "Session/ULLMConversationStore.h"

using namespace RDK::LLM;

TEST(LLMConnectEndpoints, SessionDeltaWithoutState)
{
    URdkDomainAccess domain(nullptr);
    ParsedConnectGoal parsed;
    parsed.remaining_scope = ConnectRemainingScope::SessionDelta;
    nlohmann::json components = nlohmann::json::array();
    bool incomplete = false;
    const std::vector<std::string> out =
        collectRemainingEndpoints(domain, components, parsed, nullptr, 0, incomplete);
    EXPECT_TRUE(out.empty());
    EXPECT_FALSE(incomplete);
}

TEST(LLMConnectEndpoints, ModelGraphWithoutEngineIncomplete)
{
    URdkDomainAccess domain(nullptr);
    ParsedConnectGoal parsed;
    parsed.remaining_scope = ConnectRemainingScope::ModelGraph;
    nlohmann::json components = nlohmann::json::array({{{"long_name", "/ch0/A"}}});
    bool incomplete = false;
    const std::vector<std::string> out =
        collectRemainingEndpoints(domain, components, parsed, nullptr, 0, incomplete);
    EXPECT_TRUE(out.empty());
    EXPECT_TRUE(incomplete);
}
