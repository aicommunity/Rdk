#include "ULLMTurnPhaseLegacy.h"

#include "../ULLMAgentOrchestrator.h"

namespace RDK::LLM {

ULLMTurnPhaseLegacy::ULLMTurnPhaseLegacy(BodyFn body)
    : m_body(std::move(body))
{
}

TurnPhaseResult ULLMTurnPhaseLegacy::run(TurnContext& ctx, TurnServices& svc)
{
    ctx.final = m_body(svc.orch, ctx.req, ctx.stream);
    return TurnPhaseResult::ShortCircuit;
}

} // namespace RDK::LLM
