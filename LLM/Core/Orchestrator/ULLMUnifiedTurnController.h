#ifndef RDK_ULLM_UNIFIED_TURN_CONTROLLER_H
#define RDK_ULLM_UNIFIED_TURN_CONTROLLER_H

#include "ULLMAgentOrchestrator.h"

namespace RDK::LLM {

/// Single entry point for user turns (Agent v2). Delegates implementation to orchestrator.
class ULLMUnifiedTurnController {
public:
    static LLMFinalResponse handleTurn(ULLMAgentOrchestrator& orch, const LLMRequestEnvelope& req,
                                       const LLMStreamHandlers* stream = nullptr);
};

} // namespace RDK::LLM

#endif
