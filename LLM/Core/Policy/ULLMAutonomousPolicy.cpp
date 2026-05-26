#include "ULLMAutonomousPolicy.h"

#include "ULLMPolicyLimits.h"

#include <unordered_set>

namespace RDK::LLM {

namespace {

const std::unordered_set<std::string>& defaultAutonomousWhitelist()
{
    static const std::unordered_set<std::string> k = {
        "get_net_snapshot",
        "list_registered_classes",
        "describe_class",
        "find_component",
        "add_component",
    };
    return k;
}

} // namespace

bool ULLMAutonomousPolicy::isToolWhitelisted(const std::string& tool_name, LLMAutonomousMode mode)
{
    if(mode == LLMAutonomousMode::Off)
        return true;
    return defaultAutonomousWhitelist().count(tool_name) > 0;
}

AutonomousStepDecision ULLMAutonomousPolicy::checkStep(const std::string& tool_name,
                                                       LLMAutonomousMode mode, int steps_taken,
                                                       int max_steps)
{
    AutonomousStepDecision out;
    if(mode == LLMAutonomousMode::Off)
        return out;

    if(steps_taken >= max_steps)
    {
        out.allowed = false;
        out.deny_code = "AUTONOMOUS_STEP_LIMIT";
        out.deny_message = "Autonomous step limit reached (" + std::to_string(max_steps) + ")";
        return out;
    }

    if(!isToolWhitelisted(tool_name, mode))
    {
        out.allowed = false;
        out.deny_code = "AUTONOMOUS_TOOL_NOT_WHITELISTED";
        out.deny_message = "Tool not allowed in autonomous mode: " + tool_name;
        return out;
    }

    return out;
}

} // namespace RDK::LLM
