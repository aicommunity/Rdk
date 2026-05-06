#include "UComponentGuiService.h"

#include <QWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMainWindow>

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
        assignHostMode(instanceKey, resolveMdiSubWindow(existing) ? UComponentGuiHostMode::Mdi : UComponentGuiHostMode::Floating);
        m_lastActiveSession = instanceKey;
        ++m_activationCounter;
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
    {
        m_instances[instanceKey] = widget;
        m_instanceContexts[instanceKey] = context;
        m_instanceFormIds[instanceKey] = descriptor->formId;
        assignHostMode(instanceKey, mdiSubWindow ? UComponentGuiHostMode::Mdi : UComponentGuiHostMode::Floating);
        m_lastActiveSession = instanceKey;
        ++m_activationCounter;
    }
    return widget;
}

bool UComponentGuiService::detachToFloating(const UComponentGuiContext& context)
{
    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    if(!widget)
        return false;

    if(auto* sub = resolveMdiSubWindow(widget))
    {
        sub->setWidget(nullptr);
        sub->close();
    }

    widget->setParent(nullptr);
    widget->setWindowFlags(Qt::Window);
    widget->show();
    widget->raise();
    widget->activateWindow();
    assignHostMode(key, UComponentGuiHostMode::Floating);
    m_lastActiveSession = key;
    ++m_activationCounter;
    return true;
}

bool UComponentGuiService::attachToMdi(const UComponentGuiContext& context, QMdiArea* mdiArea)
{
    if(!mdiArea)
        return false;

    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    if(!widget)
        return false;

    if(auto* sub = resolveMdiSubWindow(widget))
    {
        mdiArea->setActiveSubWindow(sub);
        sub->show();
        sub->raise();
        assignHostMode(key, UComponentGuiHostMode::Mdi);
        return true;
    }

    widget->hide();
    widget->setParent(nullptr);
    QMdiSubWindow* sub = mdiArea->addSubWindow(widget, Qt::SubWindow);
    if(!sub)
        return false;

    sub->setAttribute(Qt::WA_DeleteOnClose, true);
    sub->setWindowTitle(widget->windowTitle());
    sub->show();
    mdiArea->setActiveSubWindow(sub);
    sub->raise();
    assignHostMode(key, UComponentGuiHostMode::Mdi);
    m_lastActiveSession = key;
    ++m_activationCounter;
    return true;
}

bool UComponentGuiService::moveToGridCell(const UComponentGuiContext& context,
                                          const QString& gridId,
                                          int row,
                                          int col,
                                          QWidget* cellHost)
{
    if(!cellHost)
        return false;

    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    if(!widget)
        return false;

    if(auto* sub = resolveMdiSubWindow(widget))
    {
        sub->setWidget(nullptr);
        sub->close();
    }

    widget->hide();
    widget->setParent(cellHost);
    widget->setWindowFlags(Qt::Widget);
    widget->setGeometry(cellHost->rect());
    widget->show();
    assignHostMode(key, UComponentGuiHostMode::Grid, gridId, row, col);
    m_lastActiveSession = key;
    ++m_activationCounter;
    return true;
}

QList<UComponentGuiSessionSnapshot> UComponentGuiService::snapshotOpenSessions() const
{
    QList<UComponentGuiSessionSnapshot> out;
    int order = 0;
    for(auto it = m_instances.constBegin(); it != m_instances.constEnd(); ++it)
    {
        if(it.value().isNull())
            continue;

        const QString key = it.key();
        const UComponentGuiContext ctx = m_instanceContexts.value(key);
        const UInstanceHostInfo host = m_instanceHostInfo.value(key);

        UComponentGuiSessionSnapshot snapshot;
        snapshot.sessionId = key;
        snapshot.formId = m_instanceFormIds.value(key);
        snapshot.componentClassName = ctx.componentClassName;
        snapshot.componentLongName = ctx.componentLongName;
        snapshot.channelIndex = ctx.channelIndex;
        snapshot.hostMode = host.mode;
        snapshot.containerId = host.containerId;
        snapshot.cellRow = host.row;
        snapshot.cellCol = host.col;
        snapshot.orderIndex = order++;
        snapshot.isActive = (m_lastActiveSession == key);
        out.push_back(snapshot);
    }
    return out;
}

void UComponentGuiService::clearClosedInstances()
{
    for(auto it = m_instances.begin(); it != m_instances.end();)
    {
        if(it.value().isNull())
        {
            clearSessionState(it.key());
            it = m_instances.erase(it);
        }
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
    m_instanceContexts.clear();
    m_instanceFormIds.clear();
    m_instanceHostInfo.clear();
    m_lastActiveSession.clear();
}

bool UComponentGuiService::applyFloatingState(const QString& sessionId, const QByteArray& geometry, const QByteArray& state)
{
    if(!m_instances.contains(sessionId) || m_instances[sessionId].isNull())
        return false;

    UVisualControllerWidget* widget = m_instances[sessionId].data();
    if(!widget)
        return false;

    if(!geometry.isEmpty())
        widget->restoreGeometry(geometry);
    if(!state.isEmpty())
    {
        if(auto* mw = qobject_cast<QMainWindow*>(widget))
            mw->restoreState(state);
    }
    UInstanceHostInfo host = m_instanceHostInfo.value(sessionId);
    host.floatingGeometry = geometry;
    host.floatingWindowState = state;
    m_instanceHostInfo[sessionId] = host;
    return true;
}

QByteArray UComponentGuiService::floatingGeometry(const QString& sessionId) const
{
    if(m_instanceHostInfo.contains(sessionId))
        return m_instanceHostInfo.value(sessionId).floatingGeometry;
    return QByteArray();
}

QByteArray UComponentGuiService::floatingWindowState(const QString& sessionId) const
{
    if(m_instanceHostInfo.contains(sessionId))
        return m_instanceHostInfo.value(sessionId).floatingWindowState;
    return QByteArray();
}

bool UComponentGuiService::captureFloatingState(const QString& sessionId)
{
    if(!m_instances.contains(sessionId) || m_instances[sessionId].isNull())
        return false;
    UVisualControllerWidget* widget = m_instances[sessionId].data();
    if(!widget)
        return false;

    UInstanceHostInfo info = m_instanceHostInfo.value(sessionId);
    if(info.mode != UComponentGuiHostMode::Floating)
        return false;

    info.floatingGeometry = widget->saveGeometry();
    if(auto* mainWindow = qobject_cast<QMainWindow*>(widget))
        info.floatingWindowState = mainWindow->saveState();
    else
        info.floatingWindowState.clear();
    m_instanceHostInfo[sessionId] = info;
    return true;
}

bool UComponentGuiService::tryGetContextByWidget(const UVisualControllerWidget* widget, UComponentGuiContext& outContext) const
{
    const QString key = findSessionKeyByWidget(widget);
    if(key.isEmpty() || !m_instanceContexts.contains(key))
        return false;
    outContext = m_instanceContexts.value(key);
    return true;
}

bool UComponentGuiService::tryGetHostModeByWidget(const UVisualControllerWidget* widget, UComponentGuiHostMode& outMode) const
{
    const QString key = findSessionKeyByWidget(widget);
    if(key.isEmpty() || !m_instanceHostInfo.contains(key))
        return false;
    outMode = m_instanceHostInfo.value(key).mode;
    return true;
}

QString UComponentGuiService::makeInstanceKey(const UComponentGuiContext& context, const UComponentFormDescriptor& descriptor) const
{
    return descriptor.formId + "|" + makeSessionKeyFromContext(context) + "|" + QString::number(context.channelIndex);
}

void UComponentGuiService::applyContext(UVisualControllerWidget* widget, const UComponentGuiContext& context) const
{
    IComponentControllerWidget* controller = dynamic_cast<IComponentControllerWidget*>(widget);
    if(!controller)
        return;
    controller->setComponentContext(context);
    controller->refreshFromModel(true);
}

QString UComponentGuiService::makeSessionKeyFromContext(const UComponentGuiContext& context) const
{
    QString longName = context.componentLongName.trimmed();
    if(!longName.isEmpty())
        return longName;
    if(auto model = RDK::GetModel())
    {
        const std::string name = model->GetName();
        if(!name.empty())
            return QString::fromStdString(name);
    }
    return QStringLiteral("NModel");
}

bool UComponentGuiService::resolveDescriptor(const UComponentGuiContext& context, UComponentFormDescriptor& descriptor) const
{
    const UComponentFormDescriptor* found = UComponentFormRegistry::instance().findDescriptor(context.componentClassName);
    if(!found)
        return false;
    descriptor = *found;
    return true;
}

UVisualControllerWidget* UComponentGuiService::resolveInstance(const UComponentGuiContext& context, QString* outSessionKey) const
{
    UComponentFormDescriptor descriptor;
    if(!resolveDescriptor(context, descriptor))
        return nullptr;
    const QString key = makeInstanceKey(context, descriptor);
    if(outSessionKey)
        *outSessionKey = key;
    if(!m_instances.contains(key) || m_instances.value(key).isNull())
        return nullptr;
    return m_instances.value(key).data();
}

void UComponentGuiService::assignHostMode(const QString& key,
                                          UComponentGuiHostMode mode,
                                          const QString& containerId,
                                          int row,
                                          int col)
{
    UInstanceHostInfo info = m_instanceHostInfo.value(key);
    info.mode = mode;
    info.containerId = containerId;
    info.row = row;
    info.col = col;
    if(mode == UComponentGuiHostMode::Floating && m_instances.contains(key) && !m_instances[key].isNull())
    {
        UVisualControllerWidget* widget = m_instances[key].data();
        info.floatingGeometry = widget->saveGeometry();
        if(auto* mainWindow = qobject_cast<QMainWindow*>(widget))
            info.floatingWindowState = mainWindow->saveState();
        else
            info.floatingWindowState.clear();
    }
    m_instanceHostInfo[key] = info;
}

void UComponentGuiService::clearSessionState(const QString& key)
{
    m_instanceContexts.remove(key);
    m_instanceFormIds.remove(key);
    m_instanceHostInfo.remove(key);
    if(m_lastActiveSession == key)
        m_lastActiveSession.clear();
}

QString UComponentGuiService::findSessionKeyByWidget(const UVisualControllerWidget* widget) const
{
    if(!widget)
        return QString();
    for(auto it = m_instances.constBegin(); it != m_instances.constEnd(); ++it)
    {
        if(!it.value().isNull() && it.value().data() == widget)
            return it.key();
    }
    return QString();
}
