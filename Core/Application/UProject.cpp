#ifndef UAppProject_CPP
#define UAppProject_CPP

#include "UProject.h"
#include "../Utilities/USupport.h"


namespace RDK
{


TProjectChannelConfig::TProjectChannelConfig(void)
{
 ModelMode=0;

 PredefinedStructure=0;

 ClassName="UModel";

 GlobalTimeStep=30;

 DefaultTimeStep=30;

 CalculationMode=1;

 MinInterstepsInterval=0;

 InitAfterLoad=true;

 ResetAfterLoad=true;

 DebugMode=true;

 EventsLogMode=false;

 DebugSysEventsMask=0;

 DebuggerMessageFlag=false;

 MaxCalculationModelTime=0;

 UseIndTimeStepFlag=false;

}

TProjectChannelConfig::TProjectChannelConfig(const TProjectChannelConfig& copy)
{
 (*this)=copy;
}

bool TProjectChannelConfig::operator != (const TProjectChannelConfig& copy) const
{
 return (ModelMode != copy.ModelMode) ||
 (PredefinedStructure != copy.PredefinedStructure) ||
 (ClassName != copy.ClassName) ||
 (ModelFileName != copy.ModelFileName) ||
 (ParametersFileName != copy.ParametersFileName) ||
 (StatesFileName != copy.StatesFileName) ||
 (GlobalTimeStep != copy.GlobalTimeStep) ||
 (DefaultTimeStep != copy.DefaultTimeStep) ||
 (CalculationMode != copy.CalculationMode) ||
 (MinInterstepsInterval != copy.MinInterstepsInterval) ||
 (InitAfterLoad != copy.InitAfterLoad) ||
 (ResetAfterLoad != copy.ResetAfterLoad) ||
 (DebugMode != copy.DebugMode) ||
 (EventsLogMode != copy.EventsLogMode) ||
 (ChannelName != copy.ChannelName) ||
 (DebugSysEventsMask != copy.DebugSysEventsMask) ||
 (DebuggerMessageFlag != copy.DebuggerMessageFlag) ||
 (MaxCalculationModelTime != copy.MaxCalculationModelTime ||
 (UseIndTimeStepFlag != copy.UseIndTimeStepFlag));
}

bool TProjectChannelConfig::operator == (const TProjectChannelConfig& copy) const
{
 return !((*this) != copy);
}

TProjectChannelConfig& TProjectChannelConfig::operator = (const TProjectChannelConfig& copy)
{
 ModelMode=copy.ModelMode;
 PredefinedStructure=copy.PredefinedStructure;
 ClassName=copy.ClassName;
 ModelFileName=copy.ModelFileName;
 ParametersFileName=copy.ParametersFileName;
 StatesFileName=copy.StatesFileName;

 GlobalTimeStep=copy.GlobalTimeStep;

 DefaultTimeStep=copy.DefaultTimeStep;

 CalculationMode=copy.CalculationMode;

 MinInterstepsInterval=copy.MinInterstepsInterval;

 InitAfterLoad=copy.InitAfterLoad;

 ResetAfterLoad=copy.ResetAfterLoad;

 DebugMode=copy.DebugMode;
 DebugSysEventsMask=copy.DebugSysEventsMask;
 EventsLogMode=copy.EventsLogMode;
 DebuggerMessageFlag=copy.DebuggerMessageFlag;

 ChannelName=copy.ChannelName;

 MaxCalculationModelTime=copy.MaxCalculationModelTime;

 UseIndTimeStepFlag=copy.UseIndTimeStepFlag;
 return *this;
}


TProjectConfig::TProjectConfig(void)
{
 ProjectAutoSaveFlag=true;

 ProjectAutoSaveStatesFlag=false;

 EventsLogFlag=true;

 ProjectMode=1;

 ProjectType=0;

 MultiThreadingMode=1;

 CalcSourceTimeMode=0;

 ShowChannelsStateFlag=true;

 ReflectionFlag=true;

 DisableStopVideoSources=false;

 ProjectShowChannelsStates=false;

 ServerInterfacePort=45045;
 ServerInterfaceAddress="127.0.0.1";

 DebugMode=false;
 DebugSysEventsMask=0;
 DebuggerMessageFlag=false;
 EventsLogMode=false;

 OverrideLogParameters=true;

 MTUpdateInterfaceInterval=30;

 GuiUpdateMode = 0;

 NumChannels=1;
 ChannelsConfig.resize(1);
}

TProjectConfig::TProjectConfig(const TProjectConfig& copy)
{
 ProjectName=copy.ProjectName;
 UserName=copy.UserName;
 UserId=copy.UserId;
 CreationTime=copy.CreationTime;

 ProjectDescription=copy.ProjectDescription;
 ProjectAutoSaveFlag=copy.ProjectAutoSaveFlag;

 ProjectAutoSaveStatesFlag=copy.ProjectAutoSaveStatesFlag;
 EventsLogFlag=copy.EventsLogFlag;

 ProjectMode=copy.ProjectMode;
 ProjectType=copy.ProjectType;

 MultiThreadingMode=copy.MultiThreadingMode;
 CalcSourceTimeMode=copy.CalcSourceTimeMode;

 ShowChannelsStateFlag=copy.ShowChannelsStateFlag;

 ReflectionFlag=copy.ReflectionFlag;

 DisableStopVideoSources=copy.DisableStopVideoSources;

 NumChannels=copy.NumChannels;
 ChannelsConfig=copy.ChannelsConfig;

 /// Адрес интерфейса управления сервером
 ServerInterfaceAddress=copy.ServerInterfaceAddress;

 /// Порт интерфейса управления сервером
 ServerInterfacePort=copy.ServerInterfacePort;

  /// Адрес интерфейса Http-сервера
 HttpServerInterfaceAddress=copy.HttpServerInterfaceAddress;

  /// Порт интерфейса управления сервером
 HttpServerInterfacePort=copy.HttpServerInterfacePort;

  /// Порт интерфейса управления сервером
 HttpServerLogin=copy.HttpServerLogin;

  /// Порт интерфейса управления сервером
 HttpServerPassword=copy.HttpServerPassword;

 // Флаг отображения состояний каналов
 ProjectShowChannelsStates=copy.ProjectShowChannelsStates;

 InterfaceFileName=copy.InterfaceFileName;

 DescriptionFileName=copy.DescriptionFileName;
 DebugMode=copy.DebugMode;
 DebugSysEventsMask=copy.DebugSysEventsMask;
 DebuggerMessageFlag=copy.DebuggerMessageFlag;
 EventsLogMode=copy.EventsLogMode;
 OverrideLogParameters=copy.OverrideLogParameters;
 MTUpdateInterfaceInterval=copy.MTUpdateInterfaceInterval;

 GuiUpdateMode = copy.GuiUpdateMode;
}

bool TProjectConfig::operator != (const TProjectConfig& copy) const
{
 return (ProjectName != copy.ProjectName) ||
 (ProjectDescription != copy.ProjectDescription) ||
 (ProjectAutoSaveFlag != copy.ProjectAutoSaveFlag) ||
 (ProjectAutoSaveStatesFlag != copy.ProjectAutoSaveStatesFlag) ||
 (EventsLogFlag != copy.EventsLogFlag) ||
 (ProjectMode != copy.ProjectMode) ||
 (ProjectType != copy.ProjectType) ||
 (MultiThreadingMode != copy.MultiThreadingMode) ||
 (CalcSourceTimeMode != copy.CalcSourceTimeMode) ||
 (ShowChannelsStateFlag != copy.ShowChannelsStateFlag) ||
 (ReflectionFlag != copy.ReflectionFlag) ||
 (DisableStopVideoSources != copy.DisableStopVideoSources) ||
 (NumChannels != copy.NumChannels) ||
 (ChannelsConfig != copy.ChannelsConfig) ||
 (ServerInterfaceAddress != copy.ServerInterfaceAddress) ||
 (ServerInterfacePort != copy.ServerInterfacePort) ||
 (HttpServerInterfaceAddress != copy.HttpServerInterfaceAddress) ||
 (HttpServerInterfacePort != copy.HttpServerInterfacePort) ||
 (HttpServerLogin != copy.HttpServerLogin) ||
 (HttpServerPassword != copy.HttpServerPassword) ||
 (ProjectShowChannelsStates != copy.ProjectShowChannelsStates) ||
 (InterfaceFileName != copy.InterfaceFileName) ||
 (DescriptionFileName != copy.DescriptionFileName) ||
 (DebugMode != copy.DebugMode) ||
 (DebugSysEventsMask != copy.DebugSysEventsMask) ||
 (DebuggerMessageFlag != copy.DebuggerMessageFlag) ||
 (MTUpdateInterfaceInterval != copy.MTUpdateInterfaceInterval) ||
 (GuiUpdateMode != copy.GuiUpdateMode) ||
 (EventsLogMode != copy.EventsLogMode) ||
 (OverrideLogParameters != copy.OverrideLogParameters);
}

bool TProjectConfig::operator == (const TProjectConfig& copy) const
{
 return !((*this) != copy);
}

// --------------------------
/// Управление числом каналов
// --------------------------
int TProjectConfig::GetNumChannels(void) const
{
 return NumChannels;
}

bool TProjectConfig::SetNumChannels(int num)
{
 NumChannels=num;
 ChannelsConfig.resize(num);
 return true;
}

bool TProjectConfig::InsertChannel(int index)
{
 int old_num=NumChannels;
 int num=old_num+1;

 ChannelsConfig.resize(num);

 for(int i=int(ChannelsConfig.size())-1;i>index;i--)
 {
  ChannelsConfig[i]=ChannelsConfig[i-1];
 }

 return true;
}

bool TProjectConfig::DeleteChannel(int index)
{
 if(index<0 || index >= NumChannels)
  return false;

 if(NumChannels == 1)
  return false;

 ChannelsConfig.erase(ChannelsConfig.begin()+index);
 --NumChannels;
 return true;
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProject::UProject(void)
{
 ModifiedFlag=false;
 ForceOldXmlFormat=false;
 ForceNewConfigFilesStructure=false;
 ConfigVersion="1.0";
}

UProject::~UProject(void)
{

}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Данные проекта
const TProjectConfig& UProject::GetConfig(void) const
{
 return Config;
}

bool UProject::SetConfig(const TProjectConfig& value)
{
 if(Config == value)
  return true;

 Config=value;
 ModifiedFlag=true;
 return true;
}
// --------------------------

// --------------------------
// Методы управления проектом
// --------------------------
/// Проверяет наличие изменений в конфигурации проекта
bool UProject::IsModified(void) const
{
 return ModifiedFlag;
}

/// Выставляет признак наличия изменений в конфигурации проекта
bool UProject::SetModified(void)
{
 ModifiedFlag=true;
 return true;
}

/// Сбрасывает признак наличия изменений в конфигурации проекта
bool UProject::ResetModified(void)
{
 ModifiedFlag=false;
 return true;
}

/// Флаг принудительного сохранения данных конфигурации в старом формате файла
bool UProject::GetForceOldXmlFormat(void) const
{
 return ForceOldXmlFormat;
}

bool UProject::SetForceOldXmlFormat(bool value)
{
 if(ForceOldXmlFormat == value)
  return true;

 ForceOldXmlFormat=value;
/* if(ForceOldXmlFormat == true)
  ConfigVersion="1.0";
 else
 if(ForceNewConfigFilesStructure == true)
  ConfigVersion="2.1";
 else
  ConfigVersion="2.0";
  */
 return true;
}

/// Флаг включения нового представления файловой структуры конфигурации
bool UProject::GetForceNewConfigFilesStructure(void) const
{
 return ForceNewConfigFilesStructure;
}

bool UProject::SetForceNewConfigFilesStructure(bool value)
{
 if(ForceNewConfigFilesStructure == value)
  return true;
 ForceNewConfigFilesStructure=value;
/* if(ForceOldXmlFormat == true)
  ConfigVersion="1.0";
 else
 if(ForceNewConfigFilesStructure == true)
  ConfigVersion="2.1";
 else
  ConfigVersion="2.0";
  */
 return true;
}

/// Сбрасывает конфигурацию проекта в состояние по умолчанию
/// Метод также сбрасывает ModifiedFlag
void UProject::ResetToDefault(void)
{
 TProjectConfig default_config;
 SetConfig(default_config);
 ResetModified();
}

/// Доступ к пути до проекта
const std::string& UProject::GetProjectPath(void) const
{
 return ProjectPath;
}

bool UProject::SetProjectPath(const std::string& value)
{
 if(ProjectPath == value)
  return true;
 ProjectPath=value;
 return true;
}



bool UProject::ReadFromXml(USerStorageXML &xml)
{
 ModifiedFlag=false;
 xml.SelectRoot();
 std::string version=xml.GetNodeAttribute("Version");
 if(version == "2.0")
 {
  ConfigVersion="2.0";
  return ReadFromXmlNew(xml);
 }
 else
 if(version == "2.1")
 {
  SetForceNewConfigFilesStructure(true);
  ConfigVersion="2.1";
  return ReadFromXmlNew(xml);
 }
 ConfigVersion="1.0";
 return ReadFromXmlOld(xml);
}


/// Сохраняет конфигурацию проекта в xml
bool UProject::WriteToXml(USerStorageXML &xml)
{
 if(ForceOldXmlFormat)
  return WriteToXmlOld(xml);

 return WriteToXmlNew(xml);
}

bool UProject::ReadFromXmlOld(USerStorageXML &xml)
{
 xml.SelectNodeRoot("Project/MultiGeneral");
 Config.DebugMode=xml.ReadBool("DebugModeFlag",false);
 Config.DebugSysEventsMask=xml.ReadUnsigned("DebugSysEventsMask",0);
 Config.DebuggerMessageFlag=xml.ReadBool("DebuggerMessageFlag",false);
 Config.EventsLogMode=xml.ReadBool("EventsLogMode",false);
 Config.OverrideLogParameters=xml.ReadBool("OverrideLogParameters",true);

 Config.MultiThreadingMode=xml.ReadInteger("EnginesMode",0);

 Config.EventsLogFlag=xml.ReadBool("EventsLogEnabled",true);
 Config.ProjectShowChannelsStates=xml.ReadBool("ProjectShowChannelsStates",true);

 int calc_time_mode=xml.ReadInteger("CalculationTimeSourceMode",0);

 Config.ProjectMode=xml.ReadInteger("ProjectMode",1);

 Config.MTUpdateInterfaceInterval=xml.ReadInteger("MTUpdateInterfaceInterval",30);
 Config.GuiUpdateMode=xml.ReadInteger("GuiUpdateMode",0);


 int num_engines=xml.ReadInteger("NumEngines",1);
 if(num_engines<=0)
  num_engines=1;

 Config.DisableStopVideoSources=xml.ReadBool("DisableStopVideoSources",false);
 /*
 UShowProgressBarForm->SetBarHeader(1,Lang_LoadingData);
 UShowProgressBarForm->SetBarHeader(2,Lang_Total);
 UShowProgressBarForm->ResetBarStatus(1, 1, num_engines-1);
 UShowProgressBarForm->ResetBarStatus(2, 1, 2);

 if(AppWinState)
  UShowProgressBarForm->Show();
 UShowProgressBarForm->Update();
   */

 Config.NumChannels=num_engines;
 Config.ChannelsConfig.resize(num_engines);

// int selected_channel_index=xml.ReadInteger("SelectedEngineIndex",0);

 xml.SelectNodeRoot("Project/General");

 Config.ProjectName=xml.ReadString("ProjectName","NoName").c_str();

 // Число входов среды
 //NumEnvInputs=xml.ReadInteger("NumEnvInputs",1);

 // Число выходов среды
 //NumEnvOutputs=xml.ReadInteger("NumEnvOutputs",1);

 //InputEnvImageWidth=xml.ReadInteger("InputEnvImageWidth",360);
 //InputEnvImageHeight=xml.ReadInteger("InputEnvImageHeight",240);

 // Флаг автоматического сохранения проекта
 Config.ProjectAutoSaveFlag=xml.ReadInteger("ProjectAutoSaveFlag",1);

 // Флаг автоматического сохранения проекта
 Config.ProjectAutoSaveStatesFlag=xml.ReadInteger("ProjectAutoSaveStateFlag",0);

 std::string descriptionfilename=xml.ReadString("ProjectDescriptionFileName","");
 Config.DescriptionFileName=descriptionfilename;

 Config.InterfaceFileName=xml.ReadString("InterfaceFileName","");

 Config.CalcSourceTimeMode=calc_time_mode;
 Config.ChannelsConfig[0].PredefinedStructure=xml.ReadInteger("PredefinedStructure",0);
 Config.ChannelsConfig[0].DefaultTimeStep=xml.ReadInteger("DefaultTimeStep",30);
 Config.ChannelsConfig[0].GlobalTimeStep=xml.ReadInteger("GlobalTimeStep",30);
 Config.ChannelsConfig[0].CalculationMode=xml.ReadInteger("CalculationMode",1);
 Config.ChannelsConfig[0].InitAfterLoad=xml.ReadBool("InitAfterLoadFlag",1);
 Config.ChannelsConfig[0].ResetAfterLoad=xml.ReadBool("ResetAfterLoadFlag",true);
 Config.ChannelsConfig[0].DebugMode=xml.ReadBool("DebugModeFlag",false);
 Config.ChannelsConfig[0].MinInterstepsInterval=xml.ReadInteger("MinInterstepsInterval",20);

 Config.ChannelsConfig[0].ModelFileName=xml.ReadString("ModelFileName","");
 Config.ChannelsConfig[0].ParametersFileName=xml.ReadString("ParametersFileName","");
 Config.ChannelsConfig[0].StatesFileName=xml.ReadString("StatesFileName","");
 Config.ChannelsConfig[0].EventsLogMode=xml.ReadBool("EventsLogMode",false);
 Config.ChannelsConfig[0].DebugSysEventsMask=xml.ReadUnsigned("DebugSysEventsMask",Config.ChannelsConfig[0].DebugSysEventsMask);
 Config.ChannelsConfig[0].DebuggerMessageFlag=xml.ReadBool("DebuggerMessageFlag",false);
 Config.ChannelsConfig[0].MaxCalculationModelTime=xml.ReadFloat("MaxCalculationModelTime", 0.0);
 Config.ChannelsConfig[0].UseIndTimeStepFlag=xml.ReadBool("UseIndTimeStepFlag", false);



 for(int i=1;i<num_engines;i++)
 {
 // Config.ChannelsConfig[i].CalcSourceTimeMode=calc_time_mode;
  Config.ChannelsConfig[i].PredefinedStructure=xml.ReadInteger(std::string("PredefinedStructure_")+RDK::sntoa(i),0);
  Config.ChannelsConfig[i].DefaultTimeStep=xml.ReadInteger(std::string("DefaultTimeStep_")+RDK::sntoa(i),30);
  Config.ChannelsConfig[i].GlobalTimeStep=xml.ReadInteger(std::string("GlobalTimeStep_")+RDK::sntoa(i),30);
  Config.ChannelsConfig[i].CalculationMode=xml.ReadInteger(std::string("CalculationMode_")+RDK::sntoa(i),1);
  Config.ChannelsConfig[i].InitAfterLoad=xml.ReadBool(std::string("InitAfterLoadFlag_")+RDK::sntoa(i),1);
  Config.ChannelsConfig[i].ResetAfterLoad=xml.ReadBool(std::string("ResetAfterLoadFlag_")+RDK::sntoa(i),true);
  Config.ChannelsConfig[i].DebugMode=xml.ReadBool(std::string("DebugModeFlag_")+RDK::sntoa(i),false);
  Config.ChannelsConfig[i].EventsLogMode=xml.ReadBool(std::string("EventsLogMode_")+RDK::sntoa(i),false);

  Config.ChannelsConfig[i].MinInterstepsInterval=xml.ReadInteger(std::string("MinInterstepsInterval_")+RDK::sntoa(i),20);

  Config.ChannelsConfig[i].ModelFileName=xml.ReadString(std::string("ModelFileName_")+RDK::sntoa(i),"");
  Config.ChannelsConfig[i].ParametersFileName=xml.ReadString(std::string("ParametersFileName_")+RDK::sntoa(i),"");
  Config.ChannelsConfig[i].StatesFileName=xml.ReadString(std::string("StatesFileName_")+RDK::sntoa(i),"");

  Config.ChannelsConfig[i].DebugSysEventsMask=xml.ReadUnsigned(std::string("DebugSysEventsMask_")+RDK::sntoa(i),Config.ChannelsConfig[i].DebugSysEventsMask);
  Config.ChannelsConfig[i].DebuggerMessageFlag=xml.ReadBool(std::string("DebuggerMessageFlag_")+RDK::sntoa(i),false);
  Config.ChannelsConfig[i].MaxCalculationModelTime=xml.ReadFloat(std::string("MaxCalculationModelTime_")+RDK::sntoa(i), 0.0);
  Config.ChannelsConfig[i].UseIndTimeStepFlag=xml.ReadBool(std::string("UseIndTimeStepFlag")+RDK::sntoa(i), false);
 }

 // TODO: Реализовать загрузку описания
/*
 if(descriptionfilename.Length() != 0 && FileExists(descriptionfilename))
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->Visible=false;

  RichEdit->PlainText=true;

  RichEdit->Lines->LoadFromFile(descriptionfilename);
  ProjectDescription=RichEdit->Text;
  delete RichEdit;
 }      */
 return true;
}

bool UProject::ReadFromXmlNew(USerStorageXML &xml)
{
 xml.SelectNodeRoot("Project/MultiGeneral");
 Config.DebugMode=xml.ReadBool("DebugModeFlag",false);
 Config.DebugSysEventsMask=xml.ReadUnsigned("DebugSysEventsMask",0);
 Config.DebuggerMessageFlag=xml.ReadBool("DebuggerMessageFlag",false);
 Config.EventsLogMode=xml.ReadBool("EventsLogMode",false);
 Config.OverrideLogParameters=xml.ReadBool("OverrideLogParameters",true);

 Config.MultiThreadingMode=xml.ReadInteger("EnginesMode",0);

 Config.EventsLogFlag=xml.ReadBool("EventsLogEnabled",true);
 Config.ProjectShowChannelsStates=xml.ReadBool("ProjectShowChannelsStates",true);
 Config.GuiUpdateMode=xml.ReadInteger("GuiUpdateMode",0);

 int calc_time_mode=xml.ReadInteger("CalculationTimeSourceMode",0);

 Config.ProjectMode=xml.ReadInteger("ProjectMode",1);

 Config.MTUpdateInterfaceInterval=xml.ReadInteger("MTUpdateInterfaceInterval",30);
 int num_engines=xml.ReadInteger("NumEngines",1);
 if(num_engines<=0)
  num_engines=1;

 Config.DisableStopVideoSources=xml.ReadBool("DisableStopVideoSources",false);

 Config.NumChannels=num_engines;
 Config.ChannelsConfig.resize(num_engines);

// int selected_channel_index=xml.ReadInteger("SelectedEngineIndex",0);

 xml.SelectNodeRoot("Project/General");

 Config.ProjectName=xml.ReadString("ProjectName","NoName").c_str();
 Config.UserName = xml.ReadString("UserName","");
 Config.UserId = xml.ReadInteger("UserId", -1);
 Config.CreationTime = xml.ReadString("CreationTime","");

 // Флаг автоматического сохранения проекта
 Config.ProjectAutoSaveFlag=xml.ReadInteger("ProjectAutoSaveFlag",1);

 // Флаг автоматического сохранения проекта
 Config.ProjectAutoSaveStatesFlag=xml.ReadInteger("ProjectAutoSaveStateFlag",0);

 std::string descriptionfilename=xml.ReadString("ProjectDescriptionFileName","");
 Config.DescriptionFileName=descriptionfilename;

 Config.InterfaceFileName=xml.ReadString("InterfaceFileName","");

 Config.CalcSourceTimeMode=calc_time_mode;

 xml.SelectNodeRoot("Project/Channels");
 int num_ch_nodes=xml.GetNumNodes();

 num_ch_nodes=(num_ch_nodes<num_engines)?num_ch_nodes:num_engines;
 if(num_ch_nodes == 0)
 {
  Config.ChannelsConfig[0].PredefinedStructure=0;
  Config.ChannelsConfig[0].DefaultTimeStep=30;
  Config.ChannelsConfig[0].GlobalTimeStep=30;
  Config.ChannelsConfig[0].CalculationMode=1;
  Config.ChannelsConfig[0].InitAfterLoad=1;
  Config.ChannelsConfig[0].ResetAfterLoad=true;
  Config.ChannelsConfig[0].DebugMode=false;
  Config.ChannelsConfig[0].MinInterstepsInterval=20;

  Config.ChannelsConfig[0].ModelFileName="";
  Config.ChannelsConfig[0].ParametersFileName="";
  Config.ChannelsConfig[0].StatesFileName="";
  Config.ChannelsConfig[0].EventsLogMode=false;
  Config.ChannelsConfig[0].DebugSysEventsMask=Config.ChannelsConfig[0].DebugSysEventsMask;
  Config.ChannelsConfig[0].DebuggerMessageFlag=false;
  Config.ChannelsConfig[0].MaxCalculationModelTime=0.0;
  Config.ChannelsConfig[0].UseIndTimeStepFlag=false;
 }
 else
 for(int i=0;i<num_engines;i++)
 {
  if(!xml.SelectNode(sntoa(i,2)))
   continue;
  Config.ChannelsConfig[i].PredefinedStructure=xml.ReadInteger("PredefinedStructure",0);
  Config.ChannelsConfig[i].DefaultTimeStep=xml.ReadInteger("DefaultTimeStep",30);
  Config.ChannelsConfig[i].GlobalTimeStep=xml.ReadInteger("GlobalTimeStep",30);
  Config.ChannelsConfig[i].CalculationMode=xml.ReadInteger("CalculationMode",1);
  Config.ChannelsConfig[i].InitAfterLoad=xml.ReadBool("InitAfterLoadFlag",1);
  Config.ChannelsConfig[i].ResetAfterLoad=xml.ReadBool("ResetAfterLoadFlag",true);
  Config.ChannelsConfig[i].DebugMode=xml.ReadBool("DebugModeFlag",false);
  Config.ChannelsConfig[i].EventsLogMode=xml.ReadBool("EventsLogMode",false);

  Config.ChannelsConfig[i].MinInterstepsInterval=xml.ReadInteger("MinInterstepsInterval",20);

  Config.ChannelsConfig[i].ModelFileName=xml.ReadString("ModelFileName","");
  Config.ChannelsConfig[i].ParametersFileName=xml.ReadString("ParametersFileName","");
  Config.ChannelsConfig[i].StatesFileName=xml.ReadString("StatesFileName","");

  Config.ChannelsConfig[i].DebugSysEventsMask=xml.ReadUnsigned("DebugSysEventsMask",Config.ChannelsConfig[i].DebugSysEventsMask);
  Config.ChannelsConfig[i].DebuggerMessageFlag=xml.ReadBool("DebuggerMessageFlag",false);
  Config.ChannelsConfig[i].MaxCalculationModelTime=xml.ReadFloat("MaxCalculationModelTime", 0.0);
  Config.ChannelsConfig[i].UseIndTimeStepFlag=xml.ReadBool("UseIndTimeStepFlag", false);
  xml.SelectUp();
 }
 return true;
}

bool UProject::WriteToXmlOld(USerStorageXML &xml)
{
 xml.SelectRoot();
 xml.SetNodeAttribute("Version","1.0");

 xml.SelectNodeRoot("Project/MultiGeneral");
 xml.WriteBool("DebugModeFlag",Config.DebugMode);
 xml.WriteUnsigned("DebugSysEventsMask",Config.DebugSysEventsMask);
 xml.WriteBool("DebuggerMessageFlag",Config.DebuggerMessageFlag);
 xml.WriteBool("EventsLogMode",Config.EventsLogMode);
 xml.WriteBool("OverrideLogParameters",Config.OverrideLogParameters);

 xml.SelectNodeRoot("Project/General");

 if(Config.InterfaceFileName.empty())
  Config.InterfaceFileName="Interface.xml";
 xml.WriteString("InterfaceFileName",Config.InterfaceFileName);

 if(Config.DescriptionFileName.empty())
  Config.DescriptionFileName="Description.rtf";
 xml.WriteString("ProjectDescriptionFileName",Config.DescriptionFileName);

 xml.WriteInteger("ProjectAutoSaveFlag",Config.ProjectAutoSaveFlag);
 xml.WriteInteger("ProjectAutoSaveStateFlag",Config.ProjectAutoSaveStatesFlag);
 xml.WriteInteger("MTUpdateInterfaceInterval",Config.MTUpdateInterfaceInterval);
 xml.WriteInteger("GuiUpdateMode",Config.GuiUpdateMode);

 for(int i=0;i<Config.NumChannels;i++)
 {
  TProjectChannelConfig &channel_config=Config.ChannelsConfig[i];
  if(channel_config.ModelFileName.empty())
  {
   if(i == 0)
   {
	channel_config.ModelFileName="model.xml";
	xml.WriteString("ModelFileName",channel_config.ModelFileName);
   }
   else
   {
	channel_config.ModelFileName=std::string("model_")+RDK::sntoa(i)+".xml";
	xml.WriteString(std::string("ModelFileName_")+RDK::sntoa(i),channel_config.ModelFileName);
   }
  }
  else
  {
   if(i == 0)
	xml.WriteString("ModelFileName",channel_config.ModelFileName);
   else
	xml.WriteString(std::string("ModelFileName_")+RDK::sntoa(i),channel_config.ModelFileName);
  }

  if(channel_config.ParametersFileName.empty())
  {
   if(i == 0)
   {
	channel_config.ParametersFileName="Parameters.xml";
	xml.WriteString("ParametersFileName",channel_config.ParametersFileName);
  }
   else
   {
	channel_config.ParametersFileName=std::string("Parameters_")+RDK::sntoa(i)+".xml";
	xml.WriteString(std::string("ParametersFileName_")+RDK::sntoa(i),channel_config.ParametersFileName);
   }
  }
  else
  {
   if(i == 0)
	xml.WriteString("ParametersFileName",channel_config.ParametersFileName);
   else
	xml.WriteString(std::string("ParametersFileName_")+RDK::sntoa(i),channel_config.ParametersFileName);
  }

  if(Config.ProjectAutoSaveStatesFlag)
  {
   if(channel_config.StatesFileName.empty())
   {
	if(i == 0)
	{
	 channel_config.StatesFileName="States.xml";
	 xml.WriteString("StatesFileName",channel_config.StatesFileName);
	}
	else
	{
	 channel_config.StatesFileName=std::string("States_")+RDK::sntoa(i)+".xml";
	 xml.WriteString(std::string("StatesFileName_")+RDK::sntoa(i),channel_config.StatesFileName);
	}
   }
   else
   {
	if(i == 0)
	 xml.WriteString("StatesFileName",channel_config.StatesFileName);
	else
	 xml.WriteString(std::string("StatesFileName_")+RDK::sntoa(i),channel_config.StatesFileName);
   }
  }

  if(i == 0)
  {
   xml.WriteInteger("PredefinedStructure",channel_config.PredefinedStructure);
   // Шаг счета по умолчанию
   xml.WriteInteger("DefaultTimeStep",channel_config.DefaultTimeStep);

   // Глобальный шаг счета модели
   xml.WriteInteger("GlobalTimeStep",channel_config.GlobalTimeStep);

   xml.WriteInteger("CalculationMode",channel_config.CalculationMode);
   xml.WriteInteger("MinInterstepsInterval",channel_config.MinInterstepsInterval);
   xml.WriteBool("InitAfterLoadFlag",channel_config.InitAfterLoad);
   xml.WriteBool("ResetAfterLoadFlag",channel_config.ResetAfterLoad);
   xml.WriteBool("DebugModeFlag",channel_config.DebugMode);
   xml.WriteBool("EventsLogMode",channel_config.EventsLogMode);
   xml.WriteUnsigned("DebugSysEventsMask",channel_config.DebugSysEventsMask);
   xml.WriteBool("DebuggerMessageFlag",channel_config.DebuggerMessageFlag);
   xml.WriteFloat("MaxCalculationModelTime",channel_config.MaxCalculationModelTime);
   xml.WriteBool("UseIndTimeStepFlag",channel_config.UseIndTimeStepFlag);
  }
  else
  {
   std::string suffix=RDK::sntoa(i);
   xml.WriteInteger(std::string("PredefinedStructure_")+suffix,channel_config.PredefinedStructure);
   // Шаг счета по умолчанию
   xml.WriteInteger(std::string("DefaultTimeStep_")+suffix,channel_config.DefaultTimeStep);

   // Глобальный шаг счета модели
   xml.WriteInteger(std::string("GlobalTimeStep_")+suffix,channel_config.GlobalTimeStep);

   xml.WriteInteger(std::string("CalculationMode_")+suffix,channel_config.CalculationMode);

   xml.WriteInteger(std::string("MinInterstepsInterval_")+suffix,channel_config.MinInterstepsInterval);
   xml.WriteBool(std::string("InitAfterLoadFlag_")+suffix,channel_config.InitAfterLoad);
   xml.WriteBool(std::string("ResetAfterLoadFlag_")+suffix,channel_config.ResetAfterLoad);
   xml.WriteBool(std::string("DebugModeFlag_")+suffix,channel_config.DebugMode);
   xml.WriteBool(std::string("EventsLogMode_")+suffix,channel_config.EventsLogMode);
   xml.WriteUnsigned(std::string("DebugSysEventsMask_")+suffix,channel_config.DebugSysEventsMask);
   xml.WriteBool(std::string("DebuggerMessageFlag_")+suffix,channel_config.DebuggerMessageFlag);
   xml.WriteFloat("MaxCalculationModelTime_"+suffix,channel_config.MaxCalculationModelTime);
   xml.WriteBool(std::string("UseIndTimeStepFlag_")+suffix,channel_config.UseIndTimeStepFlag);
  }
 }

 xml.WriteInteger("ProjectAutoSaveFlag",Config.ProjectAutoSaveFlag);

 // Флаг автоматического сохранения проекта
 xml.WriteInteger("ProjectAutoSaveStateFlag",Config.ProjectAutoSaveStatesFlag);

 // Число входов среды
// xml.WriteInteger("NumEnvInputs",NumEnvInputs);

 // Число выходов среды
// xml.WriteInteger("NumEnvOutputs",NumEnvOutputs);

 xml.WriteInteger("ProjectMode",Config.ProjectMode);

// xml.WriteInteger("InputEnvImageWidth",InputEnvImageWidth);
// xml.WriteInteger("InputEnvImageHeight",InputEnvImageHeight);

// xml.WriteBool("ReflectionFlag",ReflectionFlag);

 xml.WriteString("ProjectName",Config.ProjectName);

 xml.SelectNodeRoot("Project/MultiGeneral");
 xml.WriteInteger("EnginesMode",Config.MultiThreadingMode);
 xml.WriteInteger("CalculationTimeSourceMode",Config.CalcSourceTimeMode);

 xml.WriteInteger("NumEngines",Config.NumChannels);
// xml.WriteInteger("SelectedEngineIndex",GetSelectedEngineIndex());

 xml.WriteBool("ProjectShowChannelsStates",Config.ProjectShowChannelsStates);

 xml.WriteBool("DisableStopVideoSources",Config.DisableStopVideoSources);
 return true;
}

bool UProject::WriteToXmlNew(USerStorageXML &xml)
{
 xml.SelectRoot();
 if(ForceNewConfigFilesStructure)
  xml.SetNodeAttribute("Version","2.1");
 else
  xml.SetNodeAttribute("Version","2.0");
 xml.SelectNodeRoot("Project/MultiGeneral");
 xml.WriteBool("DebugModeFlag",Config.DebugMode);
 xml.WriteUnsigned("DebugSysEventsMask",Config.DebugSysEventsMask);
 xml.WriteBool("DebuggerMessageFlag",Config.DebuggerMessageFlag);
 xml.WriteBool("EventsLogMode",Config.EventsLogMode);
 xml.WriteBool("OverrideLogParameters",Config.OverrideLogParameters);

 xml.SelectNodeRoot("Project/General");
 xml.DelNodeInternalContent();

 if(Config.InterfaceFileName.empty())
  Config.InterfaceFileName="Interface.xml";
 xml.WriteString("InterfaceFileName",Config.InterfaceFileName);

 if(Config.DescriptionFileName.empty())
  Config.DescriptionFileName="Description.rtf";
 xml.WriteString("ProjectDescriptionFileName",Config.DescriptionFileName);

 xml.WriteInteger("ProjectAutoSaveFlag",Config.ProjectAutoSaveFlag);
 xml.WriteInteger("ProjectAutoSaveStateFlag",Config.ProjectAutoSaveStatesFlag);
 xml.WriteInteger("MTUpdateInterfaceInterval",Config.MTUpdateInterfaceInterval);
 xml.WriteInteger("GuiUpdateMode",Config.GuiUpdateMode);

 xml.WriteInteger("ProjectAutoSaveFlag",Config.ProjectAutoSaveFlag);

 // Флаг автоматического сохранения проекта
 xml.WriteInteger("ProjectAutoSaveStateFlag",Config.ProjectAutoSaveStatesFlag);

 xml.WriteInteger("ProjectMode",Config.ProjectMode);

 xml.WriteString("ProjectName",Config.ProjectName);

 xml.WriteString("UserName",Config.UserName);
 xml.WriteInteger("UserId", Config.UserId);
 xml.WriteString("CreationTime",Config.CreationTime);

 xml.SelectNodeRoot("Project/MultiGeneral");
 xml.WriteInteger("EnginesMode",Config.MultiThreadingMode);
 xml.WriteInteger("CalculationTimeSourceMode",Config.CalcSourceTimeMode);

 xml.WriteInteger("NumEngines",Config.NumChannels);
// xml.WriteInteger("SelectedEngineIndex",GetSelectedEngineIndex());

 xml.WriteBool("ProjectShowChannelsStates",Config.ProjectShowChannelsStates);

 xml.WriteBool("DisableStopVideoSources",Config.DisableStopVideoSources);

 xml.SelectNodeRoot("Project/Channels");
 for(int i=0;i<Config.NumChannels;i++)
 {
  TProjectChannelConfig &channel_config=Config.ChannelsConfig[i];
  if(!xml.SelectNodeForce(sntoa(i,2)))
   continue;

  if(channel_config.ModelFileName.empty())
  {
   if(ForceNewConfigFilesStructure)
	channel_config.ModelFileName=std::string("Model_")+RDK::sntoa(i,2)+".xml";
   else
	channel_config.ModelFileName=(i==0)?std::string("model.xml"):std::string("model_")+RDK::sntoa(i)+".xml";
  }
  xml.WriteString("ModelFileName",channel_config.ModelFileName);

  if(channel_config.ParametersFileName.empty())
  {
   if(ForceNewConfigFilesStructure)
	channel_config.ParametersFileName=std::string("Parameters_")+RDK::sntoa(i,2)+".xml";
   else
	channel_config.ParametersFileName=(i==0)?std::string("Parameters.xml"):std::string("Parameters_")+RDK::sntoa(i)+".xml";
  }
  xml.WriteString("ParametersFileName",channel_config.ParametersFileName);

  if(Config.ProjectAutoSaveStatesFlag)
  {
   if(channel_config.StatesFileName.empty())
   {
	if(ForceNewConfigFilesStructure)
	 channel_config.StatesFileName=std::string("States_")+RDK::sntoa(i,2)+".xml";
	else
	 channel_config.StatesFileName=(i==0)?std::string("States.xml"):std::string("States_")+RDK::sntoa(i)+".xml";
   }
   xml.WriteString("StatesFileName",channel_config.StatesFileName);
  }

  xml.WriteInteger("PredefinedStructure",channel_config.PredefinedStructure);

  // Шаг счета по умолчанию
  xml.WriteInteger("DefaultTimeStep",channel_config.DefaultTimeStep);

  // Глобальный шаг счета модели
  xml.WriteInteger("GlobalTimeStep",channel_config.GlobalTimeStep);

  xml.WriteInteger("CalculationMode",channel_config.CalculationMode);
  xml.WriteInteger("MinInterstepsInterval",channel_config.MinInterstepsInterval);
  xml.WriteBool("InitAfterLoadFlag",channel_config.InitAfterLoad);
  xml.WriteBool("ResetAfterLoadFlag",channel_config.ResetAfterLoad);
  xml.WriteBool("DebugModeFlag",channel_config.DebugMode);
  xml.WriteBool("EventsLogMode",channel_config.EventsLogMode);
  xml.WriteUnsigned("DebugSysEventsMask",channel_config.DebugSysEventsMask);
  xml.WriteBool("DebuggerMessageFlag",channel_config.DebuggerMessageFlag);
  xml.WriteFloat("MaxCalculationModelTime",channel_config.MaxCalculationModelTime);
  xml.WriteBool("UseIndTimeStepFlag",channel_config.UseIndTimeStepFlag);

  xml.SelectUp();
 }

 return true;
}

bool UProject::FixSavePoint(USerStorageXML &xml)
{
    xml.SelectNodeRoot("History");

    int size = 0;

    std::string size_str = xml.GetNodeAttribute("Size");
    if(!size_str.empty())
        size = RDK::atoi(size_str);

    size++;

    xml.SetNodeAttribute("Size",RDK::sntoa(size));

    xml.SelectNodeForce("save_point_"+RDK::sntoa(size));
    xml.SetNodeAttribute("UserName",Config.UserName);
    xml.SetNodeAttribute("UserId",  RDK::sntoa(Config.UserId));

    time_t time_data;
    time(&time_data);
    xml.SetNodeAttribute("Time",  RDK::get_text_time(time_data, '.', '_'));

    xml.SelectRoot();
}
// --------------------------

// --------------------------
/// Управление числом каналов
// --------------------------
int UProject::GetNumChannels(void) const
{
 return Config.NumChannels;
}

bool UProject::SetNumChannels(int num)
{
 if(!Config.SetNumChannels(num))
  return false;

 ModifiedFlag=true;
 return true;
}

bool UProject::InsertChannel(int index)
{
 if(!Config.InsertChannel(index))
  return false;

 ModifiedFlag=true;
 return true;
}

bool UProject::DeleteChannel(int index)
{
 if(!Config.DeleteChannel(index))
  return false;

 ModifiedFlag=true;
 return true;

}
// --------------------------



}

#endif

