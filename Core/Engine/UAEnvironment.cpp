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
#include "UAEnvironment.h"

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


// --------------------------
// Constructors & destructors
// --------------------------
UAEnvironment::UAEnvironment(void)
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

 MaxModelDuration=0.1;

 // ������� ��������� ������
// CurrentComponent=0;
}

UAEnvironment::~UAEnvironment(void)
{
 DestroyModel();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ������ ������������� �������� ������ ���������
int UAEnvironment::GetPredefinedStructure(void) const
{
 return PredefinedStructure;
}

bool UAEnvironment::SetPredefinedStructure(int value)
{
 if(PredefinedStructure == value)
  return true;

 PredefinedStructure=value;
 return DestroyStructure();
}


// ������������� ���������� ������, ������� ����� �������������
const ULongId& UAEnvironment::GetModelCalculationComponent(void) const
{
 return ModelCalculationComponent;
}

bool UAEnvironment::SetModelCalculationComponent(const ULongId& value)
{
 if(ModelCalculationComponent == value)
  return true;

 ModelCalculationComponent=value;

 return true;
}
// --------------------------


// --------------------------
// ������ ���������� �����������
// --------------------------
// ������� ������� �������������� ���������
bool UAEnvironment::IsStructured(void) const
{
 return Structured;
}
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
UAStorage* UAEnvironment::GetStorage(void)
{
 return Storage;
}

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
bool UAEnvironment::SetStorage(UAStorage *storage)
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
UEPtr<UAContainer> UAEnvironment::GetModel(void)
{
 return Model;
}

// ������� ����� ������ �� ��������� �� id ������
bool UAEnvironment::CreateModel(const UId& classid)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UAContainer>(Storage->TakeObject(classid));
 Ready=false;
 if(Model)
  return true;

 return false;
}

// ���������� ������� ������
bool UAEnvironment::DestroyModel(void)
{
 if(!Model)
  return true;

 Model->Free();
 Model=0;
 CurrentComponent=0;

 return true;
}

// ���������� ���������� �� �������
UALibrary* UAEnvironment::GetClassLibrary(int index)
{
 return ClassLibraryList[index];
}

// ���������� ����� ���������
int UAEnvironment::GetNumClassLibraries(void) const
{
 return ClassLibraryList.GetSize();
}

// ��������������� ��������� ����� ������� ������ � ���������
bool UAEnvironment::AddClass(UAContainer *newclass)
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
bool UAEnvironment::AddClassLibrary(UALibrary *library)
{
 if(!library)
  return false;

 ClassLibraryList.PushBack(library);
 return true;
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
bool UAEnvironment::DelClassLibrary(int index)
{
 if(index < 0 || index >= int(ClassLibraryList.GetSize()))
  return false;

 ClassLibraryList.Erase(index);
 return true;
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UAEnvironment::DelAllClassLibraries(void)
{
 ClassLibraryList.Clear();
 return true;
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UAEnvironment::BuildStorage(void)
{
 if(!IsStoragePresent())
  return false;

 if(!DestroyModel())
  return false;

// if(!Storage->ClearClassesStorage())
//  return false;

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ClassLibraryList[i]->Upload(Storage);
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
UEPtr<UAComponent> UAEnvironment::GetCurrentComponent(void)
{
 return CurrentComponent;
}
// --------------------------

// --------------------------
// ������ ������������ ��������� ������������
// --------------------------
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
bool UAEnvironment::IsStoragePresent(void) const
{
 return StoragePresent;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������������� �����
void UAEnvironment::Init(void)
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
void UAEnvironment::UnInit(void)
{
 if(Model)
  Model->UnInit();

 if(!IsInit())
  return;

 AUnInit();

 Initialized=false;
}

// ��������� �������������� �������� ������ ���������
bool UAEnvironment::CreateStructure(void)
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
bool UAEnvironment::DestroyStructure(void)
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
void UAEnvironment::RTCalculate(void)
{
 Build();

// StartProcTime=GetCurrentStartupTime();

 CurrentTime=GetCurrentStartupTime();
 UAContainer::SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);

 long long curtime;
 long long TimerInterval=0;
 //double devicemodeltime=0;

 TimerInterval=GetCurrentStartupTime()-ProcEndTime;
 if(TimerInterval<=0)
  TimerInterval=1;

 int i=0;
 if(LastDuration < TimerInterval)
  LastDuration=TimerInterval;
 double model_duration=(UAContainer::GetRealTime()-Model->GetDoubleTime()*1e6)/1000.0;

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
 if(UAContainer::GetRealTime()/1e6<Model->GetDoubleTime())
 {
  Sleep(int(Model->GetDoubleTime()*1000-UAContainer::GetRealTime()/1000));
  UAContainer::SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);
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
void UAEnvironment::AInit(void)
{
 return;
}

// ��������������� �����
void UAEnvironment::AUnInit(void)
{
 return;
}

// ��������� �������������� �������� ������ ���������
bool UAEnvironment::ACreateStructure(void)
{
 return true;
}


// ���������� ������� ������ ���������
bool UAEnvironment::ADestroyStructure(void)
{
 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool UAEnvironment::ADefault(void)
{
 if(!Model)
  return true;

// UAComponent::SetTime(0);
 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Default();
 else
 {
  UEPtr<UAContainer> destcont;
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
bool UAEnvironment::ABuild(void)
{
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Build();
 else
 {
  UEPtr<UAContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Build())
   return false;
 }

 return true;
}

// ����� �������� �����.
bool UAEnvironment::AReset(void)
{
 StartupTime=GetCurrentStartupTime();
 LastDuration=1;
 ProcEndTime=StartupTime;

 if(!Model)
  return true;

// RDK::DefaultTimeStep=30;
// Model->SetTimeStep(DefaultTimeStep);

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Reset();
 else
 {
  UEPtr<UAContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Reset())
   return false;
 }

 return true;
}

// ��������� ������ ����� �������
bool UAEnvironment::ACalculate(void)
{
 UAContainer::SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Calculate();
 else
 {
  UEPtr<UAContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Calculate())
   return false;
 }

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
