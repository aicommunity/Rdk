#ifndef RDK_ULLM_INPUT_UNDERSTANDING_H
#define RDK_ULLM_INPUT_UNDERSTANDING_H

#include "../LlmTypes.h"
#include "../Providers/ILLMProvider.h"
#include "ULLMIntentParser.h"

namespace RDK::LLM {

struct InputUnderstandingResult {
    LLMIntentKind intent = LLMIntentKind::Query;
    float confidence = 0.5f;
    std::string method = "ensemble";
    bool needs_clarification = false;
    std::string rationale;
};

InputUnderstandingResult understandUserInput(ILLMProvider* provider, const std::string& text_en,
                                             const IntentParseResult& heuristic);

} // namespace RDK::LLM

#endif
