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

