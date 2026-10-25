#include "ULLMTurnPhaseTaskPath.h"

#include "../ULLMAgentOrchestrator.h"

namespace RDK::LLM {

TurnPhaseResult ULLMTurnPhaseTaskPath::run(TurnContext& ctx, TurnServices& svc)
{
    return svc.orch.runTaskPathPhase(ctx, svc);
}

} // namespace RDK::LLM
