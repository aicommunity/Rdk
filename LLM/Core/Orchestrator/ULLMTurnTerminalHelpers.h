#ifndef RDK_ULLM_TURN_TERMINAL_HELPERS_H
#define RDK_ULLM_TURN_TERMINAL_HELPERS_H

#include <string>
#include <vector>

#include "../LlmTypes.h"
#include "../Session/ULLMConversationStore.h"

namespace RDK::LLM {

void appendCancelledToolResults(ULLMConversationStore& store, const std::string& session_id,
                                const std::vector<LLMToolCall>& tool_calls);

} // namespace RDK::LLM

#endif
