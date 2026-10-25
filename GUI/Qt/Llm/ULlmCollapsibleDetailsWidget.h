#ifndef RDK_ULLM_COLLAPSIBLE_DETAILS_WIDGET_H
#define RDK_ULLM_COLLAPSIBLE_DETAILS_WIDGET_H

#include <QWidget>

class QToolButton;
class QTextBrowser;

/// Qt visualization of HTML5 `<details>/<summary>` (not supported by QTextEdit subset).
class ULlmCollapsibleDetailsWidget : public QWidget {
    Q_OBJECT
public:
    ULlmCollapsibleDetailsWidget(const QString& summary_html, const QString& body_html,
                                 bool initially_expanded, QWidget* parent = nullptr);

    bool isExpanded() const;
    void setExpanded(bool expanded);

private slots:
    void onToggleClicked();

private:
    void updateChrome();

    QToolButton* m_toggle = nullptr;
    QTextBrowser* m_body = nullptr;
    QString m_summary_plain;
    bool m_expanded = true;
};

#endif
