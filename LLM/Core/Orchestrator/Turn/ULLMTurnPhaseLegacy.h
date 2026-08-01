#ifndef RDK_LLM_TURN_PHASE_LEGACY_H
#define RDK_LLM_TURN_PHASE_LEGACY_H

#include <functional>

#include "ITurnPhase.h"

namespace RDK::LLM {

/// Strangler phase: runs the existing orchestrator body (behavior-preserving extract).
class ULLMTurnPhaseLegacy : public ITurnPhase {
public:
    using BodyFn = std::function<LLMFinalResponse(ULLMAgentOrchestrator&, const LLMRequestEnvelope&,
                                                   const LLMStreamHandlers*)>;

    explicit ULLMTurnPhaseLegacy(BodyFn body);

    const char* name() const override { return "legacy_impl"; }
    TurnPhaseResult run(TurnContext& ctx, TurnServices& svc) override;

private:
    BodyFn m_body;
};

} // namespace RDK::LLM

#endif
