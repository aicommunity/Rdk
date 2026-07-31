#include <gtest/gtest.h>

#include "Orchestrator/ULLMConnectPlanParsing.h"
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

TEST(LLMContextAssembler, GuiFocusHintIncludesDiagramScope)
{
    LLMGuiContextSnapshot gui;
    gui.diagram_scope_long_name = "Hardware/Drill";
    LLMSessionContext session;
    session.project_loaded = true;
    const std::string hint = buildGuiFocusSystemHint(gui, session);
    EXPECT_NE(hint.find("diagram_scope_long_name"), std::string::npos);
    EXPECT_NE(hint.find("Hardware/Drill"), std::string::npos);
}

TEST(LLMContextAssembler, GuiFocusHintIncludesKernelCurrentComponent)
{
    LLMGuiContextSnapshot gui;
    gui.current_component_long_name = "Hardware/Container1";
    gui.current_component_id = "123";
    gui.focused_component_long_name = "Hardware/Container1/PNeuron1";
    gui.channel_index = 0;

    LLMSessionContext session;
    session.project_loaded = true;

    const std::string hint = buildGuiFocusSystemHint(gui, session);
    EXPECT_NE(hint.find("Current component"), std::string::npos);
    EXPECT_NE(hint.find("Hardware/Container1"), std::string::npos);
    EXPECT_NE(hint.find("add_component"), std::string::npos);
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

TEST(LLMContextAssembler, QueryHintPrefersLiveGraphSnapshot)
{
    std::vector<LLMMessage> messages;
    messages.push_back({LLMMessage::Role::User, "дай информацию о текущей модели"});

    ConversationState conv;
    EphemeralContextInput input{conv,
                                LLMSessionContext{},
                                LLMGuiContextSnapshot{},
                                LLMIntentKind::Query,
                                ConfigurationLifecycleAction::None,
                                true};
    prependEphemeralSystemMessages(messages, input);

    bool found_live = false;
    bool found_docs = false;
    for(const LLMMessage& m : messages)
    {
        if(m.role != LLMMessage::Role::System)
            continue;
        if(m.content.find("get_net_snapshot") != std::string::npos
           && m.content.find("Inspect live project") != std::string::npos)
            found_live = true;
        if(m.content.find("search_project_docs") != std::string::npos)
            found_docs = true;
    }
    EXPECT_TRUE(found_live);
    EXPECT_TRUE(found_docs);
}

TEST(LLMContextAssembler, ExplainHintIncludesDecisionTree)
{
    std::vector<LLMMessage> messages;
    messages.push_back({LLMMessage::Role::User, "how does the current model work"});

    ConversationState conv;
    EphemeralContextInput input{conv,
                                LLMSessionContext{},
                                LLMGuiContextSnapshot{},
                                LLMIntentKind::Explain,
                                ConfigurationLifecycleAction::None,
                                true};
    prependEphemeralSystemMessages(messages, input);

    bool found = false;
    for(const LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::System
           && m.content.find("get_net_snapshot") != std::string::npos)
            found = true;
    }
    EXPECT_TRUE(found);
}

TEST(LLMContextAssembler, NoHardcodedLtZoneConnectParagraph)
{
    std::vector<LLMMessage> messages;
    messages.push_back({LLMMessage::Role::User, "connect two neurons"});

    ConversationState conv;
    conv.last_user_text_en = "connect PNeuron to PNeuron2";
    EphemeralContextInput input{conv,
                                LLMSessionContext{},
                                LLMGuiContextSnapshot{},
                                LLMIntentKind::Mutate,
                                ConfigurationLifecycleAction::None,
                                true};
    input.connect_semantics_block =
        "## Connect semantics (index)\n- from_class: NSPNeuron\n- Out -> In\n";
    input.link_patterns_block = "## Link patterns (index)\n- LTZone -> Soma1.ExcSynapse1\n";

    prependEphemeralSystemMessages(messages, input);
    for(const LLMMessage& m : messages)
    {
        if(m.role != LLMMessage::Role::System)
            continue;
        EXPECT_EQ(m.content.find("Typical NSPNeuron"), std::string::npos);
        EXPECT_EQ(m.content.find("Connect semantics (summary)"), std::string::npos);
    }
}

TEST(LLMContextAssembler, InjectsCatalogConnectBlocks)
{
    std::vector<LLMMessage> messages;
    messages.push_back({LLMMessage::Role::User, "connect"});

    ConversationState conv;
    EphemeralContextInput input{conv,
                                LLMSessionContext{},
                                LLMGuiContextSnapshot{},
                                LLMIntentKind::Mutate,
                                ConfigurationLifecycleAction::None,
                                true};
    input.connect_semantics_block = "## Connect semantics (index)\n- A -> B\n";

    prependEphemeralSystemMessages(messages, input);
    bool found = false;
    for(const LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::System
           && m.content.find("Connect semantics (index)") != std::string::npos)
            found = true;
    }
    EXPECT_TRUE(found);
}

TEST(LLMContextAssembler, SessionGraphIncludesLastAddedClass)
{
    std::vector<LLMMessage> messages;
    messages.push_back({LLMMessage::Role::User, "add more"});

    ConversationState conv;
    conv.session_graph.added_long_names = {"PNeuronGen"};
    conv.session_graph.last_add = LastAddComponentMemory{"NSPNeuronGen", "Model", "PNeuronGen"};
    EphemeralContextInput input{conv,
                                LLMSessionContext{},
                                LLMGuiContextSnapshot{},
                                LLMIntentKind::Mutate,
                                ConfigurationLifecycleAction::None,
                                true};

    prependEphemeralSystemMessages(messages, input);
    bool found = false;
    for(const LLMMessage& m : messages)
    {
        if(m.role == LLMMessage::Role::System
           && m.content.find("last_added_class: NSPNeuronGen") != std::string::npos)
            found = true;
    }
    EXPECT_TRUE(found);
}
