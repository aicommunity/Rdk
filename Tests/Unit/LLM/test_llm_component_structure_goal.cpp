#include <gtest/gtest.h>

#include "Orchestrator/ULLMComponentStructureGoal.h"
#include "Orchestrator/ULLMActOrClarifyGate.h"
#include "Orchestrator/ULLMIntentParser.h"
#include "Orchestrator/ULLMWatchPlotGoal.h"

#include <algorithm>

using namespace RDK::LLM;

TEST(LLMComponentStructureGoal, DetectsDendriteRu)
{
    EXPECT_TRUE(isComponentStructureGoal("сделай 4 дендрита длины 2,3,8,1"));
    EXPECT_TRUE(isComponentStructureGoal("NumSomaMembraneParts = 4"));
    EXPECT_TRUE(isComponentStructureGoal("настрой нейрон StructureBuildMode"));
    EXPECT_FALSE(isComponentStructureGoal("запусти расчет"));
}

TEST(LLMComponentStructureGoal, ParseDendriteLengths)
{
    const ParsedDendriteStructureGoal g =
        parseDendriteStructureGoal("у PNeuron сделай 4 дендрита длинами 2, 3, 8, 1");
    EXPECT_TRUE(g.ok);
    EXPECT_EQ(g.dendrite_count, 4);
    ASSERT_EQ(g.lengths.size(), 4u);
    EXPECT_EQ(g.lengths[0], 2);
    EXPECT_EQ(g.lengths[1], 3);
    EXPECT_EQ(g.lengths[2], 8);
    EXPECT_EQ(g.lengths[3], 1);
    EXPECT_EQ(g.component_token, "PNeuron");
}

TEST(LLMComponentStructureGoal, EphemeralHintMentionsCanon)
{
    const std::string hint = componentStructureEphemeralHint();
    EXPECT_NE(hint.find("StructureBuildMode"), std::string::npos);
    EXPECT_NE(hint.find("NumDendriteMembranePartsVec"), std::string::npos);
    EXPECT_NE(hint.find("add_component"), std::string::npos);
}

TEST(LLMIntentParser, StructureGoalIsMutate)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("4 дендрита длины 2 3 8 1"), LLMIntentKind::Mutate);
}

TEST(LLMActOrClarifyGate, StructureGoalActionable)
{
    EXPECT_TRUE(isActionableGoalForActOrClarify("настрой дендриты", LLMIntentKind::Query,
                                                ConfigurationLifecycleAction::None, false));
}

TEST(LLMWatchPlotGoal, DetectAndParse)
{
    EXPECT_TRUE(isWatchPlotGoal("на график выход генератора и нейрона"));
    const ParsedWatchPlotGoal g = parseWatchPlotGoal("на график выход генератора и нейрона");
    EXPECT_TRUE(g.ok);
    ASSERT_GE(g.component_tokens.size(), 2u);
    EXPECT_EQ(g.property_name, "Output");
    EXPECT_NE(std::find(g.component_tokens.begin(), g.component_tokens.end(), "PGenerator"),
              g.component_tokens.end());
    EXPECT_NE(std::find(g.component_tokens.begin(), g.component_tokens.end(), "PNeuron"),
              g.component_tokens.end());
    EXPECT_FALSE(watchPlotEphemeralHint().empty());
}

TEST(LLMWatchPlotGoal, SoftTokensMergeWithCapWords)
{
    // CapWord "Generator" must not suppress RU soft «нейрон».
    const ParsedWatchPlotGoal g =
        parseWatchPlotGoal("на график Output Generator и нейрона");
    EXPECT_TRUE(g.ok);
    EXPECT_NE(std::find(g.component_tokens.begin(), g.component_tokens.end(), "PNeuron"),
              g.component_tokens.end());
    EXPECT_TRUE(g.want_new_mdi == false);
}

TEST(LLMWatchPlotGoal, NestedLtzonePneuron)
{
    const ParsedWatchPlotGoal g = parseWatchPlotGoal("Добавь на графки выход ltzone pneuron");
    EXPECT_TRUE(g.ok);
    EXPECT_EQ(g.nested_hint, "LTZone");
    EXPECT_EQ(g.property_name, "Output");
    EXPECT_NE(std::find(g.component_tokens.begin(), g.component_tokens.end(), "PNeuron"),
              g.component_tokens.end());
    EXPECT_EQ(std::find(g.component_tokens.begin(), g.component_tokens.end(), "LTZone"),
              g.component_tokens.end());
    EXPECT_FALSE(g.anchor_tokens.empty());
}

TEST(LLMWatchPlotGoal, NestedLowThresholdZoneRu)
{
    const ParsedWatchPlotGoal g =
        parseWatchPlotGoal("добавь на график выход низкопороговой зоны компонента PNeuron");
    EXPECT_TRUE(g.ok);
    EXPECT_EQ(g.nested_hint, "LTZone");
    EXPECT_EQ(g.property_name, "Output");
    EXPECT_NE(std::find(g.component_tokens.begin(), g.component_tokens.end(), "PNeuron"),
              g.component_tokens.end());
    EXPECT_NE(watchPlotEphemeralHint().find("Parent.Child"), std::string::npos);
    EXPECT_NE(watchPlotEphemeralHint().find("get_net_snapshot"), std::string::npos);
}

TEST(LLMWatchPlotGoal, SeparateWatchesMdi)
{
    const ParsedWatchPlotGoal g =
        parseWatchPlotGoal("создай отдельное Watches и добавь Output");
    EXPECT_TRUE(isWatchPlotGoal("создай отдельное Watches и добавь Output"));
    EXPECT_TRUE(g.ok);
    EXPECT_TRUE(g.want_new_mdi);
    EXPECT_EQ(g.property_name, "Output");
}

TEST(LLMComponentStructureGoal, SoftNeuronTokenWithoutCapWord)
{
    const ParsedDendriteStructureGoal g =
        parseDendriteStructureGoal("сделай 4 дендрита длинами 2, 3, 8, 1 у нейрона");
    EXPECT_TRUE(g.ok);
    EXPECT_EQ(g.component_token, "PNeuron");
    EXPECT_EQ(g.dendrite_count, 4);
}
