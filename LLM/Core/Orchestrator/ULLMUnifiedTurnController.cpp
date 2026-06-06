#include "ULLMUnifiedTurnController.h"

namespace RDK::LLM {

LLMFinalResponse ULLMUnifiedTurnController::handleTurn(ULLMAgentOrchestrator& orch,
                                                       const LLMRequestEnvelope& req,
                                                       const LLMStreamHandlers* stream)
{
    return orch.handleUserMessageImpl(req, stream);
}

} // namespace RDK::LLM
