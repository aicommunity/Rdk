#include "ULlmChatHistoryPanel.h"

#include "ULlmChatMarkdown.h"
#include "ULlmCollapsibleDetailsWidget.h"
#include "ULlmDetailsHtml.h"

#include <QMetaObject>
#include <QScrollBar>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace {

void fitBrowserHeight(QTextBrowser* browser)
{
    if(!browser)
        return;
    browser->document()->adjustSize();
    const int h = qMax(24, int(browser->document()->size().height()) + 8);
    browser->setMinimumHeight(h);
}

} // namespace

ULlmChatHistoryPanel::ULlmChatHistoryPanel(QWidget* parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_container = new QWidget(this);
    m_layout = new QVBoxLayout(m_container);
    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setSpacing(4);
    m_layout->addStretch(1);
    setWidget(m_container);
}

void ULlmChatHistoryPanel::clear()
{
    cancelStream();
    while(m_layout->count() > 0)
    {
        QLayoutItem* item = m_layout->takeAt(0);
        if(item->widget())
            item->widget()->deleteLater();
        delete item;
    }
    m_layout->addStretch(1);
}

void ULlmChatHistoryPanel::addRow(QWidget* w)
{
    const int stretch_index = m_layout->count() - 1;
    m_layout->insertWidget(stretch_index < 0 ? 0 : stretch_index, w);
    scrollToEnd();
}

QTextBrowser* ULlmChatHistoryPanel::makeTextRow(const QString& html)
{
    auto* browser = new QTextBrowser(m_container);
    browser->setOpenExternalLinks(true);
    browser->setFrameShape(QFrame::NoFrame);
    browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    browser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    browser->setHtml(html);
    fitBrowserHeight(browser);
    return browser;
}

void ULlmChatHistoryPanel::appendPlainHtmlRow(const QString& html)
{
    if(html.trimmed().isEmpty())
        return;
    addRow(makeTextRow(html));
}

void ULlmChatHistoryPanel::appendDetails(const QString& summary, const QString& body_html,
                                         bool expanded)
{
    addRow(new ULlmCollapsibleDetailsWidget(summary, body_html, expanded, m_container));
}

void ULlmChatHistoryPanel::appendHtml(const QString& html, bool details_expanded_default)
{
    QString summary;
    QString body;
    if(llmTryParseSingleDetails(html, &summary, &body))
    {
        appendDetails(summary, body, details_expanded_default);
        return;
    }

    const QVector<ULlmDetailsHtmlSegment> segs = llmParseDetailsHtmlSegments(html);
    if(segs.isEmpty())
    {
        appendPlainHtmlRow(html);
        return;
    }
    for(const ULlmDetailsHtmlSegment& seg : segs)
    {
        if(seg.kind == ULlmDetailsHtmlSegment::Kind::Details)
        {
            bool expanded = details_expanded_default;
            // Reasoning blocks default collapsed when loading mixed HTML.
            if(seg.summary.contains(QStringLiteral("Reasoning"), Qt::CaseInsensitive))
                expanded = false;
            appendDetails(seg.summary, seg.body_html, expanded);
        }
        else
            appendPlainHtmlRow(seg.plain_html);
    }
}

void ULlmChatHistoryPanel::beginAssistantStream(const QString& header_html)
{
    cancelStream();
    m_stream_plain.clear();
    m_stream_row = makeTextRow(header_html);
    addRow(m_stream_row);
}

void ULlmChatHistoryPanel::appendStreamText(const QString& token)
{
    if(!m_stream_row || token.isEmpty())
        return;
    m_stream_plain += token;
    const QString header = QStringLiteral("<b>%1:</b> ").arg(tr("Assistant"));
    m_stream_row->setHtml(header + QString(m_stream_plain).toHtmlEscaped().replace(
                                       QLatin1Char('\n'), QStringLiteral("<br/>")));
    fitBrowserHeight(m_stream_row);
    scrollToEnd();
}

void ULlmChatHistoryPanel::finalizeStreamMarkdown(const QString& body_markdown_or_plain)
{
    if(!m_stream_row)
        return;
    const QString header = QStringLiteral("<b>%1:</b> ").arg(tr("Assistant"));
    QString body = body_markdown_or_plain;
    if(body.startsWith(header))
        body = body.mid(header.size());
    m_stream_row->setHtml(header + llmMarkdownToHtmlFragment(body));
    fitBrowserHeight(m_stream_row);
    m_stream_row = nullptr;
    m_stream_plain.clear();
    scrollToEnd();
}

void ULlmChatHistoryPanel::cancelStream()
{
    if(m_stream_row && m_stream_plain.isEmpty())
    {
        m_layout->removeWidget(m_stream_row);
        m_stream_row->deleteLater();
    }
    m_stream_row = nullptr;
    m_stream_plain.clear();
}

void ULlmChatHistoryPanel::scrollToEnd()
{
    if(QScrollBar* bar = verticalScrollBar())
        QMetaObject::invokeMethod(
            this,
            [bar]() { bar->setValue(bar->maximum()); }, Qt::QueuedConnection);
}
