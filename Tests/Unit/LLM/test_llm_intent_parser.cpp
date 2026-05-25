#include <gtest/gtest.h>

#include "Orchestrator/ULLMIntentParser.h"

using namespace RDK::LLM;

TEST(LLMIntentParser, QueryIntent)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("покажи список компонентов"), LLMIntentKind::Query);
    EXPECT_EQ(parser.parse("what components are on the diagram"), LLMIntentKind::Query);
}

TEST(LLMIntentParser, PlanIntent)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("составь план изменений"), LLMIntentKind::Plan);
    EXPECT_EQ(parser.parse("plan steps to add component"), LLMIntentKind::Plan);
}

TEST(LLMIntentParser, MutateIntent)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("добавь MatrixSource"), LLMIntentKind::Mutate);
    EXPECT_EQ(parser.parse("add component Foo"), LLMIntentKind::Mutate);
}
