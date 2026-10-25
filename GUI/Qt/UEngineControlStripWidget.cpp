#include "UEngineControlStripWidget.h"

#include "UGuiShellController.h"
#include "UGEngineControlWidget.h"
#include "UStatusPanel.h"

#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QScreen>
#include <QGuiApplication>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QShowEvent>
#include <QWidget>
#include <QWidgetAction>

UEngineControlStripWidget::UEngineControlStripWidget(UGuiShellController* shell,
                                                     RDK::UApplication* app,
                                                     QWidget* parent)
    : QMainWindow(parent)
    , m_shell(shell)
    , m_application(app)
{
    setObjectName(QStringLiteral("UEngineControlStripWidget"));
    setWindowTitle(tr("NeuroModeler — Control Bar"));
    setWindowFlags(Qt::Window
                   | Qt::CustomizeWindowHint
                   | Qt::WindowTitleHint
                   | Qt::WindowCloseButtonHint
                   | Qt::WindowMinimizeButtonHint);

    menuBar()->setNativeMenuBar(false);

    m_toolBar = addToolBar(tr("Main"));
    m_toolBar->setObjectName(QStringLiteral("ControlStripMainToolBar"));
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);

    auto* central = new QWidget(this);
    central->setFixedHeight(0);
    setCentralWidget(central);

    statusBar()->setSizeGripEnabled(false);
    m_statusPanel = new UStatusPanel(this, app);
    statusBar()->addWidget(m_statusPanel, 1);

    setMinimumWidth(640);
    setMaximumHeight(160);
    setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AnimatedDocks);
}

UEngineControlStripWidget::~UEngineControlStripWidget() = default;

void UEngineControlStripWidget::bindHost(UGEngineControlWidget* host)
{
    m_host = host;
    if(host && !host->windowTitle().isEmpty())
        setSessionWindowTitle(host->windowTitle());
    rebuildMenusFromHost();
    rebuildToolBarFromHost();
}

void UEngineControlStripWidget::setSessionWindowTitle(const QString& sessionTitle)
{
    if(sessionTitle.isEmpty())
        setWindowTitle(tr("NeuroModeler — Control Bar"));
    else
        setWindowTitle(tr("%1 — Control Bar").arg(sessionTitle));
}

void UEngineControlStripWidget::mirrorMenu(QMenu* src, QMenu* dst)
{
    if(!src || !dst)
        return;
    dst->clear();
    for(QAction* action : src->actions())
    {
        if(!action)
            continue;
        if(action->isSeparator())
        {
            dst->addSeparator();
            continue;
        }
        if(QMenu* sub = action->menu())
        {
            QMenu* mirrored = dst->addMenu(sub->title());
            mirrorMenu(sub, mirrored);
            continue;
        }
        dst->addAction(action);
    }
}

void UEngineControlStripWidget::rebuildMenusFromHost()
{
    if(!m_host)
        return;
    QMenuBar* srcBar = m_host->menuBar();
    QMenuBar* dstBar = menuBar();
    if(!srcBar || !dstBar)
        return;

    dstBar->clear();
    for(QAction* action : srcBar->actions())
    {
        if(!action)
            continue;
        if(QMenu* srcMenu = action->menu())
        {
            QMenu* dstMenu = dstBar->addMenu(srcMenu->title());
            mirrorMenu(srcMenu, dstMenu);
        }
        else
        {
            dstBar->addAction(action);
        }
    }
}

void UEngineControlStripWidget::rebuildToolBarFromHost()
{
    if(!m_toolBar || !m_host)
        return;
    QToolBar* src = m_host->primaryToolBar();
    m_toolBar->clear();
    if(!src)
        return;
    for(QAction* action : src->actions())
    {
        if(!action)
            continue;
        if(action->isSeparator())
        {
            m_toolBar->addSeparator();
            continue;
        }
        if(action->objectName() == QStringLiteral("maxCalcTimeAction"))
        {
            // Do not share QWidgetAction default widget across two toolbars.
            auto* twin = new QWidgetAction(m_toolBar);
            twin->setObjectName(QStringLiteral("maxCalcTimeAction"));
            twin->setDefaultWidget(m_host->createMaxCalcTimeWidget(m_toolBar));
            m_toolBar->addAction(twin);
            continue;
        }
        m_toolBar->addAction(action);
    }
    m_host->syncMaxCalcTimeFromProject();
}

void UEngineControlStripWidget::setAlwaysOnTop(bool on)
{
    m_alwaysOnTop = on;
    const bool wasVisible = isVisible();
    Qt::WindowFlags flags = windowFlags();
    if(on)
        flags |= Qt::WindowStaysOnTopHint;
    else
        flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);
    if(wasVisible)
        show();
}

void UEngineControlStripWidget::setLoggerDockExpanded(bool expanded)
{
    if(m_loggerDockExpanded == expanded && expanded)
    {
        setMinimumHeight(280);
        setMaximumHeight(QWIDGETSIZE_MAX);
        return;
    }
    if(m_loggerDockExpanded == expanded)
        return;

    m_loggerDockExpanded = expanded;
    if(expanded)
    {
        m_heightApplied = false;
        setMinimumHeight(280);
        setMaximumHeight(QWIDGETSIZE_MAX);
        if(height() < 320)
            resize(width(), 360);
    }
    else
    {
        setMinimumHeight(0);
        setMaximumHeight(160);
        m_heightApplied = false;
        applyNarrowHeight();
    }
}

void UEngineControlStripWidget::saveGeometrySettings()
{
    QSettings settings(QStringLiteral("NeuroModeler"), QStringLiteral("NeuroModeler"));
    settings.setValue(QStringLiteral("GuiShell/StripGeometry"), saveGeometry());
}

void UEngineControlStripWidget::restoreGeometrySettings()
{
    QSettings settings(QStringLiteral("NeuroModeler"), QStringLiteral("NeuroModeler"));
    const QByteArray geo = settings.value(QStringLiteral("GuiShell/StripGeometry")).toByteArray();
    if(!geo.isEmpty())
        restoreGeometry(geo);
    else if(QScreen* screen = QGuiApplication::primaryScreen())
    {
        const QRect avail = screen->availableGeometry();
        const int w = qMax(800, avail.width() * 3 / 5);
        resize(w, 100);
        move(avail.left() + (avail.width() - w) / 2, avail.top() + 8);
    }
}

void UEngineControlStripWidget::applyNarrowHeight()
{
    if(m_loggerDockExpanded)
        return;
    if(m_heightApplied)
        return;
    const int h = sizeHint().height();
    if(h > 0)
    {
        setFixedHeight(qBound(60, h, 160));
        m_heightApplied = true;
    }
}

void UEngineControlStripWidget::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    if(!m_loggerDockExpanded)
        applyNarrowHeight();
}

void UEngineControlStripWidget::closeEvent(QCloseEvent* event)
{
    if(m_shell && m_shell->isExiting())
    {
        event->accept();
        return;
    }
    if(m_shell && m_shell->preset() == GuiShellPreset::Studio)
    {
        event->ignore();
        m_shell->setStripVisible(false);
        return;
    }
    if(m_shell)
        m_shell->requestApplicationExit();
    event->accept();
}
