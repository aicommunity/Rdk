#include <gtest/gtest.h>

#include "Orchestrator/ULLMTaskPlanParsing.h"

using namespace RDK::LLM;

TEST(LLMTaskPlanParsing, ExtractsMultipleClassSpecs)
{
    const std::vector<ClassAddSpec> specs =
        extractClassAddSpecsFromGoal("add 3 NSPGenerator and 2 UModel");
    ASSERT_EQ(specs.size(), 2u);
    EXPECT_EQ(specs[0].class_name, "NSPGenerator");
    EXPECT_EQ(specs[0].count, 3);
    EXPECT_EQ(specs[1].class_name, "UModel");
    EXPECT_EQ(specs[1].count, 2);
}

TEST(LLMTaskPlanParsing, ExtractsRussianMultiClass)
{
    const std::vector<ClassAddSpec> specs =
        extractClassAddSpecsFromGoal("Добавь 2 NCGenerator и 3 NLPNeuron");
    ASSERT_EQ(specs.size(), 2u);
    EXPECT_EQ(specs[0].class_name, "NCGenerator");
    EXPECT_EQ(specs[0].count, 2);
    EXPECT_EQ(specs[1].class_name, "NLPNeuron");
    EXPECT_EQ(specs[1].count, 3);
}

TEST(LLMTaskPlanParsing, SinglePairReturnsOneSpec)
{
    const std::vector<ClassAddSpec> specs = extractClassAddSpecsFromGoal("add 2 NSPNeuron");
    ASSERT_EQ(specs.size(), 1u);
    EXPECT_EQ(specs[0].count, 2);
}

TEST(LLMTaskPlanParsing, NoPairsReturnsEmpty)
{
    EXPECT_TRUE(extractClassAddSpecsFromGoal("add NSPNeuron").empty());
}
