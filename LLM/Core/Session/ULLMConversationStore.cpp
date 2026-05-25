#include "ULLMConversationStore.h"

namespace RDK::LLM {

ConversationState& ULLMConversationStore::getOrCreate(const std::string& session_id)
{
    return m_sessions[session_id];
}

void ULLMConversationStore::appendMessage(const std::string& session_id, const LLMMessage& msg)
{
    m_sessions[session_id].messages.push_back(msg);
}

void ULLMConversationStore::setPending(const std::string& session_id, PendingConfirmation p)
{
    m_sessions[session_id].pending = std::move(p);
}

void ULLMConversationStore::clearPending(const std::string& session_id)
{
    m_sessions[session_id].pending.reset();
}

} // namespace RDK::LLM
