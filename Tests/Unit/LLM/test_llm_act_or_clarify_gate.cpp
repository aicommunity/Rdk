#include <gtest/gtest.h>

#include "Orchestrator/ULLMActOrClarifyGate.h"
#include "Orchestrator/ULLMConnectPlanParsing.h"
#include "Orchestrator/ULLMConfigurationLifecycle.h"

using namespace RDK::LLM;

TEST(LLMActOrClarifyGate, ConnectGoalRequiresActionEvenIfQueryIntent)
{
    EXPECT_TRUE(isActionableGoalForActOrClarify(
        "подключил PGenerator к этим нейронам также как к PNeuron", LLMIntentKind::Query,
        ConfigurationLifecycleAction::None, false));
    EXPECT_TRUE(shouldRequireActOrClarify(
        true, true, "подключил PGenerator к PNeuron", LLMIntentKind::Query,
        ConfigurationLifecycleAction::None, false, false, false));
}

TEST(LLMActOrClarifyGate, PureQueryAllowsProse)
{
    EXPECT_FALSE(isActionableGoalForActOrClarify(
        "что на схеме", LLMIntentKind::Query, ConfigurationLifecycleAction::None, false));
    EXPECT_FALSE(shouldRequireActOrClarify(true, true, "что на схеме", LLMIntentKind::Query,
                                           ConfigurationLifecycleAction::None, false, false,
                                           false));
}

TEST(LLMActOrClarifyGate, MutateWithWriteRequiresAction)
{
    EXPECT_TRUE(shouldRequireActOrClarify(true, true, "add NSPNeuron", LLMIntentKind::Mutate,
                                          ConfigurationLifecycleAction::None, true, false, false));
}

TEST(LLMActOrClarifyGate, NoToolsOfferedSkipsGate)
{
    EXPECT_FALSE(shouldRequireActOrClarify(false, true, "подключи A к B", LLMIntentKind::Mutate,
                                           ConfigurationLifecycleAction::None, true, false, false));
}

TEST(LLMActOrClarifyGate, UnderstandingPhaseSkipsGate)
{
    EXPECT_FALSE(shouldRequireActOrClarify(true, true, "подключи A к B", LLMIntentKind::Mutate,
                                           ConfigurationLifecycleAction::None, true, false, true));
}
