#include <gtest/gtest.h>

#include "Orchestrator/ULLMConfigurationLifecycle.h"
#include "Orchestrator/ULLMEmbeddedToolCalls.h"
#include "Tools/RegisterCoreRdkTools.h"
#include "Tools/ULLMToolRegistry.h"
#include "Domain/URdkDomainAccess.h"

using namespace RDK::LLM;

TEST(LLMConfigurationLifecycle, DetectCreateConfigPhrases)
{
    EXPECT_EQ(detectConfigurationLifecycleAction("create new config"),
              ConfigurationLifecycleAction::Create);
    EXPECT_EQ(detectConfigurationLifecycleAction("create config"),
              ConfigurationLifecycleAction::Create);
    EXPECT_EQ(detectConfigurationLifecycleAction("создай новый конфиг"),
              ConfigurationLifecycleAction::Create);
    EXPECT_EQ(detectConfigurationLifecycleAction("создай новый проект"),
              ConfigurationLifecycleAction::Create);
}

TEST(LLMConfigurationLifecycle, ForcedToolCreateWhenNoProject)
{
    EXPECT_EQ(forcedToolForLifecycle(ConfigurationLifecycleAction::Create, false),
              std::optional<std::string>("create_configuration"));
}

TEST(LLMConfigurationLifecycle, DetectLoadAndRecentPhrases)
{
    EXPECT_EQ(detectConfigurationLifecycleAction("открой последний конфиг"),
              ConfigurationLifecycleAction::Load);
    EXPECT_TRUE(wantsRecentConfiguration("открой последний конфиг"));
    EXPECT_TRUE(wantsRecentConfiguration("open last configuration"));
    EXPECT_FALSE(wantsRecentConfiguration("open /tmp/demo/project.ini"));
}

TEST(LLMEmbeddedToolCalls, ParsesMarkdownJsonToolBlock)
{
    ULLMToolRegistry registry;
    URdkDomainAccess domain(nullptr);
    RegisterCoreRdkTools(registry, domain, nullptr);

    const std::string text = R"(Use this:
```json
{
  "name": "add_component",
  "arguments": {
    "class_name": "Config",
    "parent_long_name": "",
    "short_name": "config"
  }
}
```)";

    const std::vector<LLMToolCall> calls = tryExtractEmbeddedToolCalls(text, registry);
    ASSERT_EQ(calls.size(), 1u);
    EXPECT_EQ(calls[0].name, "add_component");
    EXPECT_EQ(calls[0].arguments["class_name"], "Config");
}
