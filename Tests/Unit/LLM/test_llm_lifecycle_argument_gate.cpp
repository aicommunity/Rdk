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

TEST(LLMLifecycleArgumentGate, ExtractUnixPathFromCreateSentence)
{
    EXPECT_EQ(extractPathFromUserText("create new config in /tmp/nmsdk_llm_e2e_1234 name DemoCfg"),
              "/tmp/nmsdk_llm_e2e_1234");
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

TEST(LLMLifecycleArgumentGate, PendingOpenRecentFromConfigurationList)
{
    const nlohmann::json list_payload = {
        {"items",
         nlohmann::json::array(
             {{{"index", 1}, {"path", "/tmp/a/project.ini"}}, {{"index", 2}, {"path", "/tmp/b/project.ini"}}})}};
    const std::optional<PendingToolArguments> pending =
        pendingOpenRecentFromConfigurationList(list_payload);
    ASSERT_TRUE(pending.has_value());
    EXPECT_EQ(pending->tool_name, "open_recent_configuration");
    EXPECT_EQ(pending->disambiguation_candidates.size(), 2u);
}

TEST(LLMLifecycleArgumentGate, MergeOpenRecentIndexThenConfirmVerb)
{
    nlohmann::json list_payload = {
        {"items",
         nlohmann::json::array(
             {{{"index", 1}, {"path", "/tmp/one/project.ini"}},
              {{"index", 2}, {"path", "/tmp/two/project.ini"}}})}};
    const std::optional<PendingToolArguments> pending_opt =
        pendingOpenRecentFromConfigurationList(list_payload);
    ASSERT_TRUE(pending_opt.has_value());
    PendingToolArguments pending = *pending_opt;

    nlohmann::json merged = mergeArgumentsFromUserText(pending, "2", nullptr);
    EXPECT_EQ(merged["index"], 2);
    EXPECT_EQ(merged["configuration_path"], "/tmp/two/project.ini");
    EXPECT_TRUE(findMissingLifecycleFields("open_recent_configuration", merged, nullptr).empty());

    pending.partial_arguments = merged;
    merged = mergeArgumentsFromUserText(pending, "открой", nullptr);
    EXPECT_EQ(merged["index"], 2);
    EXPECT_TRUE(findMissingLifecycleFields("open_recent_configuration", merged, nullptr).empty());
}

TEST(LLMLifecycleArgumentGate, MergeCreateArgumentsFromSentence)
{
    PendingToolArguments pending;
    pending.tool_name = "create_configuration";
    pending.action = ConfigurationLifecycleAction::Create;
    pending.partial_arguments = nlohmann::json::object();

    const nlohmann::json merged = mergeArgumentsFromUserText(
        pending,
        "I want a brand new configuration. Create it under parent directory /tmp/nmsdk_llm_e2e_1234 "
        "with project name E2eScenarioTest and one channel class Model.",
        nullptr);
    ASSERT_TRUE(merged.contains("parent_directory"));
    EXPECT_EQ(merged["parent_directory"], "/tmp/nmsdk_llm_e2e_1234");
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

TEST(LLMLifecycleArgumentGate, ResolveClassPickByListNumber)
{
    const nlohmann::json candidates = {
        {{"class_name", "NSPNeuron"}, {"score", 0.9}},
        {{"class_name", "NPNeuron"}, {"score", 0.8}},
    };
    ASSERT_TRUE(resolveClassNameFromDisambiguationList("1", candidates).has_value());
    EXPECT_EQ(*resolveClassNameFromDisambiguationList("1", candidates), "NSPNeuron");
    EXPECT_EQ(*resolveClassNameFromDisambiguationList("2", candidates), "NPNeuron");
    EXPECT_FALSE(resolveClassNameFromDisambiguationList("9", candidates).has_value());
}

TEST(LLMLifecycleArgumentGate, ResolveClassPickByNameCaseInsensitive)
{
    const nlohmann::json candidates = {{{"class_name", "NSPNeuron"}, {"score", 0.9}}};
    EXPECT_EQ(*resolveClassNameFromDisambiguationList("nspneuron", candidates), "NSPNeuron");
}

TEST(LLMLifecycleArgumentGate, MergeAddComponentListNumber)
{
    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.partial_arguments = nlohmann::json::object();
    pending.class_disambiguation_candidates = {
        {{"class_name", "NSPNeuron"}, {"score", 0.9}},
        {{"class_name", "NPNeuron"}, {"score", 0.8}},
    };

    const nlohmann::json merged = mergeArgumentsFromUserText(pending, "1", nullptr);
    EXPECT_EQ(merged["class_name"], "NSPNeuron");
    EXPECT_EQ(merged["short_name"], "SPNeuron");
}

TEST(LLMLifecycleArgumentGate, MergeAddComponentUserPicksExactClass)
{
    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.partial_arguments = {{"class_name", "SpikeSource"}, {"parent_long_name", ""}};

    const nlohmann::json merged = mergeArgumentsFromUserText(pending, "NLPNeuron", nullptr);
    EXPECT_EQ(merged["class_name"], "NLPNeuron");
    EXPECT_EQ(merged["short_name"], "LPNeuron");
}

TEST(LLMLifecycleArgumentGate, MergeAddComponentMultiWordUsesRawLatinToken)
{
    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.partial_arguments = nlohmann::json::object();

    const nlohmann::json merged =
        mergeArgumentsFromUserText(pending, "добавь nspneuron", nullptr);
    EXPECT_EQ(merged["class_name"], "nspneuron");
}

TEST(LLMLifecycleArgumentGate, MergeAddComponentFromTypoClassNameKeepsRawToken)
{
    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.partial_arguments = nlohmann::json::object();

    const nlohmann::json merged = mergeArgumentsFromUserText(pending, "NPLNeuron", nullptr);
    EXPECT_EQ(merged["class_name"], "NPLNeuron");
    EXPECT_EQ(merged["parent_long_name"], "");
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
