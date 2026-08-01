#ifndef RDK_LLM_TURN_CONTEXT_H
#define RDK_LLM_TURN_CONTEXT_H

#include <string>
#include <vector>

#include "../ULLMAgentOrchestrator.h"
#include "../ULLMConfigurationLifecycle.h"
#include "../../Packs/ILLMCapabilityPack.h"
#include "../../Session/ULLMConversationStore.h"

namespace RDK::LLM {

class ILLMProvider;

struct TurnServices {
    ILLMProvider& provider;
    ULLMToolRegistry& registry;
    ULLMToolGateway& gateway;
    ULLMConversationStore& store;
    ILLMCapabilityPackRegistry& packs;
    ULLMAgentOrchestrator& orch;
};

struct TurnContext {
    LLMRequestEnvelope req;
    const LLMStreamHandlers* stream = nullptr;
    ConversationState* state = nullptr;
    LLMSessionContext session;
    LLMGuiContextSnapshot gui;
    LLMIntentKind intent = LLMIntentKind::Auto;
    ConfigurationLifecycleAction lifecycle = ConfigurationLifecycleAction::None;
    ToolFilter tool_filter{};
    std::string planning_text;
    std::string entity_user_text_hint;
    std::vector<std::string> matched_pack_ids;
    float top_pack_score = 0.f;
    LLMFinalResponse final{};
    bool skip_pre_llm_funnel = false;
};

} // namespace RDK::LLM

#endif
