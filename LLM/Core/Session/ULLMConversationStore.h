#ifndef RDK_ULLM_CONVERSATION_STORE_H
#define RDK_ULLM_CONVERSATION_STORE_H

#include <map>
#include <optional>

#include "../LlmTypes.h"
#include "../Orchestrator/ULLMExecutionPlan.h"
#include "../Orchestrator/ULLMLifecycleArgumentGate.h"
#include "../Orchestrator/ULLMWorkflowState.h"

namespace RDK::LLM {

struct PendingConfirmation {
    std::string confirmation_id;
    ToolInvokeRequest request;
    /// UTC unix seconds when confirmation was requested (TD-025).
    int64_t created_at_unix_sec = 0;
};

struct ConversationState {
    std::string session_id;
    std::vector<LLMMessage> messages;
    std::optional<PendingConfirmation> pending;
    std::optional<PendingToolArguments> pending_tool_arguments;
    std::optional<ULLMExecutionPlan> pending_plan;
    std::string last_user_text_original;
    std::string last_user_text_en;
    LLMIntentKind intent_contract_kind = LLMIntentKind::Auto;
    float intent_contract_confidence = 0.0f;
    bool intent_contract_requires_confirmation_for_writes = true;
    LLMWorkflowPhase workflow_phase = LLMWorkflowPhase::Idle;
    int cloud_provider_rounds = 0;
};

class ULLMConversationStore {
public:
    void setStorageDirectory(const std::string& path);
    ConversationState& getOrCreate(const std::string& session_id);
    void appendMessage(const std::string& session_id, const LLMMessage& msg);
    void setPending(const std::string& session_id, PendingConfirmation p);
    void clearPending(const std::string& session_id);
    void setPendingToolArguments(const std::string& session_id, PendingToolArguments p);
    void clearPendingToolArguments(const std::string& session_id);
    /// Clears pending confirmation if older than ttl_seconds. Returns true if expired.
    bool expirePendingIfStale(const std::string& session_id, int ttl_seconds);
    bool persistToDisk(const std::string& session_id);
    bool loadFromDisk(const std::string& session_id);
    void removeFromDisk(const std::string& session_id);
    /// Drop in-memory state and persisted file for this session (new chat / project switch).
    void removeSession(const std::string& session_id);

private:
    static nlohmann::json messageToJson(const LLMMessage& msg);
    static LLMMessage messageFromJson(const nlohmann::json& j);

    std::string m_storage_dir;
    std::map<std::string, ConversationState> m_sessions;
};

} // namespace RDK::LLM

#endif
