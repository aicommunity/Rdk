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
  QAction *actionSeparator1 = new QAction(this);
  actionSeparator1->setSeparator(true);

  ui->listWidgetChannels->addAction(ui->actionAddChannel);
  ui->listWidgetChannels->addAction(ui->actionInsertChannel);
  ui->listWidgetChannels->addAction(ui->actionDeleteSelectedChannel);
  ui->listWidgetChannels->addAction(ui->actionCloneChannel);
  ui->listWidgetChannels->addAction(actionSeparator1);
  ui->listWidgetChannels->addAction(ui->actionStartChannel);
  ui->listWidgetChannels->addAction(ui->actionPauseChannel);
  ui->listWidgetChannels->addAction(ui->actionResetChannel);
  connect(ui->actionAddChannel, SIGNAL(triggered(bool)), this, SLOT(actionAddChannel()));
  connect(ui->actionInsertChannel, SIGNAL(triggered(bool)), this, SLOT(actionInsertChannel()));
  connect(ui->actionDeleteSelectedChannel, SIGNAL(triggered(bool)), this, SLOT(actionDeleteSelectedChannel()));
  connect(ui->actionCloneChannel, SIGNAL(triggered(bool)), this, SLOT(actionCloneChannel()));
  connect(ui->actionStartChannel, SIGNAL(triggered(bool)), this, SLOT(actionStartChannel()));
  connect(ui->actionPauseChannel, SIGNAL(triggered(bool)), this, SLOT(actionPauseChannel()));
  connect(ui->actionResetChannel, SIGNAL(triggered(bool)), this, SLOT(actionResetChannel()));

  connect(ui->listWidgetChannels, SIGNAL(itemSelectionChanged()), this, SLOT(channelSelectionChanged()));

  //connect(ui->action, SIGNAL(triggered(bool)), this, SLOT(action));

  UpdateInterface(true);
}

UCalculationChannelsWidget::~UCalculationChannelsWidget()
{
    delete ui;
}

void UCalculationChannelsWidget::AUpdateInterface()
{
  ui->listWidgetChannels->clear();
  channelsCounter = Core_GetNumChannels();
  for(int i = 0; i < channelsCounter; i++)
  {
    QListWidgetItem *item = new QListWidgetItem(ui->listWidgetChannels);
    item->setText(QString::number(i));
//    item->setText(0, QString::number(i) + " ch.");
    item->setData(Qt::UserRole, i);
    if(i == currentChannel) ui->listWidgetChannels->setCurrentItem(item);
  }
  emit updateVisibility();
}

void UCalculationChannelsWidget::channelSelectionChanged()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  QListWidgetItem *item = ui->listWidgetChannels->currentItem();
  if(!item) return;

  int selectedChannel = item->data(Qt::UserRole).toInt();
  if(selectedChannel == currentChannel) return;
  currentChannel = item->data(Qt::UserRole).toInt();
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

  QListWidgetItem *item = ui->listWidgetChannels->currentItem();
  if(!item) return;

  application->InsertChannel(item->data(Qt::UserRole).toInt());
  UpdateInterface(true);
}

void UCalculationChannelsWidget::actionDeleteSelectedChannel()
{
  if(!application) return;
  if(!application->GetProjectOpenFlag()) return;

  QListWidgetItem *item = ui->listWidgetChannels->currentItem();
  if(!item)
      return;

  application->DeleteChannel(item->data(Qt::UserRole).toInt());
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
