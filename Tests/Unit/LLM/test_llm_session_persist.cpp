#include <gtest/gtest.h>

#include <filesystem>

#include "Session/ULLMConversationStore.h"

using namespace RDK::LLM;

TEST(LLMSessionPersist, RoundTripMessages)
{
    const std::string dir = "/tmp/rdk_llm_sessions_test";
    std::filesystem::remove_all(dir);

    ULLMConversationStore store;
    store.setStorageDirectory(dir);

    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "hello";
    store.appendMessage("session-a", user);

    ULLMConversationStore store2;
    store2.setStorageDirectory(dir);
    EXPECT_TRUE(store2.loadFromDisk("session-a"));
    ConversationState& loaded = store2.getOrCreate("session-a");
    ASSERT_EQ(loaded.messages.size(), 1u);
    EXPECT_EQ(loaded.messages[0].content, "hello");
}
