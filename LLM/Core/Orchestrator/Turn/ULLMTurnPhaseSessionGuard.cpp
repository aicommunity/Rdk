#include "ULLMTurnPhaseSessionGuard.h"

namespace RDK::LLM {

TurnPhaseResult ULLMTurnPhaseSessionGuard::run(TurnContext& ctx, TurnServices& svc)
{
    if(!svc.orch.tryAcquireSessionBusy(ctx.req.session_id))
    {
        ctx.final.ok = false;
        ctx.final.error = ULLMAgentOrchestrator::sessionBusyErrorMessage();
        return TurnPhaseResult::ShortCircuit;
    }
    ctx.busy_held = true;
    return TurnPhaseResult::Continue;
}

} // namespace RDK::LLM
