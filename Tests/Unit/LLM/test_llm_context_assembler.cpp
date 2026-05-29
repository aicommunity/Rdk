#include <gtest/gtest.h>

#include "Orchestrator/ULLMContextAssembler.h"
#include "Tools/ULLMToolRegistry.h"

using namespace RDK::LLM;

TEST(LLMContextAssembler, GuiFocusHintContainsFocusedComponent)
{
    LLMGuiContextSnapshot gui;
    gui.focused_component_long_name = "TestNeuron";
    gui.focused_class_name = "Neuron";
    gui.channel_index = 0;

    LLMSessionContext session;
    session.project_loaded = true;
    session.allow_cloud_llm = false;

    const std::string hint = buildGuiFocusSystemHint(gui, session);
    EXPECT_NE(hint.find("TestNeuron"), std::string::npos);
    EXPECT_NE(hint.find("GUI focus"), std::string::npos);
}

TEST(LLMContextAssembler, QueryPrefetchBlockPrepended)
{
    std::vector<LLMMessage> messages;
    LLMMessage user;
    user.role = LLMMessage::Role::User;
    user.content = "question";
    messages.push_back(user);

    ConversationState conv;
    EphemeralContextInput input{conv,
                                LLMSessionContext{},
                                LLMGuiContextSnapshot{},
                                LLMIntentKind::Query,
                                ConfigurationLifecycleAction::None,
                                true,
                                false,
                                nullptr,
                                ToolFilter{},
                                "ArduinoBoard",
                                {},
                                nullptr,
                                "## Prefetched documentation\n- [doc1] path: excerpt\n",
                                {}};

    prependEphemeralSystemMessages(messages, input);
    ASSERT_FALSE(messages.empty());
    bool found_prefetch = false;
    bool found_query_hint = false;
    for(const LLMMessage& m : messages)
    {
        if(m.role != LLMMessage::Role::System)
            continue;
        if(m.content.find("Prefetched documentation") != std::string::npos)
            found_prefetch = true;
        if(m.content.find("search_project_docs") != std::string::npos)
            found_query_hint = true;
    }
    EXPECT_TRUE(found_prefetch);
    EXPECT_TRUE(found_query_hint);
}
