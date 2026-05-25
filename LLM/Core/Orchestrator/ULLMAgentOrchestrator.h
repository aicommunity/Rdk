#ifndef RDK_ULLM_AGENT_ORCHESTRATOR_H
#define RDK_ULLM_AGENT_ORCHESTRATOR_H

#include <atomic>
#include <functional>

#include "../LlmTypes.h"
#include "../Providers/ILLMProvider.h"
#include "../Tools/ULLMToolGateway.h"
#include "../Tools/ULLMToolRegistry.h"
#include "ULLMIntentParser.h"
#include "../Session/ULLMConversationStore.h"

namespace RDK::LLM {

struct LLMRequestEnvelope {
    std::string session_id;
    std::string trace_id;
    std::string user_text;
    LLMSessionContext session;
    LLMProviderProfile provider_profile;
};

struct LLMFinalResponse {
    bool ok = true;
    std::string text;
    std::string error;
    bool pending_confirmation = false;
};

class ULLMAgentOrchestrator {
public:
    ULLMAgentOrchestrator(ILLMProvider& provider, ULLMToolRegistry& registry,
                          ULLMToolGateway& gateway, ULLMConversationStore& store);

    LLMFinalResponse handleUserMessage(const LLMRequestEnvelope& req);
    void confirmPending(const std::string& session_id, const std::string& confirmation_id);
    void rejectPending(const std::string& session_id);
    void cancel() { m_cancelled = true; }

private:
    ILLMProvider& m_provider;
    ULLMToolRegistry& m_registry;
    ULLMToolGateway& m_gateway;
    ULLMConversationStore& m_store;
    ULLMIntentParser m_intent;
    std::atomic<bool> m_cancelled{false};
    static constexpr int kMaxRounds = 8;
};

} // namespace RDK::LLM

#endif
