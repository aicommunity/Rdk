#include "Intent/ULLMIntentAmbiguityGate.h"

#include <cstdlib>

namespace RDK::LLM {

float intentMinWriteConfidence()
{
    static float cached = -1.0f;
    if(cached >= 0.0f)
        return cached;
    if(const char* env = std::getenv("NMSDK_LLM_INTENT_MIN_WRITE_CONFIDENCE"))
    {
        try
        {
            cached = std::stof(env);
        }
        catch(...)
        {
            cached = 0.65f;
        }
    }
    else
    {
        cached = 0.65f;
    }
    return cached;
}

IntentAmbiguityDecision evaluateIntentAmbiguity(const ConversationState& state,
                                                const IntentParseResult& intent_result,
                                                const std::vector<LLMToolCall>& tool_calls,
                                                const ULLMToolRegistry& registry)
{
    IntentAmbiguityDecision decision;
    bool has_read = false;
    bool has_write = false;
    for(const LLMToolCall& call : tool_calls)
    {
        const LLMToolDefinition* def = registry.find(call.name);
        if(!def)
            continue;
        if(def->kind == LLMToolKind::Write)
            has_write = true;
        else if(def->kind == LLMToolKind::Read)
            has_read = true;
    }
    if(!has_write)
        return decision;

    if(intent_result.confidence < intentMinWriteConfidence())
    {
        decision.block_writes = true;
        decision.reason_code = "LOW_CONFIDENCE";
        decision.user_message =
            "Your request is ambiguous for write actions. Please clarify what to change.";
        return decision;
    }

    if(has_read && has_write && intent_result.kind != LLMIntentKind::Mutate)
    {
        decision.block_writes = true;
        decision.reason_code = "MIXED_INTENT";
        decision.user_message =
            "Mixed read/write intent detected. Confirm the change you want before applying writes.";
        return decision;
    }

    if(state.intent_contract_kind == LLMIntentKind::Auto
       && intent_result.kind != LLMIntentKind::Mutate)
    {
        decision.block_writes = true;
        decision.reason_code = "AMBIGUOUS_INTENT";
        decision.user_message =
            "Write action blocked until intent is explicit. Rephrase as a clear change request.";
        return decision;
    }

    return decision;
}

} // namespace RDK::LLM
