#ifndef RDK_LLM_E2E_SCENARIOS_H
#define RDK_LLM_E2E_SCENARIOS_H

#include <string>
#include <vector>

#include "llm_e2e_analyzer.h"

namespace RDK::LLM::E2eLab {

struct E2eScenarioDef {
    std::string id;
    std::string user_message;
    bool project_loaded = false;
    bool auto_confirm_pending = true;
    E2eScenarioExpectation expectation;
};

std::vector<E2eScenarioDef> allNaturalLanguageScenarios();

} // namespace RDK::LLM::E2eLab

#endif
