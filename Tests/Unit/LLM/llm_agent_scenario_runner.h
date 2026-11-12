#ifndef RDK_LLM_AGENT_SCENARIO_RUNNER_H
#define RDK_LLM_AGENT_SCENARIO_RUNNER_H

#include <memory>

#include "llm_agent_scenario_types.h"
#include "llm_e2e_lab_harness.h"
#include "Providers/ULLMMockProvider.h"

namespace RDK::LLM::AgentScenarios {

struct AgentScenarioHarness {
    std::unique_ptr<ULLMMockProvider> mock_provider;
};

void registerToolsForProfile(RegistryProfile profile, ULLMToolRegistry& registry);

AgentScenarioHarness makeHarness(RegistryProfile profile);

LLMSessionContext sessionFromSpec(const AgentSessionSpec& spec, const std::string& session_id);

void enqueueMockScript(ULLMMockProvider& provider, const std::vector<nlohmann::json>& script);

AgentScenarioRun runDeterministicScenario(AgentScenarioHarness& harness, const AgentScenarioCase& scenario);

AgentScenarioRun runE2eScenario(E2eLab::Harness& lab, const AgentScenarioCase& scenario);

} // namespace RDK::LLM::AgentScenarios

#endif
