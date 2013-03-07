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

/* *********************************************************************** */
/* *********************************************************************** */
// ������� ������ ������������ �������� ������������� ������
// class UInstancesStorageElement
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorageElement::UInstancesStorageElement(void)
 : UseFlag(false)
{
}

UInstancesStorageElement::UInstancesStorageElement(const UInstancesStorageElement &copy)
 : Object(copy.Object), UseFlag(copy.UseFlag)
{
}

UInstancesStorageElement::UInstancesStorageElement(const UEPtr<UAContainer> &object, bool useflag)
 : Object(object), UseFlag(useflag)
{

}


UInstancesStorageElement::~UInstancesStorageElement(void)
{
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UInstancesStorageElement& UInstancesStorageElement::operator = (const UInstancesStorageElement &copy)
{
 // ��������� �� ������
 Object=copy.Object;

 // ������� ���� �������� �� ������
 UseFlag=copy.UseFlag;

 return *this;
}

// ��������� ���������
bool UInstancesStorageElement::operator < (const UInstancesStorageElement &value)
{
 return Object<value.Object;
}

bool UInstancesStorageElement::operator > (const UInstancesStorageElement &value)
{
 return Object>value.Object;
}

bool UInstancesStorageElement::operator <= (const UInstancesStorageElement &value)
{
 return Object<=value.Object;
}

bool UInstancesStorageElement::operator >= (const UInstancesStorageElement &value)
{
 return Object>=value.Object;
}

bool UInstancesStorageElement::operator == (const UInstancesStorageElement &value)
{
 return Object == value.Object;
}

bool UInstancesStorageElement::operator != (const UInstancesStorageElement &value)
{
 return Object != value.Object;
}
// --------------------------

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
 ClearObjectsStorage();
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
const UId& UAStorage::FindClassId(const NameT &name) const
{
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  throw new EClassNameNotExist(name);
 return I->second;
}

// ���������� ��� ������ �� ��� Id
const NameT UAStorage::FindClassName(const UId &id) const
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
 UEPtr<UAStorage> storage=classtemplate->GetStorage();
 if(storage)
  storage->PopObject(classtemplate);

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

 // ��������!!! ��� ������������, �����-�� ���.
// ClassesDescription[FindClassName(id)]=classtemplate->NewDescription();

 return id;
}

// ��������� ������� ������ ������� � ���������
UId UAStorage::AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid)
{
 if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  throw new EClassNameAlreadyExist(classname);

 UId id=AddClass(classtemplate,classid);
 ClassesLookupTable[classname]=id;
 ClassesDescription[classname]=classtemplate->NewDescription();
 ClassesDescription[classname]->SetClassNameValue(classname);
 return id;
}

// ������� ������� ������ ������� �� ���������
void UAStorage::DelClass(const UId &classid)
{
 UObjectsStorageCIterator temp=ObjectsStorage.find(classid);

 if(temp != ObjectsStorage.end() && temp->second.size() > 0)
  throw new EObjectStorageNotEmpty(classid);

 UClassesStorageIterator I=ClassesStorage.find(classid);
 std::string name=FindClassName(classid);

 if(I != ClassesStorage.end())
  ClassesStorage.erase(I);
 else
  throw new EClassIdNotExist(classid);

 UClassStorageElement element=I->second;
 if(element)
  delete element.Get();

 UClassesDescriptionIterator J=ClassesDescription.find(name);

 if(J != ClassesDescription.end())
 {
  if(J->second)
   delete J->second;

  ClassesDescription.erase(J);
 }


 for(map<NameT,UId>::iterator I=ClassesLookupTable.begin(),
							  J=ClassesLookupTable.end(); I!=J; ++I)
 {
  if(I->second == classid)
  {
   ClassesLookupTable.erase(I);
   break;
  }
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



// ������� ��� �� ������������ ������� ������� �� ���������
void UAStorage::FreeClassesStorage(void)
{
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageCIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() == 0)
  {
   DelClass(I->first);
   break;
  }
 }
}

// ������� ��� ������� ������� �� ���������
void UAStorage::ClearClassesStorage(void)
{
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() != 0)
   throw EObjectStorageNotEmpty(I->first);
 }

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
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  throw EClassIdNotExist(classid);

 UClassStorageElement tmpl=tmplI->second;
 UEPtr<UAContainer> classtemplate=dynamic_pointer_cast<UAContainer>(tmpl);

 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances != ObjectsStorage.end())
 {
  UInstancesStorageElement* element=0;// ��������!! instances->FindFree();
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
											   J=instances->second.end(); I!=J; ++I)
  {
   if(I->UseFlag == false)
   {
	element=&(*I);
	break;
   }
  }

  if(element)
  {
   UEPtr<UAContainer> obj=element->Object;

   if(obj)
   {
    obj->Default();
    if(!prototype)
	 classtemplate->Copy(obj,this);
	else
	 dynamic_pointer_cast<const UAContainer>(prototype)->Copy(obj,this);

    obj->SetActivity(true);
    element->UseFlag=true;
   }
   return static_pointer_cast<UAComponent>(obj);
  }
 }


 // ���� ���������� ������� �� �����
 UEPtr<UAContainer> obj=classtemplate->New();
 obj->Default();

 // � ������, ���� ������ ��������� ��������������� ��� ����� �� ���������...
 if(!prototype)
  classtemplate->Copy(obj,this);
 else
  // � ������, ���� ������ ��������� �� ��������� ��� ����� ����� ��������
  // �������
  dynamic_pointer_cast<const UAContainer>(prototype)->Copy(obj,this);

 PushObject(classid,obj);
 obj->SetActivity(true);

 return static_pointer_cast<UAComponent>(obj);
}

UEPtr<UAComponent> UAStorage::TakeObject(const NameT &classname, const UEPtr<UAComponent> prototype)
{
 return TakeObject(FindClassId(classname),prototype);
}


// ���������� Id ������, ���������� ������� 'object'
UId UAStorage::FindClass(UEPtr<UAComponent> object) const
{
 if(!object)
  return ForbiddenId;

 return object->GetClass();
}

// ��������� ���������� �� ������ 'object' � ���������
bool UAStorage::CheckObject(UEPtr<UAContainer> object) const
{
 if(!object)
  return false;

 UObjectsStorageCIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances != ObjectsStorage.end())
 {
  for(list<UInstancesStorageElement>::const_iterator I=instances->second.begin(),
								J=instances->second.end(); I!=J; ++I)
  {
   if(I->Object == object)
	return true;
  }
 }
 return false;
}

// ��������� ��������� ����� �������� � ���������
int UAStorage::CalcNumObjects(void) const
{
 int result=0;

 for(UObjectsStorageCIterator I=ObjectsStorage.begin(),J=ObjectsStorage.end(); I!=J; ++I)
  result+=I->second.size();

 return result;
}

int UAStorage::CalcNumObjects(const UId &classid) const
{
 UObjectsStorageCIterator instances=ObjectsStorage.find(classid);

 if(instances == ObjectsStorage.end())
  throw EClassIdNotExist(classid);

 return instances->second.size();
}

size_t UAStorage::CalcNumObjects(const string &classname) const
{
 return CalcNumObjects(FindClassId(classname));
}


// �������� ��� ��������� ������� �� ���������
void UAStorage::FreeObjectsStorage(void)
{
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
				 								instances != iend; ++instances)
 {
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J;)
  {
   if(!I->UseFlag)
   {
	J=I; ++J;
	UEPtr<UAContainer> object=I->Object;
	PopObject(instances,I);
	delete object;
	I=J;
   }
   else
    ++I;
  }
  instances->second.clear();
 }
}

// ������� ��� ������� �� ���������
void UAStorage::ClearObjectsStorage(void)
{
 for(UObjectsStorageIterator instances=ObjectsStorage.begin(),iend=ObjectsStorage.end();
				 								instances != iend; ++instances)
 {
  for(list<UInstancesStorageElement>::iterator I=instances->second.begin(), J=instances->second.end(); I!=J; ++I)
   I->Object->Free();
 }

 FreeObjectsStorage();
}

// --------------------------

// --------------------------
// ������ ���������� ��������� �������
// --------------------------
// ���������� XML �������� ������
const UEPtr<UComponentDescription> UAStorage::GetClassDescription(const UId &classid) const
{
 UClassesDescriptionCIterator I=ClassesDescription.find(FindClassName(classid));

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

 ClassesDescription[FindClassName(classid)]=description;
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
  xml.AddNode(I->first);
  I->second->Save(xml);
  xml.SelectUp();
 }
}

// ��������� �������� ���� ������� �� xml
void UAStorage::LoadClassesDescription(Serialize::USerStorageXML &xml)
{
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  if(!xml.SelectNode(I->first))
   continue;
  I->second->Load(xml);
  xml.SelectUp();
 }
}

// ��������� ����� �������� ���� ������� � xml
bool UAStorage::SaveCommonClassesDescription(Serialize::USerStorageXML &xml)
{
 xml.AddNode("Default");
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
 if(xml.SelectNode("Default"))
 {
  UContainerDescription::LoadCommon(xml);
  xml.SelectUp();
 }

 UClassesDescriptionIterator I=ClassesDescription.begin();

 while(I != ClassesDescription.end())
 {
  dynamic_pointer_cast<UContainerDescription>(I->second)->RemoveCommonDuplicatesProperties();
  ++I;
 }

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ���������� ��������
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// --------------------------
// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
void UAStorage::PushObject(const UId &classid, UEPtr<UAContainer> object)
{
 UEPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;

 UInstancesStorage &instances=ObjectsStorage[classid];

 UInstancesStorageElement element(object,true);
 list<UInstancesStorageElement>::iterator instI=instances.insert(instances.end(),element);
 object->ObjectIterator=&(*instI);
 object->SetClass(classid);

 object->SetStorage(this);
}

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
UId UAStorage::PopObject(UEPtr<UAContainer> object)
{
 UObjectsStorageIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances == ObjectsStorage.end())
  throw new EClassIdNotExist(object->GetClass());

 for(list<UInstancesStorageElement>::iterator I=instances->second.begin(),
						J=instances->second.end(); I!=J; ++I)
 {
  if(I->Object == object)
   return PopObject(instances, I);
 }

 return ForbiddenId;
}

// ���������� ������ � ������ ���������
void UAStorage::MoveObject(UEPtr<UAContainer> object, UEPtr<UAStorage> newstorage)
{
 newstorage->PushObject(PopObject(object),object);
}

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
void UAStorage::ReturnObject(UEPtr<UAComponent> object)
{
 UEPtr<UAContainer> obj=dynamic_pointer_cast<UAContainer>(object);
 obj->ObjectIterator->UseFlag=false;
 obj->Activity=false;
 obj->BreakOwner();
}

// � ������ ������ ���������� ForbiddenId
UId UAStorage::PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator)
{
 UEPtr<UAContainer> object=object_iterator->Object;

 instance_iterator->second.erase(object_iterator);

 UId classid=object->GetClass();
 object->ObjectIterator=0;
 object->SetStorage(0);
 object->SetClass(ForbiddenId);
 return classid;
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

