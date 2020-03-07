#include "UWatchFormWidget.h"
#include "ui_UWatchFormWidget.h"
#include "UWatchSettingsDialog.h"

UWatchFormWidget::UWatchFormWidget(QWidget *parent,RDK::UApplication* app) :
    UVisualControllerWidget(parent,app),
    ui(new Ui::UWatchFormWidget)
{
 ui->setupUi(this);

 contextMenu = new QMenu(this);


 contextMenu->addAction(ui->actionWatch_Settings);
 connect(ui->actionWatch_Settings, SIGNAL(triggered()), this, SLOT(actionWatch_Settings()));


}

UWatchFormWidget::~UWatchFormWidget()
{
 delete contextMenu;
 delete ui;
}

void UWatchFormWidget::actionWatch_Settings()
{
 UWatchSettingsDialog dialog(this);
 dialog.exec();
}

void UWatchFormWidget::on_graphicsView_customContextMenuRequested(const QPoint &pos)
{
 contextMenu->popup(QWidget::mapToGlobal(pos)); //QWidget::mapToGlobal(event->pos()));
}
