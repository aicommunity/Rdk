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

 ClassName="Model";

 GlobalTimeStep=30;

 DefaultTimeStep=30;

 CalculationMode=2;

 MinInterstepsInterval=0;

 InitAfterLoad=true;

 ResetAfterLoad=true;

 DebugMode=true;

 EventsLogMode=false;
}

TProjectChannelConfig::TProjectChannelConfig(const TProjectChannelConfig& copy)
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
 EventsLogMode=copy.EventsLogMode;


 ChannelName=copy.ChannelName;
}

bool TProjectChannelConfig::operator != (const TProjectChannelConfig& copy) const
{
 return (ModelMode != copy.ModelMode) |
 (PredefinedStructure != copy.PredefinedStructure) |
 (ClassName != copy.ClassName) |
 (ModelFileName != copy.ModelFileName) |
 (ParametersFileName != copy.ParametersFileName) |
 (StatesFileName != copy.StatesFileName) |
 (GlobalTimeStep != copy.GlobalTimeStep) |
 (DefaultTimeStep != copy.DefaultTimeStep) |
 (CalculationMode != copy.CalculationMode) |
 (MinInterstepsInterval != copy.MinInterstepsInterval) |
 (InitAfterLoad != copy.InitAfterLoad) |
 (ResetAfterLoad != copy.ResetAfterLoad) |
 (DebugMode != copy.DebugMode) |
 (EventsLogMode != copy.EventsLogMode) |
 (ChannelName != copy.ChannelName);
}

bool TProjectChannelConfig::operator == (const TProjectChannelConfig& copy) const
{
 return ((*this) != copy);
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

 NumChannels=1;
 ChannelsConfig.resize(1);
}

TProjectConfig::TProjectConfig(const TProjectConfig& copy)
{
 ProjectName=copy.ProjectName;

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

 // Флаг отображения состояний каналов
 ProjectShowChannelsStates=copy.ProjectShowChannelsStates;

 InterfaceFileName=copy.InterfaceFileName;

 DescriptionFileName=copy.DescriptionFileName;
}

bool TProjectConfig::operator != (const TProjectConfig& copy) const
{
 return (ProjectName != copy.ProjectName) |
 (ProjectDescription != copy.ProjectDescription) |
 (ProjectAutoSaveFlag != copy.ProjectAutoSaveFlag) |
 (ProjectAutoSaveStatesFlag != copy.ProjectAutoSaveStatesFlag) |
 (EventsLogFlag != copy.EventsLogFlag) |
 (ProjectMode != copy.ProjectMode) |
 (ProjectType != copy.ProjectType) |
 (MultiThreadingMode != copy.MultiThreadingMode) |
 (CalcSourceTimeMode != copy.CalcSourceTimeMode) |
 (ShowChannelsStateFlag != copy.ShowChannelsStateFlag) |
 (ReflectionFlag != copy.ReflectionFlag) |
 (DisableStopVideoSources != copy.DisableStopVideoSources) |
 (NumChannels != copy.NumChannels) |
 (ChannelsConfig != copy.ChannelsConfig) |
 (ServerInterfaceAddress != copy.ServerInterfaceAddress) |
 (ServerInterfacePort != copy.ServerInterfacePort) |
 (ProjectShowChannelsStates != copy.ProjectShowChannelsStates) |
 (InterfaceFileName != copy.InterfaceFileName) |
 (DescriptionFileName != copy.DescriptionFileName);
}

bool TProjectConfig::operator == (const TProjectConfig& copy) const
{
 return !((*this) != copy);
}


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProject::UProject(void)
{
 ModifiedFlag=false;
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

 xml.SelectNodeRoot("Project/MultiGeneral");
 Config.MultiThreadingMode=xml.ReadInteger("EnginesMode",0);

 Config.EventsLogFlag=xml.ReadBool("EventsLogEnabled",true);
 Config.ProjectShowChannelsStates=xml.ReadBool("ProjectShowChannelsStates",true);

 int calc_time_mode=xml.ReadInteger("CalculationTimeSourceMode",0);

 Config.ProjectMode=xml.ReadInteger("ProjectMode",1);

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

// int selected_engine_index=xml.ReadInteger("SelectedEngineIndex",0);

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
 if(extract_file_path(descriptionfilename).empty())
  descriptionfilename=ProjectPath+descriptionfilename;

 Config.InterfaceFileName=xml.ReadString("InterfaceFileName","");

 Config.CalcSourceTimeMode=calc_time_mode;
 Config.ChannelsConfig[0].PredefinedStructure=xml.ReadInteger("PredefinedStructure",0);
 Config.ChannelsConfig[0].DefaultTimeStep=xml.ReadInteger("DefaultTimeStep",30);
 Config.ChannelsConfig[0].GlobalTimeStep=xml.ReadInteger("GlobalTimeStep",30);
 Config.ChannelsConfig[0].CalculationMode=xml.ReadInteger("CalculationMode",2);
 Config.ChannelsConfig[0].InitAfterLoad=xml.ReadBool("InitAfterLoadFlag",1);
 Config.ChannelsConfig[0].ResetAfterLoad=xml.ReadBool("ResetAfterLoadFlag",true);
 Config.ChannelsConfig[0].DebugMode=xml.ReadBool("DebugModeFlag",false);
 Config.ChannelsConfig[0].MinInterstepsInterval=xml.ReadInteger("MinInterstepsInterval",20);

 Config.ChannelsConfig[0].ModelFileName=xml.ReadString("ModelFileName","");
 Config.ChannelsConfig[0].ParametersFileName=xml.ReadString("ParametersFileName","");
 Config.ChannelsConfig[0].StatesFileName=xml.ReadString("StatesFileName","");
 Config.ChannelsConfig[0].EventsLogMode=xml.ReadBool("EventsLogMode",false);

 for(int i=1;i<num_engines;i++)
 {
 // Config.ChannelsConfig[i].CalcSourceTimeMode=calc_time_mode;
  Config.ChannelsConfig[i].PredefinedStructure=xml.ReadInteger(std::string("PredefinedStructure_")+RDK::sntoa(i),0);
  Config.ChannelsConfig[i].DefaultTimeStep=xml.ReadInteger(std::string("DefaultTimeStep_")+RDK::sntoa(i),30);
  Config.ChannelsConfig[i].GlobalTimeStep=xml.ReadInteger(std::string("GlobalTimeStep_")+RDK::sntoa(i),30);
  Config.ChannelsConfig[i].CalculationMode=xml.ReadInteger(std::string("CalculationMode_")+RDK::sntoa(i),2);
  Config.ChannelsConfig[i].InitAfterLoad=xml.ReadBool(std::string("InitAfterLoadFlag_")+RDK::sntoa(i),1);
  Config.ChannelsConfig[i].ResetAfterLoad=xml.ReadBool(std::string("ResetAfterLoadFlag_")+RDK::sntoa(i),true);
  Config.ChannelsConfig[i].DebugMode=xml.ReadBool(std::string("DebugModeFlag_")+RDK::sntoa(i),false);
  Config.ChannelsConfig[i].EventsLogMode=xml.ReadBool(std::string("EventsLogMode_")+RDK::sntoa(i),false);

  Config.ChannelsConfig[i].MinInterstepsInterval=xml.ReadInteger(std::string("MinInterstepsInterval_")+RDK::sntoa(i),20);

  Config.ChannelsConfig[i].ModelFileName=xml.ReadString(std::string("ModelFileName_")+RDK::sntoa(i),"");
  Config.ChannelsConfig[i].ParametersFileName=xml.ReadString(std::string("ParametersFileName_")+RDK::sntoa(i),"");
  Config.ChannelsConfig[i].StatesFileName=xml.ReadString(std::string("StatesFileName_")+RDK::sntoa(i),"");
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


/// Сохраняет конфигурацию проекта в xml
bool UProject::WriteToXml(USerStorageXML &xml)
{
 xml.SelectNodeRoot("Project/MultiGeneral");

 if(Config.InterfaceFileName.empty())
  Config.InterfaceFileName="Interface.xml";

 xml.WriteString("InterfaceFileName",Config.InterfaceFileName);

 xml.SelectNodeRoot("Project/General");

 if(Config.DescriptionFileName.empty())
  Config.DescriptionFileName="Description.rtf";
 xml.WriteString("ProjectDescriptionFileName",Config.DescriptionFileName);

 // TODO: реализовать сохранение Описания
 /*
 if(descriptionfilename.Length() != 0)
 {
  TRichEdit* RichEdit=new TRichEdit(this);
  RichEdit->Parent=this;
  RichEdit->PlainText=true;
  RichEdit->Visible=false;
  RichEdit->Text=ProjectDescription;

  if(ExtractFilePath(descriptionfilename).Length() == 0)
   RichEdit->Lines->SaveToFile(ProjectPath+descriptionfilename);
  else
   RichEdit->Lines->SaveToFile(descriptionfilename);
  delete RichEdit;
 }*/

 xml.WriteInteger("ProjectAutoSaveFlag",Config.ProjectAutoSaveFlag);
 xml.WriteInteger("ProjectAutoSaveStateFlag",Config.ProjectAutoSaveStatesFlag);

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
// --------------------------


}

#endif

