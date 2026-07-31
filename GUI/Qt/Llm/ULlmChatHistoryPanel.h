#ifndef RDK_ULLM_CHAT_HISTORY_PANEL_H
#define RDK_ULLM_CHAT_HISTORY_PANEL_H

#include <QScrollArea>
#include <QString>

class QVBoxLayout;
class QTextBrowser;
class QWidget;

/// Scrollable chat transcript: rich-text rows + collapsible `<details>` widgets.
class ULlmChatHistoryPanel : public QScrollArea {
    Q_OBJECT
public:
    explicit ULlmChatHistoryPanel(QWidget* parent = nullptr);

    void clear();
    /// Append HTML; splits out `<details>` blocks into collapsible widgets.
    /// `details_expanded_default`: Tools typically true, Reasoning false when known.
    void appendHtml(const QString& html, bool details_expanded_default = true);
    void appendDetails(const QString& summary, const QString& body_html, bool expanded);

    void beginAssistantStream(const QString& header_html);
    void appendStreamText(const QString& token);
    void finalizeStreamMarkdown(const QString& body_markdown_or_plain);
    void cancelStream();

    void scrollToEnd();

private:
    void appendPlainHtmlRow(const QString& html);
    QTextBrowser* makeTextRow(const QString& html);
    void addRow(QWidget* w);

    QWidget* m_container = nullptr;
    QVBoxLayout* m_layout = nullptr;
    QTextBrowser* m_stream_row = nullptr;
    QString m_stream_plain;
};
#endif
