#ifndef RDK_ULLM_AGENT_ORCHESTRATOR_H
#define RDK_ULLM_AGENT_ORCHESTRATOR_H

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>

#include "../LlmTypes.h"
#include "../Providers/ILLMProvider.h"
#include "../Tools/ULLMToolGateway.h"
#include "../Tools/ULLMToolRegistry.h"
#include "ULLMIntentParser.h"
#include "ULLMWorkflowState.h"
#include "../Policy/ULLMPolicyLimits.h"
#include "../Session/ULLMConversationStore.h"
#include "../Observability/ULLMSystemLogReader.h"

namespace RDK::LLM {

struct LLMRequestEnvelope {
    std::string session_id;
    std::string trace_id;
    std::string user_text;
    LLMSessionContext session;
    LLMProviderProfile provider_profile;
    LLMGuiContextSnapshot gui;
};

/// Optional GUI streaming (TD-024). Called from worker thread; marshal to UI thread in callbacks.
struct LLMStreamHandlers {
    std::function<void(const std::string& token)> on_token;
};

struct LLMFinalResponse {
    bool ok = true;
    std::string text;
    std::string error;
    bool pending_confirmation = false;
    std::string pending_confirmation_id;
    bool pending_plan_execution = false;
    std::string pending_plan_id;
    bool plan_paused = false;
    bool can_resume_plan = false;
    bool needs_entity_clarification = false;
    /// TD-035: dedicated flag (also set when entity clarification is used).
    bool needs_tool_disambiguation = false;
    nlohmann::json clarification_candidates = nlohmann::json::array();
    /// Lifecycle tool needs more parameters from the user (see ConversationState::pending_tool_arguments).
    bool needs_argument_clarification = false;
    bool no_suitable_tool = false;
    /// Explicit rollback status for UI/API handling.
    std::string rollback_status;
    /// Last provider-round context size (chars), for GUI budget label.
    std::size_t context_messages_chars = 0;
    std::size_t context_ephemeral_chars = 0;
    std::size_t context_manifest_chars = 0;
    bool context_compacted = false;
};

class ULLMAgentOrchestrator {
public:
    ULLMAgentOrchestrator(ILLMProvider& provider, ULLMToolRegistry& registry,
                          ULLMToolGateway& gateway, ULLMConversationStore& store);

    LLMFinalResponse handleUserMessage(const LLMRequestEnvelope& req,
                                       const LLMStreamHandlers* stream = nullptr);
    LLMFinalResponse confirmPending(const std::string& session_id, const std::string& confirmation_id);
    LLMFinalResponse confirmPlanExecution(const std::string& session_id, const std::string& trace_id,
                                          const LLMSessionContext& session);
    LLMFinalResponse resumePlanExecution(const std::string& session_id, const std::string& trace_id,
                                         const LLMSessionContext& session);
    LLMFinalResponse rollbackPlanExecution(const std::string& session_id, const std::string& trace_id,
                                           const LLMSessionContext& session);
    void rejectPending(const std::string& session_id);
    /// Clears persisted conversation for session_id (GUI new chat / project load).
    void discardSession(const std::string& session_id);
    bool tryResumeSession(const std::string& session_id);
    void cancel();
    void cancelSession(const std::string& session_id);

    void seedSessionContext(const std::string& session_id, const LLMSessionContext& session,
                            const LLMGuiContextSnapshot& gui);

private:
    ILLMProvider& m_provider;
    ULLMToolRegistry& m_registry;
    ULLMToolGateway& m_gateway;
    ULLMConversationStore& m_store;
    ULLMIntentParser m_intent;
    std::atomic<bool> m_cancelled{false};
    mutable std::mutex m_session_busy_mu;
    std::map<std::string, bool> m_session_busy;
    mutable std::mutex m_cancel_mu;
    std::set<std::string> m_cancelled_sessions;
    std::unique_ptr<ULLMSystemLogReader> m_system_log_reader;
    void setWorkflowPhase(ConversationState& state, LLMWorkflowPhase phase,
                          const std::string& trace_id);

    LLMFinalResponse invokeLifecycleToolDirect(const std::string& session_id,
                                               const std::string& trace_id,
                                               const std::string& tool_name,
                                               const nlohmann::json& arguments,
                                               const LLMSessionContext& session,
                                               const std::string& user_text_hint = "");

    LLMFinalResponse returnArgumentRequest(ConversationState& state, const std::string& trace_id,
                                           const PendingToolArguments& pending,
                                           RDK::UApplication* app);

    LLMFinalResponse returnDisambiguationRequest(ConversationState& state,
                                                 const std::string& trace_id,
                                                 const LLMToolCall& call,
                                                 PendingDisambiguationKind kind,
                                                 const std::string& field,
                                                 const nlohmann::json& disambiguation);

    static constexpr int kMaxRounds = kDefaultMaxToolRounds;
};

} // namespace RDK::LLM

#endif
