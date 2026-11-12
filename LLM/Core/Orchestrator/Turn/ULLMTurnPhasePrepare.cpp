#include "ULLMTurnPhasePrepare.h"

namespace RDK::LLM {

TurnPhaseResult ULLMTurnPhasePrepare::run(TurnContext& ctx, TurnServices& svc)
{
    return svc.orch.prepareTurnContext(ctx, svc);
}

} // namespace RDK::LLM
