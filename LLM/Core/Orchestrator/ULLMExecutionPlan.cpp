#include "ULLMExecutionPlan.h"

#include <random>
#include <sstream>

namespace RDK::LLM {

namespace {

std::string makePlanId()
{
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist(0, 15);
    const char* hex = "0123456789abcdef";
    std::string id = "plan_";
    for(int i = 0; i < 16; ++i)
        id += hex[dist(rng)];
    return id;
}

std::optional<nlohmann::json> extractJsonPayload(const std::string& text)
{
    const std::string fence = "```json";
    const auto start = text.find(fence);
    if(start != std::string::npos)
    {
        const auto content_start = start + fence.size();
        const auto end = text.find("```", content_start);
        const std::string slice =
            text.substr(content_start, end == std::string::npos ? std::string::npos : end - content_start);
        try
        {
            return nlohmann::json::parse(slice);
        }
        catch(...)
        {
        }
    }
    try
    {
        return nlohmann::json::parse(text);
    }
    catch(...)
    {
        return std::nullopt;
    }
}

} // namespace

std::optional<ULLMExecutionPlan> parseExecutionPlanFromAssistantText(const std::string& text)
{
    const auto payload = extractJsonPayload(text);
    if(!payload)
        return std::nullopt;
    return executionPlanFromJson(*payload);
}

nlohmann::json executionPlanToJson(const ULLMExecutionPlan& plan)
{
    nlohmann::json j;
    j["plan_id"] = plan.plan_id;
    j["requires_user_confirmation"] = plan.requires_user_confirmation;
    j["steps"] = nlohmann::json::array();
    for(const ExecutionPlanStep& step : plan.steps)
    {
        j["steps"].push_back({{"step_id", step.step_id},
                              {"tool_name", step.tool_name},
                              {"arguments", step.arguments},
                              {"depends_on", step.depends_on},
                              {"status", step.status}});
    }
    return j;
}

std::optional<ULLMExecutionPlan> executionPlanFromJson(const nlohmann::json& j)
{
    if(!j.contains("steps") || !j["steps"].is_array() || j["steps"].empty())
        return std::nullopt;

    ULLMExecutionPlan plan;
    plan.plan_id = j.value("plan_id", makePlanId());
    plan.requires_user_confirmation = j.value("requires_user_confirmation", true);

    for(const auto& item : j["steps"])
    {
        ExecutionPlanStep step;
        step.step_id = item.value("step_id", 0);
        step.tool_name = item.value("tool_name", "");
        step.arguments = item.value("arguments", nlohmann::json::object());
        if(item.contains("depends_on") && item["depends_on"].is_array())
        {
            for(const auto& dep : item["depends_on"])
                step.depends_on.push_back(dep.get<int>());
        }
        step.status = item.value("status", "pending");
        if(step.tool_name.empty())
            return std::nullopt;
        plan.steps.push_back(step);
    }
    if(plan.plan_id.empty())
        plan.plan_id = makePlanId();
    return plan;
}

std::string formatExecutionPlanPreview(const ULLMExecutionPlan& plan)
{
    std::ostringstream oss;
    oss << "Plan " << plan.plan_id << " (" << plan.steps.size() << " steps):\n";
    for(const ExecutionPlanStep& step : plan.steps)
    {
        oss << "  " << step.step_id << ". " << step.tool_name;
        if(!step.depends_on.empty())
        {
            oss << " [after:";
            for(int dep : step.depends_on)
                oss << " " << dep;
            oss << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

} // namespace RDK::LLM
