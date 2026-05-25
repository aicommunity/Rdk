#include "ULlmChangePreviewWidget.h"

#include <QVBoxLayout>

ULlmChangePreviewWidget::ULlmChangePreviewWidget(QWidget* parent, RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    Name = "LlmChangePreview";
    ClassName = "ULlmChangePreviewWidget";
    auto* layout = new QVBoxLayout(this);
    m_view = new QTextEdit(this);
    m_view->setReadOnly(true);
    layout->addWidget(m_view);
}

void ULlmChangePreviewWidget::showPlan(const QString& summary)
{
    m_view->setPlainText(summary);
}
