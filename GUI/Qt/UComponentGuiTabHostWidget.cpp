#include "UComponentGuiTabHostWidget.h"

#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QMimeData>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QTabBar>

#include "UComponentGuiDndPayload.h"
#include "UComponentGuiService.h"

UComponentGuiTabHostWidget::UComponentGuiTabHostWidget(const QString& hostId,
                                                       UComponentGuiService* service,
                                                       QWidget* parent,
                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
    , m_hostId(hostId)
    , m_service(service)
{
    setAcceptDrops(true);
    setAccessibleName(QStringLiteral("ComponentGuiTabHost_%1").arg(hostId));
    setWindowTitle(QStringLiteral("Component GUI Host [%1]").arg(hostId));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::South);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setAcceptDrops(true);
    m_tabWidget->tabBar()->setAcceptDrops(true);
    if(QStackedWidget* stack = m_tabWidget->findChild<QStackedWidget*>())
    {
        stack->setAcceptDrops(true);
        stack->installEventFilter(this);
    }
    m_tabWidget->tabBar()->installEventFilter(this);
    m_tabWidget->installEventFilter(this);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
        if(index < 0 || index >= m_tabWidget->count())
            return;
        if(QWidget* host = m_tabWidget->widget(index))
        {
            host->hide();
            m_tabWidget->removeTab(index);
            syncCurrentTabState();
        }
    });
    connect(m_tabWidget, &QTabWidget::currentChanged, this, [this](int) { syncCurrentTabState(); });

    layout->addWidget(m_tabWidget);
}

QString UComponentGuiTabHostWidget::hostId() const
{
    return m_hostId;
}

bool UComponentGuiTabHostWidget::assignContext(const UComponentGuiContext& context)
{
    if(!m_service || !m_tabWidget)
        return false;

    const QString key = contextKey(context);
    int existingIndex = tabIndexForContext(context);
    QWidget* host = nullptr;

    if(existingIndex >= 0)
    {
        host = hostWidgetForIndex(existingIndex);
    }
    else
    {
        auto* frame = new QFrame(m_tabWidget);
        frame->setObjectName(QStringLiteral("ComponentGuiTabHostCell"));
        frame->setAcceptDrops(true);
        frame->installEventFilter(this);
        auto* frameLayout = new QHBoxLayout(frame);
        frameLayout->setContentsMargins(0, 0, 0, 0);
        frameLayout->setSpacing(0);
        const QString title = !context.componentLongName.trimmed().isEmpty() ? context.componentLongName : context.componentClassName;
        existingIndex = m_tabWidget->addTab(frame, title);
        host = frame;
        m_tabHosts[key] = frame;
    }

    UVisualControllerWidget* widget = m_service->createOrActivate(host, context);
    if(!widget)
        return false;

    if(widget->parentWidget() != host)
    {
        if(!m_service->moveToTabHost(context, m_hostId, host))
            return false;
    }
    embedWidgetInTabCell(host, widget);
    m_tabWidget->setCurrentIndex(existingIndex);
    syncCurrentTabState();
    if(m_afterAssignContextHook)
        m_afterAssignContextHook(context);
    return true;
}

void UComponentGuiTabHostWidget::setAfterAssignContextHook(std::function<void(const UComponentGuiContext&)> hook)
{
    m_afterAssignContextHook = std::move(hook);
}

void UComponentGuiTabHostWidget::embedWidgetInTabCell(QWidget* cell, UVisualControllerWidget* widget)
{
    if(!cell || !widget)
        return;

    if(auto* hLayout = qobject_cast<QHBoxLayout*>(cell->layout()))
    {
        if(hLayout->indexOf(widget) < 0)
        {
            while(hLayout->count())
                delete hLayout->takeAt(0);
            hLayout->addWidget(widget, 1);
        }
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    else
    {
        widget->setParent(cell);
        widget->setGeometry(cell->rect());
    }
    widget->show();
    widget->raise();
}

bool UComponentGuiTabHostWidget::removeContext(const UComponentGuiContext& context)
{
    const int index = tabIndexForContext(context);
    if(index < 0 || !m_tabWidget)
        return false;
    if(QWidget* host = m_tabWidget->widget(index))
        host->hide();
    m_tabWidget->removeTab(index);
    m_tabHosts.remove(contextKey(context));
    syncCurrentTabState();
    return true;
}

bool UComponentGuiTabHostWidget::hasContext(const UComponentGuiContext& context) const
{
    return tabIndexForContext(context) >= 0;
}

QList<UComponentGuiContext> UComponentGuiTabHostWidget::contexts() const
{
    QList<UComponentGuiContext> out;
    if(!m_tabWidget)
        return out;
    for(int i = 0; i < m_tabWidget->count(); ++i)
    {
        const QVariant data = m_tabWidget->tabBar()->tabData(i);
        if(data.canConvert<QString>())
        {
            const QStringList parts = data.toString().split('|');
            if(parts.size() >= 3)
            {
                UComponentGuiContext context;
                context.componentClassName = parts[0];
                context.componentLongName = parts[1];
                context.channelIndex = parts[2].toInt();
                out.push_back(context);
            }
        }
    }
    return out;
}

QByteArray UComponentGuiTabHostWidget::saveState() const
{
    if(!m_tabWidget || !m_tabWidget->tabBar())
        return QByteArray();
    return QByteArray::number(m_tabWidget->currentIndex());
}

void UComponentGuiTabHostWidget::restoreState(const QByteArray& state)
{
    if(!m_tabWidget || state.isEmpty())
        return;
    bool ok = false;
    const int index = QString::fromUtf8(state).toInt(&ok);
    if(ok && index >= 0 && index < m_tabWidget->count())
        m_tabWidget->setCurrentIndex(index);
}

void UComponentGuiTabHostWidget::pruneStaleTabForContext(const UComponentGuiContext& context)
{
    if(!m_tabWidget || !m_tabWidget->tabBar())
        return;

    const QString key = contextKey(context);
    bool changed = false;

    for(int i = m_tabWidget->count() - 1; i >= 0; --i)
    {
        QWidget* host = m_tabWidget->widget(i);
        if(!host)
            continue;

        const QVariant data = m_tabWidget->tabBar()->tabData(i);
        const bool keyedHere = (data.toString() == key) || (m_tabHosts.value(key).data() == host);
        if(!keyedHere)
            continue;

        if(host->findChild<UVisualControllerWidget*>(QString(), Qt::FindDirectChildrenOnly))
            continue;

        m_tabWidget->removeTab(i);
        if(m_tabHosts.contains(key) && m_tabHosts[key].data() == host)
            m_tabHosts.remove(key);
        changed = true;
    }

    if(changed)
        syncCurrentTabState();
}

bool UComponentGuiTabHostWidget::eventFilter(QObject* watched, QEvent* event)
{
    if(!m_tabWidget || !m_tabWidget->tabBar())
        return UVisualControllerWidget::eventFilter(watched, event);

    QTabBar* tabBar = m_tabWidget->tabBar();
    if(watched == tabBar)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                m_dragStartPos = mouseEvent->pos();
                m_dragStartIndex = tabBar->tabAt(mouseEvent->pos());
            }
        }
        else if(event->type() == QEvent::MouseMove)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if((mouseEvent->buttons() & Qt::LeftButton) && m_dragStartIndex >= 0)
            {
                if((mouseEvent->pos() - m_dragStartPos).manhattanLength() >= QApplication::startDragDistance())
                {
                    startDragFromTab(m_dragStartIndex);
                    m_dragStartIndex = -1;
                    return true;
                }
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            m_dragStartIndex = -1;
        }
    }
    QWidget* watchedWidget = qobject_cast<QWidget*>(watched);
    const bool isTabPage = watchedWidget && m_tabWidget && m_tabWidget->indexOf(watchedWidget) >= 0;
    const bool isKnownDropTarget = (watched == tabBar || watched == m_tabWidget || isTabPage);

    if(isKnownDropTarget && event->type() == QEvent::DragEnter)
    {
        auto* dragEvent = static_cast<QDragEnterEvent*>(event);
        if(dragEvent->mimeData() && dragEvent->mimeData()->hasFormat(UComponentGuiDndPayload::mimeType()))
            dragEvent->acceptProposedAction();
        else
            dragEvent->ignore();
        return true;
    }
    if(isKnownDropTarget && event->type() == QEvent::DragMove)
    {
        auto* dragEvent = static_cast<QDragMoveEvent*>(event);
        if(dragEvent->mimeData() && dragEvent->mimeData()->hasFormat(UComponentGuiDndPayload::mimeType()))
            dragEvent->acceptProposedAction();
        else
            dragEvent->ignore();
        return true;
    }
    if(isKnownDropTarget && event->type() == QEvent::Drop)
    {
        auto* dropEvent = static_cast<QDropEvent*>(event);
        if(handleDrop(dropEvent->mimeData()))
            dropEvent->acceptProposedAction();
        else
            dropEvent->ignore();
        return true;
    }
    return UVisualControllerWidget::eventFilter(watched, event);
}

void UComponentGuiTabHostWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData() && event->mimeData()->hasFormat(UComponentGuiDndPayload::mimeType()))
        event->acceptProposedAction();
    else
        event->ignore();
}

void UComponentGuiTabHostWidget::dragMoveEvent(QDragMoveEvent* event)
{
    if(event->mimeData() && event->mimeData()->hasFormat(UComponentGuiDndPayload::mimeType()))
        event->acceptProposedAction();
    else
        event->ignore();
}

void UComponentGuiTabHostWidget::dropEvent(QDropEvent* event)
{
    if(handleDrop(event->mimeData()))
        event->acceptProposedAction();
    else
        event->ignore();
}

bool UComponentGuiTabHostWidget::handleDrop(const QMimeData* mimeData)
{
    UComponentGuiContext context;
    QString sourceHostId;
    int sourceIndex = -1;
    int sourceCol = -1;
    if(!UComponentGuiDndPayload::decode(mimeData, context, sourceHostId, sourceIndex, sourceCol))
        return false;
    Q_UNUSED(sourceCol);

    const bool assigned = assignContext(context);
    if(!assigned)
        return false;

    if(!sourceHostId.isEmpty() && sourceHostId == m_hostId && sourceIndex >= 0 && sourceIndex < m_tabWidget->count())
    {
        // Same-host drop: if duplicate tab was added due to source drag, cleanup old slot.
        const int current = tabIndexForContext(context);
        if(sourceIndex != current && sourceIndex < m_tabWidget->count())
            m_tabWidget->removeTab(sourceIndex);
    }
    return true;
}

void UComponentGuiTabHostWidget::startDragFromTab(int index)
{
    if(index < 0 || index >= m_tabWidget->count())
        return;
    const QVariant data = m_tabWidget->tabBar()->tabData(index);
    if(!data.canConvert<QString>())
        return;
    const QStringList parts = data.toString().split('|');
    if(parts.size() < 3)
        return;
    UComponentGuiContext context;
    context.componentClassName = parts[0];
    context.componentLongName = parts[1];
    context.channelIndex = parts[2].toInt();

    auto* drag = new QDrag(m_tabWidget->tabBar());
    auto* mime = new QMimeData();
    mime->setData(UComponentGuiDndPayload::mimeType(),
                  UComponentGuiDndPayload::encode(context, m_hostId, index, -1));
    drag->setMimeData(mime);
    const Qt::DropAction result = drag->exec(Qt::MoveAction);
    Q_UNUSED(result);
}

int UComponentGuiTabHostWidget::tabIndexForContext(const UComponentGuiContext& context) const
{
    if(!m_tabWidget || !m_tabWidget->tabBar())
        return -1;
    const QString key = contextKey(context);
    for(int i = 0; i < m_tabWidget->count(); ++i)
    {
        const QVariant data = m_tabWidget->tabBar()->tabData(i);
        if(data.toString() == key)
            return i;
    }
    return -1;
}

QWidget* UComponentGuiTabHostWidget::hostWidgetForIndex(int index) const
{
    if(!m_tabWidget || index < 0 || index >= m_tabWidget->count())
        return nullptr;
    return m_tabWidget->widget(index);
}

void UComponentGuiTabHostWidget::syncCurrentTabState()
{
    if(!m_tabWidget || !m_tabWidget->tabBar())
        return;
    for(int i = 0; i < m_tabWidget->count(); ++i)
    {
        QWidget* host = m_tabWidget->widget(i);
        if(!host)
            continue;
        UVisualControllerWidget* widget = host->findChild<UVisualControllerWidget*>(QString(), Qt::FindDirectChildrenOnly);
        if(!widget)
            continue;
        UComponentGuiContext context;
        if(!m_service || !m_service->tryGetContextByWidget(widget, context))
            continue;
        const QString key = contextKey(context);
        m_tabWidget->tabBar()->setTabData(i, key);
        const QString title = !context.componentLongName.trimmed().isEmpty() ? context.componentLongName : context.componentClassName;
        m_tabWidget->setTabText(i, title);
    }
}

QString UComponentGuiTabHostWidget::contextKey(const UComponentGuiContext& context)
{
    return context.componentClassName + "|" + context.componentLongName + "|" + QString::number(context.channelIndex);
}
