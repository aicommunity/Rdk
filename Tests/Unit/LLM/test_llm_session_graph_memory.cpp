#include <gtest/gtest.h>

#include "Domain/URdkDomainAccess.h"
#include "Session/ULLMConversationStore.h"
#include "Session/ULLMSessionGraphMemory.h"

using namespace RDK::LLM;

TEST(LLMSessionGraphMemory, RemainingExcludesLinkedEndpoints)
{
    ConversationState st;
    st.session_graph.added_long_names = {"/ch0/A", "/ch0/B", "/ch0/C"};
    st.session_graph.linked_records.push_back(
        ConnectRecord{"/ch0/A", "/ch0/B", "Output", "Input", 0});

    const std::vector<std::string> remaining = sessionRemainingLongNames(st);
    ASSERT_EQ(remaining.size(), 1u);
    EXPECT_EQ(remaining[0], "/ch0/C");
}

TEST(LLMSessionGraphMemory, ResetOnProjectLoadedChange)
{
    ConversationState st;
    st.session_graph.added_long_names = {"/ch0/A"};
    st.last_session_context = LLMSessionContext{};
    st.last_session_context->project_loaded = true;

    LLMSessionContext next;
    next.project_loaded = false;
    syncSessionGraphOnSessionChange(st, next);

    EXPECT_TRUE(st.session_graph.added_long_names.empty());
}

TEST(LLMSessionGraphMemory, RecordWriteToolOutcomeAdd)
{
    ConversationState st;
    URdkDomainAccess domain(nullptr);
    recordWriteToolOutcome(st, domain, "add_component", {{"long_name", "/ch0/X"}}, 0);
    ASSERT_EQ(st.session_graph.added_long_names.size(), 1u);
    EXPECT_EQ(st.session_graph.added_long_names[0], "/ch0/X");
}

TEST(LLMSessionGraphMemory, RecordConnectAppendsKnownFact)
{
    ConversationState st;
    URdkDomainAccess domain(nullptr);
    nlohmann::json result = {{"from_long_name", "Model/A"},
                             {"to_long_name", "Model/B"},
                             {"from_property", "Out"},
                             {"to_property", "In"}};
    recordWriteToolOutcome(st, domain, "connect_components", result, 0);
    ASSERT_EQ(st.known_facts.size(), 1u);
    EXPECT_NE(st.known_facts[0].find("Model/A"), std::string::npos);
    EXPECT_NE(st.known_facts[0].find("Model/B"), std::string::npos);
    EXPECT_NE(st.known_facts[0].find("Out"), std::string::npos);
    EXPECT_NE(st.known_facts[0].find("In"), std::string::npos);

    recordWriteToolOutcome(st, domain, "connect_components",
                           {{ "already_existed", true},
                            {"from_long_name", "Model/A"},
                            {"to_long_name", "Model/B"},
                            {"from_property", "Out"},
                            {"to_property", "In"}},
                           0);
    EXPECT_EQ(st.known_facts.size(), 1u);
}
