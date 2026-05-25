#include <gtest/gtest.h>

#include "Orchestrator/ULLMLifecycleArgumentGate.h"

using namespace RDK::LLM;

TEST(LLMLifecycleArgumentGate, ExtractUnixPath)
{
    EXPECT_EQ(extractPathFromUserText("load from /home/user/MyCfg/project.ini"),
              "/home/user/MyCfg/project.ini");
}

TEST(LLMLifecycleArgumentGate, PreflightCreateWithoutPathUsesAutocreate)
{
    const LifecycleArgumentPreflight pre = preflightLifecycleArguments(
        ConfigurationLifecycleAction::Create, "create new config", nullptr);
    EXPECT_EQ(pre.tool_name, "create_configuration");
    EXPECT_TRUE(pre.ready);
    EXPECT_TRUE(pre.arguments.value("autocreate_subdirectory", false));
    EXPECT_FALSE(pre.arguments.contains("parent_directory"));
}

TEST(LLMLifecycleArgumentGate, PreflightLoadWithoutPathNotReady)
{
    const LifecycleArgumentPreflight pre =
        preflightLifecycleArguments(ConfigurationLifecycleAction::Load, "load config", nullptr);
    EXPECT_EQ(pre.tool_name, "load_configuration");
    EXPECT_FALSE(pre.ready);
    EXPECT_FALSE(pre.missing_fields.empty());
}

TEST(LLMLifecycleArgumentGate, PreflightLoadWithPathReady)
{
    const LifecycleArgumentPreflight pre = preflightLifecycleArguments(
        ConfigurationLifecycleAction::Load,
        "open config /tmp/demo/project.ini", nullptr);
    EXPECT_TRUE(pre.ready);
    EXPECT_EQ(pre.arguments["configuration_path"], "/tmp/demo/project.ini");
}

TEST(LLMLifecycleArgumentGate, MergeFollowUpPath)
{
    PendingToolArguments pending;
    pending.tool_name = "load_configuration";
    pending.action = ConfigurationLifecycleAction::Load;
    pending.partial_arguments = nlohmann::json::object();

    const nlohmann::json merged =
        mergeArgumentsFromUserText(pending, "/opt/nmsdk/Demo/project.ini", nullptr);
    EXPECT_EQ(merged["configuration_path"], "/opt/nmsdk/Demo/project.ini");
    EXPECT_TRUE(findMissingLifecycleFields("load_configuration", merged, nullptr).empty());
}

TEST(LLMLifecycleArgumentGate, FormatPromptMentionsTool)
{
    const std::vector<ToolArgumentFieldSpec> fields =
        argumentFieldsForLifecycle(ConfigurationLifecycleAction::Load);
    const std::string prompt =
        formatArgumentRequestPrompt("load_configuration", ConfigurationLifecycleAction::Load,
                                    fields, nullptr);
    EXPECT_NE(prompt.find("load_configuration"), std::string::npos);
    EXPECT_NE(prompt.find("configuration_path"), std::string::npos);
}
