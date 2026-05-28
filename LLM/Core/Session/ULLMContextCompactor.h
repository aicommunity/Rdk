#ifndef RDK_ULLM_CONTEXT_COMPACTOR_H
#define RDK_ULLM_CONTEXT_COMPACTOR_H

#include <string>

#include "ULLMConversationStore.h"

namespace RDK::LLM {

class ULLMContextCompactor {
public:
    /// Rule-based compaction + optional stale tool masking. Returns true if state changed.
    bool maybeCompact(ConversationState& state, const std::string& session_id,
                      const std::string& storage_dir);
};

} // namespace RDK::LLM

#endif
