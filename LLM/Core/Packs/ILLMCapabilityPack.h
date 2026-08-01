#ifndef RDK_LLM_CAPABILITY_PACK_H
#define RDK_LLM_CAPABILITY_PACK_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "../LlmTypes.h"
#include "../Orchestrator/ULLMAgentOrchestrator.h"
#include "../Orchestrator/ULLMWorkflowState.h"
#include "../Session/ULLMConversationStore.h"

namespace RDK::LLM {

class ULLMToolRegistry;
class ULLMToolGateway;
class ULLMSystemLogReader;
class ILLMProvider;

struct PackMatch {
    float score = 0.f;
    std::string reason;
};

struct PackHintContribution {
    std::string ephemeral_markdown;
    std::vector<std::string> extra_tool_names;
    std::vector<std::string> act_or_clarify_recovery_tools;
};

struct PackTurnSnapshot {
    const LLMRequestEnvelope* req = nullptr;
    ConversationState* state = nullptr;
    LLMSessionContext* session = nullptr;
    std::string planning_text;
    std::string entity_user_text_hint;
    bool skip_pre_llm_funnel = false;
    ULLMToolRegistry* registry = nullptr;
    ULLMToolGateway* gateway = nullptr;
    ILLMProvider* provider = nullptr;
    ULLMConversationStore* store = nullptr;
    ULLMSystemLogReader* log_reader = nullptr;
    std::function<void(LLMWorkflowPhase)> set_phase;
    /// Optional: orchestrator write path with preview/HITL (add_component_direct).
    std::function<LLMFinalResponse(const std::string& tool_name, const nlohmann::json& arguments)>
        invoke_tool_direct;
    /// Optional: class/component disambiguation ("class" | "component").
    std::function<LLMFinalResponse(const LLMToolCall& call, const std::string& kind,
                                   const std::string& field, const nlohmann::json& disambiguation)>
        route_clarification;
};

struct RecordedStrategyResult {
    bool handled = false;
    LLMFinalResponse response;
};

class ILLMCapabilityPack {
public:
    virtual ~ILLMCapabilityPack() = default;
    virtual const char* id() const = 0;
    virtual PackMatch match(const PackTurnSnapshot& snap) const = 0;
    virtual PackHintContribution hints(const PackTurnSnapshot& snap) const
    {
        (void)snap;
        return {};
    }
    virtual RecordedStrategyResult tryRecorded(PackTurnSnapshot& snap)
    {
        (void)snap;
        return {};
    }
};

class ILLMCapabilityPackRegistry {
public:
    virtual ~ILLMCapabilityPackRegistry() = default;
    virtual void registerPack(std::unique_ptr<ILLMCapabilityPack> pack) = 0;
    virtual std::vector<ILLMCapabilityPack*> packs() const = 0;
    virtual std::vector<std::pair<ILLMCapabilityPack*, PackMatch>>
    rank(const PackTurnSnapshot& snap) const = 0;
};

/// Runs Recorded strategies for packs at/above `score_threshold`. When `matched_ids_out` is set,
/// fills pack ids with match score >= `hint_min_score` (default 0.4, DD-PACK-001 hints band).
RecordedStrategyResult tryRecordedCapabilityPacks(ILLMCapabilityPackRegistry& packs,
                                                    PackTurnSnapshot& snap,
                                                    float score_threshold = 0.85f,
                                                    std::vector<std::string>* matched_ids_out = nullptr,
                                                    float hint_min_score = 0.4f);

inline void mergePackToolNames(ToolFilter& filter, const std::vector<std::string>& names)
{
    if(names.empty())
        return;
    if(!filter.allowed_tool_names)
        filter.allowed_tool_names = std::unordered_set<std::string>{};
    for(const std::string& name : names)
        filter.allowed_tool_names->insert(name);
}

std::vector<std::string> collectPackRecoveryTools(ILLMCapabilityPackRegistry& packs,
                                                  const std::vector<std::string>& matched_ids);

std::string collectPackHintsMarkdown(ILLMCapabilityPackRegistry& packs,
                                     const PackTurnSnapshot& snap, float min_score = 0.4f);

std::vector<std::string> collectPackExtraToolNames(ILLMCapabilityPackRegistry& packs,
                                                   const PackTurnSnapshot& snap,
                                                   float min_score = 0.4f);

} // namespace RDK::LLM

#endif
