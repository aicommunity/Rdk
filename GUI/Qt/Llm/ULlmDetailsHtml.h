#ifndef RDK_ULLM_DETAILS_HTML_H
#define RDK_ULLM_DETAILS_HTML_H

#include <QString>
#include <QVector>

/// Parse HTML that may contain one or more `<details><summary>…</summary>…</details>` blocks
/// (tool traces / reasoning). Plain segments keep original HTML; details are split for Qt UI.
struct ULlmDetailsHtmlSegment {
    enum class Kind { Plain, Details };
    Kind kind = Kind::Plain;
    QString plain_html;
    QString summary;
    QString body_html;
};

/// Returns true when `html` is a single details element (optional surrounding whitespace).
bool llmTryParseSingleDetails(const QString& html, QString* summary_out, QString* body_out);

QVector<ULlmDetailsHtmlSegment> llmParseDetailsHtmlSegments(const QString& html);

#endif
