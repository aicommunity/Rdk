#include "UCalculationChannelsWidget.h"
#include "ui_UCalculationChannelsWidget.h"

UCalculationChannelsWidget::UCalculationChannelsWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UCalculationChannelsWidget)
{
  ui->setupUi(this);
  UpdateInterval = 0;
  currentChannel = 0;
  CheckModelFlag =false;
  setAccessibleName("UCalculationChannelsWidget");

  //contextMenu
  ui->treeWidgetChannels->addAction(ui->actionAddChannel);
  ui->treeWidgetChannels->addAction(ui->actionInsertChannel);
  ui->treeWidgetChannels->addAction(ui->actionDeleteSelectedChannel);
  ui->treeWidgetChannels->addAction(ui->actionCloneChannel);
  ui->treeWidgetChannels->addAction(ui->actionStartChannel);
  ui->treeWidgetChannels->addAction(ui->actionPauseChannel);
  ui->treeWidgetChannels->addAction(ui->actionResetChannel);
  connect(ui->actionAddChannel, SIGNAL(triggered(bool)), this, SLOT(actionAddChannel()));
  connect(ui->actionInsertChannel, SIGNAL(triggered(bool)), this, SLOT(actionInsertChannel()));
  connect(ui->actionDeleteSelectedChannel, SIGNAL(triggered(bool)), this, SLOT(actionDeleteSelectedChannel()));
  connect(ui->actionCloneChannel, SIGNAL(triggered(bool)), this, SLOT(actionCloneChannel()));
  connect(ui->actionStartChannel, SIGNAL(triggered(bool)), this, SLOT(actionStartChannel()));
  connect(ui->actionPauseChannel, SIGNAL(triggered(bool)), this, SLOT(actionPauseChannel()));
  connect(ui->actionResetChannel, SIGNAL(triggered(bool)), this, SLOT(actionResetChannel()));

  connect(ui->treeWidgetChannels, SIGNAL(itemSelectionChanged()), this, SLOT(channelSelectionChanged()));

  //connect(ui->action, SIGNAL(triggered(bool)), this, SLOT(action));

  UpdateInterface(true);
}

UCalculationChannelsWidget::~UCalculationChannelsWidget()
{
    delete ui;
}

void UCalculationChannelsWidget::AUpdateInterface()
{
  ui->treeWidgetChannels->clear();
  channelsCounter = Core_GetNumChannels();
  for(int i = 0; i < channelsCounter; i++)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetChannels);
    item->setText(0, QString::number(i) + " ch.");
    item->setData(0, Qt::UserRole, i);
    if(i == currentChannel) ui->treeWidgetChannels->setCurrentItem(item);
  }
}

void UCalculationChannelsWidget::channelSelectionChanged()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  QTreeWidgetItem *item = ui->treeWidgetChannels->currentItem();
  if(!item) return;

  int selectedChannel = item->data(0, Qt::UserRole).toInt();
  if(selectedChannel == currentChannel) return;
  currentChannel = item->data(0, Qt::UserRole).toInt();
  Core_SelectChannel(currentChannel);
  RDK::UIVisualControllerStorage::UpdateInterface(true);
}

void UCalculationChannelsWidget::actionAddChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  application->SetNumChannels(++channelsCounter);
  UpdateInterface(true);
}

void UCalculationChannelsWidget::actionInsertChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  QTreeWidgetItem *item = ui->treeWidgetChannels->currentItem();
  if(!item) return;

  application->InsertChannel(item->data(0, Qt::UserRole).toInt());
  UpdateInterface(true);
}

void UCalculationChannelsWidget::actionDeleteSelectedChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  QTreeWidgetItem *item = ui->treeWidgetChannels->currentItem();
  if(!item)
      return;

  application->DeleteChannel(item->data(0, Qt::UserRole).toInt());
  UpdateInterface(true);
}

void UCalculationChannelsWidget::actionStartChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  application->StartChannel(currentChannel);
}

void UCalculationChannelsWidget::actionPauseChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  application->PauseChannel(currentChannel);
}

void UCalculationChannelsWidget::actionResetChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  application->ResetChannel(currentChannel);
}

void UCalculationChannelsWidget::actionCloneChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  int cloned_id=Core_GetNumChannels();
  application->CloneChannel(currentChannel, cloned_id);
  UpdateInterface(true);
}
