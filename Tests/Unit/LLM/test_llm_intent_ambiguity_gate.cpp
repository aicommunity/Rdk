#include <gtest/gtest.h>

#include "Intent/ULLMIntentAmbiguityGate.h"
#include "Tools/ULLMToolGateway.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

TEST(LLMIntentAmbiguityGate, BlocksWriteOnLowConfidence)
{
    ConversationState state;
    state.intent_contract_kind = LLMIntentKind::Mutate;

    IntentParseResult intent;
    intent.kind = LLMIntentKind::Query;
    intent.confidence = 0.1f;

    ULLMToolRegistry registry;
    LLMToolDefinition write_def;
    write_def.name = "set_property";
    write_def.kind = LLMToolKind::Write;
    write_def.input_schema = {{"type", "object"}};
    registry.registerTool(write_def, [](const nlohmann::json&) {
        return ToolGatewayResult{};
    });

    LLMToolCall call;
    call.name = "set_property";
    call.arguments = nlohmann::json::object();

    const IntentAmbiguityDecision decision =
        evaluateIntentAmbiguity(state, intent, {call}, registry);
    EXPECT_TRUE(decision.block_writes);
    EXPECT_EQ(decision.reason_code, "LOW_CONFIDENCE");
}

TEST(LLMIntentAmbiguityGate, BlocksMixedReadWriteWhenNotMutate)
{
    ConversationState state;
    state.intent_contract_kind = LLMIntentKind::Query;

    IntentParseResult intent;
    intent.kind = LLMIntentKind::Query;
    intent.confidence = 0.9f;

    ULLMToolRegistry registry;
    LLMToolDefinition read_def;
    read_def.name = "get_net_snapshot";
    read_def.kind = LLMToolKind::Read;
    read_def.input_schema = {{"type", "object"}};
    registry.registerTool(read_def, [](const nlohmann::json&) {
        return ToolGatewayResult{};
    });

    LLMToolDefinition write_def;
    write_def.name = "set_property";
    write_def.kind = LLMToolKind::Write;
    write_def.input_schema = {{"type", "object"}};
    registry.registerTool(write_def, [](const nlohmann::json&) {
        return ToolGatewayResult{};
    });

    LLMToolCall read_call;
    read_call.name = "get_net_snapshot";
    LLMToolCall write_call;
    write_call.name = "set_property";

    const IntentAmbiguityDecision decision = evaluateIntentAmbiguity(
        state, intent, {read_call, write_call}, registry);
    EXPECT_TRUE(decision.block_writes);
    EXPECT_EQ(decision.reason_code, "MIXED_INTENT");
}
