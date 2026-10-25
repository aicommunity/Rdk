#ifndef RDK_LLM_TURN_PHASE_PREPARE_H
#define RDK_LLM_TURN_PHASE_PREPARE_H

#include "ITurnPhase.h"

namespace RDK::LLM {

/// Initializes turn state, normalizes input, and determines intent.
class ULLMTurnPhasePrepare : public ITurnPhase {
public:
    const char* name() const override { return "prepare"; }
    TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) override;
};

} // namespace RDK::LLM

#endif
