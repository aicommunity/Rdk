#include <gtest/gtest.h>

#include "llm_test_ollama_lab.h"

#include "LlmTypes.h"
#include "Providers/UOllamaNativeProvider.h"

using namespace RDK::LLM;
using namespace RDK::LLM::Test;

TEST(LLMThinkingLab, NativeThinkReturnsSeparateThinking)
{
    RDK_LLM_SKIP_IF_LAB_THINKING_MODEL_MISSING();

    LLMProviderProfile profile;
    profile.profile_id = "ollama-thinking";
    profile.kind = LLMProviderKind::OllamaNative;
    profile.base_url = kLabOllamaHost;
    profile.model = labOllamaThinkingModelName();
    profile.prefer_local = true;

    UOllamaNativeProvider provider(profile);
    ASSERT_TRUE(provider.capabilities().supports_thinking);

    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "How many letter r are in strawberry? Answer with one number.";

    LLMCompletionOptions opts;
    opts.think_mode = LLMThinkMode::On;
    opts.max_tokens = 2048;
    opts.temperature = 0.2f;

    const LLMCompletionResult result = provider.chat({user}, opts);
    ASSERT_TRUE(result.ok) << result.error_message;
    // Thinking models should separate reasoning; content may be short.
    EXPECT_FALSE(result.thinking.empty() && result.text.empty()) << "expected thinking and/or answer";
}
