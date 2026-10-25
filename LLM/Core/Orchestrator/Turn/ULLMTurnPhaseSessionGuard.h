#ifndef RDK_LLM_TURN_PHASE_SESSION_GUARD_H
#define RDK_LLM_TURN_PHASE_SESSION_GUARD_H

#include "ITurnPhase.h"

namespace RDK::LLM {

/// Acquires the per-session lock once for the complete turn pipeline.
class ULLMTurnPhaseSessionGuard : public ITurnPhase {
public:
    const char* name() const override { return "session_guard"; }
    TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) override;
};

} // namespace RDK::LLM

#endif
