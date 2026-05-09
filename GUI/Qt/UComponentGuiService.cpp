#include "UComponentGuiService.h"

#include <QWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMainWindow>
#include <QDockWidget>
#include <QDebug>

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

QDockWidget* resolveDockHost(UVisualControllerWidget* widget)
{
    if(!widget)
        return nullptr;

    if(auto* dockByWindow = qobject_cast<QDockWidget*>(widget->window()))
        return dockByWindow;
    if(auto* dockByParent = qobject_cast<QDockWidget*>(widget->parentWidget()))
        return dockByParent;

    QObject* current = widget->parent();
    while(current)
    {
        if(auto* dock = qobject_cast<QDockWidget*>(current))
            return dock;
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

    if(auto* dock = resolveDockHost(widget))
    {
        dock->show();
        dock->raise();
        dock->activateWindow();
        return;
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

void UComponentGuiService::setHostMainWindow(QMainWindow* mainWindow)
{
    m_hostMainWindow = mainWindow;
}

void UComponentGuiService::setSecondaryHostMainWindow(QMainWindow* mainWindow)
{
    m_secondaryHostMainWindow = mainWindow;
}

void UComponentGuiService::setTabHostMainWindow(QMainWindow* mainWindow)
{
    m_tabHostMainWindow = mainWindow;
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
        if(auto* dock = resolveDockHost(existing))
            dock->setWindowTitle(title);
        applyContext(existing, context);
        activateWidgetHost(existing);
        if(auto* dock = resolveDockHost(existing))
            assignHostMode(instanceKey, dock->isFloating() ? UComponentGuiHostMode::Floating : UComponentGuiHostMode::Mdi);
        else
            assignHostMode(instanceKey, UComponentGuiHostMode::Mdi);
        m_lastActiveSession = instanceKey;
        ++m_activationCounter;
        return existing;
    }

    UVisualControllerWidget* widget = descriptor->factory(m_application);
    if(!widget)
        return nullptr;

    QMainWindow* hostMainWindow = m_hostMainWindow.data();
    if(!hostMainWindow && parentWindow)
        hostMainWindow = qobject_cast<QMainWindow*>(parentWindow->window());

    QDockWidget* dockHost = nullptr;
    QMdiSubWindow* mdiSubWindow = nullptr;
    if(hostMainWindow)
    {
        dockHost = new QDockWidget(resolveComponentGuiTitle(context), hostMainWindow);
        QString objectName = QStringLiteral("ComponentGuiDock_%1").arg(instanceKey);
        objectName.replace('|', '_');
        objectName.replace('.', '_');
        objectName.replace(':', '_');
        dockHost->setObjectName(objectName);
        dockHost->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable |
                              QDockWidget::DockWidgetClosable);
        dockHost->setAllowedAreas(Qt::AllDockWidgetAreas);
        dockHost->setWidget(widget);
        dockHost->setAttribute(Qt::WA_DeleteOnClose, true);
        hostMainWindow->addDockWidget(Qt::RightDockWidgetArea, dockHost);
        dockHost->show();
    }
    else if(auto* mdiArea = qobject_cast<QMdiArea*>(parentWindow))
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
    if(dockHost)
        dockHost->setWindowTitle(title);
    if(mdiSubWindow)
        mdiSubWindow->setWindowTitle(title);
    activateWidgetHost(widget);

    if(descriptor->singleInstance)
    {
        m_instances[instanceKey] = widget;
        if(dockHost)
            m_dockHosts[instanceKey] = dockHost;
        m_instanceContexts[instanceKey] = context;
        m_instanceFormIds[instanceKey] = descriptor->formId;
        assignHostMode(instanceKey,
                       dockHost ? (dockHost->isFloating() ? UComponentGuiHostMode::Floating : UComponentGuiHostMode::Mdi)
                                : (mdiSubWindow ? UComponentGuiHostMode::Mdi : UComponentGuiHostMode::Mdi));
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

    QDockWidget* dock = m_dockHosts.value(key).data();
    if(!dock)
        dock = resolveDockHost(widget);
    if(!dock)
    {
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

    if(!dock->isFloating())
        dock->setFloating(true);
    dock->show();
    dock->raise();
    dock->activateWindow();
    assignHostMode(key, UComponentGuiHostMode::Floating);
    m_lastActiveSession = key;
    ++m_activationCounter;
    return true;
}

bool UComponentGuiService::attachToMdi(const UComponentGuiContext& context, QMdiArea* mdiArea)
{
    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    if(!widget)
        return false;

    QMainWindow* hostMainWindow = m_hostMainWindow.data();
    if(!hostMainWindow && mdiArea)
        hostMainWindow = qobject_cast<QMainWindow*>(mdiArea->window());
    if(!hostMainWindow && widget)
        hostMainWindow = qobject_cast<QMainWindow*>(widget->window());
    if(!hostMainWindow)
    {
        if(!mdiArea)
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
        widget->setWindowFlags(Qt::Widget);
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

    QDockWidget* dock = m_dockHosts.value(key).data();
    if(!dock)
    {
        dock = resolveDockHost(widget);
        if(!dock)
        {
            dock = new QDockWidget(widget->windowTitle(), hostMainWindow);
            QString objectName = QStringLiteral("ComponentGuiDock_%1").arg(key);
            objectName.replace('|', '_');
            objectName.replace('.', '_');
            objectName.replace(':', '_');
            dock->setObjectName(objectName);
            dock->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable |
                              QDockWidget::DockWidgetClosable);
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            dock->setWidget(widget);
            dock->setAttribute(Qt::WA_DeleteOnClose, true);
            m_dockHosts[key] = dock;
        }
    }

    if(dock->widget() != widget)
        dock->setWidget(widget);
    hostMainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setFloating(false);
    dock->show();
    dock->raise();
    dock->activateWindow();
    assignHostMode(key, UComponentGuiHostMode::Mdi);
    m_lastActiveSession = key;
    ++m_activationCounter;
    return true;
}

bool UComponentGuiService::attachToSecondaryDock(const UComponentGuiContext& context)
{
    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    if(!widget)
        return false;

    QMainWindow* hostMainWindow = m_secondaryHostMainWindow.data();
    if(!hostMainWindow)
        return false;

    QDockWidget* dock = m_dockHosts.value(key).data();
    if(!dock)
    {
        dock = resolveDockHost(widget);
        if(!dock)
        {
            dock = new QDockWidget(widget->windowTitle(), hostMainWindow);
            QString objectName = QStringLiteral("ComponentGuiDockSecondary_%1").arg(key);
            objectName.replace('|', '_');
            objectName.replace('.', '_');
            objectName.replace(':', '_');
            dock->setObjectName(objectName);
            dock->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable |
                              QDockWidget::DockWidgetClosable);
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            dock->setWidget(widget);
            dock->setAttribute(Qt::WA_DeleteOnClose, true);
            m_dockHosts[key] = dock;
        }
    }

    if(dock->widget() != widget)
        dock->setWidget(widget);
    hostMainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setFloating(false);
    dock->show();
    dock->raise();
    dock->activateWindow();
    assignHostMode(key, UComponentGuiHostMode::SecondaryDock);
    m_lastActiveSession = key;
    ++m_activationCounter;
    return true;
}

bool UComponentGuiService::attachToTabHostDock(const UComponentGuiContext& context)
{
    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    if(!widget)
        return false;

    QMainWindow* hostMainWindow = m_tabHostMainWindow.data();
    if(!hostMainWindow)
        return false;

    QDockWidget* dock = m_dockHosts.value(key).data();
    if(!dock)
    {
        dock = resolveDockHost(widget);
        if(!dock)
        {
            dock = new QDockWidget(widget->windowTitle(), hostMainWindow);
            QString objectName = QStringLiteral("ComponentGuiDockTabHost_%1").arg(key);
            objectName.replace('|', '_');
            objectName.replace('.', '_');
            objectName.replace(':', '_');
            dock->setObjectName(objectName);
            dock->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable |
                              QDockWidget::DockWidgetClosable);
            dock->setAllowedAreas(Qt::AllDockWidgetAreas);
            dock->setWidget(widget);
            dock->setAttribute(Qt::WA_DeleteOnClose, true);
            m_dockHosts[key] = dock;
        }
    }

    if(dock->widget() != widget)
        dock->setWidget(widget);
    hostMainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setFloating(false);
    dock->show();
    dock->raise();
    dock->activateWindow();
    assignHostMode(key, UComponentGuiHostMode::TabHost, QStringLiteral("MainTabHost"), -1, -1);
    m_lastActiveSession = key;
    ++m_activationCounter;
    return true;
}

bool UComponentGuiService::moveToTabHost(const UComponentGuiContext& context,
                                         const QString& hostId,
                                         QWidget* hostWidget)
{
    if(!m_tabHostMainWindow.isNull())
        return attachToTabHostDock(context);

    if(!hostWidget)
        return false;

    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    if(!widget)
        return false;

    if(QDockWidget* dock = m_dockHosts.value(key).data())
    {
        if(dock->widget() == widget)
            dock->setWidget(nullptr);
        dock->hide();
    }
    if(auto* sub = resolveMdiSubWindow(widget))
    {
        sub->setWidget(nullptr);
        sub->close();
    }

    widget->hide();
    widget->setParent(hostWidget);
    widget->setWindowFlags(Qt::Widget);
    // Tab host cells use a layout and often still have an empty rect right after addTab();
    // UComponentGuiTabHostWidget::embedWidgetInTabCell finishes sizing in that case.
    if(!hostWidget->layout())
    {
        widget->setGeometry(hostWidget->rect());
        widget->show();
    }

    assignHostMode(key, UComponentGuiHostMode::TabHost, hostId, -1, -1);
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
        UInstanceHostInfo host = m_instanceHostInfo.value(key);
        if(m_dockHosts.contains(key) && !m_dockHosts.value(key).isNull())
        {
            QDockWidget* dock = m_dockHosts.value(key).data();
            if(dock->widget() == it.value().data())
            {
                if(dock->isFloating())
                {
                    host.mode = UComponentGuiHostMode::Floating;
                }
                else
                {
                    QMainWindow* owner = qobject_cast<QMainWindow*>(dock->parentWidget());
                    if(owner == m_hostMainWindow.data())
                        host.mode = UComponentGuiHostMode::Mdi;
                    else if(owner == m_tabHostMainWindow.data())
                        host.mode = UComponentGuiHostMode::TabHost;
                    else if(owner == m_secondaryHostMainWindow.data())
                        host.mode = UComponentGuiHostMode::SecondaryDock;
                }
            }
        }

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
    for(auto it = m_dockHosts.begin(); it != m_dockHosts.end();)
    {
        if(it.value().isNull())
            it = m_dockHosts.erase(it);
        else
            ++it;
    }

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
    for(auto it = m_dockHosts.begin(); it != m_dockHosts.end(); ++it)
    {
        if(!it.value().isNull())
            it.value()->close();
    }
    m_dockHosts.clear();

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
    if(!m_dockHosts.contains(sessionId) || m_dockHosts[sessionId].isNull())
        return false;

    QDockWidget* dock = m_dockHosts[sessionId].data();
    if(!dock)
        return false;

    if(!geometry.isEmpty())
        dock->restoreGeometry(geometry);

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
    UInstanceHostInfo info = m_instanceHostInfo.value(sessionId);
    if(info.mode != UComponentGuiHostMode::Floating)
        return false;

    if(!m_dockHosts.contains(sessionId) || m_dockHosts[sessionId].isNull())
        return false;
    info.floatingGeometry = m_dockHosts[sessionId]->saveGeometry();
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

bool UComponentGuiService::tryGetWidgetByContext(const UComponentGuiContext& context, UVisualControllerWidget*& outWidget) const
{
    outWidget = resolveInstance(context, nullptr);
    return outWidget != nullptr;
}

bool UComponentGuiService::tryGetHostModeByContext(const UComponentGuiContext& context, UComponentGuiHostMode& outMode) const
{
    QString key;
    UVisualControllerWidget* widget = resolveInstance(context, &key);
    Q_UNUSED(widget);
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
        if(m_dockHosts.contains(key) && !m_dockHosts[key].isNull())
            info.floatingGeometry = m_dockHosts[key]->saveGeometry();
        else
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
    m_dockHosts.remove(key);
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
