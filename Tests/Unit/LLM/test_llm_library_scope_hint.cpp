#include <gtest/gtest.h>

#include "Orchestrator/ULLMLibraryScopeHint.h"

using namespace RDK::LLM;

TEST(LLMLibraryScopeHint, InferNeuronFromRussianUserText)
{
    const std::vector<std::string> registered = {"NPulseNeuron", "NNeuronsLayer"};
    const std::optional<std::string> inferred =
        inferAddComponentClassFromUserText("добавь нейрон", registered);
    ASSERT_TRUE(inferred.has_value());
    EXPECT_EQ(*inferred, "NPulseNeuron");
}

TEST(LLMLibraryScopeHint, ResolveNeuronAlias)
{
    EXPECT_EQ(resolveComponentClassName("нейрон", LibraryScopeHint::None), "NPulseNeuron");
}

TEST(LLMLibraryScopeHint, DetectPulseScopeFromNeuronKeyword)
{
    EXPECT_EQ(detectLibraryScopeFromUserText("добавь нейрон"), LibraryScopeHint::Pulse);
}
