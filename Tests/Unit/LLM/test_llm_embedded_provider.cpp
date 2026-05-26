#include <gtest/gtest.h>

#include "Providers/ULLMProviderFactory.h"

#ifdef RDK_LLM_EMBEDDED
TEST(LlmEmbeddedProvider, FactoryCreatesInProcessProvider)
{
    RDK::LLM::LLMProviderProfile profile;
    profile.kind = RDK::LLM::LLMProviderKind::EmbeddedLlama;
    profile.profile_id = "embedded-offline";
    auto provider = RDK::LLM::ULLMProviderFactory::create(profile);
    ASSERT_NE(provider, nullptr);
    EXPECT_EQ(provider->kind(), RDK::LLM::LLMProviderKind::EmbeddedLlama);
    EXPECT_TRUE(provider->capabilities().runs_in_process);
    EXPECT_FALSE(provider->capabilities().requires_network);
    EXPECT_TRUE(provider->capabilities().supports_streaming);

    std::string err;
    EXPECT_FALSE(provider->healthCheck(err));
    EXPECT_FALSE(err.empty());
}
#else
TEST(LlmEmbeddedProvider, StubWhenNotBuilt)
{
    RDK::LLM::LLMProviderProfile profile;
    profile.kind = RDK::LLM::LLMProviderKind::EmbeddedLlama;
    auto provider = RDK::LLM::ULLMProviderFactory::create(profile);
    ASSERT_NE(provider, nullptr);
    std::string err;
    EXPECT_FALSE(provider->healthCheck(err));
}
#endif
