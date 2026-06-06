#include "UGenericComponentControllerWidget.h"

UGenericComponentControllerWidget::UGenericComponentControllerWidget(const QString& controllerId,
                                                                     const QString& controllerTitle,
                                                                     QWidget* parent,
                                                                     RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
    , m_controllerId(controllerId)
    , m_controllerTitle(controllerTitle)
    , m_titleLabel(new QLabel(this))
    , m_details(new QPlainTextEdit(this))
{
    setAccessibleName(controllerTitle);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_details);

    m_details->setReadOnly(true);
    m_titleLabel->setText(controllerTitle);
    m_details->setPlainText("Component GUI form is initialized.");
}

void UGenericComponentControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
}

void UGenericComponentControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    QString details;
    details += "Controller: " + m_controllerTitle + "\n";
    details += "Component: " + m_context.componentLongName + "\n";
    details += "Class: " + m_context.componentClassName + "\n";
    details += "Channel: " + QString::number(m_context.channelIndex) + "\n";
    m_details->setPlainText(details);
}

QString UGenericComponentControllerWidget::componentGuiId() const
{
    return m_controllerId;
}
