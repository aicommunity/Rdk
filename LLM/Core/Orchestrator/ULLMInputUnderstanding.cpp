#include "ULLMInputUnderstanding.h"

#include <cstdlib>
#include <cstring>

namespace RDK::LLM {

namespace {

bool inputEnsembleEnabled()
{
    const char* v = std::getenv("NMSDK_LLM_INPUT_ENSEMBLE");
    if(!v)
        return true;
    return v[0] != '0' && std::strcmp(v, "false") != 0;
}

} // namespace

InputUnderstandingResult understandUserInput(ILLMProvider* provider, const std::string& text_en,
                                             const IntentParseResult& heuristic)
{
    InputUnderstandingResult out;
    out.intent = heuristic.kind;
    out.confidence = heuristic.confidence;
    out.method = heuristic.method.empty() ? "heuristic" : heuristic.method;
    if(!inputEnsembleEnabled() || !provider)
        return out;

    (void)text_en;
    if(out.confidence < 0.35f)
    {
        out.needs_clarification = true;
        out.rationale = "low_confidence_intent";
    }
    return out;
}

} // namespace RDK::LLM
