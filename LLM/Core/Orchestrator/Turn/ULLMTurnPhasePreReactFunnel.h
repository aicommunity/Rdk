#ifndef RDK_LLM_TURN_PHASE_PRE_REACT_FUNNEL_H
#define RDK_LLM_TURN_PHASE_PRE_REACT_FUNNEL_H

#include "ITurnPhase.h"

namespace RDK::LLM {

class ULLMTurnPhasePreReactFunnel : public ITurnPhase {
public:
    const char* name() const override { return "PreReactFunnel"; }
    TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) override;
};

} // namespace RDK::LLM

#endif
