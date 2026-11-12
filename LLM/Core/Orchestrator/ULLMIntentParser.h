#ifndef RDK_ULLM_INTENT_PARSER_H
#define RDK_ULLM_INTENT_PARSER_H

#include "../LlmTypes.h"
#include "../Providers/ILLMProvider.h"

namespace RDK::LLM {

struct IntentParseResult {
    LLMIntentKind kind = LLMIntentKind::Query;
    float confidence = 0.5f;
    std::string method = "rules";
};

class ULLMIntentParser {
public:
    IntentParseResult parseDetailed(const std::string& user_text) const;
    LLMIntentKind parse(const std::string& user_text) const;

    /// Optional: one-shot LLM classify when `NMSDK_LLM_INTENT_LLM=1` and provider given.
    IntentParseResult parseWithOptionalLlm(ILLMProvider* provider,
                                         const std::string& user_text) const;
};

} // namespace RDK::LLM

#endif
