#include <gtest/gtest.h>

#include "Orchestrator/ULLMIntentParser.h"

using namespace RDK::LLM;

TEST(LLMIntentParser, QueryIntent)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("покажи список компонентов"), LLMIntentKind::Query);
    EXPECT_EQ(parser.parse("what components are on the diagram"), LLMIntentKind::Query);
    EXPECT_EQ(parser.parse("расскажи о проекте"), LLMIntentKind::Query);
    EXPECT_EQ(parser.parse("tell me about the project"), LLMIntentKind::Query);
}

TEST(LLMIntentParser, ProjectLifecycleStaysMutate)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("создай новый проект"), LLMIntentKind::Mutate);
    EXPECT_EQ(parser.parse("открой проект"), LLMIntentKind::Mutate);
    EXPECT_EQ(parser.parse("create project"), LLMIntentKind::Mutate);
}

TEST(LLMIntentParser, ConfidencePrefersMutateOverWeakQuery)
{
    ULLMIntentParser parser;
    const auto result = parser.parseDetailed("add component MatrixSource to diagram");
    EXPECT_EQ(result.kind, LLMIntentKind::Mutate);
    EXPECT_GE(result.confidence, 0.4f);
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
    EXPECT_EQ(parser.parse("create new config"), LLMIntentKind::Mutate);
    EXPECT_EQ(parser.parse("create config"), LLMIntentKind::Mutate);
}

TEST(LLMIntentParser, ConnectPodkluchIsMutate)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("подключи PNeuron2 к PNeuron3"), LLMIntentKind::Mutate);
    EXPECT_EQ(parser.parse("подключил PGenerator к этим нейронам также как к PNeuron"),
              LLMIntentKind::Mutate);
    EXPECT_EQ(parser.parse("connect A to B"), LLMIntentKind::Mutate);
}
