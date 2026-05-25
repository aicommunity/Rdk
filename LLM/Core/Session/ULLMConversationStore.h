#ifndef RDK_ULLM_CONVERSATION_STORE_H
#define RDK_ULLM_CONVERSATION_STORE_H

#include <map>
#include <optional>

#include "../LlmTypes.h"

namespace RDK::LLM {

struct PendingConfirmation {
    std::string confirmation_id;
    ToolInvokeRequest request;
};

struct ConversationState {
    std::string session_id;
    std::vector<LLMMessage> messages;
    std::optional<PendingConfirmation> pending;
};

class ULLMConversationStore {
public:
    ConversationState& getOrCreate(const std::string& session_id);
    void appendMessage(const std::string& session_id, const LLMMessage& msg);
    void setPending(const std::string& session_id, PendingConfirmation p);
    void clearPending(const std::string& session_id);

private:
    std::map<std::string, ConversationState> m_sessions;
};

} // namespace RDK::LLM

#endif
