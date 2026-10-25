#include "ULlmCollapsibleDetailsWidget.h"

#include <QTextBrowser>
#include <QTextDocument>
#include <QToolButton>
#include <QVBoxLayout>

ULlmCollapsibleDetailsWidget::ULlmCollapsibleDetailsWidget(const QString& summary_html,
                                                           const QString& body_html,
                                                           bool initially_expanded, QWidget* parent)
    : QWidget(parent)
    , m_expanded(initially_expanded)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 2, 0, 2);
    layout->setSpacing(2);

    m_toggle = new QToolButton(this);
    m_toggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toggle->setAutoRaise(true);
    m_toggle->setCursor(Qt::PointingHandCursor);
    // summary may contain simple markup; show as plain for the button label
    QTextDocument doc;
    doc.setHtml(summary_html);
    m_summary_plain = doc.toPlainText().trimmed();
    if(m_summary_plain.isEmpty())
        m_summary_plain = tr("Details");
    connect(m_toggle, &QToolButton::clicked, this, &ULlmCollapsibleDetailsWidget::onToggleClicked);
    layout->addWidget(m_toggle);

    m_body = new QTextBrowser(this);
    m_body->setOpenExternalLinks(false);
    m_body->setFrameShape(QFrame::NoFrame);
    m_body->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_body->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_body->setHtml(body_html);
    m_body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(m_body);

    updateChrome();
}

bool ULlmCollapsibleDetailsWidget::isExpanded() const
{
    return m_expanded;
}

void ULlmCollapsibleDetailsWidget::setExpanded(bool expanded)
{
    if(m_expanded == expanded)
        return;
    m_expanded = expanded;
    updateChrome();
}

void ULlmCollapsibleDetailsWidget::onToggleClicked()
{
    setExpanded(!m_expanded);
}

void ULlmCollapsibleDetailsWidget::updateChrome()
{
    const QString mark = m_expanded ? QStringLiteral("▼ ") : QStringLiteral("▶ ");
    m_toggle->setText(mark + m_summary_plain);
    m_body->setVisible(m_expanded);
    if(m_expanded)
    {
        m_body->document()->adjustSize();
        const int h = int(m_body->document()->size().height()) + 8;
        m_body->setMinimumHeight(h);
        m_body->setMaximumHeight(h);
    }
}
