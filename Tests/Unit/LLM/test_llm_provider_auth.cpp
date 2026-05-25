#include <gtest/gtest.h>

#include <cstdlib>

#include "Settings/ULLMProviderAuth.h"
#include "Settings/ULLMProviderCatalog.h"

using namespace RDK::LLM;

TEST(LLMProviderAuth, ResolvesEnvApiKey)
{
    const LLMProviderProfile* deepseek = ULLMProviderCatalog::findById("deepseek");
    ASSERT_NE(deepseek, nullptr);
#ifdef _WIN32
    _putenv_s("DEEPSEEK_API_KEY", "test-key-123");
#else
    setenv("DEEPSEEK_API_KEY", "test-key-123", 1);
#endif
    LLMRuntimeProviderSettings runtime;
    const std::string key = ULLMProviderAuth::resolveApiKey(*deepseek, runtime);
    EXPECT_EQ(key, "test-key-123");
}

TEST(LLMProviderAuth, DenyCloudWithoutAllowFlag)
{
    const LLMProviderProfile* deepseek = ULLMProviderCatalog::findById("deepseek");
    ASSERT_NE(deepseek, nullptr);
    LLMRuntimeProviderSettings runtime;
    runtime.allow_cloud_providers = false;
    runtime.api_keys_by_profile_id["deepseek"] = "k";
    ProviderAccessCheck check = ULLMProviderAuth::checkProviderAccess(*deepseek, runtime);
    EXPECT_FALSE(check.allowed);
    EXPECT_EQ(check.deny_code, "CLOUD_PROVIDER_DISABLED");
}

TEST(LLMProviderAuth, DenyCloudWithoutApiKey)
{
    const LLMProviderProfile* openai = ULLMProviderCatalog::findById("openai");
    ASSERT_NE(openai, nullptr);
    LLMRuntimeProviderSettings runtime;
    runtime.allow_cloud_providers = true;
    ProviderAccessCheck check = ULLMProviderAuth::checkProviderAccess(*openai, runtime);
    EXPECT_FALSE(check.allowed);
    EXPECT_EQ(check.deny_code, "API_KEY_MISSING");
}
