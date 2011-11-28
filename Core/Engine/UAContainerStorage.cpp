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

//#include <algorithm>
//#include <sstream>
#include <string.h>
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
{
 // ��������� �� ������
 Object=0;

 // ������� ���� �������� �� ������
 UseFlag=false;
}

UInstancesStorageElement::UInstancesStorageElement(const UInstancesStorageElement &copy)
{
 *this=copy;
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
// ������ ������� � �������� �����������
// --------------------------
// ���������� Id ������ �� ��� �����
const UId& UAContainerStorage::GetClassId(const NameT &name) const
{
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  return ForbiddenId;
 else return I->second;
}

// ���������� ��� ������ �� ��� Id
const NameT UAContainerStorage::GetClassName(const UId &id) const
{
 map<NameT,UId>::const_iterator I,J;
 I=ClassesLookupTable.begin();
 J=ClassesLookupTable.end();
 while(I != J)
 {
  if(I->second == id)
   return I->first;
  ++I;
 }
 return NameT("");
}
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� ������� ������ ������� � ��������� � �������������� ����������� id
// ���������� id ������
/*UId UAContainerStorage::AddClass(UAComponent *classtemplate)
{
 return UAStorage::AddClass(classtemplate);
} */

// ��������� ������� ������ ������� � ���������
UId UAContainerStorage::AddClass(UAComponent *classtemplate, const string &classname, const UId &classid)
{
 UAContainerStorage *storage=dynamic_cast<UAContainerStorage*>(classtemplate->GetStorage());
// UAContainer* contclasstemplate=dynamic_cast<UAContainer*>(classtemplate);
 UObjectsStorageIterator temp=ObjectsStorage.find(classid);
 if(storage)
  storage->PopObject(UEPtr<UAContainer>(dynamic_cast<UAContainer*>(classtemplate)));
 UId id=UAStorage::AddClass(classtemplate,classid);
 if(id != ForbiddenId)
 {
  if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  {
   UAStorage::DelClass(classid);
   return ForbiddenId;
  }
  ClassesLookupTable[classname]=id;
 }

 return id;
}

UId UAContainerStorage::AddClass(UAComponent *classtemplate, const UId &classid)
{
 return UAStorage::AddClass(classtemplate, classid);
}
/*
UId UAContainerStorage::AddClass(const NameT &classname, UAComponent *classtemplate)
{
 UId id;

 if(!classtemplate || classname == "")
  return false;

 id=AddLookupClass(classname);
 if(id == ForbiddenId)
  return false;

 if(classtemplate->GetStorage())
 {
  UAContainerStorage *storage=dynamic_cast<UAContainerStorage*>(classtemplate->GetStorage());
  if(storage)
   storage->PopObject(static_cast<UAContainer*>(classtemplate));
 }
 return AddClass(id,classtemplate);
}       */

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
bool UAContainerStorage::DelClass(const UId &classid)
{
 UObjectsStorageCIterator temp=ObjectsStorage.find(classid);

 if(temp != ObjectsStorage.end() && temp->second.size() > 0)
  return false;

 bool res=UAStorage::DelClass(classid);

 map<NameT,UId>::iterator oI=ClassesLookupTable.begin(),
                          oJ=ClassesLookupTable.end();
 while(oI != oJ)
  {
   if(oI->second == classid)
    {
     ClassesLookupTable.erase(oI);
     break;
    }
   ++oI;
  }

 return res;
}

// ���������� ������ ��������������� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UAContainerStorage::GetClassIdList(UId* buffer, int max_num_classes) const
{
 return UAStorage::GetClassIdList(buffer,max_num_classes);
}

void UAContainerStorage::GetClassIdList(UIdVector &buffer) const
{
 buffer.Resize(GetNumClasses());
 GetClassIdList(&buffer[0],buffer.GetSize());
}

// ���������� ������ ���� ���� ������� ���������
// ����� 'buffer' ����� ������ �� ���������� ��������
void UAContainerStorage::GetClassNameList(vector<NameT> &buffer) const
{
 map<NameT,UId>::const_iterator I,J;
 NameT* pbuffer=0;

 I=ClassesLookupTable.begin();
 J=ClassesLookupTable.end();
 buffer.resize(ClassesLookupTable.size());

 if(buffer.size()>0)
  pbuffer=&buffer[0];
 while(I != J)
  {
   *pbuffer++=I->first;
   ++I;
  }
}

// ������� ��� �� ������������ ������� ������� �� ���������
void UAContainerStorage::FreeClassesStorage(void)
{
 UClassesStorageCIterator I;
 for(I=ClassesStorage.begin();I != ClassesStorage.end();++I)
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
bool UAContainerStorage::ClearClassesStorage(void)
{
 UClassesStorageCIterator I;
 for(I=ClassesStorage.begin();I != ClassesStorage.end();++I)
  {
   UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
   if(temp != ObjectsStorage.end() && temp->second.size() != 0)
    return false;
  }

 return UAStorage::ClearClassesStorage();
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
UESharedPtr<UAComponent> UAContainerStorage::TakeObject(const UId &classid, const UAComponent *prototype)
{
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  return 0;

 UClassStorageElement tmpl=tmplI->second;
// UAContainer* classtemplate=dynamic_cast<UAContainer*>(tmpl.operator ->());
 UESharedPtr<UAContainer> classtemplate=dynamic_pointer_cast<UAContainer>(tmpl);

 if(!classtemplate)
  return 0;

 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances != ObjectsStorage.end())
 {
  UInstancesStorageElement* element=0;// ��������!! instances->FindFree();
//  UInstancesStorageElement* element=instances->FindFree();
  list<UInstancesStorageElement>::iterator elementI;
  for(elementI=instances->second.begin(); elementI!= instances->second.end();++elementI)
  {
   if(elementI->UseFlag == false)
   {
	element=&(*elementI);
	break;
   }
  }

  if(element)
  {
   UESharedPtr<UAContainer> obj=element->Object;

   if(obj)
   {
    if(!prototype)
    {
	 if(!classtemplate->Copy(obj,this))
      return 0;
    }
    else
	{
	 if(!dynamic_cast<const UAContainer*>(prototype)->Copy(obj,this))
      return 0;
    }

    obj->SetActivity(true);
    element->UseFlag=true;
   }
   return static_pointer_cast<UAComponent>(obj);
  }
 }


 // ���� ���������� ������� �� �����
// UAComponent *obj=CI->second->New(CI->second->Name());
 UESharedPtr<UAContainer> obj=classtemplate->New();
 if(!obj)
  return 0;

 if(!obj->Default())
 {
//   delete obj;
//   obj=0;
   return 0;
 }

 // � ������, ���� ������ ��������� ��������������� ��� ����� �� ���������...
 if(!prototype)
 {
  if(!classtemplate->Copy(obj,this))
  {
//   delete obj;
//   obj=0;
   return 0;
  }
 }
 else
 {
  // � ������, ���� ������ ��������� �� ��������� ��� ����� ����� ��������
  // �������
  if(!dynamic_cast<const UAContainer*>(prototype)->Copy(obj,this))
  {
//   delete obj;
//   obj=0;
   return 0;
  }
 }

 if(!PushObject(classid,obj))
  {
//   delete obj;
//   obj=0;
   return 0;
  }

// PushObject(classid,obj);
 obj->SetActivity(true);

 return static_pointer_cast<UAComponent>(obj);
}

UESharedPtr<UAComponent> UAContainerStorage::TakeObject(const NameT &classname, const UAComponent *prototype)
{
 return TakeObject(GetClassId(classname),prototype);
}

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
bool UAContainerStorage::ReturnObject(UEPtr<UAComponent> object)
{
 if(!object)
  return false;

 UEPtr<UAContainer> obj=dynamic_pointer_cast<UAContainer>(object);
 obj->ObjectIterator->UseFlag=false;
 obj->Activity=false;
 obj->SetOwner(0); // �������� ��� �� ���? ��� �� ���������
// FreeObjectsTableIterator tI=FreeObjectsTable.find(object->GetClass());
// if(tI != FreeObjectsTable.end())
// {
//  tI->second.push_back(object->ObjectIterator);
// }

 // ���������� ��� ���������� ����������
 for(int i=0;i<obj->GetNumComponents();i++)
 {
  UEPtr<UAComponent> temp=static_pointer_cast<UAComponent>(obj->GetComponentByIndex(i));
  if(!ReturnObject(temp))
   return false;
 }
 return true;
}

// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
bool UAContainerStorage::PushObject(const UId &classid, UESharedPtr<UAContainer> object)
{
 if(!object || classid == ForbiddenId || object->GetStorage())
  return false;

 UESharedPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;
 if(!classtemplate)
  return false;

 ObjectsStorage[classid];
 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances == ObjectsStorage.end())
 {
  return false;
 }
 UInstancesStorageElement element;
 element.Object=object;
 element.UseFlag=true;
 list<UInstancesStorageElement>::iterator instI=instances->second.insert(instances->second.end(),element);
// instances->second.push_back(element);
 object->ObjectIterator=&(*instI);
 object->SetClass(classid);


// ObjectsStorage[classid][object]=true;//(object->GetOwner())?true:false;
 object->SetStorage(this);
 return true;
}

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
UId UAContainerStorage::PopObject(UEPtr<UAContainer> object)
{
 if(!object)
  return ForbiddenId;

 UId classid=object->GetClass();
 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances != ObjectsStorage.end())
 {
  list<UInstancesStorageElement>::iterator elementI;
  for(elementI=instances->second.begin(); elementI!= instances->second.end();++elementI)
  {
   if(elementI->Object == object)
   {
	instances->second.erase(elementI);
	break;
   }
  }
 }
 else
  classid=ForbiddenId;

 object->ObjectIterator=0;
 object->SetStorage(0);
 object->SetClass(ForbiddenId);

 return classid;
}

// ���������� ������ � ������ ���������
bool UAContainerStorage::MoveObject(UESharedPtr<UAContainer> object, UAContainerStorage *newstorage)
{
 if(!newstorage)
  return false;

 return newstorage->PushObject(PopObject(object),object);
}

// ��������� ���������� �� ������ 'object' � ���������
bool UAContainerStorage::CheckObject(UESharedPtr<UAContainer> object) const
{
 if(!object)
  return false;

 UObjectsStorageCIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances != ObjectsStorage.end())
 {
  list<UInstancesStorageElement>::const_iterator elementI;
  for(elementI=instances->second.begin(); elementI!= instances->second.end();++elementI)
  {
   if(elementI->Object == object)
   {
	return true;
   }
  }
 }
 return false;
}

// ��������� ��������� ����� �������� � ���������
int UAContainerStorage::CalcNumObjects(void) const
{
 int result=0;

 UObjectsStorageCIterator instances;
 for(instances=ObjectsStorage.begin();instances != ObjectsStorage.end();++instances)
 {
  result+=instances->second.size();
 }

 return result;
}

int UAContainerStorage::CalcNumObjects(const UId &classid) const
{
 UObjectsStorageCIterator instances=ObjectsStorage.find(classid);

 if(instances != ObjectsStorage.end())
  return instances->second.size();

 return 0;
}

size_t UAContainerStorage::CalcNumObjects(const string &classname) const
{
 return CalcNumObjects(GetClassId(classname));
}


// �������� ��� ��������� ������� �� ���������
void UAContainerStorage::FreeObjectsStorage(void)
{
 UObjectsStorageIterator instances;
 for(instances=ObjectsStorage.begin();instances != ObjectsStorage.end();++instances)
 {
  list<UInstancesStorageElement>::iterator elementI,elementJ;
  for(elementI=instances->second.begin(); elementI!= instances->second.end();++elementI)
  {
   if(!elementI->UseFlag)
   {
    elementJ=elementI; ++elementJ;
	instances->second.erase(elementI);
	elementI=elementJ;
   }
  }
 }
}

// ������� ��� ������� �� ���������
void UAContainerStorage::ClearObjectsStorage(void)
{
 ObjectsStorage.clear();
/* for(int i=0;i<ObjectsStorage.GetSize();i++)
 {
  UInstancesStorage& instances=ObjectsStorage[i].Objects;
  UInstancesStorageElement* current=instances.GetFirst();
  UInstancesStorageElement* next=0;
  while(current != 0)
  {
   next=current->Next;
   if(current->Object)
   {
    current->Object->SetStorage(0);
	delete current->Object;
	current->Object=0;
   }
   current=next;
  }
  instances.Clear();
 }  */
}
// --------------------------

// --------------------------
// ������� ������ ������� ������������ �������
// --------------------------
// ��������� ����� � ������ 'name' � ������� ������������
UId UAContainerStorage::AddLookupClass(const NameT &name)
{
 if(ClassesLookupTable.find(name) != ClassesLookupTable.end())
  return ForbiddenId;

 ClassesLookupTable.insert(make_pair(name,LastClassId+1));

 return LastClassId+1;
}

// ������� ����� � ������ 'name' �� ������� �����������
bool UAContainerStorage::DelLookupClass(const NameT &name)
{
 map<NameT,UId>::iterator I=ClassesLookupTable.find(name);

 if(I == ClassesLookupTable.end())
  return false;

 ClassesLookupTable.erase(I);
 return true;
}
// --------------------------
/* *************************************************************************** */

}

#endif
