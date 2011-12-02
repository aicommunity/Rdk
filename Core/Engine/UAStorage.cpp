/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UASTORAGE_CPP
#define UASTORAGE_CPP

#include <string.h>
#include "UAStorage.h"

namespace RDK {

/* *************************************************************************** */
// Class UAStorage
/* *************************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UAStorage::UAStorage(void)
{
 LastClassId=0;
}

UAStorage::~UAStorage(void)
{
 ClearClassesStorage();
}
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �������������� Id �������
UId UAStorage::GetLastClassId(void) const
{
 return LastClassId;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
UId UAStorage::AddClass(UEPtr<UAComponent> classtemplate, const UId &classid)
{
 if(!classtemplate)
  return ForbiddenId;

 UId id=classid;
 if(id == ForbiddenId)
  id=LastClassId+1;

 if(ClassesStorage.find(id) != ClassesStorage.end())
  return ForbiddenId;

 if(!classtemplate->Build())
  return ForbiddenId;

 ClassesStorage[id]=classtemplate;
 classtemplate->SetClass(id);
 LastClassId=id;

 return id;
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
bool UAStorage::DelClass(const UId &classid)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  ClassesStorage.erase(I);

 UClassStorageElement element=I->second;
 if(element)
  delete element.Get();

 return true;
}

// ��������� ������� ������� ������ ������� � ���������
bool UAStorage::CheckClass(const UId &classid) const
{
 if(ClassesStorage.find(classid) == ClassesStorage.end())
  return false;

 return true;
}

// ���������� ������� ������
UEPtr<UAComponent> UAStorage::GetClass(const UId &classid) const
{
 UClassesStorageCIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  return I->second;

 return UEPtr<UAComponent>();
}

// ���������� ����� �������
int UAStorage::GetNumClasses(void) const
{
 return ClassesStorage.size();
}

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UAStorage::GetClassIdList(UId* buffer, int max_num_classes) const
{
 if(!buffer || !max_num_classes || !ClassesStorage.size())
  return;

 UClassesStorageCIterator I;
 int count=0;
 for(I=ClassesStorage.begin();I != ClassesStorage.end();++I,++count)
 {
  if(count == max_num_classes)
   break;

  *buffer++=I->first;
 }
}

// ������� ��� ������� ������� �� ���������
bool UAStorage::ClearClassesStorage(void)
{
 UClassesStorageCIterator I;
 for(I=ClassesStorage.begin();I != ClassesStorage.end();++I)
 {
  if(I->second)
   delete I->second.Get();
 }
 ClassesStorage.clear();
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ��������
// --------------------------
// ��������� ������ �� ���������
// ���������� ��������� �� ��������� ������ �� ����� ������
// ��������� ������ ���������� ��� ������� � ���������
// ���� 'Activity' ������� ������������ � true
// ���� ���������� ������� �� ���������� �� ��������� � �����������
// � ���������
UEPtr<UAComponent> UAStorage::TakeObject(const UId &classid, const UAComponent *prototype)
{
 UEPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;
 UEPtr<UAComponent> obj;

 if(!classtemplate)
  return obj;

 obj=classtemplate->New();
 if(!obj)
  return obj;

 obj->SetClass(classid);
 obj->Default();
// return obj.operator ->();
 return obj;
}

// ���������� Id ������, ���������� ������� 'object'
UId UAStorage::FindClass(UEPtr<UAComponent> object) const
{
 if(!object)
  return ForbiddenId;

 return object->GetClass();
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� ��������
// --------------------------
// ���������� ������ � ���������
// � ������� ���������� ������ ������� ������ � ���������� true
bool UAStorage::ReturnObject(UEPtr<UAComponent> object)
{
 delete object.Get();

 return true;
}
// --------------------------
/* *************************************************************************** */

}


#endif

