#include <gtest/gtest.h>

#include "ULlmDetailsHtml.h"

TEST(LLMDetailsHtml, ParseSingleDetails)
{
    const QString html =
        QStringLiteral("<details><summary>Tools (1)</summary><pre>add_component\nok</pre></details>");
    QString summary;
    QString body;
    ASSERT_TRUE(llmTryParseSingleDetails(html, &summary, &body));
    EXPECT_EQ(summary, QStringLiteral("Tools (1)"));
    EXPECT_NE(body.indexOf(QStringLiteral("add_component")), -1);
}

TEST(LLMDetailsHtml, ParseMixedSegments)
{
    const QString html = QStringLiteral(
        "<p>before</p><details><summary>Tools (1)</summary><pre>x</pre></details>"
        "<details><summary>Reasoning</summary><pre>think</pre></details><p>after</p>");
    const QVector<ULlmDetailsHtmlSegment> segs = llmParseDetailsHtmlSegments(html);
    ASSERT_EQ(segs.size(), 4);
    EXPECT_EQ(segs[0].kind, ULlmDetailsHtmlSegment::Kind::Plain);
    EXPECT_EQ(segs[1].kind, ULlmDetailsHtmlSegment::Kind::Details);
    EXPECT_EQ(segs[1].summary, QStringLiteral("Tools (1)"));
    EXPECT_EQ(segs[2].kind, ULlmDetailsHtmlSegment::Kind::Details);
    EXPECT_EQ(segs[2].summary, QStringLiteral("Reasoning"));
    EXPECT_EQ(segs[3].kind, ULlmDetailsHtmlSegment::Kind::Plain);
}

TEST(LLMDetailsHtml, RejectsNonDetails)
{
    QString s, b;
    EXPECT_FALSE(llmTryParseSingleDetails(QStringLiteral("<p>hi</p>"), &s, &b));
}
