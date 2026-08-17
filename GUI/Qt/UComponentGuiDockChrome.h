#ifndef UCOMPONENTGUIDOCKCHROME_H
#define UCOMPONENTGUIDOCKCHROME_H

#include <QByteArray>
#include <QDockWidget>
#include <QObject>

/** Native sizable chrome (min/max/close + thick frame) when a component GUI dock floats. */
inline void installComponentGuiFloatingChrome(QDockWidget* dock)
{
    if (!dock)
        return;
    QObject::connect(dock, &QDockWidget::topLevelChanged, dock, [dock](bool floating) {
        if (!floating)
            return;
        const QByteArray geom = dock->saveGeometry();
        const Qt::WindowFlags flags = Qt::Window | Qt::WindowTitleHint
            | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint
            | Qt::WindowCloseButtonHint;
        if (dock->windowFlags() != flags) {
            dock->setWindowFlags(flags);
            dock->setStyleSheet(QString());
            dock->restoreGeometry(geom);
            dock->show();
        }
    });
}

#endif
