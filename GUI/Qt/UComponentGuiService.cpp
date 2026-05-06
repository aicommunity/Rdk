#include "UComponentGuiService.h"

#include <QWidget>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "../../Deploy/Include/rdk_cpp_init.h"
#include "UVisualControllerWidget.h"

namespace
{
QString resolveComponentGuiTitle(const UComponentGuiContext& context)
{
    if(!context.componentLongName.trimmed().isEmpty())
        return context.componentLongName;

    if(auto model = RDK::GetModel())
    {
        const std::string modelName = model->GetName();
        if(!modelName.empty())
            return QString::fromStdString(modelName);
    }

    return QStringLiteral("NModel");
}

QMdiSubWindow* resolveMdiSubWindow(UVisualControllerWidget* widget)
{
    if(!widget)
        return nullptr;

    if(auto* subByWindow = qobject_cast<QMdiSubWindow*>(widget->window()))
        return subByWindow;
    if(auto* subByParent = qobject_cast<QMdiSubWindow*>(widget->parentWidget()))
        return subByParent;

    QObject* current = widget->parent();
    while(current)
    {
        if(auto* sub = qobject_cast<QMdiSubWindow*>(current))
            return sub;
        current = current->parent();
    }
    return nullptr;
}

void activateWidgetHost(UVisualControllerWidget* widget)
{
    if(!widget)
        return;

    if(auto* sub = resolveMdiSubWindow(widget))
    {
        if(auto* mdiArea = sub->mdiArea())
        {
            // In tabbed MDI mode, explicit subwindow activation is required,
            // but we must not alter the window state (showNormal) to avoid
            // breaking other MDI tabs layout.
            sub->show();
            mdiArea->setActiveSubWindow(sub);
            sub->raise();
            return;
        }
    }

    widget->show();
    widget->raise();
    widget->activateWindow();
}
}

UComponentGuiService::UComponentGuiService(RDK::UApplication* app)
    : m_application(app)
{
}

void UComponentGuiService::setApplication(RDK::UApplication* app)
{
    m_application = app;
}

bool UComponentGuiService::canOpen(const UComponentGuiContext& context) const
{
    return UComponentFormRegistry::instance().canOpen(context);
}

UVisualControllerWidget* UComponentGuiService::createOrActivate(QWidget* parentWindow, const UComponentGuiContext& context)
{
    const UComponentFormDescriptor* descriptor = UComponentFormRegistry::instance().findDescriptor(context.componentClassName);
    if(!descriptor || !descriptor->factory || !m_application)
        return nullptr;

    clearClosedInstances();

    const QString instanceKey = makeInstanceKey(context, *descriptor);
    if(descriptor->singleInstance && m_instances.contains(instanceKey) && !m_instances[instanceKey].isNull())
    {
        UVisualControllerWidget* existing = m_instances[instanceKey].data();
        const QString title = resolveComponentGuiTitle(context);
        existing->setWindowTitle(title);
        if(auto* sub = resolveMdiSubWindow(existing))
            sub->setWindowTitle(title);
        applyContext(existing, context);
        activateWidgetHost(existing);
        return existing;
    }

    UVisualControllerWidget* widget = descriptor->factory(m_application);
    if(!widget)
        return nullptr;

    QMdiSubWindow* mdiSubWindow = nullptr;
    if(auto* mdiArea = qobject_cast<QMdiArea*>(parentWindow))
    {
        mdiSubWindow = mdiArea->addSubWindow(widget, Qt::SubWindow);
        if(mdiSubWindow)
            mdiSubWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    }
    else if(parentWindow)
    {
        widget->setParent(parentWindow);
    }
    const QString title = resolveComponentGuiTitle(context);
    widget->setWindowTitle(title);
    widget->setAttribute(Qt::WA_DeleteOnClose, true);
    applyContext(widget, context);
    if(mdiSubWindow)
        mdiSubWindow->setWindowTitle(title);
    activateWidgetHost(widget);

    if(descriptor->singleInstance)
        m_instances[instanceKey] = widget;
    return widget;
}

void UComponentGuiService::clearClosedInstances()
{
    for(auto it = m_instances.begin(); it != m_instances.end();)
    {
        if(it.value().isNull())
            it = m_instances.erase(it);
        else
            ++it;
    }
}

void UComponentGuiService::clearAllInstances()
{
    for(auto it = m_instances.begin(); it != m_instances.end(); ++it)
    {
        if(!it.value().isNull())
            it.value().data()->close();
    }
    m_instances.clear();
}

QString UComponentGuiService::makeInstanceKey(const UComponentGuiContext& context, const UComponentFormDescriptor& descriptor) const
{
    return descriptor.formId + "|" + context.componentLongName + "|" + QString::number(context.channelIndex);
}

void UComponentGuiService::applyContext(UVisualControllerWidget* widget, const UComponentGuiContext& context) const
{
    IComponentControllerWidget* controller = dynamic_cast<IComponentControllerWidget*>(widget);
    if(!controller)
        return;
    controller->setComponentContext(context);
    controller->refreshFromModel(true);
}
