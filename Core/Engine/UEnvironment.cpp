/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAENVIRONMENT_CPP
#define UAENVIRONMENT_CPP

#include <string.h>
#include "limits.h"
#include "UEnvironment.h"
#include "../System/rdk_system.h"
#include "../Application/UIVisualController.h"


namespace RDK {

// --------------------------
// Constructors & destructors
// --------------------------
UEnvironment::UEnvironment(void)
{
 LogMutex=UCreateMutex();
 // ���������
 // ������ ������������� �������� ������ ���������
 // 0 - ��������� ������������ �����
 PredefinedStructure=0;

 // ���������
 // ������� �������� �������������
 Initialized=false;

 // ������� ������� �������������� ���������
 Structured=false;

 Model=0;
 Storage=0;
 StoragePresent=false;

 MaxModelDuration=100;

 // ������� ��������� ������
// CurrentComponent=0;

 StartupTime=0;
 CurrentTime=0;
 LastDuration=1;
 ProcEndTime=0;
 MinInterstepsInterval=0;

 CurrentExceptionsLogSize=0;
 ExceptionHandler=0;

 LastReadExceptionLogIndex=0;
 MaxExceptionsLogSize=1000;

 LastErrorLevel=INT_MAX;
 DebugMode=false;
 ChannelIndex=0;
 LastStepStartTime=0;

 LogIndex=1;
}

UEnvironment::~UEnvironment(void)
{
 DestroyModel();
 if(LogMutex)
  UDestroyMutex(LogMutex);
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ������ ������������� �������� ������ ���������
int UEnvironment::GetPredefinedStructure(void) const
{
 return PredefinedStructure;
}

bool UEnvironment::SetPredefinedStructure(int value)
{
 if(PredefinedStructure == value)
  return true;

 PredefinedStructure=value;
 return DestroyStructure();
}


// ������������� ���������� ������, ������� ����� �������������
const ULongId& UEnvironment::GetModelCalculationComponent(void) const
{
 return ModelCalculationComponent;
}

bool UEnvironment::SetModelCalculationComponent(const ULongId& value)
{
 if(ModelCalculationComponent == value)
  return true;

 ModelCalculationComponent=value;

 return true;
}

// ��� �������� �������� �������� ������
const std::string& UEnvironment::GetCurrentDataDir(void)
{
 return CurrentDataDir;
}

void UEnvironment::SetCurrentDataDir(const std::string& dir)
{
 CurrentDataDir=dir;

 for(std::string::size_type i=0; i<CurrentDataDir.size();i++)
  if(CurrentDataDir[i] == '\\')
   CurrentDataDir[i]='/';

 if(CurrentDataDir.size()>0 && CurrentDataDir[CurrentDataDir.size()-1] != '/')
  CurrentDataDir+='/';
}

// ��� �������� �������� ������
const std::string& UEnvironment::GetSystemDir(void) const
{
 return SystemDir;
}

void UEnvironment::SetSystemDir(const std::string& dir)
{
 SystemDir=dir;
 for(std::string::size_type i=0; i<SystemDir.size();i++)
  if(SystemDir[i] == '\\')
   SystemDir[i]='/';

 if(SystemDir.size()>0 && SystemDir[SystemDir.size()-1] != '/')
  SystemDir+='/';
}

/// ����������� �������� ������� ����� ���������� ����� (��)
long long UEnvironment::GetMinInterstepsInterval(void) const
{
 return MinInterstepsInterval;
}

bool UEnvironment::SetMinInterstepsInterval(long long value)
{
 if(MinInterstepsInterval == value)
  return true;

 MinInterstepsInterval = value;
 return true;
}

/// ���� ��������� ������ �������
bool UEnvironment::GetDebugMode(void) const
{
 return DebugMode;
}

bool UEnvironment::SetDebugMode(bool value)
{
 if(DebugMode == value)
  return true;

 DebugMode=value;
 return true;
}


/// ���� ��������� ���������� ����������� ������� � ���-����
/// true - ����������� ��������
bool UEnvironment::GetEventsLogMode(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return EventsLogMode;
}

bool UEnvironment::SetEventsLogMode(bool value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 EventsLogMode=value;
 return true;
}
// --------------------------


// --------------------------
// ������ ���������� �����������
// --------------------------
// ������� ������� �������������� ���������
bool UEnvironment::IsStructured(void) const
{
 return Structured;
}

/// ���������� �����, ����������� �� ��������� RT ������
double UEnvironment::GetRTLastDuration(void) const
{
 return double(LastDuration)/1000.0;
}

/// �����, ����������� � ������ �� ���� ����� RTCalculate;
double UEnvironment::GetRTModelCalcTime(void) const
{
 return RTModelCalcTime;
}

/// ������������������ RT ������� (��������� RTModelCalcTime/RTLastDuration)
double UEnvironment::CalcRTPerformance(void) const
{
 if(LastDuration>0)
  return RTModelCalcTime/(double(LastDuration)/1000.0);

 return 1000000; // TODO:
}
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
UStorage* UEnvironment::GetStorage(void)
{
 return Storage;
}

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
bool UEnvironment::SetStorage(UStorage *storage)
{
 if(!storage)
  return false;

 if(Storage == storage)
  return true;

 if(!DestroyModel())
  return false;

 Storage=storage;
 StoragePresent=true;

 return true;
}

// ���������� ��������� �� ������
UEPtr<UContainer> UEnvironment::GetModel(void)
{
 return Model;
}

// ������� ����� ������ �� ��������� �� ����� ������
bool UEnvironment::CreateModel(const NameT& classname)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UContainer>(GetStorage()->TakeObject(classname));
 Model->SetEnvironment(this);
 Ready=false;
 if(Model)
 {
  return true;
 }

 return false;
}

// ������� ����� ������ �� ��������� �� id ������
bool UEnvironment::CreateModel(const UId& classid)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UContainer>(Storage->TakeObject(classid));
 Model->SetEnvironment(this);
 Ready=false;
 if(Model)
  return true;

 return false;
}

// ���������� ������� ������
bool UEnvironment::DestroyModel(void)
{
 if(!Model)
  return true;

 Model->Free();
 Model=0;
 CurrentComponent=0;

 return true;
}
// --------------------------

// --------------------------
// ������ ���������� �������������
// --------------------------
// --------------------------

// --------------------------
// ��������� ������� � ������ �����
// --------------------------
// ���������� ��������� �� ������� ��������� ������
UEPtr<UContainer> UEnvironment::GetCurrentComponent(void)
{
 return CurrentComponent;
}

// ������������� ��������� �� ������� ��������� ������
void UEnvironment::SelectCurrentComponent(const NameT &name)
{
 if(name == ForbiddenName)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(name);
}

void UEnvironment::SelectCurrentComponent(const ULongId &id)
{
 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(id);
}

// ������������� ��������� �� ������� ��������� ������ �� ���� ������
void UEnvironment::ResetCurrentComponent(void)
{
 CurrentComponent=Model;
}

// ������������� ��������� �� ������� ��������� ������ �� ������������ ���������
// (������� �� ������� �����). ���� ��� ��������� �� ������, �� �� ������ ������
void UEnvironment::UpCurrentComponent(void)
{
 if(CurrentComponent == Model)
  return;

 CurrentComponent=CurrentComponent->GetOwner();
}

// ������������� ��������� �� ������� ��������� ������ �� �������� ��������� ��
// ����� ������ (������� �� ������� ����).
void UEnvironment::DownCurrentComponent(const NameT &name)
{
 CurrentComponent=GetCurrentComponent()->GetComponentL(name);
}

void UEnvironment::DownCurrentComponent(const ULongId &id)
{
 CurrentComponent=GetCurrentComponent()->GetComponentL(id);
}

/// ����� �����
const UTimeControl& UEnvironment::GetTime(void) const
{
 return Time;
}

UTimeControl& UEnvironment::GetTime(void)
{
 return Time;
}

/// ������ �������� ������ � �������������� ����������
int UEnvironment::GetChannelIndex(void) const
{
 return ChannelIndex;
}

bool UEnvironment::SetChannelIndex(int value)
{
 if(ChannelIndex == value)
  return true;

 ChannelIndex=value;
 return true;
}

/// ������
RDK::UBitmapFontCollection& UEnvironment::GetFonts(void)
{
 return Fonts;
}

bool UEnvironment::SetFonts(const RDK::UBitmapFontCollection& value)
{
 Fonts=value;
 return true;
}
// --------------------------


// --------------------------
// ������ ���������� �������������
// --------------------------
/// ������������� ���������� � ���������� ��������� ������ �������� �����
/// ������������ ������ � ���� � ������
/// ��������� ������
bool UEnvironment::RegisterSourceController(const std::string &component_name, const std::string &property_name)
{
 SourceControllerName=component_name;
 SourceControllerProperty=property_name;
 return true;
}


/// ��������� ��������� � ���� � ������ ��������� ������
bool UEnvironment::CallSourceController(void)
{
 if(SourceControllerName.empty() || SourceControllerProperty.empty())
  return false;
 try
 {
  if(!GetModel())
   return false;
  UEPtr<UContainer> cont=GetModel()->GetComponentL(SourceControllerName);

  UEPtr<UIProperty> iproperty=cont->FindProperty(SourceControllerProperty);
  bool value=true;
  if(!iproperty->ReadFromMemory(&value))
   return false;
 }
 catch(UContainer::EComponentNameNotExist &exception)
 {
  return false;
 }
 return true;
}
// --------------------------


// --------------------------
// ������ ���������� ������
// --------------------------
// ���������� ���������� ������� ������ �� ��������� ��������
void UEnvironment::IncreaseModelTimeByStep(void)
{
 Time.IncreaseModelTimeByStep(GetModel()->GetTimeStep());
}
// --------------------------

// --------------------------
// ������ ������������� �����
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � ������ ���������
// --------------------------
// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
bool UEnvironment::IsStoragePresent(void) const
{
 return StoragePresent;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������������� �����
void UEnvironment::Init(void)
{
 if(IsInit())
  return;

 SetFonts(RDK::GlobalFonts);
 AInit();

 ModelCalculationComponent.Resize(0);
 if(Model)
  Model->Init();
 Initialized=true;
 return;
}

// ��������������� �����
void UEnvironment::UnInit(void)
{
 if(Model)
  Model->UnInit();

 if(!IsInit())
  return;

 AUnInit();

 Initialized=false;
}

// ��������� �������������� �������� ������ ���������
bool UEnvironment::CreateStructure(void)
{
 if(Structured)
  return true;

 Init();

 if(!ACreateStructure())
  return false;

 Structured=true;

 if(!Build())
  return false;

 return true;
}

// ���������� ������� ������ ���������
bool UEnvironment::DestroyStructure(void)
{
 if(!Structured)
  return true;

 if(!IsReady())
  return false;

 if(!ADestroyStructure())
  return false;

 Structured=false;
 return true;
}

// ������ ������ � �������� �������
void UEnvironment::RTCalculate(void)
{
 Build();

 CurrentTime=GetCurrentStartupTime();
 Time.SetSourceCurrentLocalTime(double(GetCurrentStartupTime())/1000.0);

 // ���� ������ ��� ������� ����� Reset
 if(Time.GetTime() == 0)
 {
  Time.SetSourceStartLocalTime(Time.GetSourceCurrentLocalTime());
  Time.SetSourceStartGlobalTime(Time.GetSourceCurrentGlobalTime());
  StartupTime=CurrentTime;
  ProcEndTime=StartupTime;
 }


 unsigned long long curtime;
 unsigned long long timer_interval=0;
 //double devicemodeltime=0;

 timer_interval=CalcDiffTime(GetCurrentStartupTime(),ProcEndTime);
 if(timer_interval<=0)
  timer_interval=1;

 int i=0;
 if(LastDuration < timer_interval)
  LastDuration=timer_interval;
 double model_duration=(Time.GetRealTime()-Time.GetDoubleTime()*1e6)/1000.0;
 double model_start_calc_time=Time.GetDoubleTime();

 if(model_duration>MaxModelDuration)
  model_duration=MaxModelDuration;

 int elapsed_counter=0;
 if(model_duration>0)
 {
  elapsed_counter=int((model_duration*Model->GetTimeStep())/1000);
 }
 else
 {
  elapsed_counter=0;
 }

 curtime=GetCurrentStartupTime();
 while(curtime-CurrentTime<timer_interval && i<elapsed_counter)
 {
  Calculate();

  ++i;
  curtime=GetCurrentStartupTime();
 }

 if(Time.GetRealTime()/1e6<Time.GetDoubleTime())
 {
  Sleep(int(Time.GetDoubleTime()*1000-Time.GetRealTime()/1000));
  Time.SetSourceCurrentLocalTime(double(GetCurrentStartupTime())/1000.0);
 }

 LastDuration=CalcDiffTime(GetCurrentStartupTime(),CurrentTime);
 ProcEndTime=GetCurrentStartupTime();
 double model_stop_calc_time=Time.GetDoubleTime();
 RTModelCalcTime=model_stop_calc_time-model_start_calc_time;

 return;
}
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
// ������������ ��������� ����������
void UEnvironment::ProcessException(UException &exception) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);

 if(LastErrorLevel>exception.GetType())
  LastErrorLevel=exception.GetType();
 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize > MaxExceptionsLogSize)
 {
  int erase_size=CurrentExceptionsLogSize - MaxExceptionsLogSize-1;
  if(int(LogList.size())>erase_size)
  {
   for(int i=0;i<erase_size;i++)
    LogList.erase(LogList.begin());
  }
  else
   LogList.clear();
 }

 pair<std::string, int> log;
 log.first=sntoa(ChannelIndex)+std::string("> ")+exception.what();
 log.second=exception.GetType();
 LogList[LogIndex++]=log;

 if(EventsLogMode) // ���� ��������, �� ��������� ������� � ����
 {

 }

 if(ExceptionHandler)
  ExceptionHandler(ChannelIndex);
}

// ���������� ������ ����� ����
const char* UEnvironment::GetLog(int &error_level) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 TempString.clear();
 std::map<unsigned, pair<std::string, int> >::const_iterator I,J;
 I=LogList.begin(); J=LogList.end();
 for(;I != J;++I)
 {
  TempString+=I->second.first;
  TempString+="/r/n";
 }
 error_level=LastErrorLevel;
 LastErrorLevel=INT_MAX;
 return TempString.c_str();
}

/// ���������� ����� ����� ����
int UEnvironment::GetNumLogLines(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 return int(LogList.size());
}

/// ���������� ������ ���� � �������� i
const char* UEnvironment::GetLogLine(int i) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 std::map<unsigned, pair<std::string, int> >::const_iterator I=LogList.find(i);

 if(I == LogList.end())
 {
  TempString.clear();
  return TempString.c_str();
 }

 TempString=I->second.first;
 return TempString.c_str();
}

/// ���������� ����� ������������� ����� ����
int UEnvironment::GetNumUnreadLogLines(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);

 std::map<unsigned, pair<std::string, int> >::const_iterator I=LogList.find(LastReadExceptionLogIndex);
 if(I == LogList.end())
  return int(LogList.size());

 int size=0;
 //int(LogList.size())-LastReadExceptionLogIndex-1;
 for(;I!=LogList.end();++I)
  ++size;
 return size;
}

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* UEnvironment::GetUnreadLog(int &error_level)
{
 UGenericMutexSharedLocker lock(LogMutex);
 TempString.clear();
 error_level=INT_MAX;

 if(LogList.empty())
  return TempString.c_str();

 if(LastReadExceptionLogIndex == 0)
 {
  LastReadExceptionLogIndex=LogList.begin()->first;
  TempString=LogList.begin()->second.first;
  error_level=LogList.begin()->second.second;
 }
 else
 {
  std::map<unsigned, pair<std::string, int> >::const_iterator I=LogList.find(LastReadExceptionLogIndex);
  if(I != LogList.end())
  {
   ++I;
   if(I != LogList.end())
   {
	LastReadExceptionLogIndex=I->first;
	TempString=I->second.first;
	error_level=I->second.second;
	return TempString.c_str();
   }
  }
  else
   LastReadExceptionLogIndex=LogList.begin()->first;
 }

 error_level=RDK_EX_UNKNOWN;
 return TempString.c_str();
}

// ���������� ��������-������������ ����������
UEnvironment::PExceptionHandler UEnvironment::GetExceptionHandler(void) const
{
 return ExceptionHandler;
}

bool UEnvironment::SetExceptionHandler(PExceptionHandler value)
{
 if(ExceptionHandler == value)
  return true;

 ExceptionHandler=value;
 return true;
}

// ������������ ����� �������� ����������
// ���� 0, �� �������������
int UEnvironment::GetMaxExceptionsLogSize(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 return MaxExceptionsLogSize;
}

void UEnvironment::SetMaxExceptionsLogSize(int value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(MaxExceptionsLogSize == value)
  return;

 MaxExceptionsLogSize=value;
 if(MaxExceptionsLogSize>0 && /*ExceptionsLog.size()*/CurrentExceptionsLogSize>MaxExceptionsLogSize)
 {
  //ExceptionsLog.erase(ExceptionsLog.begin(), ExceptionsLog.begin()+int(ExceptionsLog.size())-MaxExceptionsLogSize);
  CurrentExceptionsLogSize=MaxExceptionsLogSize;
 }
}

/// ������� ���
void UEnvironment::ClearLog(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 LastReadExceptionLogIndex=0;
 CurrentExceptionsLogSize=0;
 LastErrorLevel=INT_MAX;
 LogList.clear();
}

/// ������� ��� ����������� ���������
void UEnvironment::ClearReadLog(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 std::map<unsigned, pair<std::string, int> >::iterator I=LogList.find(LastReadExceptionLogIndex);
 if(I != LogList.end())
 {
  ++I;
  std::map<unsigned, pair<std::string, int> >::iterator J=LogList.begin(),K;
  while(J != I)
  {
   K=J; ++K;
   LogList.erase(J);
   J=K;
  }
 }
 LastReadExceptionLogIndex=0;
 CurrentExceptionsLogSize=0;
 LastErrorLevel=INT_MAX;
// TempLogString.clear();
}

// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
void UEnvironment::LogMessage(int msg_level, const std::string &line)
{
 switch (msg_level)
 {
 case RDK_EX_FATAL:
 {
  EStringFatal exception(line);
  ProcessException(exception);
 }
 break;

 case RDK_EX_ERROR:
 {
  EStringError exception(line);
  ProcessException(exception);
 }
 break;

 case RDK_EX_WARNING:
 {
  EStringWarning exception(line);
  ProcessException(exception);
 }
 break;

 case RDK_EX_INFO:
 {
  EStringInfo exception(line);
  ProcessException(exception);
 }
 break;

 case RDK_EX_DEBUG:
 {
  if(DebugMode)
  {
   EStringDebug exception(line);
   ProcessException(exception);
  }
 }
 break;
 }
}

void UEnvironment::LogMessage(int msg_level, const std::string &method_name, const std::string &line)
{
 LogMessage(msg_level, method_name+std::string(" - ")+line);
}

// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������� �����
void UEnvironment::AInit(void)
{
 return;
}

// ��������������� �����
void UEnvironment::AUnInit(void)
{
 return;
}

// ��������� �������������� �������� ������ ���������
bool UEnvironment::ACreateStructure(void)
{
 RdkCreatePredefinedStructure(this, PredefinedStructure);
 return true;
}


// ���������� ������� ������ ���������
bool UEnvironment::ADestroyStructure(void)
{
 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool UEnvironment::ADefault(void)
{
 if(!Model)
  return true;

 MinInterstepsInterval=0;
 DebugMode=false;

// UComponent::SetTime(0);
 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Default())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Default())
   return false;
 }

 ChannelIndex=0;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UEnvironment::ABuild(void)
{
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Build())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Build())
   return false;
 }

 return true;
}

// ����� �������� �����.
bool UEnvironment::AReset(void)
{
 StartupTime=0;
 ProcEndTime=StartupTime;
 LastDuration=1;
 LastStepStartTime=0;
 LastErrorLevel=INT_MAX;
 RTModelCalcTime=0;

 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Reset())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Reset())
   return false;
 }

 Time.SetTime(0);
 return true;
}

// ��������� ������ ����� �������
bool UEnvironment::ACalculate(void)
{
 // ���� ������ ��� ������� ����� Reset
 if(Time.GetTime() == 0)
 {
  Time.SetSourceStartLocalTime(0);
  Time.SetSourceStartGlobalTime(Time.GetSourceCurrentGlobalTime());
  StartupTime=GetCurrentStartupTime();
  ProcEndTime=StartupTime;
 }

 ULongTime cur_time=(ULongTime)((Time.GetSourceCurrentGlobalTime()-Time.GetSourceStartGlobalTime())*(86400.0*1000.0));
 Time.SetSourceCurrentLocalTime(cur_time/1000.0);

 if(!Model)
  return true;

 // ���������, ���������� �� ����� �������� ������� ����� ���������� �����
 if(MinInterstepsInterval>0 && CalcDiffTime(cur_time,LastStepStartTime)<MinInterstepsInterval)
  return true;

 LastStepStartTime=cur_time;
 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Calculate())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Calculate())
   return false;
 }

 // ���� �� ������� ��� ������, �� ����������� ����� ������ �����,
 // ����� �� �������, ��� ���������� ������ ��� ��������� ����� ������
 if(ModelCalculationComponent.GetSize() == 0)
  IncreaseModelTimeByStep();

 return true;
}
// --------------------------

// --------------------------
// ��������������� ������ ������������� �����
// --------------------------
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// --------------------------

}
#endif
