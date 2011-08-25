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

 CurrentComponent=0;
 Model=GetStorage()->TakeObject(classname);
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

 if(!GetStorage()->ClearClassesStorage())
  return false;

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
UAComponent* UAContainerEnvironment::operator () (void)
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
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
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
