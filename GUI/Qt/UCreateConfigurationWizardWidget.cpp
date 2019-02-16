#include "UCreateConfigurationWizardWidget.h"
#include "ui_UCreateConfigurationWizardWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include "QStandardItemModel"
#include "QStandardItem"

#define SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(param, value) \
  if(ui->checkBoxSettingToAllChannels->isChecked()) \
  { \
    for(int i = 0; i < ProjectConfig.NumChannels; ++i) \
      ProjectConfig.ChannelsConfig[i].param = value; \
  } \
  else \
    ProjectConfig.ChannelsConfig[channelNumber].param = value;

UCreateConfigurationWizardWidget::UCreateConfigurationWizardWidget(QWidget *parent, RDK::UApplication *app) :
    QWizard(parent),
    ui(new Ui::UCreateConfigurationWizardWidget)
{
  ui->setupUi(this);

  application = app;

  classesList = new UClassesListWidget(this);
  ui->horizontalLayoutFromComponent->addWidget(classesList);

  // radio buttons Channel Properties - Model selection
  connect(ui->radioButtonMSPredefinedModel, SIGNAL(toggled(bool))       , this, SLOT(onMSPredefinedModel(bool)));
  connect(ui->radioButtonMSFromComponent  , SIGNAL(toggled(bool))       , this, SLOT(onMSModelFromComponent(bool)));
  connect(ui->radioButtonMSFromFile       , SIGNAL(toggled(bool))       , this, SLOT(onMSLoadModelFromFile(bool)));
  connect(ui->lineEditModelFromFile       , SIGNAL(textChanged(QString)), this, SLOT(modelFromFileNameChanged(QString)));

  // channels number stuff
  connect(ui->checkBoxSettingToAllChannels, SIGNAL(toggled(bool)), this, SLOT(setApplySettingToAllChannels(bool)));
  connect(ui->spinBoxChannelsNumber, SIGNAL(valueChanged(int)), this, SLOT(setCalculationChannelsNumber(int)));
  connect(ui->listWidgetChannels, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
          this, SLOT(channelSelectionChanged(QListWidgetItem*,QListWidgetItem*)));
  connect(classesList, SIGNAL(classSelectionChanged()), this, SLOT(modelFromComponentChanged()));

  // Browse buttons
  connect(ui->pushButtonBrowseProjectDirectory, SIGNAL(pressed()), this, SLOT(browseNewProjectDirectory()));
  connect(ui->pushButtonBrowseModel, SIGNAL(pressed()), this, SLOT(browseModelXML()));

  // radio buttons Project Type
  connect(ui->radioButtonPTUniversal, SIGNAL(toggled(bool)), this, SLOT(onPTUniversal(bool)));
  connect(ui->radioButtonPTVideoAnalysis, SIGNAL(toggled(bool)), this, SLOT(onPTVideoAnalysis(bool)));
  connect(ui->radioButtonPTModeling, SIGNAL(toggled(bool)), this, SLOT(onPTModeling(bool)));

  // time step duration
  connect(ui->lineEditTimeStepDurationDefault, SIGNAL(textChanged(QString)), this, SLOT(defaultTimeChanged(QString)));
  connect(ui->lineEditTimeStepDurationGlobal , SIGNAL(textChanged(QString)), this, SLOT(globalTimeChanged(QString)));

  // radio buttons Calculation Mode
  connect(ui->radioButtonCMSimple, SIGNAL(toggled(bool)), this, SLOT(onCMSimple(bool)));
  connect(ui->radioButtonCMRealTime, SIGNAL(toggled(bool)), this, SLOT(onCMRealTime(bool)));
  connect(ui->radioButtonCMBySignal, SIGNAL(toggled(bool)), this, SLOT(onCMBySignal(bool)));
  connect(ui->radioButtonCMFastest, SIGNAL(toggled(bool)), this, SLOT(onCMFastest(bool)));

  connect(ui->lineEditMaxCalculationModelTime, SIGNAL(textChanged(QString)), this, SLOT(maxCalculationModelTimeChanged(QString)));
  connect(ui->lineEditMinInterstepInterval   , SIGNAL(textChanged(QString)), this, SLOT(minInterstepIntervalChanged(QString)));

  // checkboxes at the bottom
  connect(ui->checkBoxCPInitAfterLoad , SIGNAL(toggled(bool)), this, SLOT(setInitAfterLoad(bool)));
  connect(ui->checkBoxCPResetAfterLoad, SIGNAL(toggled(bool)), this, SLOT(setResetAfterLoad(bool)));
  connect(ui->checkBoxCPDebug         , SIGNAL(toggled(bool)), this, SLOT(setDebugMode(bool)));


  connect(ui->listViewPredefinedStructures, SIGNAL(itemActivated(QListWidgetItem*)),
                      this, SLOT(selectPredefinedStructure(QListWidgetItem*)));

  channelNumber = 0;
  ui->radioButtonMSFromComponent->setChecked(true);
  ui->frameMaxCalcModelTime->hide();
  ui->radioButtonPTUniversal->setChecked(true);

  ProjectConfig.NumChannels = 1;
  ProjectConfig.ChannelsConfig.resize(1);
}

UCreateConfigurationWizardWidget::~UCreateConfigurationWizardWidget()
{
  delete ui;
}

void UCreateConfigurationWizardWidget::onMSPredefinedModel(bool checked)
{
  if(checked)
  {
    ui->frameFromComponent->hide();
    ui->frameFromFile->hide();
    ui->framePredefinedModel->show();


    PredefinedStructuresData.clear();

    RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();
    if(env)
    {
     std::map<int, RDK::UEnvPredefinedStructDescription> descrs=env->GetPredefinedStructures();
     std::map<int, RDK::UEnvPredefinedStructDescription>::iterator I=descrs.begin();
     for(;I!=descrs.end();++I)
     {
      std::string str=RDK::sntoa(I->first);
      str+=": ";
      str+=I->second.ShortDescription;
      PredefinedStructuresData.append(QString::fromLocal8Bit(str.c_str()));
     }

    }
    stringListModelPredefinedStructures.setStringList(PredefinedStructuresData);
    ui->listViewPredefinedStructures->setModel(&stringListModelPredefinedStructures);
    ui->listViewPredefinedStructures->show();

    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(PredefinedStructure, ui->spinBoxPredefinedmModelID->value());
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ModelFileName      , "");
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ClassName          , "");
  }
}

void UCreateConfigurationWizardWidget::modelFromFileNameChanged(QString value)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ModelFileName, value.toLocal8Bit().constData());
}

void UCreateConfigurationWizardWidget::modelFromComponentChanged()
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ClassName, classesList->selctedClass().toLocal8Bit().constData());
}

void UCreateConfigurationWizardWidget::onMSLoadModelFromFile(bool checked)
{
  if(checked)
  {
    ui->frameFromComponent->hide();
    ui->framePredefinedModel->hide();
    ui->frameFromFile->show();

    /*ProjectConfig.ChannelsConfig[channelNumber].ModelFileName = ui->lineEditModelFromFile->text().toLocal8Bit().constData();
    ProjectConfig.ChannelsConfig[channelNumber].ClassName = "";
    ProjectConfig.ChannelsConfig[channelNumber].PredefinedStructure = 0;*/
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(PredefinedStructure, 0);
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ModelFileName      , ui->lineEditModelFromFile->text().toLocal8Bit().constData());
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ClassName          , "");
  }
}

void UCreateConfigurationWizardWidget::onMSModelFromComponent(bool checked)
{
  if(checked)
  {
    ui->frameFromFile->hide();
    ui->framePredefinedModel->hide();
    ui->frameFromComponent->show();

    /*ProjectConfig.ChannelsConfig[channelNumber].ClassName =
        classesList->selctedClass().toStdString();
    ProjectConfig.ChannelsConfig[channelNumber].ModelFileName = "";
    ProjectConfig.ChannelsConfig[channelNumber].PredefinedStructure = 0;*/
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(PredefinedStructure, 0);
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ModelFileName      , "");
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ClassName          , classesList->selctedClass().toLocal8Bit().constData());
  }
}

void UCreateConfigurationWizardWidget::setApplySettingToAllChannels(bool checked)
{
  if(checked)
  {
    if(QMessageBox::Cancel == QMessageBox::question
       (this, "Warning", "Are you sure you want to change channel settings strategy?\n"
              "This will remove all your settings for other channels.",
        QMessageBox::Yes|QMessageBox::Cancel))
    {
      ui->checkBoxSettingToAllChannels->setChecked(false);
      return;
    }

    for(int i = 0; i < ProjectConfig.NumChannels; ++i)
    {
      if( i == channelNumber) continue;

      ProjectConfig.ChannelsConfig[i].DefaultTimeStep         = ProjectConfig.ChannelsConfig[channelNumber].DefaultTimeStep;
      ProjectConfig.ChannelsConfig[i].GlobalTimeStep          = ProjectConfig.ChannelsConfig[channelNumber].GlobalTimeStep;
      ProjectConfig.ChannelsConfig[i].CalculationMode         = ProjectConfig.ChannelsConfig[channelNumber].CalculationMode;
      ProjectConfig.ChannelsConfig[i].MaxCalculationModelTime = ProjectConfig.ChannelsConfig[channelNumber].MaxCalculationModelTime;
      ProjectConfig.ChannelsConfig[i].MinInterstepsInterval   = ProjectConfig.ChannelsConfig[channelNumber].MinInterstepsInterval;
      ProjectConfig.ChannelsConfig[i].InitAfterLoad           = ProjectConfig.ChannelsConfig[channelNumber].InitAfterLoad;
      ProjectConfig.ChannelsConfig[i].ResetAfterLoad          = ProjectConfig.ChannelsConfig[channelNumber].ResetAfterLoad;
      ProjectConfig.ChannelsConfig[i].DebugMode               = ProjectConfig.ChannelsConfig[channelNumber].DebugMode;
    }

    ui->listWidgetChannels->hide();
  }
  else
  {
    ui->listWidgetChannels->show();
  }
}

void UCreateConfigurationWizardWidget::setCalculationChannelsNumber(int value)
{
  int listSize = ui->listWidgetChannels->count();

  for(int i = listSize; i > value; --i)
  {
    delete ui->listWidgetChannels->takeItem(i-1);
  }

  for(int i = listSize; i < value; ++i)
  {
    ui->listWidgetChannels->addItem(QString::number(i));
  }

  if(value == ProjectConfig.NumChannels)
   return;

  int old_channels = ProjectConfig.NumChannels;
  ProjectConfig.NumChannels = value;
  ProjectConfig.ChannelsConfig.resize(ProjectConfig.NumChannels);

  for(int i = old_channels; i < value; ++i)
  {
    ProjectConfig.ChannelsConfig[i].ClassName="Model";
    ProjectConfig.ChannelsConfig[i].GlobalTimeStep  = ui->lineEditTimeStepDurationGlobal->text().toInt();
    ProjectConfig.ChannelsConfig[i].DefaultTimeStep = ui->lineEditTimeStepDurationDefault->text().toInt();
    ProjectConfig.ChannelsConfig[i].CalculationMode = ProjectConfig.ChannelsConfig[i-1].CalculationMode;
  }
}

void UCreateConfigurationWizardWidget::channelSelectionChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
  channelNumber = current->text().toInt();

  if(!ProjectConfig.ChannelsConfig[channelNumber].ClassName.empty())
  {
    ui->radioButtonMSFromComponent->setChecked(true);
  }

    /*ProjectConfig.ChannelsConfig[channelNumber].ClassName =
            classesList->selctedClass().toStdString();
        ProjectConfig.ChannelsConfig[channelNumber].ModelFileName = "";
        ProjectConfig.ChannelsConfig[channelNumber].PredefinedStructure = 0;*/

  ui->lineEditTimeStepDurationDefault->setText(QString::number(ProjectConfig.ChannelsConfig[channelNumber].DefaultTimeStep));
  ui->lineEditTimeStepDurationGlobal->setText(QString::number(ProjectConfig.ChannelsConfig[channelNumber].GlobalTimeStep));

  switch(ProjectConfig.ChannelsConfig[channelNumber].CalculationMode)
  {
    case 0:
      ui->radioButtonCMSimple->setChecked(true);
      break;
    case 1:
      ui->radioButtonCMRealTime->setChecked(true);
      break;
    case 2:
      ui->radioButtonCMBySignal->setChecked(true);
      break;
    case 3:
      ui->radioButtonCMFastest->setChecked(true);
      break;
  }

  ui->lineEditMaxCalculationModelTime->setText(QString::number(ProjectConfig.ChannelsConfig[channelNumber].MaxCalculationModelTime));
  ui->lineEditMinInterstepInterval->setText(QString::number(ProjectConfig.ChannelsConfig[channelNumber].MinInterstepsInterval));
  ui->checkBoxCPInitAfterLoad->setChecked(ProjectConfig.ChannelsConfig[channelNumber].InitAfterLoad);
  ui->checkBoxCPResetAfterLoad->setChecked(ProjectConfig.ChannelsConfig[channelNumber].ResetAfterLoad);
  ui->checkBoxCPDebug->setChecked(ProjectConfig.ChannelsConfig[channelNumber].DebugMode);
}

void UCreateConfigurationWizardWidget::browseNewProjectDirectory()
{
  ui->lineEditProjectDirectory->setText(
        QFileDialog::getExistingDirectory(this, tr("Create project directory"), ""));
}

void UCreateConfigurationWizardWidget::browseModelXML()
{
  QString XMLFileName = QFileDialog::getOpenFileName(this, tr("Open model xml-file"),
                                                 QApplication::applicationDirPath()+"/../../../Configs", tr("model*.xml"));
  ui->lineEditModelFromFile->setText(XMLFileName);
  modelsFromFile[channelNumber] = XMLFileName;
}

void UCreateConfigurationWizardWidget::onPTUniversal(bool checked)
{
  if(checked)
  {
    ui->lineEditTimeStepDurationDefault->clear();
    ui->lineEditTimeStepDurationGlobal ->clear();
    ui->lineEditMinInterstepInterval   ->clear();
    ui->radioButtonCMSimple->setChecked(true);
  }
}

void UCreateConfigurationWizardWidget::onPTVideoAnalysis(bool checked)
{
  if(checked)
  {
    ui->lineEditTimeStepDurationDefault->setText("30");
    ui->lineEditTimeStepDurationGlobal ->setText("30");
    ui->lineEditMinInterstepInterval   ->setText("30");
    ui->radioButtonCMRealTime->setChecked(true);
  }
}

void UCreateConfigurationWizardWidget::onPTModeling(bool checked)
{
  if(checked)
  {
    ui->lineEditTimeStepDurationDefault->setText("2000");
    ui->lineEditTimeStepDurationGlobal ->setText("2000");
    ui->lineEditMinInterstepInterval   ->setText("30");
    ui->radioButtonCMFastest->setChecked(true);
  }
}

void UCreateConfigurationWizardWidget::defaultTimeChanged(QString value)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(DefaultTimeStep, value.toInt());
}

void UCreateConfigurationWizardWidget::globalTimeChanged(QString value)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(GlobalTimeStep, value.toInt());
}

void UCreateConfigurationWizardWidget::onCMSimple(bool checked)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(CalculationMode, 0);
}

void UCreateConfigurationWizardWidget::onCMRealTime(bool checked)
{
  if(checked)
  {
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(CalculationMode, 1);

    ui->frameMaxCalcModelTime->show();
  }
  else
    ui->frameMaxCalcModelTime->hide();
}

void UCreateConfigurationWizardWidget::onCMBySignal(bool checked)
{
  if(checked)
  {
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(CalculationMode, 2);
  }
}

void UCreateConfigurationWizardWidget::onCMFastest(bool checked)
{
  if(checked)
  {
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(CalculationMode, 3);

    ui->frameMaxCalcModelTime->show();
  }
  else
    ui->frameMaxCalcModelTime->hide();
}

void UCreateConfigurationWizardWidget::maxCalculationModelTimeChanged(QString text)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(MaxCalculationModelTime, text.toDouble());
}

void UCreateConfigurationWizardWidget::minInterstepIntervalChanged(QString text)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(MinInterstepsInterval, text.toDouble());
}

void UCreateConfigurationWizardWidget::setInitAfterLoad(bool checked)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(InitAfterLoad, checked);
}

void UCreateConfigurationWizardWidget::setResetAfterLoad(bool checked)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ResetAfterLoad, checked);
}

void UCreateConfigurationWizardWidget::setDebugMode(bool checked)
{
  SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(DebugMode, checked);
}

void UCreateConfigurationWizardWidget::selectPredefinedStructure(QListWidgetItem* item)
{

}

void UCreateConfigurationWizardWidget::accept()
{
  // first page
  ProjectConfig.ProjectName = ui->lineEditProjectName->text().toLocal8Bit().constData();
  ProjectConfig.ProjectDescription = ui->plainTextEditProjectDescription->toPlainText().toLocal8Bit().constData();

  if(ui->radioButtonSimplePM->isChecked())
    ProjectConfig.ProjectMode = 0;
  else
    ProjectConfig.ProjectMode = 1;

  if(ui->radioButtonSingleThread->isChecked())
    ProjectConfig.MultiThreadingMode = 0;
  else
    ProjectConfig.MultiThreadingMode = 1;

  if(ui->radioButtonSystemTime->isChecked())
    ProjectConfig.CalcSourceTimeMode = 0;
  else
    ProjectConfig.CalcSourceTimeMode = 1;

  ProjectConfig.ProjectAutoSaveFlag = ui->checkBoxAutosaveProject->isChecked();
  ProjectConfig.ProjectAutoSaveStatesFlag = ui->checkBoxAutosaveStates->isChecked();
  ProjectConfig.EventsLogFlag = ui->checkBoxLogEvents->isChecked();
  ProjectConfig.DisableStopVideoSources = ui->checkBoxSDiasbleStopVS->isChecked();

  if(application)
  {
    application->CreateProject(
          (ui->lineEditProjectDirectory->text()
           + "/project.ini").toLocal8Bit().constData(), ProjectConfig);
  }

  QWizard::accept();
}

void UCreateConfigurationWizardWidget::on_listViewPredefinedStructures_activated(const QModelIndex &index)
{

}

void UCreateConfigurationWizardWidget::on_listViewPredefinedStructures_clicked(const QModelIndex &index)
{
 QString str=PredefinedStructuresData[index.row()];
 int pos=str.indexOf(':');
 int value=str.mid(0,pos).toInt();
 ui->spinBoxPredefinedmModelID->setValue(value);
 SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(PredefinedStructure, ui->spinBoxPredefinedmModelID->value());
}
