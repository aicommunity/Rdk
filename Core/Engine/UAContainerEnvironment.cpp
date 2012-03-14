/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UACONTAINERENVIRONMENT_CPP
#define UACONTAINERENVIRONMENT_CPP

#include "UAContainerEnvironment.h"

namespace RDK {

// --------------------------
// Constructors & destructors
// --------------------------
UAContainerEnvironment::UAContainerEnvironment(void)
{
}

UAContainerEnvironment::~UAContainerEnvironment(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� ������� �����
// --------------------------
// ���������� ��������� �� ���������
UAContainerStorage* UAContainerEnvironment::GetStorage(void)
{
 return dynamic_cast<UAContainerStorage*>(Storage);
}

// ������������� ����� ���������
// ��������� �� ������ ��������� ����� �� ������������ ������
// ��������������� �� ������������ ������ ����� �� ���������� �������
// ������� ������ ������������.
/*bool UAContainerEnvironment::SetStorage(UContainerStorage *storage)
{
 if(!storage)
  return false;

 if(Storage == storage)
  return true;

 if(!DestroyModel())
  return false;

 Storage=storage;
 Init=true;

 return true;
}        */
/*
// ���������� ��������� �� ������
UAComponent* UAContainerEnvironment::GetModel(void)
{
 return Model;
}
  */
// ������� ����� ������ �� ��������� �� id ������
bool UAContainerEnvironment::CreateModel(const UId& classid)
{
 return UAEnvironment::CreateModel(classid);
}

// ������� ����� ������ �� ��������� �� ����� ������
bool UAContainerEnvironment::CreateModel(const NameT& classname)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UAContainer>(GetStorage()->TakeObject(classname));
 if(Model)
 {
  return true;
 }

 return false;
}
                    /*
// ���������� ������� ������
bool UAContainerEnvironment::DestroyModel(void)
{
 if(!Model)
  return true;

 Model=0;
 CurrentComponent=0;

 return true;
}

// ���������� ���������� �� �������
ULibrary* UAContainerEnvironment::GetClassLibrary(int index)
{
 return ClassLibraryList[index];
}
                    */
// ���������� ���������� �� �����
UALibrary* UAContainerEnvironment::GetClassLibrary(const string &name)
{
 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  UALibrary *lib=dynamic_cast<UALibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == name)
   return lib;
 }

 return 0;
}

// ���������� ��� ���������� �� �������
const string& UAContainerEnvironment::GetClassLibraryName(int index)
{
 return ClassLibraryList[index]->GetName();
}

// ���������� ������ ���������� �� �������
const string& UAContainerEnvironment::GetClassLibraryVersion(int index)
{
 return ClassLibraryList[index]->GetVersion();
}

                /*
// ���������� ����� ���������
size_t UAContainerEnvironment::GetNumClassLibraries(void) const
{
 return ClassLibraryList.size();
}                 */

// ���������� ������������ ���������� � ������� �������� �������.
// ���� ����������� � ����� ������ ��� ���������� �� ���������� false.
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
bool UAContainerEnvironment::AddClassLibrary(UALibrary *library)
{
 if(!library)
  return false;

 UALibrary *newlib=dynamic_cast<UALibrary*>(library);

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  UALibrary *lib=dynamic_cast<UALibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == newlib->GetName())
   return false;
 }


 return UAEnvironment::AddClassLibrary(library);
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����������� ����� �� ���������� �������.
bool UAContainerEnvironment::DelClassLibrary(int index)
{
 return UAEnvironment::DelClassLibrary(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UAContainerEnvironment::DelClassLibrary(const string &name)
{
 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  UALibrary *lib=dynamic_cast<UALibrary*>(ClassLibraryList[i]);
  if(lib && lib->GetName() == name)
   return DelClassLibrary(i);
 }
 return true;
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
bool UAContainerEnvironment::DelAllClassLibraries(void)
{
 return UAEnvironment::DelAllClassLibraries();
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
bool UAContainerEnvironment::BuildStorage(void)
{
 if(!IsStoragePresent())
  return false;

 if(!DestroyModel())
  return false;

 GetStorage()->ClearObjectsStorage();

 GetStorage()->ClearClassesStorage();

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ClassLibraryList[i]->Upload(GetStorage());
  UALibrary *lib=dynamic_cast<UALibrary *>(ClassLibraryList[i]);
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
UEPtr<UAContainer> UAContainerEnvironment::GetCurrentComponent(void)
{
 return CurrentComponent;
}

// ������������� ��������� �� ������� ��������� ������
void UAContainerEnvironment::SelectCurrentComponent(const NameT &name)
{
 if(name == ForbiddenName)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(name);
}

void UAContainerEnvironment::SelectCurrentComponent(const ULongId &id)
{
 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(id);
}

// ������������� ��������� �� ������� ��������� ������ �� ���� ������
void UAContainerEnvironment::ResetCurrentComponent(void)
{
 CurrentComponent=Model;
}

// ������������� ��������� �� ������� ��������� ������ �� ������������ ���������
// (������� �� ������� �����). ���� ��� ��������� �� ������, �� �� ������ ������
void UAContainerEnvironment::UpCurrentComponent(void)
{
 if(CurrentComponent == Model)
  return;

 CurrentComponent=CurrentComponent->GetOwner();
}

// ������������� ��������� �� ������� ��������� ������ �� �������� ��������� ��
// ����� ������ (������� �� ������� ����).
void UAContainerEnvironment::DownCurrentComponent(const NameT &name)
{
 CurrentComponent=GetCurrentComponent()->GetComponentL(name);
}

void UAContainerEnvironment::DownCurrentComponent(const ULongId &id)
{
 CurrentComponent=GetCurrentComponent()->GetComponentL(id);
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
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UAContainerEnvironment::ADefault(void)
{
 return UAEnvironment::ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UAContainerEnvironment::ABuild(void)
{
 return UAEnvironment::ABuild();
}

// ����� �������� �����.
bool UAContainerEnvironment::AReset(void)
{
 if(!UAEnvironment::AReset())
  return false;
 UAContainer::SetTime(0);
 return true;
}

// ��������� ������ ����� �������
bool UAContainerEnvironment::ACalculate(void)
{
 if(!UAEnvironment::ACalculate())
  return false;
 UAContainer::SetTime(UAContainer::GetTime()+1000000/GetModel()->GetTimeStep());
 return true;
}
// --------------------------



// --------------------------
// ��������������� ������ ������������� �����
// --------------------------
// ��������� ������������ ���������� � ����������� ��������� � ����������
// ��������� �� ����.
// � ������� ������ �� ������ ������ � ���������� 0.
UAContainerStorage* UAContainerEnvironment::LoadStorageLibrary(const string &filename)
{
 return 0;
}

// ��������� ������������ ���������� � ����������� ���������.
// � ������� ������ �� ������ ������ � ���������� true.
bool UAContainerEnvironment::UnLoadStorageLibrary(void)
{
 return true;
}

// ��������� ������������ ���������� � ������� �������� �������
// � ���� ���������� ��������� � ���������� ��������� �� ����.
// � ������� ������ �� ������ ������ � ���������� 0.
UAContainerStorage* UAContainerEnvironment::LoadClassLibrary(const string &libname, const string &filename)
{
 return 0;
}

// ��������� ������������ ���������� � ������� �������� �������.
// � ������� ������ �� ������ ������ � ���������� true.
bool UAContainerEnvironment::UnLoadClassLibrary(const string &libname)
{
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// --------------------------


}
#endif
