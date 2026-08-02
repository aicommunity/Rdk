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
        ConfigurationLifecycleAction::None, false, false, false, false));
}

TEST(LLMActOrClarifyGate, PureQueryRequiresInitialTool)
{
    EXPECT_TRUE(isActionableGoalForActOrClarify(
        "что на схеме", LLMIntentKind::Query, ConfigurationLifecycleAction::None, false));
    EXPECT_TRUE(shouldRequireActOrClarify(true, true, "что на схеме", LLMIntentKind::Query,
                                          ConfigurationLifecycleAction::None, false, false, false,
                                          false));
}

TEST(LLMActOrClarifyGate, QueryAllowsProseAfterSuccessfulReadOnlyEvidence)
{
    EXPECT_FALSE(shouldRequireActOrClarify(true, true, "расскажи о проекте", LLMIntentKind::Query,
                                           ConfigurationLifecycleAction::None, false, false, false,
                                           true, true));
    EXPECT_FALSE(shouldRequireActOrClarify(true, true, "explain the model", LLMIntentKind::Explain,
                                           ConfigurationLifecycleAction::None, false, false, false,
                                           true, true));
}

TEST(LLMActOrClarifyGate, QueryRequiresActionAfterFailedOnlyEvidence)
{
    // chat 17-25-18: PATH_NOT_ALLOWED must not unlock an error essay.
    EXPECT_TRUE(shouldRequireActOrClarify(true, true, "расскажи о проекте", LLMIntentKind::Query,
                                          ConfigurationLifecycleAction::None, false, false, false,
                                          true, false));
}

TEST(LLMActOrClarifyGate, MutateAllowsProseAfterSuccessfulReadOnlyEvidence)
{
    // chat 17-11-11: bare «проект» → Mutate, but get_net_snapshot already ok.
    EXPECT_FALSE(shouldRequireActOrClarify(true, true, "расскажи о проекте", LLMIntentKind::Mutate,
                                           ConfigurationLifecycleAction::None, true, false, false,
                                           true, true));
}

TEST(LLMActOrClarifyGate, MutateStillRequiresActionWithoutReadEvidence)
{
    EXPECT_TRUE(shouldRequireActOrClarify(true, true, "add NSPNeuron", LLMIntentKind::Mutate,
                                          ConfigurationLifecycleAction::None, true, false, false,
                                          true, false));
}

TEST(LLMActOrClarifyGate, TurnHasSuccessfulReadOnlyEvidence)
{
    TurnToolInvocationView snap;
    snap.tool_name = "get_net_snapshot";
    snap.ok = true;
    EXPECT_TRUE(turnHasSuccessfulReadOnlyEvidence({snap}));

    TurnToolInvocationView write;
    write.tool_name = "add_component";
    write.ok = true;
    EXPECT_FALSE(turnHasSuccessfulReadOnlyEvidence({snap, write}));

    TurnToolInvocationView fail;
    fail.tool_name = "get_net_snapshot";
    fail.ok = false;
    EXPECT_FALSE(turnHasSuccessfulReadOnlyEvidence({fail}));
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
