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
// ������ ������� � �������� �����������
// --------------------------
// ���������� Id ������ �� ��� �����
const UId& UAStorage::GetClassId(const NameT &name) const
{
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  throw new EClassNameNotExist(name);
 return I->second;
}

// ���������� ��� ������ �� ��� Id
const NameT UAStorage::GetClassName(const UId &id) const
{
 for(map<NameT,UId>::const_iterator I=ClassesLookupTable.begin(),
									J=ClassesLookupTable.end();I != J;++I)
 {
  if(I->second == id)
   return I->first;
 }
 throw new EClassIdNotExist(id);
}
// --------------------------


// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ���������
// ���������� id ������
UId UAStorage::AddClass(UEPtr<UAComponent> classtemplate, const UId &classid)
{
 UId id=classid;
 if(id == ForbiddenId)
  id=LastClassId+1;

 if(ClassesStorage.find(id) != ClassesStorage.end())
  throw new EClassIdAlreadyExist(id);

 if(!classtemplate->Build())
  return ForbiddenId;

 ClassesStorage[id]=classtemplate;
 classtemplate->SetClass(id);
 LastClassId=id;

 ClassesDescription[id]=classtemplate->NewDescription();

 return id;
}

// ��������� ������� ������ ������� � ���������
UId UAStorage::AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid)
{
 if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  throw new EClassNameAlreadyExist(classname);

 UId id=AddClass(classtemplate,classid);
 ClassesLookupTable[classname]=id;
 ClassesDescription[id]->SetClassName(classname);
 return id;
}

// ������� ������� ������ ������� �� ���������
void UAStorage::DelClass(const UId &classid)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  ClassesStorage.erase(I);
 else
  throw new EClassIdNotExist(classid);

 UClassStorageElement element=I->second;
 if(element)
  delete element.Get();

 UClassesDescriptionIterator J=ClassesDescription.find(classid);

 if(J != ClassesDescription.end())
 {
  if(J->second)
   delete J->second;

  ClassesDescription.erase(J);
 }
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

 if(I == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 return I->second;
}

// ���������� ����� �������
int UAStorage::GetNumClasses(void) const
{
 return ClassesStorage.size();
}

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UAStorage::GetClassIdList(std::vector<UId> &buffer) const
{
 buffer.resize(0);
 buffer.reserve(ClassesStorage.size());
 for(UClassesStorageCIterator I = ClassesStorage.begin(), J=ClassesStorage.end(); I != J; ++I)
  buffer.push_back(I->first);
}

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UAStorage::GetClassNameList(vector<NameT> &buffer) const
{
 map<NameT,UId>::const_iterator I,J;

 I=ClassesLookupTable.begin();
 J=ClassesLookupTable.end();
 buffer.resize(0);
 buffer.reserve(ClassesLookupTable.size());

 for(map<NameT,UId>::const_iterator I=ClassesLookupTable.begin(),
									J=ClassesLookupTable.end(); I!=J; ++I)
 {
  buffer.push_back(I->first);
 }
}


// ������� ��� ������� ������� �� ���������
void UAStorage::ClearClassesStorage(void)
{
 for(UClassesStorageCIterator I = ClassesStorage.begin(), J=ClassesStorage.end(); I != J; ++I)
 {
  if(I->second)
   delete I->second.Get();
 }
 ClassesStorage.clear();

 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  if(I->second)
   delete I->second.Get();
 }
 ClassesDescription.clear();
 LastClassId=0;
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
UEPtr<UAComponent> UAStorage::TakeObject(const UId &classid, const UEPtr<UAComponent> prototype)
{
 UEPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;
 UEPtr<UAComponent> obj;

 if(!classtemplate)
  throw EClassIdNotExist(classid);

 obj=classtemplate->New();

 obj->SetClass(classid);
 obj->Default();
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
// ������ ���������� ��������� �������
// --------------------------
// ���������� XML �������� ������
const UEPtr<UComponentDescription> UAStorage::GetClassDescription(const UId &classid) const
{
 UClassesDescriptionCIterator I=ClassesDescription.find(classid);

 if(I == ClassesDescription.end())
  throw EClassIdNotExist(classid);

 return I->second;
}

// ������������� XML �������� ������
// ����� � ��������� ������ ������������
void UAStorage::SetClassDescription(const UId &classid, const UEPtr<UComponentDescription>& description)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 ClassesDescription[classid]=description;
}

// ��������� �������� ������ � xml
void UAStorage::SaveClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml)
{
 GetClassDescription(classid)->Save(xml);
}

// ��������� �������� ������ �� xml
void UAStorage::LoadClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml)
{
 GetClassDescription(classid)->Load(xml);
}

// ��������� �������� ���� ������� � xml
void UAStorage::SaveClassesDescription(Serialize::USerStorageXML &xml)
{
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  xml.AddNode(sntoa(I->first));
  I->second->Save(xml);
  xml.SelectUp();
 }
}

// ��������� �������� ���� ������� �� xml
void UAStorage::LoadClassesDescription(Serialize::USerStorageXML &xml)
{
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  if(!xml.SelectNode(sntoa(I->first)))
   continue;
  I->second->Load(xml);
  xml.SelectUp();
 }
}

// ��������� ����� �������� ���� ������� � xml
bool UAStorage::SaveCommonClassesDescription(Serialize::USerStorageXML &xml)
{
 xml.AddNode(sntoa(0));
 if(!UContainerDescription::SaveCommon(xml))
 {
  xml.SelectUp();
  return false;
 }
 xml.SelectUp();
 return true;
}

// ��������� ����� �������� ���� ������� �� xml
bool UAStorage::LoadCommonClassesDescription(Serialize::USerStorageXML &xml)
{
 if(xml.SelectNode(sntoa(0)))
 {
  UContainerDescription::LoadCommon(xml);
  xml.SelectUp();
 }

 UClassesDescriptionIterator I=ClassesDescription.begin();

 while(I != ClassesDescription.end())
 {
  dynamic_pointer_cast<UContainerDescription>(I->second)->RemoveCommonDuplicatesParameters();
  ++I;
 }

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� ��������
// --------------------------
// ���������� ������ � ���������
// � ������� ���������� ������ ������� ������ � ���������� true
void UAStorage::ReturnObject(UEPtr<UAComponent> object)
{
 delete object.Get();
}
// --------------------------

// --------------------------
// ������� ������ ������� ������������ �������
// --------------------------
// ��������� ����� � ������ 'name' � ������� ������������
UId UAStorage::AddLookupClass(const NameT &name)
{
 if(ClassesLookupTable.find(name) != ClassesLookupTable.end())
  throw EClassNameAlreadyExist(name);

 ClassesLookupTable.insert(make_pair(name,LastClassId+1));

 return LastClassId+1;
}

// ������� ����� � ������ 'name' �� ������� �����������
void UAStorage::DelLookupClass(const NameT &name)
{
 map<NameT,UId>::iterator I=ClassesLookupTable.find(name);

 if(I == ClassesLookupTable.end())
  throw EClassNameNotExist(name);

 ClassesLookupTable.erase(I);
}
// --------------------------
/* *************************************************************************** */
              /*
// --------------------------
// ������������ � �����������
// --------------------------
UAStorage::EClassIdNotExist::EClassIdNotExist(UId id)
 : Id(id)
{

}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UAStorage::EClassIdNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Id=")+sntoa(Id);
}
// --------------------------

// ������� ������ � ������� �� �����, �������������� � ���������
//class UAStorage::EClassNameNotExist: public EError
// --------------------------
// ������������ � �����������
// --------------------------
UAStorage::EClassNameNotExist::EClassNameNotExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UAStorage::EClassNameNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------

// ������������ ��� ������
// class UAStorage::EInvalidClassName: public EError
// --------------------------
// ������������ � �����������
// --------------------------
UAStorage::EInvalidClassName::EInvalidClassName(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UAStorage::EInvalidClassName::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------



// ����� � �������� ������ ��� ����������
//class UAStorage::EClassNameAlredyExist: public EError
// --------------------------
// ������������ � �����������
// --------------------------
UAStorage::EClassNameAlreadyExist::EClassNameAlreadyExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UAStorage::EClassNameAlreadyExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------
       */
}


#endif

