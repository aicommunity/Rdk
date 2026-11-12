#include <gtest/gtest.h>

#include <cstdlib>

#include "Orchestrator/ULLMQuantityResolver.h"
#include "Providers/ULLMMockProvider.h"

using namespace RDK::LLM;

TEST(LLMQuantityResolver, MergesOriginalAndEnglishTakingMax)
{
    QuantityResolveRequest req;
    req.text_original = "добавь шесть нейронов";
    req.text_en = "add two neurons";
    req.allow_llm_fallback = false;
    const QuantityResolveResult res = resolveUserQuantity(req, nullptr);
    ASSERT_TRUE(res.quantity.valid);
    EXPECT_EQ(res.quantity.primary, 6);
    EXPECT_EQ(res.quantity.source, QuantitySource::Heuristic);
}

TEST(LLMQuantityResolver, PrimaryQuantityOrUsesSession)
{
    ConversationState state;
    state.last_quantity.valid = true;
    state.last_quantity.primary = 4;
    EXPECT_EQ(primaryQuantityOr(state), 4);
    EXPECT_EQ(primaryQuantityOr(ConversationState{}), 1);
}

TEST(LLMQuantityResolver, DisambiguationFollowUpSkipsReResolve)
{
    ConversationState state;
    state.last_quantity.valid = true;
    state.last_quantity.primary = 3;
    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.disambiguation_candidates = nlohmann::json::array({{{"class_name", "NLPNeuron"}}});
    state.pending_tool_arguments = pending;
    EXPECT_TRUE(isDisambiguationOnlyFollowUp("1", state));
}

TEST(LLMQuantityResolver, LlmFallbackWhenHeuristicMisses)
{
    ULLMMockProvider mock;
    LLMCompletionResult llm;
    llm.ok = true;
    llm.text = R"({"primary_count": 6})";
    mock.enqueue(llm);

    QuantityResolveRequest req;
    req.text_original = "добавь нейронов";
    req.text_en = "add neurons please";
    req.allow_llm_fallback = true;

    unsetenv("NMSDK_LLM_QUANTITY_LLM");
    setenv("NMSDK_LLM_QUANTITY_LLM", "1", 1);

    const QuantityResolveResult res = resolveUserQuantity(req, &mock);
    EXPECT_TRUE(res.quantity.valid);
    EXPECT_EQ(res.quantity.primary, 6);
    EXPECT_EQ(res.quantity.source, QuantitySource::Llm);
    EXPECT_EQ(mock.invokeCount(), 1u);

    unsetenv("NMSDK_LLM_QUANTITY_LLM");
}
