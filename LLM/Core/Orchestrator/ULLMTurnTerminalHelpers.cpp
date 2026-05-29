#include "ULLMTurnTerminalHelpers.h"

namespace RDK::LLM {

void appendCancelledToolResults(ULLMConversationStore& store, const std::string& session_id,
                                const std::vector<LLMToolCall>& tool_calls)
{
    for(const LLMToolCall& call : tool_calls)
    {
        LLMMessage tool_msg;
        tool_msg.role = LLMMessage::Role::Tool;
        tool_msg.tool_call_id = call.id;
        tool_msg.tool_name = call.name;
        tool_msg.content = R"({"error":"cancelled","message":"Turn cancelled by user"})";
        store.appendMessage(session_id, tool_msg);
    }
}

} // namespace RDK::LLM
