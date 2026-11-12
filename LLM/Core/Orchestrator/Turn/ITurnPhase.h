#ifndef RDK_LLM_TURN_PHASE_H
#define RDK_LLM_TURN_PHASE_H

#include "ULLMTurnContext.h"

namespace RDK::LLM {

enum class TurnPhaseResult {
    Continue,
    ShortCircuit, // ctx.final is set; stop pipeline
};

class ITurnPhase {
public:
    virtual ~ITurnPhase() = default;
    virtual const char* name() const = 0;
    virtual TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) = 0;
};

} // namespace RDK::LLM

#endif
