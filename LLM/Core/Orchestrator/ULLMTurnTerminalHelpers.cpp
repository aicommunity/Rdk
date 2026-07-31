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

void appendDirectToolTranscript(ULLMConversationStore& store, const std::string& session_id,
                                const std::string& tool_call_id, const std::string& tool_name,
                                const nlohmann::json& arguments, const std::string& tool_content,
                                const std::string& outcome_text)
{
    LLMToolCall call;
    call.id = tool_call_id.empty() ? ("direct-" + tool_name) : tool_call_id;
    call.name = tool_name;
    call.arguments = arguments.is_object() ? arguments : nlohmann::json::object();

    LLMMessage assistant_tools;
    assistant_tools.role = LLMMessage::Role::Assistant;
    assistant_tools.assistant_tool_calls = std::vector<LLMToolCall>{call};
    store.appendMessage(session_id, assistant_tools);

    LLMMessage tool_msg;
    tool_msg.role = LLMMessage::Role::Tool;
    tool_msg.tool_call_id = call.id;
    tool_msg.tool_name = tool_name;
    tool_msg.content = tool_content;
    store.appendMessage(session_id, tool_msg);

    if(!outcome_text.empty())
    {
        LLMMessage outcome;
        outcome.role = LLMMessage::Role::Assistant;
        outcome.content = outcome_text;
        store.appendMessage(session_id, outcome);
    }
}

} // namespace RDK::LLM
