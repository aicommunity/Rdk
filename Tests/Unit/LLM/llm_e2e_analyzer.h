#ifndef RDK_LLM_E2E_ANALYZER_H
#define RDK_LLM_E2E_ANALYZER_H

#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "LlmTypes.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Session/ULLMConversationStore.h"

namespace RDK::LLM::E2eLab {

struct E2eConversationDigest {
    bool orchestrator_ok = false;
    std::string orchestrator_error;
    std::string assistant_text;
    bool had_pending_confirmation = false;
    std::string pending_tool_name;
    std::vector<std::string> tools_invoked;
    std::map<std::string, nlohmann::json> tool_results_by_name;
};

enum class E2eGoalKind {
    OrchestratorOk,
    ToolInvoked,
    ValidateRan,
    ClassesListed,
    LifecycleToolUsed,
    LoadRequested,
    CreateRequested,
    InformativeAnswer,
};

struct E2eScenarioExpectation {
    std::string id;
    std::vector<std::string> expected_tools_any;
    std::vector<std::string> forbidden_tools;
    E2eGoalKind goal = E2eGoalKind::ToolInvoked;
    bool require_orchestrator_ok = true;
    bool allow_app_unavailable_result = true;
};

struct E2eScenarioEvaluation {
    bool passed = false;
    std::string diagnosis;
};

bool digestUsedAnyTool(const E2eConversationDigest& digest,
                       const std::vector<std::string>& tool_names);

bool digestUsedForbiddenTool(const E2eConversationDigest& digest,
                             const std::vector<std::string>& forbidden);

E2eScenarioEvaluation evaluateScenario(const E2eConversationDigest& digest,
                                       const E2eScenarioExpectation& expectation);

std::string toLowerAscii(std::string s);
bool textMentionsAny(const std::string& text, const std::vector<std::string>& needles);

} // namespace RDK::LLM::E2eLab

#endif
