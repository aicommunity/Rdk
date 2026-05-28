#include <gtest/gtest.h>

#include <cstdlib>

#include "Session/ULLMContextCompactor.h"

using namespace RDK::LLM;

TEST(LLMContextCompactor, CompactsLongHistory)
{
    setenv("NMSDK_LLM_CONTEXT_COMPACT", "1", 1);
    setenv("NMSDK_LLM_CONTEXT_COMPACT_THRESHOLD", "100", 1);
    setenv("NMSDK_LLM_CONTEXT_COMPACT_KEEP_TAIL", "2", 1);

    ConversationState state;
    state.session_id = "compact-test";
    for(int i = 0; i < 40; ++i)
    {
        LLMMessage user;
        user.role = LLMMessage::Role::User;
        user.content = "User turn " + std::to_string(i) + " with enough text to exceed threshold.";
        state.messages.push_back(user);
        LLMMessage assistant;
        assistant.role = LLMMessage::Role::Assistant;
        assistant.content = "Assistant reply " + std::to_string(i);
        state.messages.push_back(assistant);
    }

    ULLMContextCompactor compactor;
    ASSERT_TRUE(compactor.maybeCompact(state, state.session_id, "/tmp/llm-test-sessions"));
    EXPECT_TRUE(state.session_summary.has_value());
    EXPECT_LT(state.messages.size(), 40u);
    EXPECT_FALSE(state.messages.empty());
    EXPECT_EQ(state.messages.front().role, LLMMessage::Role::System);

    unsetenv("NMSDK_LLM_CONTEXT_COMPACT");
    unsetenv("NMSDK_LLM_CONTEXT_COMPACT_THRESHOLD");
    unsetenv("NMSDK_LLM_CONTEXT_COMPACT_KEEP_TAIL");
}
