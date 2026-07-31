#include "ULLMUnifiedTurnController.h"

#include "../LlmModuleInit.h"
#include "../Observability/ULLMAuditLog.h"

#include <exception>
#include <typeinfo>

namespace RDK::LLM {

LLMFinalResponse ULLMUnifiedTurnController::handleTurn(ULLMAgentOrchestrator& orch,
                                                       const LLMRequestEnvelope& req,
                                                       const LLMStreamHandlers* stream)
{
    try
    {
        return orch.handleUserMessageImpl(req, stream);
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
