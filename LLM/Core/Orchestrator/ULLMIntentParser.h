#ifndef RDK_ULLM_INTENT_PARSER_H
#define RDK_ULLM_INTENT_PARSER_H

#include "../LlmTypes.h"

namespace RDK::LLM {

class ULLMIntentParser {
public:
    LLMIntentKind parse(const std::string& user_text) const;
};

} // namespace RDK::LLM

#endif
