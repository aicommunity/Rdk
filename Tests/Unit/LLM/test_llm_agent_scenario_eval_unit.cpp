#include <gtest/gtest.h>

#include "llm_agent_scenario_eval.h"
#include "llm_agent_scenario_loader.h"

using namespace RDK::LLM::AgentScenarios;

TEST(AgentScenarioEvalUnit, ParseExpectFields)
{
    const nlohmann::json j = {{"orchestrator_ok", true},
                              {"no_suitable_tool", false},
                              {"final_text_not_contains", nlohmann::json::array({"Done."})},
                              {"provider_rounds_max", 2}};
    const AgentScenarioExpect e = parseExpect(j);
    ASSERT_TRUE(e.orchestrator_ok);
    EXPECT_FALSE(*e.no_suitable_tool);
    ASSERT_EQ(e.final_text_not_contains.size(), 1u);
    ASSERT_EQ(*e.provider_rounds_max, 2);
}
