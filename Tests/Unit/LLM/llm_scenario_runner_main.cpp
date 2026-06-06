#include <iostream>
#include <string>

#include "llm_agent_scenario_eval.h"
#include "llm_agent_scenario_loader.h"
#include "llm_agent_scenario_runner.h"
#include "llm_e2e_analyzer.h"
#include "llm_e2e_lab_harness.h"
#include "llm_test_ollama_lab.h"

using namespace RDK::LLM;
using namespace RDK::LLM::AgentScenarios;

#ifndef RDK_LLM_AGENT_SCENARIOS_DIR
#define RDK_LLM_AGENT_SCENARIOS_DIR "Rdk/Tests/Fixtures/LLM/agent-scenarios"
#endif

namespace {

bool matchTier(const AgentScenarioCase& c, const std::string& tier)
{
    if(tier.empty() || tier == "all")
        return true;
    if(tier == "deterministic")
        return c.tier == AgentScenarioTier::Deterministic;
    if(tier == "e2e")
        return c.tier == AgentScenarioTier::E2e;
    return true;
}

bool matchFilter(const AgentScenarioCase& c, const std::string& filter)
{
    if(filter.empty())
        return true;
    return c.id.find(filter) != std::string::npos;
}

} // namespace

int main(int argc, char** argv)
{
    std::string tier = "deterministic";
    std::string filter;
    std::string root = RDK_LLM_AGENT_SCENARIOS_DIR;
    for(int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if(arg == "--tier" && i + 1 < argc)
            tier = argv[++i];
        else if(arg == "--filter" && i + 1 < argc)
            filter = argv[++i];
        else if(arg == "--root" && i + 1 < argc)
            root = argv[++i];
    }

    nlohmann::json report = nlohmann::json::array();
    int failed = 0;

    for(const AgentScenarioCase& scenario : loadAllAgentScenarios(root))
    {
        if(!matchTier(scenario, tier) || !matchFilter(scenario, filter))
            continue;

        nlohmann::json row;
        row["id"] = scenario.id;
        row["tier"] = scenario.tier == AgentScenarioTier::E2e ? "e2e" : "deterministic";

        if(scenario.tier == AgentScenarioTier::Deterministic)
        {
            AgentScenarioHarness harness = makeHarness(scenario.registry_profile);
            const AgentScenarioRun run = runDeterministicScenario(harness, scenario);
            const AgentScenarioVerdict v = evaluateDeterministic(scenario, run);
            row["passed"] = v.passed;
            row["diagnosis"] = v.diagnosis;
            row["tools"] = run.digest.tools_invoked;
            row["provider_rounds"] = run.provider_invoke_count;
            if(!v.passed)
                ++failed;
        }
        else
        {
            if(!RDK::LLM::Test::isLabOllamaReachable())
            {
                row["passed"] = false;
                row["skipped"] = true;
                row["diagnosis"] = "Ollama lab unreachable";
                report.push_back(row);
                continue;
            }
            std::string skip;
            RDK::LLM::E2eLab::Harness lab;
            const AgentScenarioRun run = runE2eScenario(lab, scenario);
            if(RDK::LLM::E2eLab::shouldSkipForProviderError(run.final_response, &skip))
            {
                row["passed"] = false;
                row["skipped"] = true;
                row["diagnosis"] = skip;
            }
            else
            {
                const E2eLab::E2eScenarioEvaluation ev = evaluateE2e(scenario, run);
                row["passed"] = ev.passed;
                row["diagnosis"] = ev.diagnosis;
                row["tools"] = run.digest.tools_invoked;
                if(!ev.passed)
                    ++failed;
            }
        }
        report.push_back(row);
    }

    std::cout << report.dump(2) << std::endl;
    return failed == 0 ? 0 : 1;
}
