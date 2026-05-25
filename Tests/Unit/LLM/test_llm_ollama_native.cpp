#include <gtest/gtest.h>

#include "Providers/ULLMProviderFactory.h"

TEST(LlmOllamaNative, FactoryCreatesNativeProvider)
{
    RDK::LLM::LLMProviderProfile profile;
    profile.kind = RDK::LLM::LLMProviderKind::OllamaNative;
    profile.base_url = "http://127.0.0.1:11434";
    profile.model = "qwen2.5:7b";
    auto provider = RDK::LLM::ULLMProviderFactory::create(profile);
    ASSERT_NE(provider, nullptr);
    EXPECT_EQ(provider->kind(), RDK::LLM::LLMProviderKind::OllamaNative);
    EXPECT_FALSE(provider->capabilities().supports_tool_calling);
}
