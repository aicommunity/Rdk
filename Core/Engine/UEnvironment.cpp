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
#include "UEnvironment.h"
#include "../System/rdk_system.h"

namespace RDK {

// --------------------------
// Constructors & destructors
// --------------------------
UEnvironment::UEnvironment(void)
{
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

 LastReadExceptionLogIndex=-1;
 MaxExceptionsLogSize=1000;

 LastErrorLevel=INT_MAX;
 DebugMode=false;
 ChannelIndex=0;
 LastStepStartTime=0;
}

UEnvironment::~UEnvironment(void)
{
 DestroyModel();
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
// --------------------------


// --------------------------
// ������ ���������� �����������
// --------------------------
// ������� ������� �������������� ���������
bool UEnvironment::IsStructured(void) const
{
 return Structured;
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

 return;
}
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
// ������������ ��������� ����������
void UEnvironment::ProcessException(UException &exception) const
{
// if(!exception)
//  throw exception;

// USharedPtr<UException> ptr=exception;
// ExceptionsLog.push_back(ptr);

 if(LastErrorLevel>exception.GetType())
  LastErrorLevel=exception.GetType();
 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize/*ExceptionsLog.size()*/ > MaxExceptionsLogSize)
 {
//  ExceptionsLog.erase(ExceptionsLog.begin());
  size_t i=TempLogString.find_first_of("\n");
  if(i != string::npos)
  {
   TempLogString.erase(0,i);
  }
 }
// if(!TempLogString.empty())
//  TempLogString+="\r\n";

 TempLogString+=sntoa(ChannelIndex);
 TempLogString+="> ";
 TempLogString+=exception.CreateLogMessage();

 if(ExceptionHandler)
  ExceptionHandler(ChannelIndex);
}


// ���������� ������ ������������������ ����������
/*const vector<USharedPtr<UException> > UEnvironment::GetExceptionsLog(void) const
{
 return ExceptionsLog;
}*/


// ���������� ������ ����� ����
const char* UEnvironment::GetLog(int &error_level) const
{
 error_level=LastErrorLevel;
 LastErrorLevel=INT_MAX;
 return TempLogString.c_str();
}

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
const char* UEnvironment::GetUnreadLog(int &error_level)
{
 error_level=LastErrorLevel;
 LastErrorLevel=INT_MAX;
 if(LastReadExceptionLogIndex<=0)
 {
  LastReadExceptionLogIndex=TempLogString.size();
  return TempLogString.c_str();
 }

 if(LastReadExceptionLogIndex<int(TempLogString.size()))
 {
  TempString=TempLogString.substr(LastReadExceptionLogIndex);
  LastReadExceptionLogIndex=TempLogString.size();
  return TempString.c_str();
 }

 TempString="";
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
 return MaxExceptionsLogSize;
}

void UEnvironment::SetMaxExceptionsLogSize(int value)
{
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
 LastReadExceptionLogIndex=-1;
 CurrentExceptionsLogSize=0;
 LastErrorLevel=INT_MAX;
 TempLogString.clear();
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

 unsigned long long cur_time=(Time.GetSourceCurrentGlobalTime()-Time.GetSourceStartGlobalTime())*(86400.0*1000.0);
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
