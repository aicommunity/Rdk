/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UACONTAINER_STORAGE_CPP
#define UACONTAINER_STORAGE_CPP

#include <string>
#include "UAContainerStorage.h"

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
// Class UAContainerStorage
/* *************************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UAContainerStorage::UAContainerStorage(void)
{
}

UAContainerStorage::~UAContainerStorage(void)
{
 ClearObjectsStorage();
}
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
UId UAContainerStorage::AddClass(UEPtr<UAComponent> classtemplate, const UId &classid)
{
 UEPtr<UAContainerStorage> storage=dynamic_pointer_cast<UAContainerStorage>(classtemplate->GetStorage());
 UObjectsStorageIterator temp=ObjectsStorage.find(classid);
 if(storage)
  storage->PopObject(dynamic_pointer_cast<UAContainer>(classtemplate));

 return UAStorage::AddClass(classtemplate,classid);
}

UId UAContainerStorage::AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid)
{
 return UAStorage::AddClass(classtemplate, classname, classid);
}


// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
void UAContainerStorage::DelClass(const UId &classid)
{
 UObjectsStorageCIterator temp=ObjectsStorage.find(classid);

 if(temp != ObjectsStorage.end() && temp->second.size() > 0)
  throw new EObjectStorageNotEmpty(classid);

 UAStorage::DelClass(classid);

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

// ������� ��� �� ������������ ������� ������� �� ���������
void UAContainerStorage::FreeClassesStorage(void)
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
// ���������� false ���� � ��������� ������������ �������
void UAContainerStorage::ClearClassesStorage(void)
{
 for(UClassesStorageCIterator I=ClassesStorage.begin(),
 							  J=ClassesStorage.end(); I!=J; ++I)
 {
  UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
  if(temp != ObjectsStorage.end() && temp->second.size() != 0)
   throw EObjectStorageNotEmpty(I->first);
 }

 UAStorage::ClearClassesStorage();
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
UEPtr<UAComponent> UAContainerStorage::TakeObject(const UId &classid, const UEPtr<UAComponent> prototype)
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

UEPtr<UAComponent> UAContainerStorage::TakeObject(const NameT &classname, const UEPtr<UAComponent> prototype)
{
 return TakeObject(FindClassId(classname),prototype);
}

// ��������� ���������� �� ������ 'object' � ���������
bool UAContainerStorage::CheckObject(UEPtr<UAContainer> object) const
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
int UAContainerStorage::CalcNumObjects(void) const
{
 int result=0;

 for(UObjectsStorageCIterator I=ObjectsStorage.begin(),J=ObjectsStorage.end(); I!=J; ++I)
  result+=I->second.size();

 return result;
}

int UAContainerStorage::CalcNumObjects(const UId &classid) const
{
 UObjectsStorageCIterator instances=ObjectsStorage.find(classid);

 if(instances == ObjectsStorage.end())
  throw EClassIdNotExist(classid);

 return instances->second.size();
}

size_t UAContainerStorage::CalcNumObjects(const string &classname) const
{
 return CalcNumObjects(FindClassId(classname));
}


// �������� ��� ��������� ������� �� ���������
void UAContainerStorage::FreeObjectsStorage(void)
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
 }
}

// ������� ��� ������� �� ���������
void UAContainerStorage::ClearObjectsStorage(void)
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
// ������� ������ ���������� ���������� ��������
// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// --------------------------
// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
void UAContainerStorage::PushObject(const UId &classid, UEPtr<UAContainer> object)
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
UId UAContainerStorage::PopObject(UEPtr<UAContainer> object)
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
void UAContainerStorage::MoveObject(UEPtr<UAContainer> object, UEPtr<UAContainerStorage> newstorage)
{
 newstorage->PushObject(PopObject(object),object);
}

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
void UAContainerStorage::ReturnObject(UEPtr<UAComponent> object)
{
 UEPtr<UAContainer> obj=dynamic_pointer_cast<UAContainer>(object);
 obj->ObjectIterator->UseFlag=false;
 obj->Activity=false;
 obj->BreakOwner();
}

// � ������ ������ ���������� ForbiddenId
UId UAContainerStorage::PopObject(UObjectsStorageIterator instance_iterator, list<UInstancesStorageElement>::iterator object_iterator)
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
/* *************************************************************************** */
/*
// --------------------------
// ������������ � �����������
// --------------------------
UAContainerStorage::EObjectIdNotExist::EObjectIdNotExist(UId id)
 : Id(id)
{

}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string UAContainerStorage::EObjectIdNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Id=")+sntoa(Id);
}
// --------------------------
    */
}

#endif
