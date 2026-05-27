#include "ULLMAutonomousPolicy.h"

#include "ULLMPolicyLimits.h"

#include <unordered_set>

namespace RDK::LLM {

namespace {

const std::unordered_set<std::string>& autonomousReadTools()
{
    static const std::unordered_set<std::string> k = {
        "get_net_snapshot",
        "find_component",
        "get_component_properties",
        "list_registered_classes",
        "describe_class",
        "validate_project",
    };
    return k;
}

const std::unordered_set<std::string>& autonomousWriteTools()
{
    static const std::unordered_set<std::string> k = {
        "add_component",
        "connect_components",
        "disconnect_components",
        "set_property",
        "remove_component",
    };
    return k;
}

} // namespace

bool ULLMAutonomousPolicy::isAutonomousReadTool(const std::string& tool_name)
{
    return autonomousReadTools().count(tool_name) > 0;
}

bool ULLMAutonomousPolicy::isAutonomousWriteTool(const std::string& tool_name)
{
    return autonomousWriteTools().count(tool_name) > 0;
}

bool ULLMAutonomousPolicy::isToolWhitelisted(const std::string& tool_name, LLMAutonomousMode mode)
{
    if(mode == LLMAutonomousMode::Off)
        return true;
    return isAutonomousReadTool(tool_name) || isAutonomousWriteTool(tool_name);
}

AutonomousStepDecision ULLMAutonomousPolicy::checkStep(const std::string& tool_name,
                                                       LLMAutonomousMode mode, int steps_taken,
                                                       int max_steps)
{
    AutonomousStepDecision out;
    if(mode == LLMAutonomousMode::Off)
        return out;

    if(!isToolWhitelisted(tool_name, mode))
    {
        out.allowed = false;
        out.deny_code = "AUTONOMOUS_TOOL_NOT_WHITELISTED";
        out.deny_message = "Tool not allowed in autonomous mode: " + tool_name;
        return out;
    }

    if(isAutonomousReadTool(tool_name))
        return out;

    if(steps_taken >= max_steps)
    {
        out.allowed = false;
        out.deny_code = "AUTONOMOUS_STEP_LIMIT";
        out.deny_message = "Autonomous step limit reached (" + std::to_string(max_steps) + ")";
        return out;
    }

    return out;
}

} // namespace RDK::LLM
