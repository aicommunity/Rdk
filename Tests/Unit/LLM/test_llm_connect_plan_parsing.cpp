#include <gtest/gtest.h>

#include "Orchestrator/ULLMConnectPlanParsing.h"

using namespace RDK::LLM;

TEST(LLMConnectPlanParsing, ExplicitPairsEnglish)
{
    ParsedConnectGoal g = parseConnectGoal("connect Neuron1 and Neuron2");
    EXPECT_EQ(g.kind, ConnectGoalKind::ExplicitPairs);
    ASSERT_EQ(g.explicit_links.size(), 1u);
    EXPECT_EQ(g.explicit_links[0].from.token, "Neuron1");
    EXPECT_EQ(g.explicit_links[0].to.token, "Neuron2");
}

TEST(LLMConnectPlanParsing, ExplicitPairsRussian)
{
    ParsedConnectGoal g = parseConnectGoal("свяжи Neuron1 и Neuron2");
    EXPECT_EQ(g.kind, ConnectGoalKind::ExplicitPairs);
    ASSERT_EQ(g.explicit_links.size(), 1u);
}

TEST(LLMConnectPlanParsing, ExplicitPorts)
{
    ParsedConnectGoal g = parseConnectGoal("/ch0/A.Out -> /ch0/B.In");
    EXPECT_EQ(g.kind, ConnectGoalKind::ExplicitPorts);
    ASSERT_EQ(g.explicit_links.size(), 1u);
    EXPECT_EQ(g.explicit_links[0].from.property, "Out");
    EXPECT_EQ(g.explicit_links[0].to.property, "In");
}

TEST(LLMConnectPlanParsing, CountOnly)
{
    ParsedConnectGoal g = parseConnectGoal("link 3 pairs");
    EXPECT_EQ(g.kind, ConnectGoalKind::CountOnly);
    EXPECT_EQ(g.link_count, 3);
}

TEST(LLMConnectPlanParsing, RemainingFlags)
{
    ParsedConnectGoal g = parseConnectGoal("connect rest like previous");
    EXPECT_TRUE(g.wants_remaining);
    EXPECT_TRUE(g.wants_analogous);
    EXPECT_EQ(g.kind, ConnectGoalKind::RemainingSessionDelta);
}

TEST(LLMConnectPlanParsing, AnalogousOnly)
{
    ParsedConnectGoal g = parseConnectGoal("connect like previous");
    EXPECT_TRUE(g.wants_analogous);
    EXPECT_EQ(g.kind, ConnectGoalKind::AnalogousToPrevious);
}

TEST(LLMConnectPlanParsing, ModelGraphRemaining)
{
    ParsedConnectGoal g = parseConnectGoal("свяжи оставшиеся на схеме");
    EXPECT_EQ(g.remaining_scope, ConnectRemainingScope::ModelGraph);
}

TEST(LLMConnectPlanParsing, ChainTopology)
{
    ParsedConnectGoal g = parseConnectGoal("connect rest as a chain");
    EXPECT_EQ(g.topology, ConnectTopology::Chain);
}

TEST(LLMConnectPlanParsing, TreeHubToken)
{
    ParsedConnectGoal g = parseConnectGoal("connect from Neuron1 to the rest");
    EXPECT_EQ(g.topology, ConnectTopology::Tree);
    ASSERT_TRUE(g.hub_token.has_value());
    EXPECT_EQ(*g.hub_token, "Neuron1");
}

TEST(LLMConnectPlanParsing, InternalSemanticsHint)
{
    ParsedConnectGoal g = parseConnectGoal("connect neurons via LTZone");
    EXPECT_TRUE(g.wants_internal_semantics_hint);
}

