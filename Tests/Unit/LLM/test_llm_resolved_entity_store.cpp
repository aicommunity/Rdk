#include <gtest/gtest.h>

#include <filesystem>

#include "Domain/ULLMResolvedEntityStore.h"
#include "Session/ULLMConversationStore.h"

using namespace RDK::LLM;

TEST(LLMResolvedEntityStore, LookupAfterPersistRoundTrip)
{
    const std::string dir = "/tmp/rdk_llm_resolved_entity_rt";
    std::filesystem::remove_all(dir);

    ULLMConversationStore store;
    store.setStorageDirectory(dir);
    ConversationState& state = store.getOrCreate("ent-session");
    upsertResolvedEntity(state, "component", "нейрон 3", "/ch0/Neuron3", 0);
    ASSERT_TRUE(store.persistToDisk("ent-session"));

    ULLMConversationStore reloaded;
    reloaded.setStorageDirectory(dir);
    ASSERT_TRUE(reloaded.loadFromDisk("ent-session"));
    const ConversationState& loaded = *reloaded.findSession("ent-session");
    const auto hit = lookupResolvedEntity(loaded, "component", "нейрон 3", 0);
    ASSERT_TRUE(hit.has_value());
    EXPECT_EQ(*hit, "/ch0/Neuron3");

    std::filesystem::remove_all(dir);
}

TEST(LLMResolvedEntityStore, NormalizeEntityQueryCollapsesSpaces)
{
    EXPECT_EQ(normalizeEntityQuery("  Neuron   3  "), "neuron 3");
}
