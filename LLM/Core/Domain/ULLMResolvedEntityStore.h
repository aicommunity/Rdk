#ifndef RDK_ULLM_RESOLVED_ENTITY_STORE_H
#define RDK_ULLM_RESOLVED_ENTITY_STORE_H

#include <optional>
#include <string>

#include "../Session/ULLMConversationStore.h"

namespace RDK::LLM {

std::string normalizeEntityQuery(const std::string& query);

std::optional<std::string> lookupResolvedEntity(const ConversationState& state,
                                                const std::string& kind,
                                                const std::string& query, int channel_index);

void upsertResolvedEntity(ConversationState& state, const std::string& kind,
                          const std::string& query, const std::string& canonical_value,
                          int channel_index);

} // namespace RDK::LLM

#endif
