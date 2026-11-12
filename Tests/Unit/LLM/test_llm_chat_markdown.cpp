#include <gtest/gtest.h>

#include <QString>

#include "ULlmChatMarkdown.h"

TEST(LLMChatMarkdown, RendersHeadingsListsAndBold)
{
    const QString md = QStringLiteral(
        "### Title\n\n"
        "- item one\n"
        "- item two\n\n"
        "**bold** text");
    const QString html = llmMarkdownToHtmlFragment(md);
    EXPECT_FALSE(llmLooksLikeUiHtml(md));
    EXPECT_NE(html.indexOf(QStringLiteral("Title"), 0, Qt::CaseInsensitive), -1);
    // Qt markdown typically emits <h3> or strong/b for bold.
    EXPECT_TRUE(html.contains(QStringLiteral("<h3"), Qt::CaseInsensitive)
                || html.contains(QStringLiteral("<h2"), Qt::CaseInsensitive)
                || html.contains(QStringLiteral("<p")));
    EXPECT_TRUE(html.contains(QStringLiteral("<li"), Qt::CaseInsensitive)
                || html.contains(QStringLiteral("item one")));
    EXPECT_TRUE(html.contains(QStringLiteral("<strong"), Qt::CaseInsensitive)
                || html.contains(QStringLiteral("<b"), Qt::CaseInsensitive)
                || html.contains(QStringLiteral("bold")));
}

TEST(LLMChatMarkdown, UiHtmlPassthrough)
{
    const QString details = QStringLiteral(
        "<details><summary>Reasoning</summary><pre>think</pre></details>");
    EXPECT_TRUE(llmLooksLikeUiHtml(details));
    EXPECT_EQ(llmMarkdownToHtmlFragment(details), details);

    const QString question = QStringLiteral("<b>Question</b>");
    EXPECT_TRUE(llmLooksLikeUiHtml(question));
    EXPECT_EQ(llmMarkdownToHtmlFragment(question), question);
}
