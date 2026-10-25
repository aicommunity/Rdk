#include "ULLMTurnPhaseLegacy.h"

#include "../ULLMAgentOrchestrator.h"

namespace RDK::LLM {

TurnPhaseResult ULLMTurnPhaseLegacy::run(TurnContext& ctx, TurnServices& svc)
{
    ctx.final = svc.orch.handleUserMessageAfterPacks(ctx, svc);
    return TurnPhaseResult::ShortCircuit;
}

} // namespace RDK::LLM
