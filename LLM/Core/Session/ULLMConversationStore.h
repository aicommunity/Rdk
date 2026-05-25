#ifndef RDK_ULLM_CONVERSATION_STORE_H
#define RDK_ULLM_CONVERSATION_STORE_H

#include <map>
#include <optional>

#include "../LlmTypes.h"
#include "../Orchestrator/ULLMExecutionPlan.h"
#include "../Orchestrator/ULLMWorkflowState.h"

namespace RDK::LLM {

struct PendingConfirmation {
    std::string confirmation_id;
    ToolInvokeRequest request;
};

struct ConversationState {
    std::string session_id;
    std::vector<LLMMessage> messages;
    std::optional<PendingConfirmation> pending;
    std::optional<ULLMExecutionPlan> pending_plan;
    LLMWorkflowPhase workflow_phase = LLMWorkflowPhase::Idle;
};

class ULLMConversationStore {
public:
    void setStorageDirectory(const std::string& path);
    ConversationState& getOrCreate(const std::string& session_id);
    void appendMessage(const std::string& session_id, const LLMMessage& msg);
    void setPending(const std::string& session_id, PendingConfirmation p);
    void clearPending(const std::string& session_id);
    bool persistToDisk(const std::string& session_id);
    bool loadFromDisk(const std::string& session_id);
    void removeFromDisk(const std::string& session_id);

private:
    static nlohmann::json messageToJson(const LLMMessage& msg);
    static LLMMessage messageFromJson(const nlohmann::json& j);

    std::string m_storage_dir;
    std::map<std::string, ConversationState> m_sessions;
};

} // namespace RDK::LLM

#endif
