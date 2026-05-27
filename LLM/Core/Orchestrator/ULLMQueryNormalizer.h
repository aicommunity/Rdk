#ifndef RDK_ULLM_QUERY_NORMALIZER_H
#define RDK_ULLM_QUERY_NORMALIZER_H

#include "../Providers/ILLMProvider.h"

namespace RDK::LLM {

struct QueryNormalizeResult {
    bool ok = true;
    std::string text_en;
    std::string detected_lang = "en";
    bool used_llm_translate = false;
};

QueryNormalizeResult normalizeUserQueryForPlanning(ILLMProvider& provider,
                                                   const std::string& user_text,
                                                   bool translate_queries_to_en);

} // namespace RDK::LLM

#endif
