#ifndef UAppProject_CPP
#define UAppProject_CPP

#include "UProject.h"


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
}

TProjectChannelConfig::TProjectChannelConfig(const TProjectChannelConfig& copy)
{
 ModelMode=copy.ModelMode;
 PredefinedStructure=copy.PredefinedStructure;
 ClassName=copy.ClassName;
 ModelFileName=copy.ModelFileName;

 GlobalTimeStep=copy.GlobalTimeStep;

 DefaultTimeStep=copy.DefaultTimeStep;

 CalculationMode=copy.CalculationMode;

 MinInterstepsInterval=copy.MinInterstepsInterval;

 InitAfterLoad=copy.InitAfterLoad;

 ResetAfterLoad=copy.ResetAfterLoad;

 DebugMode=copy.DebugMode;

 ChannelName=copy.ChannelName;
}

bool TProjectChannelConfig::operator != (const TProjectChannelConfig& copy) const
{
 return (ModelMode != copy.ModelMode) |
 (PredefinedStructure != copy.PredefinedStructure) |
 (ClassName != copy.ClassName) |
 (ModelFileName != copy.ModelFileName) |
 (GlobalTimeStep != copy.GlobalTimeStep) |
 (DefaultTimeStep != copy.DefaultTimeStep) |
 (CalculationMode != copy.CalculationMode) |
 (MinInterstepsInterval != copy.MinInterstepsInterval) |
 (InitAfterLoad != copy.InitAfterLoad) |
 (ResetAfterLoad != copy.ResetAfterLoad) |
 (DebugMode != copy.DebugMode) |
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

 NumChannels=1;
 ChannelsConfig.resize(1);
}

TProjectConfig::TProjectConfig(const TProjectConfig& copy)
{
 ProjectDirectory=copy.ProjectDirectory;

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
}

bool TProjectConfig::operator != (const TProjectConfig& copy) const
{
 return (ProjectDirectory != copy.ProjectDirectory) |
 (ProjectName != copy.ProjectName) |
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
 (ChannelsConfig != copy.ChannelsConfig);
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
// --------------------------


}

#endif

