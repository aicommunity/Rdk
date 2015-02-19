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

}

#endif

