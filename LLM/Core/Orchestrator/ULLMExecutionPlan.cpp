#include "ULLMExecutionPlan.h"

#include "../LlmTypes.h"

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
    j["paused"] = plan.paused;
    j["checkpoint_after_step_id"] = plan.checkpoint_after_step_id;
    j["steps"] = nlohmann::json::array();
    for(const ExecutionPlanStep& step : plan.steps)
    {
        nlohmann::json step_j = {{"step_id", step.step_id},
                                 {"tool_name", step.tool_name},
                                 {"arguments", step.arguments},
                                 {"depends_on", step.depends_on},
                                 {"status", step.status}};
        if(!step.last_result.empty())
            step_j["last_result"] = step.last_result;
        j["steps"].push_back(std::move(step_j));
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
    plan.paused = j.value("paused", false);
    plan.checkpoint_after_step_id = j.value("checkpoint_after_step_id", 0);

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
        step.last_result = item.value("last_result", nlohmann::json::object());
        if(step.tool_name.empty())
            return std::nullopt;
        plan.steps.push_back(step);
    }
    if(plan.plan_id.empty())
        plan.plan_id = makePlanId();
    return plan;
}

void prepareExecutionPlanForResume(ULLMExecutionPlan& plan)
{
    for(ExecutionPlanStep& step : plan.steps)
    {
        if(step.status == "done")
            continue;
        step.status = "pending";
    }
    plan.paused = false;
}

bool executionPlanHasCheckpoint(const ULLMExecutionPlan& plan)
{
    if(plan.paused)
        return true;
    for(const ExecutionPlanStep& step : plan.steps)
    {
        if(step.status == "done" || step.status == "failed")
            return true;
    }
    return false;
}

nlohmann::json executionPlanOpenAiResponseFormat()
{
    const nlohmann::json step_schema = {
        {"type", "object"},
        {"properties",
         {{"step_id", {{"type", "integer"}}},
          {"tool_name", {{"type", "string"}}},
          {"arguments", {{"type", "object"}}},
          {"depends_on", {{"type", "array"}, {"items", {{"type", "integer"}}}}}}},
        {"required", nlohmann::json::array({"step_id", "tool_name", "arguments"})},
        {"additionalProperties", false}};
    const nlohmann::json root_schema = {
        {"type", "object"},
        {"properties",
         {{"plan_id", {{"type", "string"}}},
          {"requires_user_confirmation", {{"type", "boolean"}}},
          {"steps", {{"type", "array"}, {"items", step_schema}}}}},
        {"required", nlohmann::json::array({"steps"})},
        {"additionalProperties", false}};
    return {{"type", "json_schema"},
            {"json_schema",
             {{"name", "execution_plan"}, {"strict", true}, {"schema", root_schema}}}};
}

bool providerSupportsStrictPlanSchema(const LLMProviderProfile& profile)
{
    if(profile.kind == LLMProviderKind::OllamaOpenAICompat
       || profile.kind == LLMProviderKind::OllamaNative || profile.kind == LLMProviderKind::EmbeddedLlama
       || profile.kind == LLMProviderKind::Mock)
        return false;
    return profile.kind == LLMProviderKind::OpenAICompat;
}

std::string formatExecutionPlanPreview(const ULLMExecutionPlan& plan)
{
    std::ostringstream oss;
    oss << "Plan " << plan.plan_id << " (" << plan.steps.size() << " steps)";
    if(plan.paused)
        oss << " [paused at step " << plan.checkpoint_after_step_id << "]";
    oss << ":\n";
    for(const ExecutionPlanStep& step : plan.steps)
    {
        oss << "  " << step.step_id << ". " << step.tool_name << " [" << step.status << "]";
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
