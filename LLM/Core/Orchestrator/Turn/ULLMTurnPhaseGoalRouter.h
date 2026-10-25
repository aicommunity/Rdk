#ifndef RDK_LLM_TURN_PHASE_GOAL_ROUTER_H
#define RDK_LLM_TURN_PHASE_GOAL_ROUTER_H

#include "ITurnPhase.h"

namespace RDK::LLM {

/// Executes Recorded packs and collects ReAct hints/tool extensions.
class ULLMTurnPhaseGoalRouter : public ITurnPhase {
public:
    const char* name() const override { return "goal_router"; }
    TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) override;
};

} // namespace RDK::LLM

#endif
