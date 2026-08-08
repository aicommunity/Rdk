#ifndef UGUISHELLCONTROLLER_H
#define UGUISHELLCONTROLLER_H

#include "UGuiShellTypes.h"

#include <QObject>

class QAction;
class QActionGroup;
class UGEngineControlWidget;
class UEngineControlStripWidget;

/// Owns Studio / Control Bar preset policy, strip visibility, and application exit.
class UGuiShellController : public QObject
{
    Q_OBJECT
public:
    explicit UGuiShellController(UGEngineControlWidget* host, QObject* parent = nullptr);

    void setStrip(UEngineControlStripWidget* strip);
    UEngineControlStripWidget* strip() const { return m_strip; }
    UGEngineControlWidget* host() const { return m_host; }

    GuiShellPreset preset() const { return m_preset; }
    void setPreset(GuiShellPreset preset);
    void applyPreset();

    bool isStripVisible() const;
    void setStripVisible(bool visible);
    void setStripAlwaysOnTop(bool on);
    bool stripAlwaysOnTop() const { return m_stripAlwaysOnTop; }

    void setHostChromeVisible(bool visible);
    void showWorkspace();
    void hideWorkspace();
    bool isWorkspaceVisible() const;

    void requestApplicationExit();
    bool isExiting() const { return m_exiting; }

    void loadSettings(const QString& iniPresetDefault);
    void saveSettings();

    void installWindowMenuActions();
    void notifyHostMenusChanged();
    void notifyWorkspaceVisibilityChanged();

    /// Apply preset + HideAdminForm / StartMinimized after widgets are bound.
    void applyStartupVisibility(bool hideAdminForm, bool startMinimized);

signals:
    void presetChanged(GuiShellPreset preset);
    void stripVisibleChanged(bool visible);
    void stripAlwaysOnTopChanged(bool on);
    void workspaceVisibleChanged(bool visible);

private slots:
    void onShellStudioTriggered(bool checked);
    void onShellControlBarTriggered(bool checked);
    void onShowControlBarToggled(bool checked);
    void onAlwaysOnTopToggled(bool checked);
    void onShowWorkspaceTriggered();
    void rebuildStripMenusDebounced();

private:
    void syncShellActionStates();
    void persistPresetOverride();

    UGEngineControlWidget* m_host = nullptr;
    UEngineControlStripWidget* m_strip = nullptr;

    GuiShellPreset m_preset = GuiShellPreset::Studio;
    bool m_stripVisibleUser = false;
    bool m_stripAlwaysOnTop = false;
    bool m_exiting = false;
    bool m_menuRebuildScheduled = false;
    bool m_suppressActionSync = false;

    QAction* m_actionStudio = nullptr;
    QAction* m_actionControlBar = nullptr;
    QAction* m_actionShowControlBar = nullptr;
    QAction* m_actionAlwaysOnTop = nullptr;
    QAction* m_actionShowWorkspace = nullptr;
    QActionGroup* m_presetGroup = nullptr;
};

#endif // UGUISHELLCONTROLLER_H
