#include "ULLMUnifiedTurnController.h"

#include "../LlmModuleInit.h"
#include "../LlmPublicApi.h"
#include "../Observability/ULLMAuditLog.h"
#include "../Packs/ULLMCapabilityPackRegistry.h"
#include "Turn/ULLMTurnPhaseGoalRouter.h"
#include "Turn/ULLMTurnPhaseLegacy.h"
#include "Turn/ULLMTurnPhasePrepare.h"
#include "Turn/ULLMTurnPhaseSessionGuard.h"
#include "Turn/ULLMTurnPipeline.h"

#include <exception>
#include <memory>
#include <typeinfo>
#include <vector>

namespace RDK::LLM {

namespace {

ILLMCapabilityPackRegistry& packsForTurn()
{
    if(LLMServices::instance().isInitialized())
        return LLMServices::instance().packs();
    static ULLMCapabilityPackRegistry empty;
    return empty;
}

} // namespace

LLMFinalResponse ULLMUnifiedTurnController::handleTurn(ULLMAgentOrchestrator& orch,
                                                       const LLMRequestEnvelope& req,
                                                       const LLMStreamHandlers* stream)
{
    try
    {
        // TD-170: SessionGuard → Prepare → GoalRouter → LegacyRest (TaskPath/ReAct).
        std::vector<std::unique_ptr<ITurnPhase>> phases;
        phases.push_back(std::make_unique<ULLMTurnPhaseSessionGuard>());
        phases.push_back(std::make_unique<ULLMTurnPhasePrepare>());
        phases.push_back(std::make_unique<ULLMTurnPhaseGoalRouter>());
        phases.push_back(std::make_unique<ULLMTurnPhaseLegacy>());
        ULLMTurnPipeline pipeline(std::move(phases));

        TurnContext ctx;
        ctx.req = req;
        ctx.stream = stream;

        TurnServices svc{orch.m_provider, orch.m_registry, orch.m_gateway, orch.m_store,
                         packsForTurn(), orch};
        return pipeline.run(ctx, svc);
    }
    catch(const std::exception& ex)
    {
        LLMFinalResponse response;
        response.ok = false;
        const char* what = ex.what();
        response.error = std::string(typeid(ex).name()) + ": " + (what && *what ? what : "std::exception");
        GetAuditLog().append("turn_exception",
                             {{"what", response.error}, {"type", typeid(ex).name()}}, req.trace_id,
                             req.session_id);
        assignTurnTerminal(response, TurnTerminal::ProviderError);
        try
        {
            ConversationState& state = orch.m_store.getOrCreate(req.session_id);
            orch.setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
            orch.setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            orch.m_store.persistToDisk(req.session_id);
        }
        catch(...)
        {
        }
        return response;
    }
    catch(...)
    {
        LLMFinalResponse response;
        response.ok = false;
        response.error = "unknown: Turn exception (non-std)";
        GetAuditLog().append("turn_exception", {{"what", response.error}, {"type", "unknown"}},
                             req.trace_id, req.session_id);
        assignTurnTerminal(response, TurnTerminal::ProviderError);
        try
        {
            ConversationState& state = orch.m_store.getOrCreate(req.session_id);
            orch.setWorkflowPhase(state, LLMWorkflowPhase::Failed, req.trace_id);
            orch.setWorkflowPhase(state, LLMWorkflowPhase::Idle, req.trace_id);
            orch.m_store.persistToDisk(req.session_id);
        }
        catch(...)
        {
        }
        return response;
    }
}

} // namespace RDK::LLM
