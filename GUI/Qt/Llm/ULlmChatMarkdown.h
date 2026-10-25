#ifndef ULLMCHATMARKDOWN_H
#define ULLMCHATMARKDOWN_H

#include <QString>

/// True when text is intentional UI/tool HTML that must not go through markdown conversion.
bool llmLooksLikeUiHtml(const QString& text);

/// Convert assistant markdown to an HTML fragment via QTextDocument (Qt >= 5.14).
/// UI HTML (details/summary/b Question/…) is returned unchanged.
QString llmMarkdownToHtmlFragment(const QString& text);

#endif
