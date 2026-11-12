#include <gtest/gtest.h>

#include "Orchestrator/ULLMQuantityParser.h"

using namespace RDK::LLM;

TEST(LLMQuantityParser, QuantityExtractTwo)
{
    const ParsedQuantity q = extractQuantityHeuristic("add two neurons");
    EXPECT_EQ(q.count, 2);
    EXPECT_TRUE(q.valid);
}

TEST(LLMQuantityParser, NumericWithTimes)
{
    const ParsedQuantity q = extractQuantityHeuristic("add 3 x NSPNeuron");
    EXPECT_EQ(q.count, 3);
    EXPECT_TRUE(q.valid);
}

TEST(LLMQuantityParser, BareDigitWithCue)
{
    const ParsedQuantity q = extractQuantityHeuristic("add 6 neurons");
    EXPECT_EQ(q.count, 6);
    EXPECT_TRUE(q.valid);
}

TEST(LLMQuantityParser, RussianSix)
{
    const ParsedQuantity q = extractQuantityHeuristic("добавь шесть нейронов");
    EXPECT_EQ(q.count, 6);
    EXPECT_TRUE(q.valid);
}

TEST(LLMQuantityParser, RussianThreeWithClass)
{
    const ParsedQuantity q = extractQuantityHeuristic("добавь три нейрона NLPNeuron");
    EXPECT_EQ(q.count, 3);
    EXPECT_TRUE(q.valid);
}

TEST(LLMQuantityParser, DefaultOne)
{
    const ParsedQuantity q = extractQuantityHeuristic("add one neuron");
    EXPECT_EQ(q.count, 1);
    EXPECT_FALSE(q.valid);
}

TEST(LLMQuantityParser, NoFalsePositiveOnExplain)
{
    const ParsedQuantity q = extractQuantityHeuristic("explain how neurons work");
    EXPECT_FALSE(q.valid);
}

TEST(LLMQuantityParser, LooksLikeQuantityCue)
{
    EXPECT_TRUE(looksLikeQuantityCue("добавь шесть нейронов"));
    EXPECT_FALSE(looksLikeQuantityCue("what is a neuron"));
}
