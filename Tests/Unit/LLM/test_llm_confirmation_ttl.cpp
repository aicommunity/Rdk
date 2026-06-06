#include <gtest/gtest.h>

#include <filesystem>

#include "Orchestrator/ULLMWorkflowState.h"
#include "Policy/ULLMPolicyLimits.h"
#include "Session/ULLMConfirmationExpiry.h"
#include "Session/ULLMConversationStore.h"

using namespace RDK::LLM;

TEST(LLMConfirmationTtl, ExpiresStalePending)
{
    const std::string dir = "/tmp/rdk_llm_confirm_ttl_test";
    std::filesystem::remove_all(dir);

    ULLMConversationStore store;
    store.setStorageDirectory(dir);

    PendingConfirmation pending;
    pending.confirmation_id = "conf-1";
    pending.created_at_unix_sec = confirmationNowUnixSec() - 700;
    pending.request.tool_name = "add_component";
    pending.request.trace_id = "trace-1";
    store.setPending("session-ttl", pending);

    ConversationState& state = store.getOrCreate("session-ttl");
    state.workflow_phase = LLMWorkflowPhase::AwaitingConfirmation;

    EXPECT_TRUE(store.expirePendingIfStale("session-ttl", defaultPolicyLimits().confirmation_ttl_seconds));
    EXPECT_FALSE(store.getOrCreate("session-ttl").pending.has_value());
    EXPECT_EQ(store.getOrCreate("session-ttl").workflow_phase, LLMWorkflowPhase::Idle);
}

TEST(LLMConfirmationTtl, PersistRoundTrip)
{
    const std::string dir = "/tmp/rdk_llm_confirm_persist_test";
    std::filesystem::remove_all(dir);

    {
        ULLMConversationStore store;
        store.setStorageDirectory(dir);
        PendingConfirmation pending;
        pending.confirmation_id = "conf-persist";
        pending.created_at_unix_sec = confirmationNowUnixSec();
        pending.request.tool_name = "set_property";
        pending.request.arguments = {{"long_name", "a"}, {"property", "x"}, {"value", 1}};
        store.setPending("session-p", pending);
        store.persistToDisk("session-p");
    }

    ULLMConversationStore store2;
    store2.setStorageDirectory(dir);
    EXPECT_TRUE(store2.loadFromDisk("session-p"));
    const ConversationState& loaded = store2.getOrCreate("session-p");
    ASSERT_TRUE(loaded.pending.has_value());
    EXPECT_EQ(loaded.pending->confirmation_id, "conf-persist");
    EXPECT_EQ(loaded.pending->request.tool_name, "set_property");
    EXPECT_GT(loaded.pending->created_at_unix_sec, 0);
}
