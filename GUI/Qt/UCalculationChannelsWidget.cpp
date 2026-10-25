#include "UCalculationChannelsWidget.h"
#include "ui_UCalculationChannelsWidget.h"

#include <rdk_init.h>
#include <QMessageBox>
#include <QKeySequence>
#include <QSignalBlocker>
#include <QToolBar>
#include <QVBoxLayout>

UCalculationChannelsWidget::UCalculationChannelsWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UCalculationChannelsWidget)
{
  ui->setupUi(this);
  UpdateInterval = 0;
  currentChannel = 0;
  CheckModelFlag =false;
  setAccessibleName("UCalculationChannelsWidget");

  // Persistent compact toolbar (actions also remain in context menu).
  if (auto* root = qobject_cast<QVBoxLayout*>(layout()))
  {
      auto* bar = new QToolBar(tr("Channels"), this);
      bar->setIconSize(QSize(16, 16));
      bar->addAction(ui->actionAddChannel);
      bar->addAction(ui->actionInsertChannel);
      bar->addAction(ui->actionDeleteSelectedChannel);
      bar->addAction(ui->actionCloneChannel);
      bar->addSeparator();
      bar->addAction(ui->actionStartChannel);
      bar->addAction(ui->actionPauseChannel);
      bar->addAction(ui->actionResetChannel);
      root->insertWidget(0, bar);
  }

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
  ui->actionStartChannel->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+S")));
  ui->actionStartChannel->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  ui->actionPauseChannel->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+P")));
  ui->actionPauseChannel->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  ui->actionStartChannel->setObjectName(QStringLiteral("startSelectedChannelAction"));
  ui->actionPauseChannel->setObjectName(QStringLiteral("pauseSelectedChannelAction"));

  connect(ui->listWidgetChannels, SIGNAL(itemSelectionChanged()), this, SLOT(channelSelectionChanged()));

  UpdateInterface(true);
}

UCalculationChannelsWidget::~UCalculationChannelsWidget()
{
    delete ui;
}

void UCalculationChannelsWidget::AUpdateInterface()
{
  const int newCount = qMax(0, Core_GetNumChannels());
  const QSignalBlocker blocker(ui->listWidgetChannels);
  ui->listWidgetChannels->setUpdatesEnabled(false);

  while(ui->listWidgetChannels->count() > newCount)
      delete ui->listWidgetChannels->takeItem(ui->listWidgetChannels->count() - 1);
  while(ui->listWidgetChannels->count() < newCount)
      new QListWidgetItem(ui->listWidgetChannels);

  channelsCounter = newCount;
  if(channelsCounter > 0)
      currentChannel = qBound(0, currentChannel, channelsCounter - 1);
  else
      currentChannel = 0;

  for(int i = 0; i < channelsCounter; i++)
  {
    QListWidgetItem *item = ui->listWidgetChannels->item(i);
    const bool active = (i == currentChannel);
    item->setText(active ? tr("Ch %1 ●").arg(i) : tr("Ch %1").arg(i));
    item->setData(Qt::UserRole, i);
    item->setToolTip(tr("Calculation channel %1").arg(i));
  }
  ui->listWidgetChannels->setCurrentRow(channelsCounter > 0 ? currentChannel : -1);
  ui->listWidgetChannels->setUpdatesEnabled(true);
  ui->listWidgetChannels->viewport()->update();
  emit updateVisibility();
}

void UCalculationChannelsWidget::setLlmActiveChannel(int channel_index)
{
    if(!application || channel_index < 0)
        return;
    Core_SelectChannel(channel_index);
    currentChannel = channel_index;
    for(int i = 0; i < ui->listWidgetChannels->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidgetChannels->item(i);
        if(!item)
            continue;
        if(item->data(Qt::UserRole).toInt() == channel_index)
        {
            ui->listWidgetChannels->setCurrentItem(item);
            break;
        }
    }
    RDK::UIVisualControllerStorage::UpdateInterface(true);
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

  const int ch = item->data(Qt::UserRole).toInt();
  if (QMessageBox::question(this, tr("Delete channel"),
                            tr("Delete channel %1?").arg(ch))
      != QMessageBox::Yes)
      return;

  application->DeleteChannel(ch);
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
