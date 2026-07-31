#ifndef RDK_ULLM_TURN_TERMINAL_HELPERS_H
#define RDK_ULLM_TURN_TERMINAL_HELPERS_H

#include <string>
#include <vector>

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"

namespace RDK::LLM {

void appendCancelledToolResults(ULLMConversationStore& store, const std::string& session_id,
                                const std::vector<LLMToolCall>& tool_calls);

/// Persist assistant tool_calls + tool result (+ optional outcome) for direct (non-LLM) tool invokes.
void appendDirectToolTranscript(ULLMConversationStore& store, const std::string& session_id,
                                const std::string& tool_call_id, const std::string& tool_name,
                                const nlohmann::json& arguments, const std::string& tool_content,
                                const std::string& outcome_text = {});

} // namespace RDK::LLM

#endif
