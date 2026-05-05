#ifndef UCOMPONENTGUISERVICE_H
#define UCOMPONENTGUISERVICE_H

#include <QString>
#include <QHash>
#include <QPointer>

#include "UComponentGuiContext.h"
#include "UComponentFormRegistry.h"
#include "IComponentControllerWidget.h"

class QWidget;
class UVisualControllerWidget;

class UComponentGuiService
{
public:
    explicit UComponentGuiService(RDK::UApplication* app = nullptr);

    void setApplication(RDK::UApplication* app);

    bool canOpen(const UComponentGuiContext& context) const;
    UVisualControllerWidget* createOrActivate(QWidget* parentWindow, const UComponentGuiContext& context);
    void clearClosedInstances();
    void clearAllInstances();

private:
    QString makeInstanceKey(const UComponentGuiContext& context, const UComponentFormDescriptor& descriptor) const;
    void applyContext(UVisualControllerWidget* widget, const UComponentGuiContext& context) const;

private:
    RDK::UApplication* m_application;
    QHash<QString, QPointer<UVisualControllerWidget>> m_instances;
};

#endif // UCOMPONENTGUISERVICE_H
