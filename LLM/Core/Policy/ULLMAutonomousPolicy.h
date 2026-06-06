#ifndef RDK_ULLM_AUTONOMOUS_POLICY_H
#define RDK_ULLM_AUTONOMOUS_POLICY_H

#include "../LlmTypes.h"

#include <string>

namespace RDK::LLM {

struct AutonomousStepDecision {
    bool allowed = true;
    std::string deny_code;
    std::string deny_message;
};

/// Scenario D policy: whitelist and step cap when autonomous_mode != Off.
class ULLMAutonomousPolicy {
public:
    static AutonomousStepDecision checkStep(const std::string& tool_name, LLMAutonomousMode mode,
                                            int steps_taken, int max_steps);

    static bool isToolWhitelisted(const std::string& tool_name, LLMAutonomousMode mode);

    static bool isAutonomousReadTool(const std::string& tool_name);
    static bool isAutonomousWriteTool(const std::string& tool_name);
    static bool isAutonomousLifecycleTool(const std::string& tool_name);
};

} // namespace RDK::LLM

#endif
