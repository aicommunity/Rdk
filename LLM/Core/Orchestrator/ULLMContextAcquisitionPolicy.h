#ifndef RDK_ULLM_CONTEXT_ACQUISITION_POLICY_H
#define RDK_ULLM_CONTEXT_ACQUISITION_POLICY_H

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"
#include "ULLMIntentParser.h"

namespace RDK::LLM {

enum class MutateSubkind { None, Connect, Add, Property, Lifecycle, Other };

struct ContextAcquisitionSignals {
    LLMIntentKind intent = LLMIntentKind::Auto;
    MutateSubkind mutate_subkind = MutateSubkind::None;
    std::string from_class;
    std::string to_class;
    std::string retrieval_query;
    bool has_resolved_component_pair = false;
};

struct ContextAcquisitionPlan {
    bool bootstrap_session = true;
    bool prefetch_snapshot = false;
    bool prefetch_docs = false;
    bool prefetch_class_schema = false;
    bool inject_link_patterns = false;
    bool inject_connect_semantics = false;
    int link_pattern_top_k = 3;
    int docs_top_k = 3;
    std::string docs_scope = "docs";
    int max_known_facts = 12;
};

ContextAcquisitionPlan defaultContextAcquisitionPlan(const ConversationState& state,
                                                     const LLMSessionContext& session);

ContextAcquisitionPlan computeContextAcquisitionPlan(const ConversationState& state,
                                                     const LLMSessionContext& session,
                                                     const LLMGuiContextSnapshot& gui,
                                                     const ContextAcquisitionSignals& acq_signals,
                                                     LLMContextAcquisitionMode mode);

ContextAcquisitionSignals buildContextAcquisitionSignals(const ConversationState& state,
                                                         const LLMSessionContext& session,
                                                         const LLMGuiContextSnapshot& gui,
                                                         LLMIntentKind intent,
                                                         const std::string& planning_text);

std::string formatKnownFactsBlock(const std::vector<std::string>& facts);

} // namespace RDK::LLM

#endif
