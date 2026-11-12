#include <gtest/gtest.h>

#include <cstdlib>
#include <string>

#include "llm_agent_scenario_eval.h"
#include "llm_agent_scenario_loader.h"
#include "llm_agent_scenario_runner.h"
#include "llm_e2e_lab_harness.h"
#include "llm_test_ollama_lab.h"

using namespace RDK::LLM;
using namespace RDK::LLM::AgentScenarios;

#ifndef RDK_LLM_AGENT_SCENARIOS_DIR
#define RDK_LLM_AGENT_SCENARIOS_DIR "Rdk/Tests/Fixtures/LLM/agent-scenarios"
#endif

namespace {

std::string fixturesRoot()
{
    return RDK_LLM_AGENT_SCENARIOS_DIR;
}

bool scenarioIdMatchesFilter(const std::string& id, const std::string& filter)
{
    if(filter.empty())
        return true;
    return id.find(filter) != std::string::npos;
}

std::vector<AgentScenarioCase> loadFiltered(AgentScenarioTier tier, const std::string& filter = "")
{
    std::vector<AgentScenarioCase> all = loadAllAgentScenarios(fixturesRoot());
    std::vector<AgentScenarioCase> out;
    for(AgentScenarioCase& c : all)
    {
        if(c.tier != tier)
            continue;
        if(!scenarioIdMatchesFilter(c.id, filter))
            continue;
        out.push_back(std::move(c));
    }
    return out;
}

} // namespace

TEST(AgentScenarioLoader, LoadsManifestScenarios)
{
    const std::vector<AgentScenarioCase> cases = loadAllAgentScenarios(fixturesRoot());
    EXPECT_GE(cases.size(), 10u) << "Expected agent-scenarios fixtures under " << fixturesRoot();
}

class AgentDeterministic : public ::testing::TestWithParam<AgentScenarioCase> {};

TEST_P(AgentDeterministic, FromFixtures)
{
    const AgentScenarioCase scenario = GetParam();
    AgentScenarioHarness harness = makeHarness(scenario.registry_profile);
    const AgentScenarioRun run = runDeterministicScenario(harness, scenario);
    const AgentScenarioVerdict verdict = evaluateDeterministic(scenario, run);
    EXPECT_TRUE(verdict.passed) << "[" << scenario.id << "] " << verdict.diagnosis << "\n"
                                << "Source: " << scenario.source_path;
}

INSTANTIATE_TEST_SUITE_P(
    Deterministic,
    AgentDeterministic,
    ::testing::ValuesIn(loadFiltered(AgentScenarioTier::Deterministic)),
    [](const ::testing::TestParamInfo<AgentScenarioCase>& info) { return info.param.id; });

class AgentE2e : public ::testing::TestWithParam<AgentScenarioCase> {
protected:
    void SetUp() override
    {
        RDK::LLM::Test::skipIfLabOllamaUnreachable();
        setenv("NMSDK_LLM_PATH_POLICY_STRICT", "0", 1);
    }
};

TEST_P(AgentE2e, FromFixtures)
{
    const AgentScenarioCase scenario = GetParam();
    if(scenario.e2e && scenario.e2e->use_thinking_profile)
    {
        // Env from fixture is applied inside runE2eScenario before rebind; probe uses
        // NMSDK_LLM_OLLAMA_THINKING_MODEL if already set in the process.
        for(const auto& [key, value] : scenario.env)
        {
            if(!value.empty())
                setenv(key.c_str(), value.c_str(), 1);
        }
        RDK_LLM_SKIP_IF_LAB_THINKING_MODEL_MISSING();
    }
    E2eLab::Harness lab;
    const AgentScenarioRun run = runE2eScenario(lab, scenario);

    std::string skip_reason;
    if(E2eLab::shouldSkipForProviderError(run.final_response, &skip_reason))
        GTEST_SKIP() << scenario.id << ": " << skip_reason;

    const E2eLab::E2eScenarioEvaluation eval = evaluateE2e(scenario, run);
    EXPECT_TRUE(eval.passed) << "[" << scenario.id << "] " << eval.diagnosis;
}

INSTANTIATE_TEST_SUITE_P(
    E2e,
    AgentE2e,
    ::testing::ValuesIn(loadFiltered(AgentScenarioTier::E2e)),
    [](const ::testing::TestParamInfo<AgentScenarioCase>& info) { return info.param.id; });

TEST(AgentScenarioEvalUnit, RejectsForbiddenTool)
{
    AgentScenarioCase scenario;
    scenario.id = "unit";
    scenario.expect.tools_invoked_any = {"get_net_snapshot"};
    scenario.expect.tools_forbidden = {"add_component"};

    AgentScenarioRun run;
    run.final_response.ok = true;
    run.digest.orchestrator_ok = true;
    run.digest.tools_invoked = {"add_component"};

    const AgentScenarioVerdict v = evaluateDeterministic(scenario, run);
    EXPECT_FALSE(v.passed);
}
