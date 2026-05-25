#ifndef RDK_ULLM_EXECUTION_PLAN_H
#define RDK_ULLM_EXECUTION_PLAN_H

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace RDK::LLM {

struct ExecutionPlanStep {
    int step_id = 0;
    std::string tool_name;
    nlohmann::json arguments = nlohmann::json::object();
    std::vector<int> depends_on;
    std::string status = "pending";
    /// Last tool result when status is done (checkpoint / rollback, TD-023).
    nlohmann::json last_result = nlohmann::json::object();
};

struct ULLMExecutionPlan {
    std::string plan_id;
    std::vector<ExecutionPlanStep> steps;
    bool requires_user_confirmation = true;
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

std::string formatExecutionPlanPreview(const ULLMExecutionPlan& plan);

} // namespace RDK::LLM

#endif
