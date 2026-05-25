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
};

struct ULLMExecutionPlan {
    std::string plan_id;
    std::vector<ExecutionPlanStep> steps;
    bool requires_user_confirmation = true;
};

std::optional<ULLMExecutionPlan> parseExecutionPlanFromAssistantText(const std::string& text);

nlohmann::json executionPlanToJson(const ULLMExecutionPlan& plan);
std::optional<ULLMExecutionPlan> executionPlanFromJson(const nlohmann::json& j);

} // namespace RDK::LLM

#endif
