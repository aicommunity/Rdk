#include <gtest/gtest.h>

#include "Orchestrator/ULLMQuantityParser.h"

using namespace RDK::LLM;

TEST(LLMQuantityParser, QuantityExtractTwo)
{
    const ParsedQuantity q = extractQuantity("add two neurons");
    EXPECT_EQ(q.count, 2);
    EXPECT_TRUE(q.valid);
}

TEST(LLMQuantityParser, NumericWithTimes)
{
    const ParsedQuantity q = extractQuantity("add 3 x NSPNeuron");
    EXPECT_EQ(q.count, 3);
    EXPECT_TRUE(q.valid);
}

TEST(LLMQuantityParser, DefaultOne)
{
    const ParsedQuantity q = extractQuantity("add one neuron");
    EXPECT_EQ(q.count, 1);
    EXPECT_FALSE(q.valid);
}
