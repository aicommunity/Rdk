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
    EXPECT_FALSE(
        ULLMAutonomousPolicy::isToolWhitelisted("set_property", LLMAutonomousMode::Strict));
}

TEST(LLMAutonomousPolicy, StepLimitDenied)
{
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "get_net_snapshot", LLMAutonomousMode::Strict, 3, 3);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "AUTONOMOUS_STEP_LIMIT");
}

TEST(LLMAutonomousPolicy, NonWhitelistedDenied)
{
    AutonomousStepDecision d = ULLMAutonomousPolicy::checkStep(
        "connect_components", LLMAutonomousMode::SemiAuto, 0, 3);
    EXPECT_FALSE(d.allowed);
    EXPECT_EQ(d.deny_code, "AUTONOMOUS_TOOL_NOT_WHITELISTED");
}
