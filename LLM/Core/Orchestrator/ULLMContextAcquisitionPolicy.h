#ifndef RDK_ULLM_CONTEXT_ACQUISITION_POLICY_H
#define RDK_ULLM_CONTEXT_ACQUISITION_POLICY_H

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"

namespace RDK::LLM {

struct ContextAcquisitionPlan {
    bool bootstrap_session = true;
    bool prefetch_snapshot = false;
    bool prefetch_docs = false;
    int max_known_facts = 12;
};

ContextAcquisitionPlan defaultContextAcquisitionPlan(const ConversationState& state,
                                                     const LLMSessionContext& session);

std::string formatKnownFactsBlock(const std::vector<std::string>& facts);

} // namespace RDK::LLM

#endif
