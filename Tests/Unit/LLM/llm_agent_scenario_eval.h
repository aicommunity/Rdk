#ifndef RDK_LLM_AGENT_SCENARIO_EVAL_H
#define RDK_LLM_AGENT_SCENARIO_EVAL_H

#include "llm_agent_scenario_types.h"

namespace RDK::LLM::AgentScenarios {

AgentScenarioVerdict evaluateDeterministic(const AgentScenarioCase& scenario,
                                           const AgentScenarioRun& run);

E2eLab::E2eScenarioEvaluation evaluateE2e(const AgentScenarioCase& scenario,
                                    const AgentScenarioRun& run);

} // namespace RDK::LLM::AgentScenarios

#endif
