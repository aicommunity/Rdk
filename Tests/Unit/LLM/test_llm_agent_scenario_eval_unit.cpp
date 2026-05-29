#include <gtest/gtest.h>

#include "llm_agent_scenario_eval.h"
#include "llm_agent_scenario_loader.h"

using namespace RDK::LLM::AgentScenarios;

TEST(AgentScenarioEvalUnit, ParseExpectFields)
{
    const nlohmann::json j = {{"orchestrator_ok", true},
                              {"no_suitable_tool", false},
                              {"final_text_not_contains", nlohmann::json::array({"Done."})},
                              {"provider_rounds_max", 2},
                              {"phase_entered", "Idle"},
                              {"tool_loop_entered", true},
                              {"audit_events_any", nlohmann::json::array({"tool_loop_entered"})},
                              {"turn_terminal", "Completed"}};
    const AgentScenarioExpect e = parseExpect(j);
    ASSERT_TRUE(e.orchestrator_ok);
    EXPECT_FALSE(*e.no_suitable_tool);
    ASSERT_EQ(e.final_text_not_contains.size(), 1u);
    ASSERT_EQ(*e.provider_rounds_max, 2);
    ASSERT_EQ(e.phase_entered, "Idle");
    ASSERT_TRUE(e.tool_loop_entered);
    ASSERT_EQ(e.audit_events_any.size(), 1u);
    ASSERT_EQ(e.turn_terminal, "Completed");
}

TEST(AgentScenarioEvalUnit, EvaluatesToolLoopAuditFields)
{
    AgentScenarioCase scenario;
    scenario.expect.tool_loop_entered = true;
    scenario.expect.phase_entered = "Idle";
    scenario.expect.turn_terminal = "Completed";

    AgentScenarioRun run;
    run.final_workflow_phase = "Idle";
    run.audit_event_types = {"unified_turn_started", "tool_loop_entered"};
    run.final_response.ok = true;
    run.final_response.turn_terminal = "Completed";

    const AgentScenarioVerdict v = evaluateDeterministic(scenario, run);
    EXPECT_TRUE(v.passed) << v.diagnosis;
}
