#ifndef RDK_ULLM_ACT_OR_CLARIFY_GATE_H
#define RDK_ULLM_ACT_OR_CLARIFY_GATE_H

#include "../LlmTypes.h"
#include "ULLMConfigurationLifecycle.h"

#include <string>

namespace RDK::LLM {

/// DD-ACT-001: when tools were offered, actionable goals must end with a tool call,
/// ask_user / pending, or NO_SUITABLE_TOOL — not free-form essay (esp. after thinking).
bool isActionableGoalForActOrClarify(const std::string& planning_text, LLMIntentKind intent,
                                     ConfigurationLifecycleAction lifecycle_action,
                                     bool filter_include_write);

/// True when empty tool_calls must trigger recovery / NO_SUITABLE_TOOL instead of prose.
bool shouldRequireActOrClarify(bool provider_tools_offered, bool tool_calls_empty,
                               const std::string& planning_text, LLMIntentKind intent,
                               ConfigurationLifecycleAction lifecycle_action,
                               bool filter_include_write, bool has_pending_tool_arguments,
                               bool in_understanding_phase);

} // namespace RDK::LLM

#endif
