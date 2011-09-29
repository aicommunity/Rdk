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
  delete Libraries;
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

 // ������� ��������� ������
 CurrentComponent=0;
}

UAEnvironment::~UAEnvironment(void)
{
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
// ���������� ��������� �������������
bool UAEnvironment::IsInit(void) const
{
 return Initialized;
}

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
UAComponent* UAEnvironment::GetModel(void)
{
 return Model;
}

// ������� ����� ������ �� ��������� �� id ������
bool UAEnvironment::CreateModel(const UId& classid)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=0;
 Model=Storage->TakeObject(classid);
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
UAComponent* UAEnvironment::operator () (void)
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
bool UAEnvironment::Init(void)
{
 if(IsInit())
  return true;

 if(!AInit())
  return false;

 ModelCalculationComponent.Resize(0);
 Initialized=true;
 return true;
}

// ��������������� �����
bool UAEnvironment::UnInit(void)
{
 if(!IsInit())
  return true;

 if(!AUnInit())
  return false;

 Initialized=false;
 return true;
}

// ��������� �������������� �������� ������ ���������
bool UAEnvironment::CreateStructure(void)
{
 if(Structured)
  return true;

 if(!Init())
  return false;

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
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������� �����
bool UAEnvironment::AInit(void)
{
 return true;
}

// ��������������� �����
bool UAEnvironment::AUnInit(void)
{
 return true;
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
  RDK::UAContainer* destcont=0;
  destcont=dynamic_cast<UAContainer*>(GetModel())->GetComponentL(ModelCalculationComponent);

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
  RDK::UAContainer* destcont=0;
  destcont=dynamic_cast<UAContainer*>(GetModel())->GetComponentL(ModelCalculationComponent);

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
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Reset();
 else
 {
  RDK::UAContainer* destcont=0;
  destcont=dynamic_cast<UAContainer*>(GetModel())->GetComponentL(ModelCalculationComponent);

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
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Calculate();
 else
 {
  RDK::UAContainer* destcont=0;
  destcont=dynamic_cast<UAContainer*>(GetModel())->GetComponentL(ModelCalculationComponent);

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
