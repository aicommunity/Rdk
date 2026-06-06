#ifndef RDK_ULLM_CONTEXT_COMPACTOR_H
#define RDK_ULLM_CONTEXT_COMPACTOR_H

#include <string>

#include "ULLMConversationStore.h"

namespace RDK::LLM {

class ILLMProvider;

class ULLMContextCompactor {
public:
    /// Rule-based compaction + optional stale tool masking. Returns true if state changed.
    /// When `NMSDK_LLM_CONTEXT_COMPACT_LLM=1` and `summarize_provider` is set, uses one provider
    /// round for the summary (falls back to rule-based on failure).
    bool maybeCompact(ConversationState& state, const std::string& session_id,
                      const std::string& storage_dir,
                      ILLMProvider* summarize_provider = nullptr);
};

} // namespace RDK::LLM

#endif
