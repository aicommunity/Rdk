#ifndef RDK_ULLM_CONVERSATION_STORE_H
#define RDK_ULLM_CONVERSATION_STORE_H

#include <map>
#include <optional>

#include "../LlmTypes.h"
#include "../Orchestrator/ULLMExecutionPlan.h"
#include "../Orchestrator/ULLMLifecycleArgumentGate.h"
#include "../Orchestrator/ULLMQuantityTypes.h"
#include "../Orchestrator/ULLMWorkflowState.h"
#include "ULLMGuiTurnPin.h"
#include "ULLMSessionGraphMemory.h"

namespace RDK::LLM {

struct PendingConfirmation {
    std::string confirmation_id;
    ToolInvokeRequest request;
    /// UTC unix seconds when confirmation was requested (TD-025).
    int64_t created_at_unix_sec = 0;
    /// Stable id for Tool message pairing after Apply (TD-150).
    std::string tool_call_id;
};

struct ResolvedEntityRecord {
    std::string kind;
    std::string query_key;
    std::string canonical_value;
    int channel_index = 0;
};

struct PendingUserQuestion {
    std::string question_id;
    std::string prompt;
    nlohmann::json choices = nlohmann::json::array();
    bool allow_free_text = true;
    std::string resume_context;
};

struct ConversationState {
    std::string session_id;
    std::vector<LLMMessage> messages;
    std::optional<PendingConfirmation> pending;
    std::optional<PendingToolArguments> pending_tool_arguments;
    std::optional<PendingUserQuestion> pending_user_question;
    std::optional<ULLMExecutionPlan> pending_plan;
    std::vector<std::string> known_facts;
    std::string last_user_text_original;
    std::string last_user_text_en;
    ResolvedUserQuantity last_quantity;
    LLMIntentKind intent_contract_kind = LLMIntentKind::Auto;
    float intent_contract_confidence = 0.0f;
    bool intent_contract_requires_confirmation_for_writes = true;
    LLMWorkflowPhase workflow_phase = LLMWorkflowPhase::Idle;
    int cloud_provider_rounds = 0;
    std::optional<LLMGuiContextSnapshot> last_gui_context;
    std::optional<GuiTurnPin> active_turn_pin;
    std::vector<ResolvedEntityRecord> resolved_entities;
    std::string agent_notes;
    bool session_context_seeded = false;
    std::optional<std::string> session_summary;
    /// Last `LLMSessionContext` from an orchestrator entry (resume parity, TD-088).
    std::optional<LLMSessionContext> last_session_context;
    SessionGraphMemory session_graph;
    /// Structured turn/session goals (DD-WM-001 / TD-162).
    std::vector<WorkingGoal> working_goals;
    /// Tool invocations for the current user turn (cleared at turn start; copied to response).
    std::vector<TurnToolInvocationView> current_turn_tool_trace;
    /// When true (project_description pack), goals stay InProgress until update_configuration ok.
    bool turn_requires_description_write = false;
    /// Cumulative explore-subagent provider rounds used in this session (TD-164).
    int subagent_rounds_used = 0;
    int store_schema_version = 4;
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
    const ConversationState* findSession(const std::string& session_id) const;
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
