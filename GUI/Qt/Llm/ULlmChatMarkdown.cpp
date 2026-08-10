#include "ULlmChatMarkdown.h"

#include <QRegularExpression>
#include <QTextDocument>

bool llmLooksLikeUiHtml(const QString& text)
{
    if(text.isEmpty())
        return false;
    const QString t = text.trimmed();
    if(t.startsWith(QStringLiteral("<details"), Qt::CaseInsensitive))
        return true;
    if(t.contains(QStringLiteral("<summary"), Qt::CaseInsensitive))
        return true;
    if(t.contains(QStringLiteral("<b>Question</b>"), Qt::CaseInsensitive))
        return true;
    if(t.contains(QStringLiteral("<b>Clarification"), Qt::CaseInsensitive))
        return true;
    if(t.contains(QStringLiteral("<b>[Rollback]</b>"), Qt::CaseInsensitive))
        return true;
    if(t.contains(QStringLiteral("<i>[Cancelled]</i>"), Qt::CaseInsensitive))
        return true;
    if(t.contains(QStringLiteral("<i>[Tool:"), Qt::CaseInsensitive))
        return true;
    // Explicit HTML paragraph/bold wrappers used by dock chrome (not model markdown).
    if(t.startsWith(QStringLiteral("<b>You:</b>")) || t.startsWith(QStringLiteral("<b>Assistant:</b>"))
       || t.startsWith(QStringLiteral("<p><b>You:</b>"))
       || t.startsWith(QStringLiteral("<p><b>Assistant:</b>")))
        return true;
    return false;
}

QString llmMarkdownToHtmlFragment(const QString& text)
{
    if(text.isEmpty() || llmLooksLikeUiHtml(text))
        return text;

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QTextDocument doc;
    doc.setMarkdown(text);
    QString html = doc.toHtml();
    // Strip Qt document chrome; keep body inner HTML.
    const int body_open = html.indexOf(QStringLiteral("<body"), 0, Qt::CaseInsensitive);
    if(body_open >= 0)
    {
        const int gt = html.indexOf(QLatin1Char('>'), body_open);
        const int body_close = html.lastIndexOf(QStringLiteral("</body>"), -1, Qt::CaseInsensitive);
        if(gt >= 0 && body_close > gt)
            html = html.mid(gt + 1, body_close - (gt + 1)).trimmed();
    }
    // Drop empty paragraphs Qt sometimes emits around content.
    html.remove(QRegularExpression(QStringLiteral("^\\s*<p\\s*style=\"[^\"]*\"\\s*>\\s*</p>\\s*"),
                                   QRegularExpression::CaseInsensitiveOption));
    return html.isEmpty() ? text.toHtmlEscaped() : html;
#else
    return text.toHtmlEscaped();
#endif
}
