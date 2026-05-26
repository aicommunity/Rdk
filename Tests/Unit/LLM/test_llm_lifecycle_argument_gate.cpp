#include <gtest/gtest.h>

#include "Orchestrator/ULLMLifecycleArgumentGate.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

namespace {

LLMToolDefinition makeAddComponentToolDef()
{
    LLMToolDefinition d;
    d.name = "add_component";
    d.input_schema = {{"type", "object"},
                      {"required", nlohmann::json::array({"class_name", "parent_long_name", "short_name"})},
                      {"properties",
                       {{"class_name", {{"type", "string"}, {"description", "Component class"}}},
                        {"parent_long_name", {{"type", "string"}, {"description", "Parent long name"}}},
                        {"short_name", {{"type", "string"}, {"description", "New component short name"}}},
                        {"channel_index", {{"type", "integer"}}}}},
                      {"additionalProperties", false}};
    return d;
}

} // namespace

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

TEST(LLMLifecycleArgumentGate, MergeAddComponentFromTypoClassName)
{
    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.partial_arguments = nlohmann::json::object();

    const nlohmann::json merged = mergeArgumentsFromUserText(pending, "NPLNeuron", nullptr);
    EXPECT_EQ(merged["class_name"], "NPulseNeuron");
    EXPECT_EQ(merged["parent_long_name"], "");
    EXPECT_EQ(merged["short_name"], "PulseNeuron");
}

TEST(LLMLifecycleArgumentGate, AddComponentArgumentsCompleteAfterUserReply)
{
    ULLMToolRegistry registry;
    registry.registerTool(makeAddComponentToolDef(),
                          [](const nlohmann::json&) -> ToolGatewayResult {
                              ToolGatewayResult r;
                              r.ok = true;
                              return r;
                          });

    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.partial_arguments = nlohmann::json::object();
    const nlohmann::json merged = mergeArgumentsFromUserText(pending, "NPulseNeuron", nullptr);
    EXPECT_TRUE(findMissingToolArguments("add_component", merged, registry).empty());
}

TEST(LLMLifecycleArgumentGate, FormatPromptListsGraphToolFields)
{
    ULLMToolRegistry registry;
    registry.registerTool(makeAddComponentToolDef(),
                          [](const nlohmann::json&) -> ToolGatewayResult { return {}; });
    const std::vector<ToolArgumentFieldSpec> missing =
        findMissingToolArguments("add_component", nlohmann::json::object(), registry);
    ASSERT_EQ(missing.size(), 3u);
    const std::string prompt = formatArgumentRequestPrompt(
        "add_component", ConfigurationLifecycleAction::None, missing, nullptr);
    EXPECT_NE(prompt.find("class_name"), std::string::npos);
    EXPECT_NE(prompt.find("NPulseNeuron"), std::string::npos);
}

TEST(LLMLifecycleArgumentGate, IsGraphAddComponentOnlyAddComponent)
{
    EXPECT_TRUE(isGraphAddComponentTool("add_component"));
    EXPECT_FALSE(isGraphAddComponentTool("add_pulse_component"));
}
