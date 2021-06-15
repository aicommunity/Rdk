#include "UCreateConfigurationWizardWidget.h"
#include "ui_UCreateConfigurationWizardWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include "QStandardItemModel"
#include "QStandardItem"
#include <QTextCodec>

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

  ChangeCheckDebugStateChannel=false;
  ChangeCheckDebugState=false;

  application = app;

  classesList = new UClassesListWidget(this);
  ui->horizontalLayoutFromComponent->addWidget(classesList);

  // radio buttons Channel Properties - Model selection
  connect(ui->radioButtonMSPredefinedModel, SIGNAL(toggled(bool))       , this, SLOT(onMSPredefinedModel(bool)));
  connect(ui->radioButtonMSFromComponent  , SIGNAL(toggled(bool))       , this, SLOT(onMSModelFromComponent(bool)));
  connect(ui->radioButtonMSFromFile       , SIGNAL(toggled(bool))       , this, SLOT(onMSLoadModelFromFile(bool)));
  connect(ui->lineEditModelFromFile       , SIGNAL(textChanged(QString)), this, SLOT(modelFromFileNameChanged(QString)));
  connect(ui->radioButtonFromModelsCollection, SIGNAL(toggled(bool))       , this, SLOT(onMSLoadModelFromModelsCollection(bool)));

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
  connect(ui->checkBoxChannelDebugMode, SIGNAL(toggled(bool)), this, SLOT(setDebugMode(bool)));


  //connect(ui->listViewPredefinedStructures, SIGNAL(itemActivated(QListWidgetItem*)),
  //                    this, SLOT(selectPredefinedStructure(QListWidgetItem*)));

 // connect(ui->checkBoxDebugSysEventsNone, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));
  connect(ui->checkBoxDebugSysEventsCalc, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));
  connect(ui->checkBoxDebugSysEventsReset, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));
  connect(ui->checkBoxDebugSysEventsProperties, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));
  connect(ui->checkBoxDebugSysEventsParameters, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));
  connect(ui->checkBoxDebugSysEventsStates, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));
  connect(ui->checkBoxDebugSysEventsInputs, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));
  connect(ui->checkBoxDebugSysEventsOutputs, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEvents(int)));

  //connect(ui->checkBoxDebugSysEventsNone_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));
  connect(ui->checkBoxDebugSysEventsCalc_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));
  connect(ui->checkBoxDebugSysEventsReset_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));
  connect(ui->checkBoxDebugSysEventsProperties_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));
  connect(ui->checkBoxDebugSysEventsParameters_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));
  connect(ui->checkBoxDebugSysEventsStates_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));
  connect(ui->checkBoxDebugSysEventsInputs_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));
  connect(ui->checkBoxDebugSysEventsOutputs_2, SIGNAL(stateChanged(int)), this, SLOT(selectDebugEventsChannel(int)));



  channelNumber = 0;
  ui->radioButtonMSFromComponent->setChecked(true);
  ui->radioButtonPTUniversal->setChecked(true);

  ProjectConfig.NumChannels = 1;
  ProjectConfig.ChannelsConfig.resize(1);
}

UCreateConfigurationWizardWidget::~UCreateConfigurationWizardWidget()
{
  delete ui;
}

void UCreateConfigurationWizardWidget::UpdateInterface(void)
{
 if(!application)
  return;

 QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
 //QByteArray byteArray(n.XMLDescription.c_str(), n.XMLDescription.length());
 //QString utf8Str = codec->toUnicode(byteArray);


 if(application->GetProjectOpenFlag())
 {
  ProjectConfig=application->GetProjectConfig();
  ui->lineEditProjectDirectory->setText((application->GetProjectPath()+application->GetProjectFileName()).c_str());

 }
 else
 {
  RDK::TProjectConfig def;
  ProjectConfig=def;
  ui->lineEditProjectDirectory->setText("");
 }

 // обновляем интерфейс в соответствии с ProjectConfig
 ui->lineEditProjectName->setText(codec->toUnicode(ProjectConfig.ProjectName.c_str()));

 ui->plainTextEditProjectDescription->setPlainText(ProjectConfig.ProjectDescription.c_str());
 ui->checkBoxAutosaveProject->setChecked(ProjectConfig.ProjectAutoSaveFlag);
 ui->checkBoxAutosaveStates->setChecked(ProjectConfig.ProjectAutoSaveStatesFlag);
 ui->checkBoxLogEvents->setChecked(ProjectConfig.EventsLogFlag);
 if(ProjectConfig.ProjectMode == 0)
 {
  ui->radioButtonSimplePM->setChecked(true);
  ui->radioButtonServerPM->setChecked(false);
 }
 else
 {
  ui->radioButtonSimplePM->setChecked(false);
  ui->radioButtonServerPM->setChecked(true);
 }

 switch(ProjectConfig.ProjectType)
 {
 case 0:
  ui->radioButtonPTUniversal->setChecked(true);
  ui->radioButtonPTVideoAnalysis->setChecked(false);
  ui->radioButtonPTModeling->setChecked(false);
 break;

 case 1:
  ui->radioButtonPTUniversal->setChecked(false);
  ui->radioButtonPTVideoAnalysis->setChecked(true);
  ui->radioButtonPTModeling->setChecked(false);
 break;

 case 2:
  ui->radioButtonPTUniversal->setChecked(false);
  ui->radioButtonPTVideoAnalysis->setChecked(false);
  ui->radioButtonPTModeling->setChecked(true);
 break;
 }

 switch (ProjectConfig.MultiThreadingMode)
 {
 case 0:
  ui->radioButtonSingleThread->setChecked(true);
  ui->radioButtonMultiThreads->setChecked(false);
 break;

 case 1:
  ui->radioButtonSingleThread->setChecked(false);
  ui->radioButtonMultiThreads->setChecked(true);
 break;
 }

 switch (ProjectConfig.MultiThreadingMode)
 {
 case 0:
  ui->radioButtonSystemTime->setChecked(true);
  ui->radioButtonExternalDataTime->setChecked(false);
 break;

 case 1:
  ui->radioButtonSystemTime->setChecked(false);
  ui->radioButtonExternalDataTime->setChecked(true);
 break;
 }

 ui->checkBoxSDiasbleStopVS->setChecked(ProjectConfig.DisableStopVideoSources);

 ui->checkBoxEventsLogMode->setChecked(ProjectConfig.EventsLogMode);

 ui->lineEditMTUpdateInterfaceInterval->setText(QString::number(ProjectConfig.MTUpdateInterfaceInterval));

 switch (ProjectConfig.GuiUpdateMode)
 {
 case 0:
  ui->radioButtonGuiUpdateMode0->setChecked(true);
  ui->radioButtonGuiUpdateMode1->setChecked(false);
 break;

 case 1:
  ui->radioButtonGuiUpdateMode0->setChecked(false);
  ui->radioButtonGuiUpdateMode1->setChecked(true);
 break;
 }

 ui->checkBox->setChecked(ProjectConfig.DebugMode);
 ui->checkBoxDebuggerMessageFlag->setChecked(ProjectConfig.DebuggerMessageFlag);
 ui->checkBoxLogEvents->setChecked(ProjectConfig.EventsLogFlag);
 ui->checkBoxOverrideLogParameters->setChecked(ProjectConfig.OverrideLogParameters);



 // каналы...
 ui->checkBoxSettingToAllChannels->setChecked(false);
 ui->spinBoxChannelsNumber->setValue(ProjectConfig.NumChannels);
 int listSize = ui->listWidgetChannels->count();

 for(int i = listSize; i > ProjectConfig.NumChannels; --i)
 {
  delete ui->listWidgetChannels->takeItem(i-1);
 }

 for(int i = listSize; i < ProjectConfig.NumChannels; ++i)
 {
  ui->listWidgetChannels->addItem(QString::number(i));
 }

 ui->listWidgetChannels->setCurrentRow(0);
 channelSelectionChanged(0);

/*
#define RDK_SYS_DEBUG_NONE 0
#define RDK_SYS_DEBUG_CALC 1
#define RDK_SYS_DEBUG_RESET 2
#define RDK_SYS_DEBUG_PROPERTIES 4
#define RDK_SYS_DEBUG_PARAMETERS 8
#define RDK_SYS_DEBUG_STATES 16
#define RDK_SYS_DEBUG_INPUTS 32
#define RDK_SYS_DEBUG_OUTPUTS 64
*/
 if(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_NONE)
 {
  ui->checkBoxDebugSysEventsNone->setChecked(true);
  ui->checkBoxDebugSysEventsCalc->setChecked(false);
  ui->checkBoxDebugSysEventsReset->setChecked(false);
  ui->checkBoxDebugSysEventsProperties->setChecked(false);
  ui->checkBoxDebugSysEventsParameters->setChecked(false);
  ui->checkBoxDebugSysEventsStates->setChecked(false);
  ui->checkBoxDebugSysEventsInputs->setChecked(false);
  ui->checkBoxDebugSysEventsOutputs->setChecked(false);
 }
 else
 {
  ui->checkBoxDebugSysEventsNone->setChecked(false);
  ui->checkBoxDebugSysEventsCalc->setChecked(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_CALC);
  ui->checkBoxDebugSysEventsReset->setChecked(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_RESET);
  ui->checkBoxDebugSysEventsProperties->setChecked(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_PROPERTIES);
  ui->checkBoxDebugSysEventsParameters->setChecked(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_PARAMETERS);
  ui->checkBoxDebugSysEventsStates->setChecked(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_STATES);
  ui->checkBoxDebugSysEventsInputs->setChecked(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_INPUTS);
  ui->checkBoxDebugSysEventsOutputs->setChecked(ProjectConfig.DebugSysEventsMask & RDK_SYS_DEBUG_OUTPUTS);
 }

   /*

 if(old_project_config.ServerInterfaceAddress != project_config.ServerInterfaceAddress)
 {

 }

 if(old_project_config.ServerInterfacePort != project_config.ServerInterfacePort)
 {

 }

 for(int i=0;i<min_num_channels;i++)
 {
  if(old_project_config.ChannelsConfig[i].ModelMode != old_project_config.ChannelsConfig[i].ModelMode)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].PredefinedStructure != old_project_config.ChannelsConfig[i].PredefinedStructure)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ModelFileName != old_project_config.ChannelsConfig[i].ModelFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ParametersFileName != old_project_config.ChannelsConfig[i].ParametersFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].StatesFileName != old_project_config.ChannelsConfig[i].StatesFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ClassName != old_project_config.ChannelsConfig[i].ClassName)
  {
   is_reload_needed=true;
  }



 }
*/



}


void UCreateConfigurationWizardWidget::onMSPredefinedModel(bool checked)
{
  if(checked)
  {
    ui->frameFromComponent->hide();
    ui->frameFromFile->hide();
    ui->framePredefinedModel->show();
    ui->frameFromModelsCollection->hide();


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
    ui->frameFromModelsCollection->hide();
    ui->frameFromFile->show();


    /*ProjectConfig.ChannelsConfig[channelNumber].ModelFileName = ui->lineEditModelFromFile->text().toLocal8Bit().constData();
    ProjectConfig.ChannelsConfig[channelNumber].ClassName = "";
    ProjectConfig.ChannelsConfig[channelNumber].PredefinedStructure = 0;*/
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(PredefinedStructure, 0);
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ModelFileName      , ui->lineEditModelFromFile->text().toLocal8Bit().constData());
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ClassName          , "");
  }
}

void UCreateConfigurationWizardWidget::onMSLoadModelFromModelsCollection(bool checked)
{
    ui->frameFromComponent->hide();
    ui->framePredefinedModel->hide();
    ui->frameFromFile->hide();
    ui->frameFromModelsCollection->show();

    std::list<RDK::StandartXMLInCatalog> fileList;
    ModelsFromFileData.clear();
    fileList = application->GetStandartXMLInCatalog();
    /*for (int i=0; i< fileList.size(); i++)
    {
       QString tmp = fileList[i].XMLName;
       ModelsFromFileData
    }*/
    for (RDK::StandartXMLInCatalog n :fileList)
    {
          QString tmp=QString::fromStdString(n.XMLName);
          QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
          QByteArray byteArray(n.XMLDescription.c_str(), int(n.XMLDescription.length()));
          QString utf8Str = codec->toUnicode(byteArray);
          tmp = tmp + "   " + utf8Str;
          ModelsFromFileData.push_back(tmp);
    }
    ModelsFromFileData.sort();

    //ModelsFromFileData
    stringListModelsFromFile.setStringList(ModelsFromFileData);
    ui->listViewModelsFromFile->setModel(&stringListModelsFromFile);
    ui->listViewModelsFromFile->show();
}

void UCreateConfigurationWizardWidget::onMSModelFromComponent(bool checked)
{
  if(checked)
  {
    ui->frameFromFile->hide();
    ui->framePredefinedModel->hide();
    ui->frameFromModelsCollection->hide();
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
 channelSelectionChanged(current->text().toInt());
}

void UCreateConfigurationWizardWidget::channelSelectionChanged(int channel_index)
{
 channelNumber = channel_index;

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
 ui->checkBoxChannelDebugMode->setChecked(ProjectConfig.ChannelsConfig[channelNumber].DebugMode);

/*
 for(int i=0;i<min_num_channels;i++)
 {
  if(old_project_config.ChannelsConfig[i].ModelMode != old_project_config.ChannelsConfig[i].ModelMode)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].PredefinedStructure != old_project_config.ChannelsConfig[i].PredefinedStructure)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ModelFileName != old_project_config.ChannelsConfig[i].ModelFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ParametersFileName != old_project_config.ChannelsConfig[i].ParametersFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].StatesFileName != old_project_config.ChannelsConfig[i].StatesFileName)
  {
   is_reload_needed=true;
  }

  if(old_project_config.ChannelsConfig[i].ClassName != old_project_config.ChannelsConfig[i].ClassName)
  {
   is_reload_needed=true;
  }


*/
}


void UCreateConfigurationWizardWidget::browseNewProjectDirectory()
{
 QString default_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../Configs/").c_str());
 QDir path1(default_path);
 if(!path1.exists(default_path))
 {
  default_path=QString::fromLocal8Bit((application->GetWorkDirectory()+"/../../../Configs/").c_str());
  QDir path2(default_path);
  if(!path2.exists(default_path))
  {
   default_path=QString::fromLocal8Bit(application->GetWorkDirectory().c_str());
  }
 }

  ui->lineEditProjectDirectory->setText(
        QFileDialog::getExistingDirectory(this, tr("Create project directory"), default_path, QFileDialog::ShowDirsOnly));
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
   ui->lineEditTimeStepDurationDefault->setText("30");
   ui->lineEditTimeStepDurationGlobal ->setText("30");
   ui->lineEditMinInterstepInterval   ->setText("30");
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
    ui->radioButtonCMSimple->setChecked(true);
  }
}

void UCreateConfigurationWizardWidget::onPTModeling(bool checked)
{
  if(checked)
  {
    ui->lineEditTimeStepDurationDefault->setText("2000");
    ui->lineEditTimeStepDurationGlobal ->setText("2000");
    ui->lineEditMinInterstepInterval   ->setText("30");
    ui->radioButtonCMRealTime->setChecked(true);
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

//    ui->groupBox_15->hide();
  }
//  else
//    ui->groupBox_15->show();
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

//    ui->groupBox_15->hide();
  }
//  else
//    ui->groupBox_15->show();
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

void UCreateConfigurationWizardWidget::selectDebugEvents(int state)
{
 if(ChangeCheckDebugState)
  return;
 ChangeCheckDebugState=true;

 if(ui->checkBoxDebugSysEventsCalc->isChecked())
  ui->checkBoxDebugSysEventsNone->setChecked(false);

 if(ui->checkBoxDebugSysEventsReset->isChecked())
  ui->checkBoxDebugSysEventsNone->setChecked(false);

 if(ui->checkBoxDebugSysEventsProperties->isChecked())
  ui->checkBoxDebugSysEventsNone->setChecked(false);

 if(ui->checkBoxDebugSysEventsParameters->isChecked())
  ui->checkBoxDebugSysEventsNone->setChecked(false);

 if(ui->checkBoxDebugSysEventsStates->isChecked())
  ui->checkBoxDebugSysEventsNone->setChecked(false);

 if(ui->checkBoxDebugSysEventsInputs->isChecked())
  ui->checkBoxDebugSysEventsNone->setChecked(false);

 if(ui->checkBoxDebugSysEventsOutputs->isChecked())
  ui->checkBoxDebugSysEventsNone->setChecked(false);
/*
 unsigned int debug_mask=0;
 if(ui->checkBoxDebugSysEventsNone->isChecked())
  debug_mask = 0;
 else
 {
  if(ui->checkBoxDebugSysEventsCalc->isChecked())
   debug_mask |= RDK_SYS_DEBUG_CALC;
  if(ui->checkBoxDebugSysEventsReset->isChecked())
   debug_mask |= RDK_SYS_DEBUG_RESET;
  if(ui->checkBoxDebugSysEventsProperties->isChecked())
   debug_mask |= RDK_SYS_DEBUG_PROPERTIES;
  if(ui->checkBoxDebugSysEventsParameters->isChecked())
   debug_mask |= RDK_SYS_DEBUG_PARAMETERS;
  if(ui->checkBoxDebugSysEventsStates->isChecked())
   debug_mask |= RDK_SYS_DEBUG_STATES;
  if(ui->checkBoxDebugSysEventsInputs->isChecked())
   debug_mask |= RDK_SYS_DEBUG_INPUTS;
  if(ui->checkBoxDebugSysEventsOutputs->isChecked())
   debug_mask |= RDK_SYS_DEBUG_OUTPUTS;
 }

 if(debug_mask == 0)
 {
  ui->checkBoxDebugSysEventsNone->setChecked(true);
  ui->checkBoxDebugSysEventsCalc->setChecked(false);
  ui->checkBoxDebugSysEventsReset->setChecked(false);
  ui->checkBoxDebugSysEventsProperties->setChecked(false);
  ui->checkBoxDebugSysEventsParameters->setChecked(false);
  ui->checkBoxDebugSysEventsStates->setChecked(false);
  ui->checkBoxDebugSysEventsInputs->setChecked(false);
  ui->checkBoxDebugSysEventsOutputs->setChecked(false);
 }
*/
 ChangeCheckDebugState=false;
}

void UCreateConfigurationWizardWidget::selectDebugEventsChannel(int state)
{
 if(ChangeCheckDebugStateChannel)
  return;
 ChangeCheckDebugStateChannel=true;

 unsigned int debug_mask=0;

 if(ui->checkBoxDebugSysEventsCalc_2->isChecked())
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);

 if(ui->checkBoxDebugSysEventsReset_2->isChecked())
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);

 if(ui->checkBoxDebugSysEventsProperties_2->isChecked())
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);

 if(ui->checkBoxDebugSysEventsParameters_2->isChecked())
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);

 if(ui->checkBoxDebugSysEventsStates_2->isChecked())
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);

 if(ui->checkBoxDebugSysEventsInputs_2->isChecked())
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);

 if(ui->checkBoxDebugSysEventsOutputs_2->isChecked())
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);

 if(ui->checkBoxDebugSysEventsNone_2->isChecked())
  debug_mask = 0;
 else
 {
  if(ui->checkBoxDebugSysEventsCalc_2->isChecked())
   debug_mask |= RDK_SYS_DEBUG_CALC;
  if(ui->checkBoxDebugSysEventsReset_2->isChecked())
   debug_mask |= RDK_SYS_DEBUG_RESET;
  if(ui->checkBoxDebugSysEventsProperties_2->isChecked())
   debug_mask |= RDK_SYS_DEBUG_PROPERTIES;
  if(ui->checkBoxDebugSysEventsParameters_2->isChecked())
   debug_mask |= RDK_SYS_DEBUG_PARAMETERS;
  if(ui->checkBoxDebugSysEventsStates_2->isChecked())
   debug_mask |= RDK_SYS_DEBUG_STATES;
  if(ui->checkBoxDebugSysEventsInputs_2->isChecked())
   debug_mask |= RDK_SYS_DEBUG_INPUTS;
  if(ui->checkBoxDebugSysEventsOutputs_2->isChecked())
   debug_mask |= RDK_SYS_DEBUG_OUTPUTS;
 }
/*
 if(debug_mask != 0)
  ui->checkBoxDebugSysEventsNone_2->setChecked(false);
 else
 {
  ui->checkBoxDebugSysEventsNone_2->setChecked(true);
  ui->checkBoxDebugSysEventsCalc_2->setChecked(false);
  ui->checkBoxDebugSysEventsReset_2->setChecked(false);
  ui->checkBoxDebugSysEventsProperties_2->setChecked(false);
  ui->checkBoxDebugSysEventsParameters_2->setChecked(false);
  ui->checkBoxDebugSysEventsStates_2->setChecked(false);
  ui->checkBoxDebugSysEventsInputs_2->setChecked(false);
  ui->checkBoxDebugSysEventsOutputs_2->setChecked(false);
 }*/
 ChangeCheckDebugStateChannel=false;
 SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(DebugSysEventsMask, debug_mask);
}

void UCreateConfigurationWizardWidget::showEvent(QShowEvent *ev)
{
 QWizard::showEvent(ev);
 UpdateInterface();
}

void UCreateConfigurationWizardWidget::accept()
{
 if(!application)
  return;

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

 ProjectConfig.DebugSysEventsMask=0;

 if(ui->checkBoxDebugSysEventsNone->isChecked())
  ProjectConfig.DebugSysEventsMask = 0;
 else
 {
  if(ui->checkBoxDebugSysEventsCalc->isChecked())
   ProjectConfig.DebugSysEventsMask |= RDK_SYS_DEBUG_CALC;
  if(ui->checkBoxDebugSysEventsReset->isChecked())
   ProjectConfig.DebugSysEventsMask |= RDK_SYS_DEBUG_RESET;
  if(ui->checkBoxDebugSysEventsProperties->isChecked())
   ProjectConfig.DebugSysEventsMask |= RDK_SYS_DEBUG_PROPERTIES;
  if(ui->checkBoxDebugSysEventsParameters->isChecked())
   ProjectConfig.DebugSysEventsMask |= RDK_SYS_DEBUG_PARAMETERS;
  if(ui->checkBoxDebugSysEventsStates->isChecked())
   ProjectConfig.DebugSysEventsMask |= RDK_SYS_DEBUG_STATES;
  if(ui->checkBoxDebugSysEventsInputs->isChecked())
   ProjectConfig.DebugSysEventsMask |= RDK_SYS_DEBUG_INPUTS;
  if(ui->checkBoxDebugSysEventsOutputs->isChecked())
   ProjectConfig.DebugSysEventsMask |= RDK_SYS_DEBUG_OUTPUTS;
 }

 ProjectConfig.EventsLogMode=ui->checkBoxEventsLogMode->isChecked();

 ProjectConfig.MTUpdateInterfaceInterval=ui->lineEditMTUpdateInterfaceInterval->text().toInt();

 if(ui->radioButtonGuiUpdateMode0->isChecked())
  ProjectConfig.GuiUpdateMode=0;
 else
  ProjectConfig.GuiUpdateMode=1;

 ProjectConfig.DebugMode=ui->checkBox->isChecked();
 ProjectConfig.DebuggerMessageFlag=ui->checkBoxDebuggerMessageFlag->isChecked();
 ProjectConfig.EventsLogFlag=ui->checkBoxLogEvents->isChecked();
 ProjectConfig.OverrideLogParameters=ui->checkBoxOverrideLogParameters->isChecked();

 if(application->GetProjectOpenFlag())
 {
  application->UpdateProject(ProjectConfig);
 }
 else
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

void UCreateConfigurationWizardWidget::on_listViewModelsFromFile_clicked(const QModelIndex &index)
{
    std::list<RDK::StandartXMLInCatalog> fileList;
    fileList = application->GetStandartXMLInCatalog();
    std::list<RDK::StandartXMLInCatalog>::iterator itt = fileList.begin();
    std::advance(itt, index.row());
    RDK::StandartXMLInCatalog tmp = *itt;
    std::string value = application->GetWorkDirectory();
    //value = "D:/VideoAnalytics/Rtv-VideoAnalytics/Bin/Models/" + tmp.XMLName;
    value = value + application->GetModelsMainPath();
    value = value + tmp.XMLName;
    SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ModelFileName, value);
}

void UCreateConfigurationWizardWidget::on_listViewModelsFromFile_activated(const QModelIndex &index)
{

}

void UCreateConfigurationWizardWidget::on_checkBoxChannelDebugMode_stateChanged(int arg1)
{

}

void UCreateConfigurationWizardWidget::on_checkBoxDebuggerMessageFlag_2_stateChanged(int arg1)
{
 SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(DebuggerMessageFlag, arg1);
}

void UCreateConfigurationWizardWidget::on_checkBoxLogEvents_2_stateChanged(int arg1)
{
 SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(EventsLogMode, arg1);
}

void UCreateConfigurationWizardWidget::on_lineEditChannelName_editingFinished()
{
 SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(ChannelName, ui->lineEditChannelName->text().toLocal8Bit().constData());
}

void UCreateConfigurationWizardWidget::on_checkBoxDebugSysEventsNone_2_clicked()
{
 if(ChangeCheckDebugStateChannel)
  return;
 ChangeCheckDebugStateChannel=true;

 if(ui->checkBoxDebugSysEventsNone_2->isChecked())
 {
  ui->checkBoxDebugSysEventsCalc_2->setChecked(false);
  ui->checkBoxDebugSysEventsReset_2->setChecked(false);
  ui->checkBoxDebugSysEventsProperties_2->setChecked(false);
  ui->checkBoxDebugSysEventsParameters_2->setChecked(false);
  ui->checkBoxDebugSysEventsStates_2->setChecked(false);
  ui->checkBoxDebugSysEventsInputs_2->setChecked(false);
  ui->checkBoxDebugSysEventsOutputs_2->setChecked(false);
 }
 ChangeCheckDebugStateChannel=false;
 SET_CHANNEL_CONFIG_TO_SINGLE_OR_ALL_CHANNELS(DebugSysEventsMask, false);
}

void UCreateConfigurationWizardWidget::on_checkBoxDebugSysEventsNone_clicked()
{
 if(ChangeCheckDebugState)
  return;
 ChangeCheckDebugState=true;

 if(ui->checkBoxDebugSysEventsNone->isChecked())
 {
  ui->checkBoxDebugSysEventsCalc->setChecked(false);
  ui->checkBoxDebugSysEventsReset->setChecked(false);
  ui->checkBoxDebugSysEventsProperties->setChecked(false);
  ui->checkBoxDebugSysEventsParameters->setChecked(false);
  ui->checkBoxDebugSysEventsStates->setChecked(false);
  ui->checkBoxDebugSysEventsInputs->setChecked(false);
  ui->checkBoxDebugSysEventsOutputs->setChecked(false);
 }
 ChangeCheckDebugState=false;
}
