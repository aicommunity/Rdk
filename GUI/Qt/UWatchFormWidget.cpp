 #include "UWatchFormWidget.h"
#include "ui_UWatchFormWidget.h"
#include "UWatchSettingsDialog.h"
#include "UComponentPropertySelectionWidget.h"

UWatchFormWidget::UWatchFormWidget(QWidget *parent,RDK::UApplication* app) :
    UVisualControllerWidget(parent,app),
    ui(new Ui::UWatchFormWidget)
{
 ui->setupUi(this);

 contextMenu = new QMenu(this);


 contextMenu->addAction(ui->actionAdd_Watch);
 connect(ui->actionAdd_Watch, SIGNAL(triggered()), this, SLOT(actionAdd_Watch()));
 contextMenu->addAction(ui->actionWatch_Settings);
 connect(ui->actionWatch_Settings, SIGNAL(triggered()), this, SLOT(actionWatch_Settings()));


}

UWatchFormWidget::~UWatchFormWidget()
{
 delete contextMenu;
 delete ui;
}

void UWatchFormWidget::actionAdd_Watch()
{
     UComponentPropertySelectionWidget dialog(this, 3, application);
     dialog.componentsList->setChannelsListVisible(false);

     if (dialog.exec())
     {
//         selectedImage->setComponentName(dialog.componentsList->getSelectedComponentLongName());
//         selectedImage->setComponentPropertyName(dialog.componentsList->getSelectedPropertyName());
//         selectedImage->setCalcChannel(dialog.componentsList->getSelectedChannelIndex());
     }
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
