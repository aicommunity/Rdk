#include "ULLMTurnPipeline.h"

#include "../../LlmModuleInit.h"
#include "../../Observability/ULLMAuditLog.h"

namespace RDK::LLM {

ULLMTurnPipeline::ULLMTurnPipeline(std::vector<std::unique_ptr<ITurnPhase>> phases)
    : m_phases(std::move(phases))
{
}

LLMFinalResponse ULLMTurnPipeline::run(TurnContext& ctx, TurnServices& svc)
{
    try
    {
        for(std::unique_ptr<ITurnPhase>& phase : m_phases)
        {
            if(!phase)
                continue;
            GetAuditLog().append("turn_phase_entered", {{"phase", phase->name()}}, ctx.req.trace_id,
                                 ctx.req.session_id);
            const TurnPhaseResult result = phase->run(ctx, svc);
            if(result == TurnPhaseResult::ShortCircuit)
                break;
        }
    }
    catch(...)
    {
        if(ctx.busy_held)
        {
            svc.orch.releaseSessionBusy(ctx.req.session_id);
            ctx.busy_held = false;
        }
        throw;
    }
    svc.orch.finalizeTurnContext(ctx);
    if(ctx.busy_held)
    {
        svc.orch.releaseSessionBusy(ctx.req.session_id);
        ctx.busy_held = false;
    }
    return ctx.final;
}

} // namespace RDK::LLM
