#ifndef RDK_ULLM_CAPABILITY_RISK_GATE_H
#define RDK_ULLM_CAPABILITY_RISK_GATE_H

#include "../LlmTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace RDK::LLM {

/// DD-CAP-001: detect when the active Cortex model is too weak for the turn and
/// choose Continue / Escalate (one cascade) / Abstain (actionable user feedback).
enum class CapabilityRiskAction { Continue, Escalate, Abstain };

struct CapabilityRiskSignals {
    // Pre-turn
    bool weak_cortex_profile = false;
    bool complex_goal = false;
    float intent_confidence = 1.f;
    bool low_intent_confidence = false;
    bool requires_pending_write = false;

    // Mid-turn
    int aoc_recovery_count = 0;
    int tool_error_count = 0;
    int path_not_allowed_count = 0;
    bool aoc_exhausted = false;
    bool cascade_already_used = false;
    bool stronger_model_available = false;
};

struct CapabilityRiskDecision {
    float score = 0.f;
    CapabilityRiskAction action = CapabilityRiskAction::Continue;
    std::string reason_code;
};

/// Heuristic: lite/tiny/mini profiles or small parameter tags (:7b, :3b, …).
bool isWeakCortexModel(const std::string& profile_id, const std::string& model_name);

/// True for Mutate writes, description force-write, structure/connect/calc goals.
bool isComplexCapabilityGoal(const std::string& planning_text, LLMIntentKind intent,
                             bool filter_include_write, bool requires_pending_write);

/// Stronger Cortex model name from catalog (typically ollama-thinking), if different.
std::optional<std::string> findStrongerCortexModel(const std::string& current_model);

/// Count failed tool rows and PATH_NOT_ALLOWED-like errors in the turn trace.
void accumulateToolFailureSignals(const std::vector<TurnToolInvocationView>& trace,
                                  int& tool_error_count, int& path_not_allowed_count);

float capabilityRiskTau();
bool capabilityRiskGateDisabled();

float scoreCapabilityRisk(const CapabilityRiskSignals& risk_signals);

CapabilityRiskDecision decideCapabilityRisk(const CapabilityRiskSignals& risk_signals);

} // namespace RDK::LLM

#endif
