#include "ULLMTurnPhasePreReactFunnel.h"

#include "../ULLMAgentOrchestrator.h"

namespace RDK::LLM {

TurnPhaseResult ULLMTurnPhasePreReactFunnel::run(TurnContext& ctx, TurnServices& svc)
{
    return svc.orch.runPreReactFunnelPhase(ctx, svc);
}

} // namespace RDK::LLM
