#include <gtest/gtest.h>

#include "Domain/ULLMAddParentResolution.h"
#include "Domain/ULLMNameResolution.h"
#include "Domain/ULLMWriteArgumentNormalizer.h"
#include "Domain/URdkDomainAccess.h"
#include "Orchestrator/ULLMTurnTerminalHelpers.h"
#include "Session/ULLMConversationStore.h"
#include "Session/ULLMSessionGraphMemory.h"

using namespace RDK::LLM;

TEST(LLMDirectToolTranscript, AppendsAssistantToolAndOutcome)
{
    ULLMConversationStore store;
    const std::string sid = "sess-direct-transcript";
    store.getOrCreate(sid);

    const nlohmann::json args = {{"class_name", "NSPNeuronGen"},
                                 {"parent_long_name", "Model"},
                                 {"short_name", "PNeuronGen"}};
    appendDirectToolTranscript(store, sid, "direct-test-1", "add_component", args,
                               R"({"ok":true,"class_name":"NSPNeuronGen","long_name":"PNeuronGen"})",
                               "Component added: NSPNeuronGen (PNeuronGen)");

    const ConversationState& st = store.getOrCreate(sid);
    ASSERT_GE(st.messages.size(), 3u);

    bool saw_tool_call = false;
    bool saw_tool_result = false;
    bool saw_outcome = false;
    for(const LLMMessage& m : st.messages)
    {
        if(m.role == LLMMessage::Role::Assistant && m.assistant_tool_calls
           && !m.assistant_tool_calls->empty())
        {
            saw_tool_call = true;
            EXPECT_EQ(m.assistant_tool_calls->front().name, "add_component");
            EXPECT_EQ(m.assistant_tool_calls->front().arguments.value("class_name", ""),
                      "NSPNeuronGen");
        }
        if(m.role == LLMMessage::Role::Tool)
        {
            saw_tool_result = true;
            EXPECT_NE(m.content.find("NSPNeuronGen"), std::string::npos);
        }
        if(m.role == LLMMessage::Role::Assistant && !m.content.empty()
           && m.content.find("Component added") != std::string::npos)
            saw_outcome = true;
    }
    EXPECT_TRUE(saw_tool_call);
    EXPECT_TRUE(saw_tool_result);
    EXPECT_TRUE(saw_outcome);
}

TEST(LLMWriteArgumentNormalizer, RepeatSameAddCueDetection)
{
    EXPECT_TRUE(looksLikeRepeatSameAddCue("добавь еще три таких же компонента"));
    EXPECT_TRUE(looksLikeRepeatSameAddCue("add three more of the same"));
    EXPECT_TRUE(looksLikeRepeatSameAddCue("create another one"));
    EXPECT_FALSE(looksLikeRepeatSameAddCue("добавь компонент NSPNeuronGen"));
}

TEST(LLMWriteArgumentNormalizer, ContinuityCueDetection)
{
    EXPECT_TRUE(looksLikeAddContinuityCue("добавь еще два таких же нейрона"));
    EXPECT_TRUE(looksLikeAddContinuityCue("добавь еще три нейрона"));
    EXPECT_TRUE(looksLikeAddContinuityCue("добавь еще два компонента"));
    EXPECT_TRUE(looksLikeAddContinuityCue("add another module"));
    EXPECT_TRUE(looksLikeAddContinuityCue("добавь блок"));
    EXPECT_FALSE(looksLikeAddContinuityCue("открой последнюю конфигурацию"));
}

TEST(LLMWriteArgumentNormalizer, RepeatCueAtRootKeepsEmptyParentNotModel)
{
    URdkDomainAccess domain(nullptr);
    LLMGuiContextSnapshot gui;
    gui.current_component_long_name = "Model";

    SessionGraphMemory graph;
    LastAddComponentMemory last;
    last.class_name = "NSPNeuronGen";
    last.parent_long_name = "";
    last.short_name_base = "PNeuronGen";
    graph.last_add = last;

    AddParentResolution res = resolveValidAddParent(domain, "", "NSPNeuronGen", 0, gui);
    EXPECT_EQ(res.parent_long_name, "");
    EXPECT_NE(res.parent_long_name, "Model");
    (void)graph;
}

TEST(LLMWriteArgumentNormalizer, NeuronNounIsFuzzyNotExactClass)
{
    // User-text continuer becomes a class query token; Latin "neuron" fuzzy-matches many
    // *Neuron* classes (Ambiguous) — trust-registered-class skips this when last_add set class.
    EXPECT_EQ(extractClassNameTokenFromUserText("добавь еще два таких же нейрона"), "нейрона");
    const std::vector<std::string> registered = {"NNeuron", "NSPNeuron", "NSPNeuronGen",
                                                 "NPNeuron", "NLPNeuron"};
    const RegisteredClassResolution resolved = resolveRegisteredClassName("neuron", registered);
    EXPECT_EQ(resolved.status, RegisteredClassResolution::Status::Ambiguous);
    EXPECT_GE(resolved.candidates.size(), 2u);
}
