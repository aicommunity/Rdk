#pragma once

#include <string>
#include <vector>

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"
#include "../Tools/ULLMToolRegistry.h"
#include "Orchestrator/ULLMIntentParser.h"

namespace RDK::LLM {

struct IntentAmbiguityDecision {
    bool block_writes = false;
    std::string reason_code;
    std::string user_message;
};

float intentMinWriteConfidence();

IntentAmbiguityDecision evaluateIntentAmbiguity(const ConversationState& state,
                                                const IntentParseResult& intent_result,
                                                const std::vector<LLMToolCall>& tool_calls,
                                                const ULLMToolRegistry& registry);

} // namespace RDK::LLM
