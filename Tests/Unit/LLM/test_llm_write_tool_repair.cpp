#include <gtest/gtest.h>

#include "Orchestrator/ULLMWriteToolRepair.h"

using namespace RDK::LLM;

TEST(LLMWriteToolRepair, ConnectClearsPortsOnFirstAttempt)
{
    nlohmann::json args = {{"from_long_name", "Model/A"},
                           {"from_property", "BadOut"},
                           {"to_long_name", "Model/B"},
                           {"to_property", "BadIn"}};
    VerifyResult verify;
    verify.satisfied = false;
    verify.detail = "link_missing_in_snapshot";

    const RepairAttemptResult repair = attemptRepair("connect_components", args, verify, 0);
    EXPECT_TRUE(repair.retry);
    EXPECT_EQ(repair.new_args["from_property"], "");
    EXPECT_EQ(repair.new_args["to_property"], "");
    EXPECT_EQ(repair.new_args["from_long_name"], "Model/A");
}

TEST(LLMWriteToolRepair, AddComponentSuffixesShortNameOnFirstAttempt)
{
    nlohmann::json args = {{"class_name", "PulseNeuron"},
                           {"parent_long_name", "Model"},
                           {"short_name", "Neuron"}};
    VerifyResult verify;
    verify.satisfied = false;
    verify.detail = "component_under_parent count=0 class=PulseNeuron";

    const RepairAttemptResult repair = attemptRepair("add_component", args, verify, 0);
    EXPECT_TRUE(repair.retry);
    EXPECT_EQ(repair.new_args["short_name"], "Neuron2");
}

TEST(LLMWriteToolRepair, SecondAttemptReturnsUserMessage)
{
    nlohmann::json args = {{"from_long_name", "Model/A"},
                           {"from_property", "Out"},
                           {"to_long_name", "Model/B"},
                           {"to_property", "In"}};
    VerifyResult verify;
    verify.satisfied = false;
    verify.detail = "link_missing_in_snapshot";

    const RepairAttemptResult repair = attemptRepair("connect_components", args, verify, 1);
    EXPECT_FALSE(repair.retry);
    EXPECT_EQ(repair.user_message, "link_missing_in_snapshot");
}

TEST(LLMWriteToolRepair, UnknownToolNoRetry)
{
    VerifyResult verify;
    verify.satisfied = false;
    verify.detail = "unsupported";

    const RepairAttemptResult repair =
        attemptRepair("set_property", nlohmann::json::object(), verify, 0);
    EXPECT_FALSE(repair.retry);
    EXPECT_EQ(repair.user_message, "unsupported");
}
