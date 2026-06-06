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

std::optional<SuccessCriteria> successCriteriaFromJson(const nlohmann::json& j)
{
    if(!j.is_object())
        return std::nullopt;
    SuccessCriteria c;
    c.type = j.value("type", "");
    c.params = j.value("params", nlohmann::json::object());
    if(c.type.empty())
        return std::nullopt;
    return c;
}

nlohmann::json successCriteriaToJson(const SuccessCriteria& c)
{
    return {{"type", c.type}, {"params", c.params}};
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
    if(!plan.goal_en.empty())
        j["goal_en"] = plan.goal_en;
    j["confidence"] = plan.confidence;
    if(plan.goal_success)
        j["goal_success"] = successCriteriaToJson(*plan.goal_success);
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
        if(step.success)
            step_j["success"] = successCriteriaToJson(*step.success);
        if(step.repeat_count > 1)
            step_j["repeat_count"] = step.repeat_count;
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
    plan.goal_en = j.value("goal_en", "");
    plan.confidence = j.value("confidence", 0.f);
    if(j.contains("goal_success"))
        plan.goal_success = successCriteriaFromJson(j["goal_success"]);
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
        if(item.contains("success"))
            step.success = successCriteriaFromJson(item["success"]);
        step.repeat_count = item.value("repeat_count", 1);
        if(step.repeat_count < 1)
            step.repeat_count = 1;
        if(step.tool_name.empty())
            return std::nullopt;
        plan.steps.push_back(step);
    }
    if(plan.plan_id.empty())
        plan.plan_id = makePlanId();
    return plan;
}

std::optional<ULLMExecutionPlan> executionPlanFromProposePlanArguments(const nlohmann::json& args)
{
    if(!args.contains("steps") || !args["steps"].is_array() || args["steps"].empty())
        return std::nullopt;

    nlohmann::json plan_j = nlohmann::json::object();
    plan_j["goal_en"] = args.value("goal", std::string());
    plan_j["confidence"] = args.value("confidence", 0.8f);
    plan_j["requires_user_confirmation"] = true;
    plan_j["steps"] = nlohmann::json::array();

    int step_id = 1;
    for(const nlohmann::json& item : args["steps"])
    {
        if(!item.is_object())
            continue;
        nlohmann::json step_j;
        step_j["step_id"] = step_id++;
        step_j["tool_name"] = item.value("tool_name", item.value("tool", std::string()));
        step_j["arguments"] = item.value("arguments", nlohmann::json::object());
        if(step_j["tool_name"].get<std::string>().empty())
            return std::nullopt;
        plan_j["steps"].push_back(std::move(step_j));
    }
    if(plan_j["steps"].empty())
        return std::nullopt;
    return executionPlanFromJson(plan_j);
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
    const nlohmann::json success_schema = {
        {"type", "object"},
        {"properties",
         {{"type", {{"type", "string"}}},
          {"params", {{"type", "object"}}}}},
        {"required", nlohmann::json::array({"type"})},
        {"additionalProperties", false}};

    const nlohmann::json step_schema = {
        {"type", "object"},
        {"properties",
         {{"step_id", {{"type", "integer"}}},
          {"tool_name", {{"type", "string"}}},
          {"arguments", {{"type", "object"}}},
          {"depends_on", {{"type", "array"}, {"items", {{"type", "integer"}}}}},
          {"success", success_schema},
          {"repeat_count", {{"type", "integer"}, {"minimum", 1}}}}},
        {"required", nlohmann::json::array({"step_id", "tool_name", "arguments"})},
        {"additionalProperties", false}};
    const nlohmann::json root_schema = {
        {"type", "object"},
        {"properties",
         {{"plan_id", {{"type", "string"}}},
          {"requires_user_confirmation", {{"type", "boolean"}}},
          {"goal_en", {{"type", "string"}}},
          {"confidence", {{"type", "number"}, {"minimum", 0}, {"maximum", 1}}},
          {"goal_success", success_schema},
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
        oss << "  " << step.step_id << ". " << step.tool_name;
        if(step.tool_name == "add_component" && step.arguments.contains("class_name")
           && step.arguments["class_name"].is_string())
            oss << " (" << step.arguments["class_name"].get<std::string>() << ")";
        if(step.tool_name == "connect_components")
        {
            const std::string from = step.arguments.value("from_long_name", "");
            const std::string to = step.arguments.value("to_long_name", "");
            const std::string from_prop = step.arguments.value("from_property", "");
            const std::string to_prop = step.arguments.value("to_property", "");
            if(!from.empty() || !to.empty())
                oss << " (" << from << "→" << to << ")";
            if(!from_prop.empty() || !to_prop.empty())
                oss << " [" << from_prop << "→" << to_prop << "]";
        }
        if(step.repeat_count > 1)
            oss << " x" << step.repeat_count;
        oss << " [" << step.status << "]";
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
