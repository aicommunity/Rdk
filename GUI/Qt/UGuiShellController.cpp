#include "UGuiShellController.h"

#include "UGEngineControlWidget.h"
#include "UEngineControlStripWidget.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QMenu>
#include <QSettings>
#include <QTimer>

UGuiShellController::UGuiShellController(UGEngineControlWidget* host, QObject* parent)
    : QObject(parent)
    , m_host(host)
{
}

void UGuiShellController::setStrip(UEngineControlStripWidget* strip)
{
    m_strip = strip;
}

void UGuiShellController::setPreset(GuiShellPreset preset)
{
    if(m_preset == preset)
    {
        applyPreset();
        return;
    }
    m_preset = preset;
    if(m_preset == GuiShellPreset::ControlBar)
        m_stripVisibleUser = true;
    else
        m_stripVisibleUser = false;

    applyPreset();
    persistPresetOverride();
    emit presetChanged(m_preset);
    syncShellActionStates();
}

void UGuiShellController::applyPreset()
{
    if(!m_host)
        return;

    if(m_preset == GuiShellPreset::ControlBar)
    {
        setHostChromeVisible(false);
        setStripVisible(true);
        if(!isWorkspaceVisible())
            showWorkspace();
    }
    else
    {
        setHostChromeVisible(true);
        setStripVisible(m_stripVisibleUser);
        if(!isWorkspaceVisible())
            showWorkspace();
    }
    m_host->ensureLoggerForShellPreset();
    m_host->AUpdateInterface();
    syncShellActionStates();
}

bool UGuiShellController::isStripVisible() const
{
    return m_strip && m_strip->isVisible();
}

void UGuiShellController::setStripVisible(bool visible)
{
    if(m_preset == GuiShellPreset::ControlBar)
        visible = true;

    m_stripVisibleUser = visible;
    if(!m_strip)
        return;

    if(visible)
    {
        if(m_host)
            m_strip->rebuildMenusFromHost();
        m_strip->setAlwaysOnTop(m_stripAlwaysOnTop);
        m_strip->show();
        m_strip->raise();
    }
    else
    {
        m_strip->hide();
        if(m_preset == GuiShellPreset::Studio)
            setHostChromeVisible(true);
    }

    emit stripVisibleChanged(isStripVisible());
    syncShellActionStates();
    saveSettings();
}

void UGuiShellController::setStripAlwaysOnTop(bool on)
{
    m_stripAlwaysOnTop = on;
    if(m_strip)
        m_strip->setAlwaysOnTop(on);
    emit stripAlwaysOnTopChanged(on);
    syncShellActionStates();
    saveSettings();
}

void UGuiShellController::setHostChromeVisible(bool visible)
{
    if(m_host)
        m_host->setHostChromeVisible(visible);
}

void UGuiShellController::showWorkspace()
{
    if(!m_host)
        return;
    m_host->show();
    m_host->raise();
    emit workspaceVisibleChanged(true);
    syncShellActionStates();
}

void UGuiShellController::hideWorkspace()
{
    if(!m_host)
        return;
    m_host->hide();
    emit workspaceVisibleChanged(false);
    syncShellActionStates();
}

bool UGuiShellController::isWorkspaceVisible() const
{
    return m_host && m_host->isVisible();
}

void UGuiShellController::requestApplicationExit()
{
    if(m_exiting)
        return;
    m_exiting = true;

    if(m_strip)
        m_strip->saveGeometrySettings();
    saveSettings();

    if(m_host)
        m_host->performSessionTeardown();

    if(m_strip)
        m_strip->hide();
    if(m_host)
        m_host->hide();

    QApplication::quit();
}

void UGuiShellController::loadSettings(const QString& iniPresetDefault)
{
    QSettings settings(QStringLiteral("NeuroModeler"), QStringLiteral("NeuroModeler"));
    const QString overridePreset = settings.value(QStringLiteral("GuiShell/PresetOverride")).toString();
    if(!overridePreset.isEmpty())
        m_preset = guiShellPresetFromString(overridePreset);
    else
        m_preset = guiShellPresetFromString(iniPresetDefault);

    m_stripVisibleUser = settings.value(QStringLiteral("GuiShell/StripVisible"), false).toBool();
    m_stripAlwaysOnTop = settings.value(QStringLiteral("GuiShell/StripAlwaysOnTop"), false).toBool();

    if(m_preset == GuiShellPreset::ControlBar)
        m_stripVisibleUser = true;

    if(m_strip)
    {
        m_strip->restoreGeometrySettings();
        m_strip->setAlwaysOnTop(m_stripAlwaysOnTop);
    }
}

void UGuiShellController::saveSettings()
{
    QSettings settings(QStringLiteral("NeuroModeler"), QStringLiteral("NeuroModeler"));
    settings.setValue(QStringLiteral("GuiShell/StripVisible"), m_stripVisibleUser);
    settings.setValue(QStringLiteral("GuiShell/StripAlwaysOnTop"), m_stripAlwaysOnTop);
    if(m_strip)
        m_strip->saveGeometrySettings();
}

void UGuiShellController::persistPresetOverride()
{
    QSettings settings(QStringLiteral("NeuroModeler"), QStringLiteral("NeuroModeler"));
    settings.setValue(QStringLiteral("GuiShell/PresetOverride"), toString(m_preset));
}

void UGuiShellController::installWindowMenuActions()
{
    if(!m_host)
        return;
    QMenu* windowMenu = m_host->windowMenu();
    if(!windowMenu)
        return;

    windowMenu->addSeparator();

    QMenu* layoutMenu = windowMenu->addMenu(tr("Layout"));

    m_presetGroup = new QActionGroup(m_host);
    m_presetGroup->setExclusive(true);

    m_actionStudio = layoutMenu->addAction(tr("Studio"));
    m_actionStudio->setCheckable(true);
    m_presetGroup->addAction(m_actionStudio);

    m_actionControlBar = layoutMenu->addAction(tr("Control Bar"));
    m_actionControlBar->setCheckable(true);
    m_presetGroup->addAction(m_actionControlBar);

    m_actionShowControlBar = layoutMenu->addAction(tr("Show Control Bar"));
    m_actionShowControlBar->setCheckable(true);

    m_actionShowWorkspace = layoutMenu->addAction(tr("Show Workspace"));

    layoutMenu->addSeparator();

    m_actionAlwaysOnTop = layoutMenu->addAction(tr("Control Bar Always on Top"));
    m_actionAlwaysOnTop->setCheckable(true);

    connect(m_actionStudio, &QAction::triggered, this, &UGuiShellController::onShellStudioTriggered);
    connect(m_actionControlBar, &QAction::triggered, this, &UGuiShellController::onShellControlBarTriggered);
    connect(m_actionShowControlBar, &QAction::toggled, this, &UGuiShellController::onShowControlBarToggled);
    connect(m_actionAlwaysOnTop, &QAction::toggled, this, &UGuiShellController::onAlwaysOnTopToggled);
    connect(m_actionShowWorkspace, &QAction::triggered, this, &UGuiShellController::onShowWorkspaceTriggered);

    syncShellActionStates();
    notifyHostMenusChanged();
}

void UGuiShellController::notifyHostMenusChanged()
{
    if(m_menuRebuildScheduled)
        return;
    m_menuRebuildScheduled = true;
    QTimer::singleShot(0, this, &UGuiShellController::rebuildStripMenusDebounced);
}

void UGuiShellController::notifyWorkspaceVisibilityChanged()
{
    emit workspaceVisibleChanged(isWorkspaceVisible());
    syncShellActionStates();
}

void UGuiShellController::rebuildStripMenusDebounced()
{
    m_menuRebuildScheduled = false;
    if(m_strip && m_strip->isVisible())
        m_strip->rebuildMenusFromHost();
}

void UGuiShellController::applyStartupVisibility(bool hideAdminForm, bool startMinimized)
{
    applyPreset();

    if(hideAdminForm)
        hideWorkspace();
    else if(startMinimized)
    {
        if(m_host)
            m_host->showMinimized();
        emit workspaceVisibleChanged(m_host && m_host->isVisible());
    }
    else
        showWorkspace();

    if(m_preset == GuiShellPreset::ControlBar || m_stripVisibleUser)
        setStripVisible(true);

    if(m_host)
        m_host->ensureLoggerForShellPreset();

    syncShellActionStates();
}

void UGuiShellController::onShellStudioTriggered(bool checked)
{
    if(!checked || m_suppressActionSync)
        return;
    setPreset(GuiShellPreset::Studio);
}

void UGuiShellController::onShellControlBarTriggered(bool checked)
{
    if(!checked || m_suppressActionSync)
        return;
    setPreset(GuiShellPreset::ControlBar);
}

void UGuiShellController::onShowControlBarToggled(bool checked)
{
    if(m_suppressActionSync)
        return;
    if(m_preset == GuiShellPreset::ControlBar)
    {
        syncShellActionStates();
        return;
    }
    setStripVisible(checked);
}

void UGuiShellController::onAlwaysOnTopToggled(bool checked)
{
    if(m_suppressActionSync)
        return;
    setStripAlwaysOnTop(checked);
}

void UGuiShellController::onShowWorkspaceTriggered()
{
    showWorkspace();
}

void UGuiShellController::syncShellActionStates()
{
    m_suppressActionSync = true;
    if(m_actionStudio)
        m_actionStudio->setChecked(m_preset == GuiShellPreset::Studio);
    if(m_actionControlBar)
        m_actionControlBar->setChecked(m_preset == GuiShellPreset::ControlBar);
    if(m_actionShowControlBar)
    {
        m_actionShowControlBar->setChecked(isStripVisible() || m_stripVisibleUser
                                           || m_preset == GuiShellPreset::ControlBar);
        m_actionShowControlBar->setEnabled(m_preset == GuiShellPreset::Studio);
    }
    if(m_actionAlwaysOnTop)
        m_actionAlwaysOnTop->setChecked(m_stripAlwaysOnTop);
    if(m_actionShowWorkspace)
        m_actionShowWorkspace->setEnabled(m_host && !m_host->isVisible());
    m_suppressActionSync = false;
}
