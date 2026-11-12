#ifndef UCOMPONENTGUIDOCKCHROME_H
#define UCOMPONENTGUIDOCKCHROME_H

#include <QByteArray>
#include <QDockWidget>
#include <QEvent>
#include <QTimer>
#include <QWidget>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

/** Native sizable chrome (min/max/close + thick frame) when a component GUI dock floats.
 *  QDockWidget overwrites WindowMaximizeButtonHint after setFloating; re-apply queued.
 *  Title-bar double-click maximizes instead of re-docking. */

inline void applyComponentGuiFloatingChrome(QDockWidget* dock)
{
    if (!dock || !dock->isFloating())
        return;

    dock->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    const Qt::WindowFlags flags = Qt::Window | Qt::WindowTitleHint
        | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint
        | Qt::WindowCloseButtonHint;
    const bool missingMax = !(dock->windowFlags() & Qt::WindowMaximizeButtonHint);
    const bool hasCustomize = dock->windowFlags() & Qt::CustomizeWindowHint;
    const bool isTool = dock->windowFlags() & Qt::Tool;

    if ((missingMax || hasCustomize || isTool) && !dock->property("hwChromeFlagsSet").toBool()) {
        dock->setProperty("hwChromeFlagsSet", true);
        const QByteArray geom = dock->saveGeometry();
        const bool wasMax = dock->isMaximized();
        dock->setWindowFlags(flags);
        dock->setStyleSheet(QString());
        dock->restoreGeometry(geom);
        if (wasMax)
            dock->showMaximized();
        else
            dock->show();
    }

#ifdef Q_OS_WIN
    const HWND hwnd = reinterpret_cast<HWND>(dock->winId());
    if (hwnd) {
        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
        style |= WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU;
        SetWindowLongPtr(hwnd, GWL_STYLE, style);
        LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        ex &= ~WS_EX_TOOLWINDOW;
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, ex);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
#endif
}

class UComponentGuiDockChromeFilter : public QObject
{
public:
    explicit UComponentGuiDockChromeFilter(QDockWidget* dock)
        : QObject(dock)
        , m_dock(dock)
    {
        dock->installEventFilter(this);
        QObject::connect(dock, &QDockWidget::topLevelChanged, dock, [this](bool floating) {
            if (!floating) {
                m_dock->setProperty("hwChromeFlagsSet", false);
                return;
            }
            QTimer::singleShot(0, m_dock, [this]() { applyComponentGuiFloatingChrome(m_dock); });
        });
    }

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (watched != m_dock)
            return QObject::eventFilter(watched, event);

        if (event->type() == QEvent::NonClientAreaMouseButtonDblClick && m_dock->isFloating()) {
            if (m_dock->isMaximized())
                m_dock->showNormal();
            else
                m_dock->showMaximized();
            return true;
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QDockWidget* m_dock = nullptr;
};

inline void installComponentGuiFloatingChrome(QDockWidget* dock)
{
    if (!dock)
        return;
    if (dock->property("componentGuiFloatingChrome").toBool())
        return;
    dock->setProperty("componentGuiFloatingChrome", true);
    new UComponentGuiDockChromeFilter(dock);
}

#endif
