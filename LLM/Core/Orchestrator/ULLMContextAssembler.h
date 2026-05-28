#ifndef RDK_ULLM_CONTEXT_ASSEMBLER_H
#define RDK_ULLM_CONTEXT_ASSEMBLER_H

#include <string>
#include <vector>

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"
#include "ULLMIntentParser.h"
#include "ULLMLifecycleArgumentGate.h"

namespace RDK::LLM {

class URdkContextRetriever;
class ULLMToolRegistry;

struct EphemeralContextInput {
    const ConversationState& state;
    LLMSessionContext session;
    LLMGuiContextSnapshot gui;
    LLMIntentKind intent = LLMIntentKind::Auto;
    ConfigurationLifecycleAction lifecycle_action = ConfigurationLifecycleAction::None;
    bool provider_tools = false;
    bool strict_plan_schema = false;
    const ULLMToolRegistry* registry = nullptr;
    ToolFilter tool_filter{};
    std::string planning_text;
    std::string system_log_summary;
    const URdkContextRetriever* context_retriever = nullptr;
    std::string prefetched_docs_block;
    std::string long_term_memory_block;
};

std::string buildGuiFocusSystemHint(const LLMGuiContextSnapshot& gui, const LLMSessionContext& session);

std::size_t sumMessagePayloadChars(const std::vector<LLMMessage>& messages);

struct LLMContextBudget {
    std::size_t messages_chars = 0;
    std::size_t ephemeral_chars = 0;
    std::size_t manifest_chars = 0;
    bool compacted = false;
};

void prependEphemeralSystemMessages(std::vector<LLMMessage>& provider_messages,
                                    const EphemeralContextInput& input,
                                    LLMContextBudget* out_budget = nullptr);

} // namespace RDK::LLM

#endif
