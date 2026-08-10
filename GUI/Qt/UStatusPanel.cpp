#include "UStatusPanel.h"
#include "ui_UStatusPanel.h"
#include "UGuiModelSnapshot.h"

UStatusPanel::UStatusPanel(QWidget *parent, RDK::UApplication *app) :
  UVisualControllerWidget(parent, app),
  ui(new Ui::UStatusPanel)
{
  setAccessibleName("UStatusPanel");
  UpdateInterval = 200;

  ui->setupUi(this);

  // Скрыты в статусбаре; настройки по-прежнему из проекта / wizard
  ui->checkBoxDetailedLog->hide();
  ui->checkBoxAutosaveStates->hide();

  AAfterLoadProject();

  connect(ui->checkBoxShowDebugMessages, SIGNAL(toggled(bool)), this, SLOT(setShowDebugMessages(bool)));
  connect(ui->checkBoxDetailedLog, SIGNAL(toggled(bool)), this, SLOT(setDetailedLog(bool)));
  connect(ui->checkBoxAutosaveStates, SIGNAL(toggled(bool)), this, SLOT(setAutosaveStates(bool)));
  connect(ui->checkBoxAutosaveProject, SIGNAL(toggled(bool)), this, SLOT(setAutosaveProject(bool)));
}

UStatusPanel::~UStatusPanel()
{
  delete ui;
}

void UStatusPanel::AUpdateInterface()
{
  if(!application) return;
  if(!Model_Check()) return;

  int sel_index = Core_GetSelectedChannelIndex();
  RDK::UChannelProfiler *profiler = application->GetEngineControl()->GetChannelProfiler(sel_index);

  if(!profiler) return;

  int calcMod = application->GetEngineControl()->GetCalculateMode(sel_index);
  if(calcMod == 1)
  {
    ui->labelTime->setText(QString::fromLocal8Bit(profiler->CalcRtTimeText().c_str()));
    ui->labelStep->setText(QString::fromLocal8Bit(profiler->CalcRtPerfomanceText().c_str()));
  }
  else
  {
    ui->labelTime->setText(QString::fromLocal8Bit(profiler->CalcNormalTimeText().c_str()));
    ui->labelStep->setText(QString::fromLocal8Bit(profiler->CalcNormalStepDurationText().c_str()));

    if(calcMod != 1)
      ui->labelFps->setText(QString::fromLocal8Bit(profiler->CalcNormalFpsText().c_str()));
    else
      ui->labelFps->clear();
  }

  int num_objects = Storage_CalcNumObjects();

  // Use lock-free snapshot to get component count instead of blocking GetModelLock
  int num_model_components = 0;
  NMSDK::UGuiSnapshotPtr snapshot = NMSDK::UGuiModelSnapshot::Instance().CurrentSnapshot();
  if (snapshot) {
    num_model_components = snapshot->Components.size();
  }

  ui->labelObjects->setText("Objects: "+  QString::fromLocal8Bit(RDK::sntoa(num_model_components).c_str()) + "/" + QString::fromLocal8Bit(RDK::sntoa(num_objects).c_str()));
}

void UStatusPanel::AAfterLoadProject()
{
  if(!application) return;

  RDK::TProjectConfig config = application->GetProjectConfig();
  ui->checkBoxShowDebugMessages->setChecked(config.DebugMode);
  ui->checkBoxDetailedLog->setChecked(static_cast<bool>(config.DebugSysEventsMask));
  ui->checkBoxAutosaveStates->setChecked(config.ProjectAutoSaveStatesFlag);
  ui->checkBoxAutosaveProject->setChecked(config.ProjectAutoSaveFlag);
}

void UStatusPanel::setShowDebugMessages(bool checked)
{
  if(!application) return;

  int size = Core_GetNumChannels();

  RDK::TProjectConfig config = application->GetProjectConfig();

  if(static_cast<int>(config.ChannelsConfig.size()) != size)
  {
    ui->checkBoxShowDebugMessages->setChecked(false);
    return;
  }

  for(int i = 0; i < size; i++)
  {
    config.DebugMode = checked;
    MLog_SetDebugMode(i, config.DebugMode);
  }

  MLog_SetDebugMode(RDK_SYS_MESSAGE , config.DebugMode);
  MLog_SetDebugMode(RDK_GLOB_MESSAGE, config.DebugMode);

  application->SetProjectConfig(config);
}

void UStatusPanel::setDetailedLog(bool checked)
{
  if(!application) return;

  int size = Core_GetNumChannels();

  RDK::TProjectConfig config = application->GetProjectConfig();

  if(static_cast<int>(config.ChannelsConfig.size()) != size)
  {
    ui->checkBoxDetailedLog->setChecked(false);
    return;
  }

  for(int i = 0; i < size; i++)
  {
    if(checked)
     config.DebugSysEventsMask = 0xFFFFFFFF;
    else
     config.DebugSysEventsMask = 0;

    MLog_SetDebugSysEventsMask(i, config.DebugSysEventsMask);
  }

  MLog_SetDebugMode(RDK_SYS_MESSAGE , config.DebugSysEventsMask);
  MLog_SetDebugMode(RDK_GLOB_MESSAGE, config.DebugSysEventsMask);

  application->SetProjectConfig(config);
}

void UStatusPanel::setAutosaveStates(bool checked)
{
  if(!application) return;

  RDK::TProjectConfig config = application->GetProjectConfig();

  if(config.ProjectAutoSaveStatesFlag == checked)
    return;

  config.ProjectAutoSaveStatesFlag = checked;

  application->SetProjectConfig(config);
}

void UStatusPanel::setAutosaveProject(bool checked)
{
  if(!application) return;

  RDK::TProjectConfig config = application->GetProjectConfig();

  if(config.ProjectAutoSaveFlag == checked)
    return;

  config.ProjectAutoSaveFlag = checked;

  application->SetProjectConfig(config);
  application->SaveProjectConfig();
}
