#ifndef UWATCHWIDGETFORM_H
#define UWATCHWIDGETFORM_H

#include <QWidget>
#include <QMenu>
#include "UVisualControllerWidget.h"

namespace Ui {
class UWatchFormWidget;
}

class UWatchFormWidget : public UVisualControllerWidget
{
 Q_OBJECT

public:
 explicit UWatchFormWidget(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
 ~UWatchFormWidget();

public slots:
        void actionWatch_Settings();
        void actionAdd_Watch();

private slots:
        void on_graphicsView_customContextMenuRequested(const QPoint &pos);

private:
 Ui::UWatchFormWidget *ui;
 QMenu *contextMenu;
};

#endif // UWATCHWIDGETFORM_H
