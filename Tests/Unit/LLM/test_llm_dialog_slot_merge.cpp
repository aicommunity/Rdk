#include <gtest/gtest.h>

#include "Orchestrator/ULLMDialogSlotMerge.h"

using namespace RDK::LLM;

namespace {

LLMMessage userMsg(const std::string& text)
{
    LLMMessage msg;
    msg.role = LLMMessage::Role::User;
    msg.content = text;
    return msg;
}

DialogSlotMergeResult mergeMessages(const std::vector<LLMMessage>& messages,
                                    const PendingToolArguments* pending = nullptr)
{
    DialogSlotMergeInput in{&messages, pending, nullptr, nullptr, {}};
    return mergeDialogSlotsIntoToolArgs(in);
}

} // namespace

TEST(LLMDialogSlotMerge, Chat2215MultiTurnConnectPartial)
{
    const std::vector<LLMMessage> messages = {
        userMsg("PNeuron2 и PNeuron"),
        userMsg("LTZone.Output"),
        userMsg("Soma1.ExcSynapse1"),
    };

    const DialogSlotMergeResult result = mergeMessages(messages);
    EXPECT_TRUE(result.has_connect_endpoints);
    EXPECT_EQ(result.inferred_tool_name, "connect_components");
    EXPECT_EQ(result.partial_arguments.value("from_long_name", ""), "PNeuron2");
    EXPECT_EQ(result.partial_arguments.value("to_long_name", ""), "PNeuron");
    EXPECT_EQ(result.partial_arguments.value("from_property", ""), "LTZone.Output");
    EXPECT_EQ(result.partial_arguments.value("to_property", ""), "Soma1.ExcSynapse1");
    ASSERT_TRUE(result.from_class.has_value());
    ASSERT_TRUE(result.to_class.has_value());
    EXPECT_EQ(*result.from_class, "PNeuron2");
    EXPECT_EQ(*result.to_class, "PNeuron");
}

TEST(LLMDialogSlotMerge, ExplicitPortsSingleTurn)
{
    const std::vector<LLMMessage> messages = {
        userMsg("connect /ch0/A.Out -> /ch0/B.In"),
    };

    const DialogSlotMergeResult result = mergeMessages(messages);
    EXPECT_TRUE(result.has_connect_endpoints);
    EXPECT_EQ(result.partial_arguments.value("from_long_name", ""), "/ch0/A");
    EXPECT_EQ(result.partial_arguments.value("from_property", ""), "Out");
    EXPECT_EQ(result.partial_arguments.value("to_long_name", ""), "/ch0/B");
    EXPECT_EQ(result.partial_arguments.value("to_property", ""), "In");
}

TEST(LLMDialogSlotMerge, MergesWithExistingPendingPartial)
{
    PendingToolArguments pending;
    pending.tool_name = "connect_components";
    pending.partial_arguments = {{"from_long_name", "PNeuron2"}};

    const std::vector<LLMMessage> messages = {
        userMsg("PNeuron2"),
        userMsg("PNeuron3"),
    };

    const DialogSlotMergeResult result =
        mergeDialogSlotsIntoToolArgs({&messages, &pending, nullptr, nullptr, {}});

    EXPECT_TRUE(result.has_connect_endpoints);
    EXPECT_EQ(result.partial_arguments.value("from_long_name", ""), "PNeuron2");
    EXPECT_EQ(result.partial_arguments.value("to_long_name", ""), "PNeuron3");
}

TEST(LLMDialogSlotMerge, OpenRecentNumericIndexWithPending)
{
    PendingToolArguments pending;
    pending.tool_name = "open_recent_configuration";
    pending.action = ConfigurationLifecycleAction::Load;
    pending.partial_arguments = nlohmann::json::object();
    pending.disambiguation_candidates = nlohmann::json::array(
        {{{"index", 1}, {"path", "/tmp/one/project.ini"}},
         {{"index", 2}, {"path", "/tmp/two/project.ini"}}});

    const std::vector<LLMMessage> messages = {
        userMsg("list recent"),
        userMsg("2"),
    };

    const DialogSlotMergeResult result =
        mergeDialogSlotsIntoToolArgs({&messages, &pending, nullptr, nullptr, {}});

    EXPECT_EQ(result.inferred_tool_name, "open_recent_configuration");
    EXPECT_EQ(result.partial_arguments.value("index", 0), 2);
    EXPECT_EQ(result.partial_arguments.value("configuration_path", ""), "/tmp/two/project.ini");
}

TEST(LLMDialogSlotMerge, EmptyWhenNoSlots)
{
    const std::vector<LLMMessage> messages = {userMsg("what is this component?")};
    const DialogSlotMergeResult result = mergeMessages(messages);
    EXPECT_FALSE(result.has_connect_endpoints);
    EXPECT_TRUE(result.partial_arguments.empty());
}
