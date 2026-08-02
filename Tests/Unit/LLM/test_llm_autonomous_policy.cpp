#include <gtest/gtest.h>

#include "Policy/ULLMAutonomousPolicy.h"

using namespace RDK::LLM;

TEST(LLMAutonomousPolicy, OffAllowsAnyTool)
{
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("delete_everything", LLMAutonomousMode::Off));
    AutonomousStepDecision d =
        ULLMAutonomousPolicy::checkStep("delete_everything", LLMAutonomousMode::Off, 99, 3);
    EXPECT_TRUE(d.allowed);
}

TEST(LLMAutonomousPolicy, StrictWhitelistsCoreTools)
{
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("add_component", LLMAutonomousMode::Strict));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("set_property", LLMAutonomousMode::Strict));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("create_configuration",
                                                         LLMAutonomousMode::Strict));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("load_configuration",
                                                         LLMAutonomousMode::SemiAuto));
}

TEST(LLMAutonomousPolicy, StepLimitDenied)
{
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "add_component", LLMAutonomousMode::Strict, 3, 3);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "AUTONOMOUS_STEP_LIMIT");
}

TEST(LLMAutonomousPolicy, ReadToolsDoNotConsumeStepBudget)
{
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "get_net_snapshot", LLMAutonomousMode::Strict, 99, 3);
    EXPECT_TRUE(d.allowed);
}

TEST(LLMAutonomousPolicy, AutonomousAllowsConnect)
{
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("connect_components", LLMAutonomousMode::SemiAuto));
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "connect_components", LLMAutonomousMode::SemiAuto, 0, 5);
    EXPECT_TRUE(d.allowed);
}

TEST(LLMAutonomousPolicy, NonWhitelistedDenied)
{
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "delete_everything", LLMAutonomousMode::SemiAuto, 0, 3);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "AUTONOMOUS_TOOL_NOT_WHITELISTED");
}

TEST(LLMAutonomousPolicy, LifecycleCreateAllowedInAutonomous)
{
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "create_configuration", LLMAutonomousMode::SemiAuto, 0, 3);
    EXPECT_TRUE(d.allowed);
}

TEST(LLMAutonomousPolicy, ProjectKnowledgeReadsWhitelisted)
{
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("inspect_configuration",
                                                          LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("search_project_docs",
                                                          LLMAutonomousMode::Strict));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("list_project_files", LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("stat_project_file", LLMAutonomousMode::Strict));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("search_tools", LLMAutonomousMode::SemiAuto));
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "inspect_configuration", LLMAutonomousMode::Strict, 99, 3);
    EXPECT_TRUE(d.allowed);
}

TEST(LLMAutonomousPolicy, UiWatchAndProposePlanWhitelistedWithoutStepBurn)
{
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("show_ui_panel", LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(ULLMAutonomousPolicy::isToolWhitelisted("open_component_gui_tab",
                                                          LLMAutonomousMode::Strict));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("list_ui_panels", LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("propose_plan", LLMAutonomousMode::Strict));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("add_watch_series", LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("list_watch_series", LLMAutonomousMode::Strict));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("create_watch_mdi", LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("list_watch_mdi", LLMAutonomousMode::Strict));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("focus_watch_mdi", LLMAutonomousMode::SemiAuto));
    EXPECT_TRUE(
        ULLMAutonomousPolicy::isToolWhitelisted("close_watch_mdi", LLMAutonomousMode::Strict));

    AutonomousStepDecision ui = ULLMAutonomousPolicy::checkStep(
        "show_ui_panel", LLMAutonomousMode::Strict, 99, 3);
    EXPECT_TRUE(ui.allowed);

    AutonomousStepDecision watch = ULLMAutonomousPolicy::checkStep(
        "add_watch_series", LLMAutonomousMode::Strict, 99, 3);
    EXPECT_TRUE(watch.allowed);

    AutonomousStepDecision plan = ULLMAutonomousPolicy::checkStep(
        "propose_plan", LLMAutonomousMode::Strict, 99, 3);
    EXPECT_TRUE(plan.allowed);
}
