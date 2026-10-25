#include "ULLMTurnPhaseGoalRouter.h"

namespace RDK::LLM {

TurnPhaseResult ULLMTurnPhaseGoalRouter::run(TurnContext& ctx, TurnServices& svc)
{
    return svc.orch.runPackGoalRouter(ctx, svc);
}

} // namespace RDK::LLM
