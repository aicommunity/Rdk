#ifndef RDK_ULLM_QUANTITY_RESOLVER_H
#define RDK_ULLM_QUANTITY_RESOLVER_H

#include "../Providers/ILLMProvider.h"
#include "../Session/ULLMConversationStore.h"
#include "ULLMQuantityTypes.h"

#include <string>

namespace RDK::LLM {

struct QuantityResolveRequest {
    std::string text_original;
    std::string text_en;
    bool allow_llm_fallback = true;
    int max_primary = 50;
};

struct QuantityResolveResult {
    ResolvedUserQuantity quantity;
};

std::string quantityTurnBindingHash(const std::string& text_original, const std::string& text_en);

QuantityResolveResult resolveUserQuantity(const QuantityResolveRequest& req,
                                          ILLMProvider* provider = nullptr);

bool isDisambiguationOnlyFollowUp(const std::string& user_text, const ConversationState& state);

int primaryQuantityOr(const ConversationState& state, int default_value = 1);

std::string quantitySourceName(QuantitySource source);

} // namespace RDK::LLM

#endif
