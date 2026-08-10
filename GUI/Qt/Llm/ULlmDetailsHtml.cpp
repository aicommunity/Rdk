#include "ULlmDetailsHtml.h"

namespace {

int indexOfTagEnd(const QString& html, int open_lt)
{
    return html.indexOf(QLatin1Char('>'), open_lt);
}

QString stripOuterWhitespace(QString s)
{
    return s.trimmed();
}

} // namespace

bool llmTryParseSingleDetails(const QString& html, QString* summary_out, QString* body_out)
{
    const QString trimmed = stripOuterWhitespace(html);
    if(!trimmed.startsWith(QStringLiteral("<details"), Qt::CaseInsensitive))
        return false;
    if(!trimmed.endsWith(QStringLiteral("</details>"), Qt::CaseInsensitive))
        return false;

    const int details_gt = indexOfTagEnd(trimmed, 0);
    if(details_gt < 0)
        return false;

    const int summary_open = trimmed.indexOf(QStringLiteral("<summary"), details_gt + 1,
                                             Qt::CaseInsensitive);
    if(summary_open < 0)
        return false;
    const int summary_gt = indexOfTagEnd(trimmed, summary_open);
    if(summary_gt < 0)
        return false;
    const int summary_close =
        trimmed.indexOf(QStringLiteral("</summary>"), summary_gt + 1, Qt::CaseInsensitive);
    if(summary_close < 0)
        return false;

    const int details_close =
        trimmed.lastIndexOf(QStringLiteral("</details>"), -1, Qt::CaseInsensitive);
    if(details_close <= summary_close)
        return false;

    if(summary_out)
        *summary_out = trimmed.mid(summary_gt + 1, summary_close - (summary_gt + 1)).trimmed();
    if(body_out)
    {
        const int body_start = summary_close + int(QStringLiteral("</summary>").size());
        *body_out = trimmed.mid(body_start, details_close - body_start).trimmed();
    }
    return true;
}

QVector<ULlmDetailsHtmlSegment> llmParseDetailsHtmlSegments(const QString& html)
{
    QVector<ULlmDetailsHtmlSegment> out;
    if(html.isEmpty())
        return out;

    int pos = 0;
    const int n = html.size();
    while(pos < n)
    {
        const int details_open = html.indexOf(QStringLiteral("<details"), pos, Qt::CaseInsensitive);
        if(details_open < 0)
        {
            const QString tail = html.mid(pos);
            if(!tail.trimmed().isEmpty())
            {
                ULlmDetailsHtmlSegment seg;
                seg.kind = ULlmDetailsHtmlSegment::Kind::Plain;
                seg.plain_html = tail;
                out.push_back(seg);
            }
            break;
        }

        if(details_open > pos)
        {
            const QString before = html.mid(pos, details_open - pos);
            if(!before.trimmed().isEmpty())
            {
                ULlmDetailsHtmlSegment seg;
                seg.kind = ULlmDetailsHtmlSegment::Kind::Plain;
                seg.plain_html = before;
                out.push_back(seg);
            }
        }

        const int details_close =
            html.indexOf(QStringLiteral("</details>"), details_open, Qt::CaseInsensitive);
        if(details_close < 0)
        {
            ULlmDetailsHtmlSegment seg;
            seg.kind = ULlmDetailsHtmlSegment::Kind::Plain;
            seg.plain_html = html.mid(details_open);
            out.push_back(seg);
            break;
        }

        const int block_end = details_close + int(QStringLiteral("</details>").size());
        const QString block = html.mid(details_open, block_end - details_open);
        QString summary;
        QString body;
        if(llmTryParseSingleDetails(block, &summary, &body))
        {
            ULlmDetailsHtmlSegment seg;
            seg.kind = ULlmDetailsHtmlSegment::Kind::Details;
            seg.summary = summary;
            seg.body_html = body;
            out.push_back(seg);
        }
        else
        {
            ULlmDetailsHtmlSegment seg;
            seg.kind = ULlmDetailsHtmlSegment::Kind::Plain;
            seg.plain_html = block;
            out.push_back(seg);
        }
        pos = block_end;
    }
    return out;
}
