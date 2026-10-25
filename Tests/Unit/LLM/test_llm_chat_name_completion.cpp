#include <gtest/gtest.h>

#include "Gui/ULLMChatNameCompletion.h"

#include <algorithm>

using namespace RDK::LLM;

TEST(LLMChatNameCompletion, ExtractTokenUnderCursor)
{
    const std::string text = "add PNeu";
    const ChatNameTokenSpan span = extractChatNameToken(text, text.size());
    EXPECT_EQ(span.text, "PNeu");
    EXPECT_EQ(span.begin, 4u);
}

TEST(LLMChatNameCompletion, RankPrefixFirst)
{
    const std::vector<std::string> dict = {"PNeuron", "PGenerator", "OtherNeuron", "NSPNeuron"};
    const std::vector<std::string> ranked = rankChatNameCompletions("PNeu", dict);
    ASSERT_FALSE(ranked.empty());
    EXPECT_EQ(ranked.front(), "PNeuron");
}

TEST(LLMChatNameCompletion, ScopedDotPrefersChildren)
{
    const std::vector<std::string> longs = {"PNeuron", "PNeuron.LTZone", "PNeuron.Soma1",
                                            "PGenerator"};
    const std::vector<std::string> classes = {"NSPNeuron"};
    const std::vector<std::string> props = {"Output", "Input"};
    const std::vector<std::string> out =
        filterCompletionsForScopedToken("PNeuron.LT", longs, classes, props);
    ASSERT_FALSE(out.empty());
    EXPECT_NE(std::find(out.begin(), out.end(), "PNeuron.LTZone"), out.end());
}

TEST(LLMChatNameCompletion, ScopedColonPrefersProperties)
{
    const std::vector<std::string> longs = {"PNeuron"};
    const std::vector<std::string> classes;
    const std::vector<std::string> props = {"Output", "Input", "Ready"};
    const std::vector<std::string> out =
        filterCompletionsForScopedToken("PNeuron:Out", longs, classes, props);
    ASSERT_EQ(out.size(), 1u);
    EXPECT_EQ(out.front(), "Output");
}
