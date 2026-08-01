#ifndef RDK_LLM_TURN_PIPELINE_H
#define RDK_LLM_TURN_PIPELINE_H

#include <memory>
#include <vector>

#include "ITurnPhase.h"

namespace RDK::LLM {

class ULLMTurnPipeline {
public:
    explicit ULLMTurnPipeline(std::vector<std::unique_ptr<ITurnPhase>> phases);

    LLMFinalResponse run(TurnContext& ctx, TurnServices& svc);

private:
    std::vector<std::unique_ptr<ITurnPhase>> m_phases;
};

} // namespace RDK::LLM

#endif
