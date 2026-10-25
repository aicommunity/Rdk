#ifndef RDK_LLM_TURN_PHASE_TASK_PATH_H
#define RDK_LLM_TURN_PHASE_TASK_PATH_H

#include "ITurnPhase.h"

namespace RDK::LLM {

class ULLMTurnPhaseTaskPath : public ITurnPhase {
public:
    const char* name() const override { return "TaskPath"; }
    TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) override;
};

} // namespace RDK::LLM

#endif
