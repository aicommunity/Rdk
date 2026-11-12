#ifndef RDK_LLM_TURN_PHASE_LEGACY_H
#define RDK_LLM_TURN_PHASE_LEGACY_H

#include "ITurnPhase.h"

namespace RDK::LLM {

/// Strangler phase: executes TaskPath, ReAct, and response finalization.
class ULLMTurnPhaseLegacy : public ITurnPhase {
public:
    const char* name() const override { return "legacy_rest"; }
    TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) override;
};

} // namespace RDK::LLM

#endif
