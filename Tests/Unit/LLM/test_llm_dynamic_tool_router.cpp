#include <gtest/gtest.h>

#include <cstdlib>

#include "Knowledge/ULLMDynamicToolRouter.h"

using namespace RDK::LLM;

namespace {

ToolFilter makeBaseMutateFilter()
{
    ToolFilter f;
    f.intent = LLMIntentKind::Mutate;
    f.include_write = true;
    f.allowed_tool_names = std::unordered_set<std::string>{
        "create_configuration", "load_configuration", "add_component", "connect_components",
        "set_property", "validate_configuration", "search_project_docs"};
    return f;
}

} // namespace

TEST(LLMDynamicToolRouter, DisabledKeepsFilterUnchanged)
{
    ::unsetenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING");
    const ToolFilter base = makeBaseMutateFilter();
    const ToolFilter out = ULLMDynamicToolRouter::apply(base, "create config in /tmp/demo");

    ASSERT_TRUE(out.allowed_tool_names.has_value());
    EXPECT_EQ(out.allowed_tool_names->size(), base.allowed_tool_names->size());
    EXPECT_TRUE(out.allowed_tool_names->count("add_component"));
    EXPECT_TRUE(out.allowed_tool_names->count("create_configuration"));
}

TEST(LLMDynamicToolRouter, EnabledCreateRouteNarrowsToCreateTool)
{
    ::setenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING", "1", 1);
    const ToolFilter base = makeBaseMutateFilter();
    const ToolFilter out = ULLMDynamicToolRouter::apply(
        base, "I want a new configuration, create project under /tmp/demo");
    ::unsetenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING");

    ASSERT_TRUE(out.allowed_tool_names.has_value());
    EXPECT_TRUE(out.allowed_tool_names->count("create_configuration"));
    EXPECT_FALSE(out.allowed_tool_names->count("add_component"));
    EXPECT_FALSE(out.allowed_tool_names->count("load_configuration"));
}

TEST(LLMDynamicToolRouter, EnabledConnectRouteKeepsGraphMutationTools)
{
    ::setenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING", "1", 1);
    const ToolFilter base = makeBaseMutateFilter();
    const ToolFilter out =
        ULLMDynamicToolRouter::apply(base, "connect component A to component B");
    ::unsetenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING");

    ASSERT_TRUE(out.allowed_tool_names.has_value());
    EXPECT_TRUE(out.allowed_tool_names->count("connect_components"));
    EXPECT_TRUE(out.allowed_tool_names->count("add_component"));
    EXPECT_FALSE(out.allowed_tool_names->count("create_configuration"));
}

TEST(LLMDynamicToolRouter, EnabledScoreSubsetLimitsToolCount)
{
    ::setenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING", "1", 1);
    ::setenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING_TOP_K", "3", 1);

    ToolFilter f;
    f.intent = LLMIntentKind::Mutate;
    f.include_write = true;
    f.allowed_tool_names = std::unordered_set<std::string>{
        "create_configuration",
        "load_configuration",
        "validate_configuration",
        "search_project_docs",
        "describe_class",
        "add_component",
        "connect_components",
    };

    const ToolFilter out = ULLMDynamicToolRouter::apply(f, "validate configuration");

    ::unsetenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING_TOP_K");
    ::unsetenv("NMSDK_LLM_DYNAMIC_TOOL_ROUTING");

    ASSERT_TRUE(out.allowed_tool_names.has_value());
    EXPECT_LE(out.allowed_tool_names->size(), 3u);
}
