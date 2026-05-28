#ifndef RDK_LLM_AGENT_SCENARIO_LOADER_H
#define RDK_LLM_AGENT_SCENARIO_LOADER_H

#include <string>
#include <vector>

#include "llm_agent_scenario_types.h"

namespace RDK::LLM::AgentScenarios {

std::vector<AgentScenarioCase> loadAllAgentScenarios(const std::string& root_dir);

AgentScenarioExpect parseExpect(const nlohmann::json& j);
AgentScenarioCase parseScenarioFile(const nlohmann::json& root, const std::string& source_path);

} // namespace RDK::LLM::AgentScenarios

#endif
