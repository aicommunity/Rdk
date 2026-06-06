#ifndef UGENERICCOMPONENTCONTROLLERWIDGET_H
#define UGENERICCOMPONENTCONTROLLERWIDGET_H

#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include "UVisualControllerWidget.h"
#include "IComponentControllerWidget.h"
#include "UComponentGuiContext.h"

class UGenericComponentControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget
{
    Q_OBJECT
public:
    explicit UGenericComponentControllerWidget(const QString& controllerId,
                                               const QString& controllerTitle,
                                               QWidget* parent = nullptr,
                                               RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private:
    QString m_controllerId;
    QString m_controllerTitle;
    UComponentGuiContext m_context;
    QLabel* m_titleLabel;
    QPlainTextEdit* m_details;
};

#endif // UGENERICCOMPONENTCONTROLLERWIDGET_H
