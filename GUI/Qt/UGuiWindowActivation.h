#ifndef UGUIWINDOWACTIVATION_H
#define UGUIWINDOWACTIVATION_H

#include <QDockWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QWidget>

/// Shared helpers so Window-menu / MDI activation always brings UI to the front.
namespace UGuiWindowActivation {

inline void raiseAndActivateWidget(QWidget* w)
{
    if(!w)
        return;
    w->show();
    w->raise();
    w->activateWindow();
}

inline void raiseAndActivateMdiSubWindow(QMdiArea* area, QMdiSubWindow* sub)
{
    if(!sub)
        return;
    sub->show();
    if(area)
        area->setActiveSubWindow(sub);
    sub->raise();
    if(QWidget* top = sub->window())
    {
        top->raise();
        top->activateWindow();
    }
}

inline void raiseAndActivateDock(QDockWidget* dock)
{
    if(!dock)
        return;
    dock->show();
    dock->raise();
    if(dock->isFloating())
        dock->activateWindow();
    else if(QWidget* top = dock->window())
    {
        top->raise();
        top->activateWindow();
    }
}

} // namespace UGuiWindowActivation

#endif // UGUIWINDOWACTIVATION_H
