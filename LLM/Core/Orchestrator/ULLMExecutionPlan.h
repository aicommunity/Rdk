#ifndef RDK_ULLM_EXECUTION_PLAN_H
#define RDK_ULLM_EXECUTION_PLAN_H

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

struct LLMProviderProfile;

struct SuccessCriteria {
    std::string type; // tool_ok | component_count | link_exists | goal_component_count
    nlohmann::json params = nlohmann::json::object();
};

struct ExecutionPlanStep {
    int step_id = 0;
    std::string tool_name;
    nlohmann::json arguments = nlohmann::json::object();
    std::vector<int> depends_on;
    std::string status = "pending";
    /// Last tool result when status is done (checkpoint / rollback, TD-023).
    nlohmann::json last_result = nlohmann::json::object();
    std::optional<SuccessCriteria> success;
    int repeat_count = 1;
};

struct ULLMExecutionPlan {
    std::string plan_id;
    std::vector<ExecutionPlanStep> steps;
    bool requires_user_confirmation = true;
    std::string goal_en;
    float confidence = 0.f;
    std::optional<SuccessCriteria> goal_success;
    /// Set when execution stopped mid-plan and may be resumed (TD-023).
    bool paused = false;
    int checkpoint_after_step_id = 0;
};

/// Reset failed/skipped steps to pending; keep done steps (resume entry point).
void prepareExecutionPlanForResume(ULLMExecutionPlan& plan);

bool executionPlanHasCheckpoint(const ULLMExecutionPlan& plan);

std::optional<ULLMExecutionPlan> parseExecutionPlanFromAssistantText(const std::string& text);

nlohmann::json executionPlanToJson(const ULLMExecutionPlan& plan);
std::optional<ULLMExecutionPlan> executionPlanFromJson(const nlohmann::json& j);

/// Build a plan from the `propose_plan` agent tool arguments (`goal`, `steps[]`).
std::optional<ULLMExecutionPlan> executionPlanFromProposePlanArguments(const nlohmann::json& args);

std::string formatExecutionPlanPreview(const ULLMExecutionPlan& plan);

/// OpenAI `response_format` for strict execution-plan JSON (cloud OpenAI-compat only).
nlohmann::json executionPlanOpenAiResponseFormat();

bool providerSupportsStrictPlanSchema(const LLMProviderProfile& profile);

} // namespace RDK::LLM

#endif
