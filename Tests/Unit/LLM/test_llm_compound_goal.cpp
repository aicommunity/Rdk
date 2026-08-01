#include <gtest/gtest.h>

#include "Packs/ULLMCompoundGoal.h"
#include "Orchestrator/ULLMChannelCalcCommand.h"

using namespace RDK::LLM;

TEST(LLMCompoundGoal, DetectsAddAndStartCalc)
{
    EXPECT_TRUE(isCompoundActionableGoal(
        "добавь три нейрона nspneurongen и запусти расчет"));
    EXPECT_TRUE(isCompoundActionableGoal("add ArduinoBoard and start calculation"));
}

TEST(LLMCompoundGoal, SingleFamilyNotCompound)
{
    EXPECT_FALSE(isCompoundActionableGoal("запусти расчет"));
    EXPECT_FALSE(isCompoundActionableGoal("добавь три нейрона nspneurongen"));
    EXPECT_EQ(detectChannelCalcAction("запусти расчет"), ChannelCalcAction::Start);
}

TEST(LLMCompoundGoal, DependencyOrderPutsAddBeforeCalc)
{
    EXPECT_LT(packDependencyRank("add_component_direct"), packDependencyRank("channel_calc"));
    EXPECT_EQ(workingGoalIdForToolName("start_channel_calculation"), "goal_calc");
    EXPECT_EQ(workingGoalIdForToolName("add_component"), "goal_add");
}
