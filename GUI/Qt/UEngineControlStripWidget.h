#ifndef UENGINECONTROLSTRIPWIDGET_H
#define UENGINECONTROLSTRIPWIDGET_H

#include <QMainWindow>

namespace RDK {
class UApplication;
}

class UGuiShellController;
class UGEngineControlWidget;
class UStatusPanel;
class QToolBar;
class QCloseEvent;
class QMenu;
class QShowEvent;

/// Narrow top-level chrome mirror: menu + toolbar + status, shared QActions with host.
class UEngineControlStripWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit UEngineControlStripWidget(UGuiShellController* shell,
                                       RDK::UApplication* app,
                                       QWidget* parent = nullptr);
    ~UEngineControlStripWidget() override;

    void bindHost(UGEngineControlWidget* host);
    void rebuildMenusFromHost();
    void rebuildToolBarFromHost();

    /// Session caption (program/user/config) shown as "%1 — Control Bar".
    void setSessionWindowTitle(const QString& sessionTitle);

    void setAlwaysOnTop(bool on);
    bool alwaysOnTop() const { return m_alwaysOnTop; }

    /// Expand strip so a Bottom-docked logger fits; collapse back to chrome-only height.
    void setLoggerDockExpanded(bool expanded);
    bool loggerDockExpanded() const { return m_loggerDockExpanded; }

    void saveGeometrySettings();
    void restoreGeometrySettings();

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void mirrorMenu(QMenu* src, QMenu* dst);
    void applyNarrowHeight();

    UGuiShellController* m_shell = nullptr;
    UGEngineControlWidget* m_host = nullptr;
    RDK::UApplication* m_application = nullptr;
    QToolBar* m_toolBar = nullptr;
    UStatusPanel* m_statusPanel = nullptr;
    bool m_alwaysOnTop = false;
    bool m_heightApplied = false;
    bool m_loggerDockExpanded = false;
};

#endif // UENGINECONTROLSTRIPWIDGET_H
