#ifndef RDK_ULLM_CONNECT_PLAN_BUILDER_H
#define RDK_ULLM_CONNECT_PLAN_BUILDER_H

#include "ULLMConnectPlanParsing.h"
#include "ULLMExecutionPlan.h"

#include "../LlmTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace RDK::LLM {

class URdkDomainAccess;
class ULinkPatternCatalog;
struct ConversationState;

struct ConnectPlanBuildRequest {
    std::string goal_en;
    ParsedConnectGoal parsed;
    LLMSessionContext session;
    const ConversationState* state = nullptr;
    URdkDomainAccess& domain;
    ULinkPatternCatalog& catalog;
    int snapshot_step_id = 1;
    /// Test-only override to avoid needing an engine-backed domain.
    std::optional<nlohmann::json> snapshot_override;
};

struct ConnectPlanBuildResult {
    bool ok = false;
    std::vector<ExecutionPlanStep> steps;
    std::optional<SuccessCriteria> goal_success;
    std::vector<std::string> issues;
    bool needs_clarification = false;
    std::string clarification_message;
    nlohmann::json clarification_payload = nlohmann::json::object();
};

ConnectPlanBuildResult buildConnectPlanSteps(const ConnectPlanBuildRequest& req);

} // namespace RDK::LLM

#endif
