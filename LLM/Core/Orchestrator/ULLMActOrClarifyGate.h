#ifndef RDK_ULLM_ACT_OR_CLARIFY_GATE_H
#define RDK_ULLM_ACT_OR_CLARIFY_GATE_H

#include "../LlmTypes.h"
#include "ULLMConfigurationLifecycle.h"

#include <string>
#include <vector>

namespace RDK::LLM {

/// DD-ACT-001: when tools were offered, actionable goals must end with a tool call,
/// ask_user / pending, or NO_SUITABLE_TOOL — not free-form essay (esp. after thinking).
/// Query/Explain count as actionable until the turn has tool evidence (then prose is allowed).
bool isActionableGoalForActOrClarify(const std::string& planning_text, LLMIntentKind intent,
                                     ConfigurationLifecycleAction lifecycle_action,
                                     bool filter_include_write);

/// True when the turn trace has at least one ok informational/read tool and no write tools.
/// Used so Mutate false-positives (e.g. «расскажи о проекте») can still answer after snapshot.
bool turnHasSuccessfulReadOnlyEvidence(const std::vector<TurnToolInvocationView>& trace);

/// True when empty tool_calls must trigger recovery / NO_SUITABLE_TOOL instead of prose.
/// Prose is allowed only after successful read-only tool evidence (failed tools do not unlock).
bool shouldRequireActOrClarify(bool provider_tools_offered, bool tool_calls_empty,
                               const std::string& planning_text, LLMIntentKind intent,
                               ConfigurationLifecycleAction lifecycle_action,
                               bool filter_include_write, bool has_pending_tool_arguments,
                               bool in_understanding_phase,
                               bool has_turn_tool_evidence = false,
                               bool has_successful_read_only_evidence = false);

} // namespace RDK::LLM

#endif
