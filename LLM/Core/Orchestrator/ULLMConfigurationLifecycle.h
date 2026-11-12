#ifndef RDK_ULLM_CONFIGURATION_LIFECYCLE_H
#define RDK_ULLM_CONFIGURATION_LIFECYCLE_H

#include "../LlmTypes.h"

#include <optional>
#include <string>

namespace RDK::LLM {

enum class ConfigurationLifecycleAction {
    None,
    Create,
    Load,
    Save,
    Close,
    Validate,
};

ConfigurationLifecycleAction detectConfigurationLifecycleAction(const std::string& user_text);

/// User asked to open the most recent configuration (EN: last/recent; RU: последний/недавний).
bool wantsRecentConfiguration(const std::string& user_text);

bool isConfigurationLifecycleToolName(const std::string& tool_name);

std::string configurationLifecycleSystemHint(ConfigurationLifecycleAction action,
                                             bool project_loaded);

std::optional<std::string> forcedToolForLifecycle(ConfigurationLifecycleAction action,
                                                  bool project_loaded);

bool isLifecycleWriteToolName(const std::string& tool_name);

std::string formatLifecycleToolUserMessage(const std::string& tool_name,
                                             const ToolGatewayResult& result);

/// After tool gateway invoke: whether to prompt the user for missing lifecycle args.
bool toolInvokeNeedsArgumentClarification(const std::string& tool_name,
                                          const ToolGatewayResult& result);

/// Agent v2: lifecycle tools are suggested via ephemeral hints, not forced tool_choice (phase J).
bool lifecycleDirectInvokeEnabled();

bool lifecycleForceToolChoiceEnabled();

bool shouldForceLifecycleToolChoice(float intent_confidence);

} // namespace RDK::LLM

#endif
