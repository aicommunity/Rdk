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

TEST(LLMSessionPersist, RemoveSessionDropsPersistedFile)
{
    const std::string dir = "/tmp/rdk_llm_sessions_remove_test";
    std::filesystem::remove_all(dir);

    ULLMConversationStore store;
    store.setStorageDirectory(dir);
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "bye";
    store.appendMessage("session-drop", user);
    EXPECT_TRUE(std::filesystem::exists(dir + "/session-drop.json"));

    store.removeSession("session-drop");
    EXPECT_FALSE(std::filesystem::exists(dir + "/session-drop.json"));
    ULLMConversationStore store2;
    store2.setStorageDirectory(dir);
    EXPECT_FALSE(store2.loadFromDisk("session-drop"));
}
