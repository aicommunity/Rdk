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

TEST(LLMSessionPersist, RoundTripPendingToolArguments)
{
    const std::string dir = "/tmp/rdk_llm_sessions_pending_args";
    std::filesystem::remove_all(dir);

    ULLMConversationStore store;
    store.setStorageDirectory(dir);
    ConversationState& state = store.getOrCreate("session-pending");
    state.session_id = "session-pending";

    PendingToolArguments pending;
    pending.tool_name = "add_component";
    pending.action = ConfigurationLifecycleAction::Create;
    pending.partial_arguments = {{"short_name", "N1"}};
    pending.disambiguation_kind = PendingDisambiguationKind::Class;
    pending.disambiguation_field = "class_name";
    pending.disambiguation_candidates = nlohmann::json::array(
        {{{"class_name", "NLPNeuron"}}, {{"class_name", "NModel"}}});
    pending.missing_fields = {ToolArgumentFieldSpec{"class_name", "string",
                                                    "Registered class name", true}};
    pending.created_at_unix_sec = 12345;
    store.setPendingToolArguments("session-pending", pending);
    ASSERT_TRUE(store.persistToDisk("session-pending"));

    ULLMConversationStore reloaded;
    reloaded.setStorageDirectory(dir);
    ASSERT_TRUE(reloaded.loadFromDisk("session-pending"));
    ConversationState& loaded = reloaded.getOrCreate("session-pending");
    ASSERT_TRUE(loaded.pending_tool_arguments.has_value());
    EXPECT_EQ(loaded.pending_tool_arguments->tool_name, "add_component");
    EXPECT_EQ(loaded.pending_tool_arguments->disambiguation_field, "class_name");
    EXPECT_EQ(loaded.pending_tool_arguments->created_at_unix_sec, 12345);
    ASSERT_EQ(loaded.pending_tool_arguments->missing_fields.size(), 1u);
    EXPECT_EQ(loaded.pending_tool_arguments->missing_fields.front().name, "class_name");
}

TEST(LLMSessionPersist, RedactsSensitiveMessageContentOnPersist)
{
    const std::string dir = "/tmp/rdk_llm_sessions_redaction";
    std::filesystem::remove_all(dir);

    ULLMConversationStore store;
    store.setStorageDirectory(dir);

    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "api_key=secret123 token=abcd password=qwerty";
    store.appendMessage("session-redact", user);

    ULLMConversationStore reloaded;
    reloaded.setStorageDirectory(dir);
    ASSERT_TRUE(reloaded.loadFromDisk("session-redact"));
    ConversationState& loaded = reloaded.getOrCreate("session-redact");
    ASSERT_EQ(loaded.messages.size(), 1u);
    EXPECT_EQ(loaded.messages[0].content.find("secret123"), std::string::npos);
    EXPECT_EQ(loaded.messages[0].content.find("qwerty"), std::string::npos);
    EXPECT_NE(loaded.messages[0].content.find("[REDACTED]"), std::string::npos);
}

TEST(LLMSessionPersist, PersistsIntentContractState)
{
    const std::string dir = "/tmp/rdk_llm_sessions_intent_contract";
    std::filesystem::remove_all(dir);

    ULLMConversationStore store;
    store.setStorageDirectory(dir);
    ConversationState& state = store.getOrCreate("session-intent");
    state.session_id = "session-intent";
    state.intent_contract_kind = LLMIntentKind::Mutate;
    state.intent_contract_confidence = 0.87f;
    state.intent_contract_requires_confirmation_for_writes = false;
    ASSERT_TRUE(store.persistToDisk("session-intent"));

    ULLMConversationStore reloaded;
    reloaded.setStorageDirectory(dir);
    ASSERT_TRUE(reloaded.loadFromDisk("session-intent"));
    ConversationState& loaded = reloaded.getOrCreate("session-intent");
    EXPECT_EQ(loaded.intent_contract_kind, LLMIntentKind::Mutate);
    EXPECT_FLOAT_EQ(loaded.intent_contract_confidence, 0.87f);
    EXPECT_FALSE(loaded.intent_contract_requires_confirmation_for_writes);
}
