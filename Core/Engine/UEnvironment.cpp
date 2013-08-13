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
// ������������ � �����������
// --------------------------
UClassLibraryList::UClassLibraryList(void)
{
 Libraries=0;
 Size=RealSize=0;
}

UClassLibraryList::UClassLibraryList(const UClassLibraryList &copy)
{
 Libraries=0;
 Size=RealSize=0;
 *this=copy;
}

UClassLibraryList::~UClassLibraryList(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void UClassLibraryList::Clear(void)
{
 if(Libraries)
 {
  delete []Libraries;
  Libraries=0;
 }
 Size=RealSize=0;
}

// ���������� ������ �������
int UClassLibraryList::GetSize(void) const
{
 return Size;
}

// �������� ������ ���������
// ����� ��������� ������ ������
void UClassLibraryList::Resize(int newsize)
{
 int newrealsize=0;
 if(newsize < 8)
  newrealsize=16;
 else
  newrealsize=newsize<<2;
 if(RealSize<newsize || !Libraries)
 {
  PUALibrary* newbuffer=new PUALibrary[newrealsize];
  memcpy(newbuffer,Libraries,sizeof(PUALibrary)*Size);

  if(Libraries)
   delete []Libraries;
  Libraries=newbuffer;
  RealSize=newrealsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ���� ����� �� Id � ������� ��� �� �������
// ���������� ��������� �� ��������� �����
PUALibrary UClassLibraryList::Erase(int index)
{
 if(!Libraries || index<0 || index>=Size)
  return 0;

 PUALibrary result=Libraries[index];

 if(index<Size-1)
  memmove(Libraries+index+1,Libraries+index,sizeof(PUALibrary)*(Size-index-1));

 Resize(Size-1);

 return result;
}

// ��������� ����� ������� � ����� ���������
void UClassLibraryList::PushBack(PUALibrary classelement)
{
 Resize(Size+1);
 Libraries[Size-1]=classelement;
}

// ���������� ��������� �� ������ �������
PUALibrary* UClassLibraryList::GetLibraries(void) const
{
 return Libraries;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UClassLibraryList& UClassLibraryList::operator = (const UClassLibraryList &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Libraries[i]=copy.Libraries[i];

 return *this;
}

// �������� �������
PUALibrary UClassLibraryList::operator [] (int i)
{
 return Libraries[i];
}
// --------------------------

// ��� �������� �������� �������� ������
//std::string UEnvironment::CurrentDataDir;

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

// ���������� ���������� �� �������
ULibrary* UEnvironment::GetClassLibrary(int index)
{
 return ClassLibraryList[index];
}

// ���������� ����� ���������
int UEnvironment::GetNumClassLibraries(void) const
{
 return ClassLibraryList.GetSize();
}

// ���������� ���������� �� �����
ULibrary* UEnvironment::GetClassLibrary(const string &name)
{
 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ULibrary *lib=dynamic_cast<ULibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == name)
   return lib;
 }

 return 0;
}

// ���������� ��� ���������� �� �������
const string& UEnvironment::GetClassLibraryName(int index)
{
 return ClassLibraryList[index]->GetName();
}

// ���������� ������ ���������� �� �������
const string& UEnvironment::GetClassLibraryVersion(int index)
{
 return ClassLibraryList[index]->GetVersion();
}

// ��������������� ��������� ����� ������� ������ � ���������
bool UEnvironment::AddClass(UContainer *newclass)
{
 if(!Storage)
  return false;

 UId classid=newclass->GetClass();
 if(!Storage->AddClass(newclass,classid) == ForbiddenId)
  return false;

 return true;
}

// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
bool UEnvironment::AddClassLibrary(ULibrary *library)
{
 if(!library)
  return false;

 ULibrary *newlib=dynamic_cast<ULibrary*>(library);

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ULibrary *lib=dynamic_cast<ULibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == newlib->GetName())
   return false;
 }

 ClassLibraryList.PushBack(library);
 return true;
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
bool UEnvironment::DelClassLibrary(int index)
{
 if(index < 0 || index >= int(ClassLibraryList.GetSize()))
  return false;

 ClassLibraryList.Erase(index);
 return true;
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEnvironment::DelClassLibrary(const string &name)
{
 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ULibrary *lib=dynamic_cast<ULibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == name)
   return DelClassLibrary(i);
 }
 return true;
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UEnvironment::DelAllClassLibraries(void)
{
 ClassLibraryList.Clear();
 return true;
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UEnvironment::BuildStorage(void)
{
 if(!IsStoragePresent())
  return false;

 if(!DestroyModel())
  return false;

 if(!IsStoragePresent())
  return false;

 if(!DestroyModel())
  return false;

// GetStorage()->ClearObjectsStorage();

// GetStorage()->ClearClassesStorage();

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ClassLibraryList[i]->Upload(GetStorage());
  ULibrary *lib=ClassLibraryList[i];
  if(lib)
  {
   CompletedClassNames.insert(CompletedClassNames.end(),
                             lib->GetComplete().begin(),
                             lib->GetComplete().end());
   IncompletedClassNames.insert(IncompletedClassNames.end(),
                             lib->GetIncomplete().begin(),
                             lib->GetIncomplete().end());
  }
 }
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

// StartProcTime=GetCurrentStartupTime();

 CurrentTime=GetCurrentStartupTime();
 Time.SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);

 long long curtime;
 long long TimerInterval=0;
 //double devicemodeltime=0;

 TimerInterval=GetCurrentStartupTime()-ProcEndTime;
 if(TimerInterval<=0)
  TimerInterval=1;

 int i=0;
 if(LastDuration < TimerInterval)
  LastDuration=TimerInterval;
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
  elapsed_counter=0;//(/*LastDuration*/model_duration*Model->GetTimeStep())/1000;
 }

 curtime=GetCurrentStartupTime();
 while(curtime-CurrentTime<TimerInterval && i<elapsed_counter)
 {
  Calculate();

  ++i;
  curtime=GetCurrentStartupTime();
 }

// LastSentTime=GetCurrentStartupTime();
 if(Time.GetRealTime()/1e6<Time.GetDoubleTime())
 {
  Sleep(int(Time.GetDoubleTime()*1000-Time.GetRealTime()/1000));
  Time.SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);
 }

 LastDuration=GetCurrentStartupTime()-CurrentTime;
 ProcEndTime=GetCurrentStartupTime();

 return;
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
 StartupTime=GetCurrentStartupTime();
 LastDuration=1;
 ProcEndTime=StartupTime;

 if(!Model)
  return true;

// RDK::DefaultTimeStep=30;
// Model->SetTimeStep(DefaultTimeStep);

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
 Time.SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);
 if(!Model)
  return true;

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
