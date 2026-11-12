#include <gtest/gtest.h>

#include "Orchestrator/ULLMChannelCalcCommand.h"
#include "Orchestrator/ULLMActOrClarifyGate.h"
#include "Orchestrator/ULLMIntentParser.h"
#include "Policy/ULLMAutonomousPolicy.h"

using namespace RDK::LLM;

TEST(LLMChannelCalcCommand, DetectStartRu)
{
    EXPECT_EQ(detectChannelCalcAction("запусти расчет"), ChannelCalcAction::Start);
    EXPECT_EQ(detectChannelCalcAction("Запусти расчёт"), ChannelCalcAction::Start);
    EXPECT_EQ(detectChannelCalcAction("запусти расч"), ChannelCalcAction::Start);
}

TEST(LLMChannelCalcCommand, DetectStartEn)
{
    EXPECT_EQ(detectChannelCalcAction("start calculation"), ChannelCalcAction::Start);
    EXPECT_EQ(detectChannelCalcAction("run calculation"), ChannelCalcAction::Start);
    EXPECT_EQ(detectChannelCalcAction("start calc"), ChannelCalcAction::Start);
}

TEST(LLMChannelCalcCommand, DetectPauseResetStep)
{
    EXPECT_EQ(detectChannelCalcAction("останови расчет"), ChannelCalcAction::Pause);
    EXPECT_EQ(detectChannelCalcAction("pause calculation"), ChannelCalcAction::Pause);
    EXPECT_EQ(detectChannelCalcAction("сброс расчета"), ChannelCalcAction::Reset);
    EXPECT_EQ(detectChannelCalcAction("reset calculation"), ChannelCalcAction::Reset);
    EXPECT_EQ(detectChannelCalcAction("шаг расчета"), ChannelCalcAction::Step);
    EXPECT_EQ(detectChannelCalcAction("step calculation"), ChannelCalcAction::Step);
}

TEST(LLMChannelCalcCommand, NonCalcIsNone)
{
    EXPECT_EQ(detectChannelCalcAction("добавь нейрон"), ChannelCalcAction::None);
    EXPECT_EQ(detectChannelCalcAction("подключи A к B"), ChannelCalcAction::None);
    EXPECT_FALSE(isChannelCalcGoalText("покажи схему"));
}

TEST(LLMChannelCalcCommand, ToolNameAndChannelIndex)
{
    EXPECT_STREQ(toolNameForChannelCalcAction(ChannelCalcAction::Start),
                 "start_channel_calculation");
    EXPECT_EQ(channelIndexForCalcRequest("запусти расчет", 2), -1);
    EXPECT_EQ(channelIndexForCalcRequest("запусти расчет на текущем канале", 2), 2);
}

TEST(LLMChannelCalcCommand, FormatOutcome)
{
    ToolGatewayResult tr;
    tr.ok = true;
    EXPECT_NE(formatChannelCalcUserMessage(ChannelCalcAction::Start, tr, -1).find("started"),
              std::string::npos);
    tr.ok = false;
    tr.message = "No configuration is open";
    EXPECT_EQ(formatChannelCalcUserMessage(ChannelCalcAction::Start, tr, -1),
              "No configuration is open");
}

TEST(LLMIntentParser, StartCalculationIsMutate)
{
    ULLMIntentParser parser;
    EXPECT_EQ(parser.parse("запусти расчет"), LLMIntentKind::Mutate);
    EXPECT_EQ(parser.parse("start calculation"), LLMIntentKind::Mutate);
}

TEST(LLMActOrClarifyGate, CalcGoalRequiresAction)
{
    EXPECT_TRUE(isActionableGoalForActOrClarify("запусти расчет", LLMIntentKind::Query,
                                                ConfigurationLifecycleAction::None, false));
    EXPECT_TRUE(shouldRequireActOrClarify(true, true, "запусти расчет", LLMIntentKind::Query,
                                          ConfigurationLifecycleAction::None, false, false,
                                          false));
}

TEST(LLMAutonomousPolicy, ChannelCalcAndAskUserWhitelisted)
{
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("start_channel_calculation",
                                                          LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("pause_channel_calculation",
                                                          LLMAutonomousMode::Strict));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("list_channels",
                                                          LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("set_active_channel",
                                                          LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("run_n_steps",
                                                          LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("select_component",
                                                          LLMAutonomousMode::Strict));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("clone_component",
                                                          LLMAutonomousMode::SemiAuto));
    EXPECT_FALSE(ULLMAutonomousPolicy::isToolWhitelisted("delete_channel",
                                                           LLMAutonomousMode::SemiAuto));
    EXPECT_FALSE(ULLMAutonomousPolicy::isToolWhitelisted("export_component",
                                                           LLMAutonomousMode::Strict));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("ask_user", LLMAutonomousMode::Strict));

    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "ask_user", LLMAutonomousMode::SemiAuto, 99, 3);
    EXPECT_TRUE(d.allowed);

    AutonomousStepDecision calc = ULLMAutonomousPolicy::checkStep(
        "start_channel_calculation", LLMAutonomousMode::Strict, 99, 3);
    EXPECT_TRUE(calc.allowed) << "calc must not consume write step budget";

    AutonomousStepDecision run_n = ULLMAutonomousPolicy::checkStep(
        "run_n_steps", LLMAutonomousMode::Strict, 99, 3);
    EXPECT_TRUE(run_n.allowed);
}
