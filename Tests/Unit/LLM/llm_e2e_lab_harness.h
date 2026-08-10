#ifndef RDK_LLM_E2E_LAB_HARNESS_H
#define RDK_LLM_E2E_LAB_HARNESS_H

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "LlmTypes.h"
#include "Orchestrator/ULLMAgentOrchestrator.h"
#include "Providers/ILLMProvider.h"
#include "Session/ULLMConversationStore.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

#include "llm_e2e_analyzer.h"
#include "llm_test_ollama_lab.h"

namespace RDK::LLM::E2eLab {

using Test::kLabOllamaHost;
using Test::kLabOllamaOpenAiV1;
using Test::labOllamaModelName;
using Test::labOllamaThinkingModelName;
using Test::skipIfLabOllamaUnreachable;
using Test::skipIfLabThinkingModelMissing;

std::string repoPath(const std::string& relative);
std::string sampleValidConfigurationIni();

struct Harness {
    ULLMToolRegistry registry;
    URdkDomainAccess domain;
    ULLMPolicyEngine policy;
    ULLMAuditLog audit;
    ULLMIdempotencyStore idempotency;
    ULLMToolArgumentValidator validator;
    ULLMToolGateway gateway;
    ULLMConversationStore store;
    std::unique_ptr<ILLMProvider> provider;
    std::unique_ptr<ULLMAgentOrchestrator> orchestrator;
    LLMProviderProfile profile;

    explicit Harness(RDK::UApplication* app = nullptr);

    /// Rebuild provider + orchestrator after env is applied (thinking vs default lab model).
    void rebindProvider(const LLMProviderProfile& new_profile);
};

/// Default lab e2e profile: OpenAI-compat + `labOllamaModelName()` (qwen2.5).
LLMProviderProfile defaultLabCompatProfile();

/// Thinking Cortex profile: Native + `labOllamaThinkingModelName()` (qwen3:14b).
LLMProviderProfile thinkingLabNativeProfile();

LLMSessionContext defaultSession(bool project_loaded = false);
LLMRequestEnvelope makeRequest(const Harness& h, const std::string& session_id,
                               const std::string& user_text, const LLMSessionContext& session);

bool findToolResultJson(const ULLMConversationStore& store, const std::string& session_id,
                        const std::string& tool_name, nlohmann::json& out);
bool conversationUsedTool(const ULLMConversationStore& store, const std::string& session_id,
                          const std::string& tool_name);
bool conversationAssistantRequestedTool(const ULLMConversationStore& store,
                                        const std::string& session_id,
                                        const std::string& tool_name);

E2eConversationDigest digestConversation(const ULLMConversationStore& store,
                                         const std::string& session_id,
                                         const LLMFinalResponse& final_resp,
                                         const ULLMToolRegistry* registry = nullptr);

struct E2eScenarioRun {
    LLMFinalResponse final_response;
    E2eConversationDigest digest;
};

/// Run a natural-language user message through the orchestrator (optional HITL confirm).
E2eScenarioRun runNaturalLanguageScenario(Harness& h, const std::string& session_id,
                                          const std::string& user_text,
                                          const LLMSessionContext& session,
                                          bool auto_confirm_pending = true);

bool shouldSkipForProviderError(const LLMFinalResponse& resp, std::string* reason = nullptr);

} // namespace RDK::LLM::E2eLab

#endif
