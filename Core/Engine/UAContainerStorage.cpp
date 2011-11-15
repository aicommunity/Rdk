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
//class UInstancesStorageElement
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorageElement::UInstancesStorageElement(void)
{
 // ��������� �� ���������� ������� ������
 Prev=0;

 // ��������� �� ��������� ������� ������
 Next=0;

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
 // ��������� �� ���������� ������� ������
 Prev=copy.Prev;

 // ��������� �� ��������� ������� ������
 Next=copy.Next;

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


// ������ ������������ �������� ������������� ������ � ������ �������������
// <��������� �������, ���� true ���� ����� ��� false ���� ��������>
//class UInstancesStorage
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UInstancesStorage::UInstancesStorage(void)
{
 // ��������� �� ������ ������� ������
 First=0;

 // ��������� �� ��������� ������� ������
 Last=0;

 // ������ ������
 Size=0;
}

UInstancesStorage::UInstancesStorage(const UInstancesStorage& copy)
{
 *this=copy;
}

UInstancesStorage::~UInstancesStorage(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ������� ������
void UInstancesStorage::Clear(void)
{
 UInstancesStorageElement *current=First;
 UInstancesStorageElement *next=0;
 while(current)
 {
  next=current->Next;
  delete current;
  current=next;
 }
 Size=0;
 First=Last=0;
}

// ���������� ������ ������
int UInstancesStorage::GetSize(void) const
{
 return Size;
}

// ��������� ������� � ������ � ���������� ��������� �� ���������� �������
// �������� ������������� ����������� �� object
// ����� �������� ������ �� ������������ � ��������� ������������ ������ ����
// ����� ������ ��� ����������
UInstancesStorageElement* UInstancesStorage::Add(UInstancesStorageElement* value)
{
 if(!value)
  return 0;

 UInstancesStorageElement *current=First;

 if(!current)
 {
  First=Last=value;
  value->Prev=0;
  value->Next=0;
  Size=1;
  return value;
 }

 while(current)
 {
  if(*value < *current)// || current == Last)
  {
   break;
  }
  current=current->Next;
 }

 // !!! ��� ���� ���������� - ���������� �������� ������� � ������ ������������ ������
 if(current)
 {
  value->Next=current->Next;
  value->Prev=current;
  current->Next=value;
  if(value->Next)
   value->Next->Prev=value;

//  if(current->Next)
//   current->Next=value;

  if(Last == current)
   Last=value;
 }
 else
 {
  value->Next=0;
  if(Last)
   Last->Next=value;
  value->Prev=Last;
  Last=value;
 }

 ++Size;
 return value;
}

UInstancesStorageElement* UInstancesStorage::Add(UAContainer* object, bool useflag)
{
 UInstancesStorageElement* element=new UInstancesStorageElement;
 element->Object=object;
 element->UseFlag=useflag;
 return Add(element);
}

// ������� ������� �� ������
void UInstancesStorage::Del(UAContainer* object)
{
 UInstancesStorageElement* element=Find(object);
 Del(element);
}

// ������� ������� �� ������ ������ � ��� ������, ���� value �������� ��������� ������
void UInstancesStorage::Del(UInstancesStorageElement* value)
{
 UInstancesStorageElement *current=First;
 UInstancesStorageElement *next=0;
 while(current)
 {
  next=current->Next;
  if(value == current)
  {
   if(next)
    next->Prev=current->Prev;
   if(current->Prev)
    current->Prev->Next=next;

   if(current == First)
    First=current->Next;
   if(current == Last)
    Last=current->Prev;
   delete current;
   --Size;
   break;
  }
  current=next;
 }
}

// ���� �������� ������ � ���������� ��������� �� ������� ������ � ���� ��������
UInstancesStorageElement* UInstancesStorage::Find(const UAContainer* object)
{
 UInstancesStorageElement *current=First;
 while(current)
 {
  if(current->Object == object)
   return current;
  current=current->Next;
 }
 return 0;
}

const UInstancesStorageElement* UInstancesStorage::Find(const UAContainer* object) const
{
 UInstancesStorageElement *current=First;
 while(current)
 {
  if(current->Object == object)
   return current;
  current=current->Next;
 }
 return 0;
}

// ���� "���������" � ���������� ��������� �� ������� ������ � ���� ��������
UInstancesStorageElement* UInstancesStorage::FindFree(void)
{
 UInstancesStorageElement *current=First;
 while(current)
 {
  if(!current->UseFlag)
   return current;
  current=current->Next;
 }
 return 0;
}

// ��������� �� ������ ������� ������
UInstancesStorageElement* UInstancesStorage::GetFirst(void)
{
 return First;
}

const UInstancesStorageElement* UInstancesStorage::GetFirst(void) const
{
 return First;
}

// ��������� �� ��������� ������� ������
UInstancesStorageElement* UInstancesStorage::GetLast(void)
{
 return Last;
}

const UInstancesStorageElement* UInstancesStorage::GetLast(void) const
{
 return Last;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UInstancesStorage& UInstancesStorage::operator = (const UInstancesStorage &copy)
{
 Clear();

 UInstancesStorageElement *current=copy.First;
 UInstancesStorageElement *element=0;
 while(current)
 {
  element=new UInstancesStorageElement(*current);
  Add(element);
  current=current->Next;
 }
 return *this;
}
// --------------------------

/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
// ������� ��������� �������� �������
//struct UObjectStorageElement
// --------------------------
// ������������ � �����������
// --------------------------
UObjectStorageElement::UObjectStorageElement(void)
{
 Id=ForbiddenId;
}

UObjectStorageElement::UObjectStorageElement(const UObjectStorageElement &copy)
{
 *this=copy;
}

UObjectStorageElement::~UObjectStorageElement(void)
{
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UObjectStorageElement& UObjectStorageElement::operator = (const UObjectStorageElement &copy)
{
 Id=copy.Id;
 Objects=copy.Objects;

 return *this;
}
// --------------------------

/* *********************************************************************** */
/* *********************************************************************** */
// --------------------------
// ������������ � �����������
// --------------------------
UObjectsStorage::UObjectsStorage(void)
{
 Objects=0;
 Size=RealSize=0;
}

UObjectsStorage::UObjectsStorage(const UObjectsStorage &copy)
{
 Objects=0;
 Size=RealSize=0;
 *this=copy;
}

UObjectsStorage::~UObjectsStorage(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void UObjectsStorage::Clear(void)
{
 if(Objects)
 {
  delete[] Objects;
  Objects=0;
 }
 Size=RealSize=0;
}

// ���������� ������ �������
int UObjectsStorage::GetSize(void) const
{
 return Size;
}

// �������� ������ ���������
// ����� ��������� ������ ������
void UObjectsStorage::Resize(int newsize)
{
 int newrealsize=0;
 if(newsize < 8)
  newrealsize=16;
 else
  newrealsize=newsize<<2;
 if(RealSize<newsize || !Objects)
 {
  UObjectStorageElement* newbuffer=new UObjectStorageElement[newrealsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Objects[i];
//  memcpy(newbuffer,Objects,sizeof(UObjectStorageElement)*Size);

  if(Objects)
   delete []Objects;
  Objects=newbuffer;
  RealSize=newrealsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ���� ����� �� Id
UInstancesStorage* UObjectsStorage::Find(const UId &id)
{
 UObjectStorageElement* pclasses=Objects;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
   return &pclasses->Objects;

 return 0;
}

const UInstancesStorage* UObjectsStorage::Find(const UId &id) const
{
 UObjectStorageElement* pclasses=Objects;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
   return &pclasses->Objects;

 return 0;
}


// ���� ������ �� Id � ������� ��� �� �������
void UObjectsStorage::Erase(const UId &id)
{
 UObjectStorageElement* pclasses=Objects;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
  {
   if(i != Size-1)
    memmove(pclasses,pclasses+1,sizeof(UObjectStorageElement)*(Size-i));
   Resize(Size-1);
  }
}

// ��������� ����� ������� � ����� ���������
UObjectStorageElement* UObjectsStorage::PushBack(const UObjectStorageElement &classelement)
{
 Resize(Size+1);
 Objects[Size-1]=classelement;
 return &Objects[Size-1];
}

UObjectStorageElement* UObjectsStorage::PushBack(const UId &id, UInstancesStorage *instance)
{
 UObjectStorageElement classelement;
 classelement.Id=id;
 classelement.Objects=*instance;
 return PushBack(classelement);
}

// ������� ����� ������� � ��������� � �������� id � ���������� ��������� �� ����
// ���� ����� ������� ��� ���������� �� ���������� ��������� �� ���� �������
UInstancesStorage* UObjectsStorage::Create(const UId &id)
{
 UInstancesStorage* instance=Find(id);
 if(instance)
  return instance;

 UObjectStorageElement classelement;
 classelement.Id=id;
 return &PushBack(classelement)->Objects;
}

// ���������� ��������� �� ������ �������
UObjectStorageElement* UObjectsStorage::GetObjects(void) const
{
 return Objects;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UObjectsStorage& UObjectsStorage::operator = (const UObjectsStorage &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Objects[i]=copy.Objects[i];

 return *this;
}

// �������� �������
UObjectStorageElement& UObjectsStorage::operator [] (int i)
{
 return Objects[i];
}

const UObjectStorageElement& UObjectsStorage::operator [] (int i) const
{
 return Objects[i];
}
// --------------------------
/* *********************************************************************** */
/* *********************************************************************** */

/* *********************************************************************** */
/* *********************************************************************** */
/*// ������
// --------------------------
// ������������ � �����������
// --------------------------
UFreeObjectsTableIteratorsVector::UFreeObjectsTableIteratorsVector(void)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;
}

UFreeObjectsTableIteratorsVector::UFreeObjectsTableIteratorsVector(const UFreeObjectsTableIteratorsVector &copy)
{
 // ������ ������� �����������
 Size=0;

 // ������ �����������
 Buffer=0;

 // �������� ������ �������
 RealSize=0;

 *this=copy;
}

UFreeObjectsTableIteratorsVector::~UFreeObjectsTableIteratorsVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���������� ������ �������
int UFreeObjectsTableIteratorsVector::GetSize(void) const
{
 return Size;
}

// �������� ������ ������� � ����������� ������� ������
void UFreeObjectsTableIteratorsVector::Resize(int newsize)
{
 if(RealSize<newsize || !Buffer)
 {
  PUInstancesStorageElement *newbuffer=new PUInstancesStorageElement[newsize];
  for(int i=0;i<Size;i++)
   newbuffer[i]=Buffer[i];

  for(int i=Size;i<newsize;i++)
   newbuffer[i]=0;

  if(Buffer)
   delete []Buffer;
  Buffer=newbuffer;
  RealSize=Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ������� ������
void UFreeObjectsTableIteratorsVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// ���������� ��������� �� ������ ������
PUInstancesStorageElement* UFreeObjectsTableIteratorsVector::GetBuffer(void)
{
 return Buffer;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ���������
// --------------------------
// ���������� ��������� �� ������ "���������" ������� �������, ��� 0
// ���� ������� �� �������
PUInstancesStorageElement UFreeObjectsTableIteratorsVector::FindFreeObject(void)
{
 PUInstancesStorageElement* buffer=Buffer;

 for(int i=0;i<Size;i++,buffer++)
  if(!(*buffer)->UseFlag)
   return *buffer;

 return 0;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UFreeObjectsTableIteratorsVector& UFreeObjectsTableIteratorsVector::operator = (const UFreeObjectsTableIteratorsVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// �������� �������
PUInstancesStorageElement UFreeObjectsTableIteratorsVector::operator [] (int index)
{
 return Buffer[index];
}

UInstancesStorageElement& UFreeObjectsTableIteratorsVector::operator () (int index)
{
 return *Buffer[index];
}
// --------------------------

// ��������� ���������� �� ��������� ������� �������� �������
//class UFreeObjectsTable

// Methods
// --------------------------
// ������������ � �����������
// --------------------------
UFreeObjectsTable::UFreeObjectsTable(void)
{
 // Number of classes into storage
 Size=0;

 // real number of allocated elements
 RealSize=0;
}

UFreeObjectsTable::UFreeObjectsTable(const UFreeObjectsTable &copy)
{
 *this=copy;
}

UFreeObjectsTable::~UFreeObjectsTable(void)
{
 Clear();
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ���������
// --------------------------
// ���������� ��������� �� ��������� ������ ��������� ������
// ��� 0 ���� ������ �� ������
PUInstancesStorageElement UFreeObjectsTable::FindFreeObject(const UId &classid)
{
 UFreeObjectsTableIteratorsVector *vec=Find(classid);

 if(!vec)
  return 0;

 PUInstancesStorageElement elem=vec->FindFreeObject();

 if(!elem)
  return 0;


 return elem;
}

// ��������� �������� ������� � ��������� ���������� �� ��������� �������
// ���������� ��������� �� ��������� ������
PUInstancesStorageElement UFreeObjectsTable::AddFreeObject(const UId &classid, UAContainer *object)
{
 UFreeObjectsTableIteratorsVector* vec=Find(classid);
 if(!vec)
  return 0;

 UInstancesStorageElement element;
 element.Object=object;
 element.UseFlag=false;
// vec->Resize(vec->GetSize()+1)
// (*vec)[]

}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ���������
void UFreeObjectsTable::Clear(void)
{
 if(FreeObjects)
 {
  delete[] FreeObjects;
  FreeObjects=0;
 }
 Size=RealSize=0;
}

// �������� ������ ���������
// ����� ��������� ������ ������
void UFreeObjectsTable::Resize(int newsize)
{
 int newrealsize=0;
 if(newsize < 8)
  newrealsize=16;
 else
  newrealsize=newsize<<2;
 if(RealSize<newsize || !FreeObjects)
 {
  UFreeObjectsTableElement* newbuffer=new UFreeObjectsTableElement[newrealsize];
  memcpy(newbuffer,FreeObjects,sizeof(UFreeObjectsTableElement)*Size);

  if(FreeObjects)
   delete []FreeObjects;
  FreeObjects=newbuffer;
  RealSize=newrealsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// ���������� ������ �������
int UFreeObjectsTable::GetSize(void) const
{
 return Size;
}

// ���� ����� �� Id
UFreeObjectsTableIteratorsVector* UFreeObjectsTable::Find(const UId &id) const
{
 UFreeObjectsTableElement* pclasses=FreeObjects;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
   return &pclasses->Objects;

 return 0;
}

// ���� ����� �� Id � ������� ��� �� �������
void UFreeObjectsTable::Erase(const UId &id)
{
 UFreeObjectsTableElement* pclasses=FreeObjects;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
  {
   if(i != Size-1)
    memmove(pclasses,pclasses+1,sizeof(UFreeObjectsTableElement)*(Size-i));
   Resize(Size-1);
  }
}

// ��������� ����� ������� � ����� ���������
void UFreeObjectsTable::PushBack(const UFreeObjectsTableElement &classelement)
{
 Resize(Size+1);
 FreeObjects[Size-1]=classelement;
}

void UFreeObjectsTable::PushBack(const UId &id, UFreeObjectsTableIteratorsVector *iterators)
{
 UFreeObjectsTableElement classelement;
 classelement.Id=id;
 classelement.Objects=*iterators;
 PushBack(classelement);
}

// ���������� ��������� �� ������ �������
UFreeObjectsTableElement* UFreeObjectsTable::GetFreeObjects(void) const
{
 return FreeObjects;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� ������������
UFreeObjectsTable& UFreeObjectsTable::operator = (const UFreeObjectsTable &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  FreeObjects[i]=copy.FreeObjects[i];

 return *this;
}

// �������� �������
UFreeObjectsTableElement& UFreeObjectsTable::operator [] (int i)
{
 return FreeObjects[i];
}
// --------------------------
             */
/* *********************************************************************** */
/* *********************************************************************** */


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
 UInstancesStorage* temp=ObjectsStorage.Find(classid);
 if(storage)
  storage->PopObject(dynamic_cast<UAContainer*>(classtemplate));
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
 UInstancesStorage* temp=ObjectsStorage.Find(classid);

 if(temp && temp->GetSize() > 0)
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
   UInstancesStorage* temp=ObjectsStorage.Find(I->first);
   if(temp && temp->GetSize() == 0)
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
   UInstancesStorage* temp=ObjectsStorage.Find(I->first);
   if(temp && temp->GetSize() != 0)
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
UAContainer* UAContainerStorage::TakeObject(const UId &classid, const UAComponent *prototype)
{
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  return 0;

 UClassStorageElement tmpl=tmplI->second;
 UAContainer* classtemplate=dynamic_cast<UAContainer*>(tmpl.operator ->());
// USharedPtr<UAContainer> classtemplate=dynamic_cast<USharedPtr<UAContainer> >(ClassesStorage.Find(classid));
 if(!classtemplate)
  return 0;

 UInstancesStorage* instances=ObjectsStorage.Find(classid);
 if(instances)
 {
  UInstancesStorageElement* element=0;// ��������!! instances->FindFree();
//  UInstancesStorageElement* element=instances->FindFree();

  if(element)
  {
   UAContainer *obj=element->Object;

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
   return obj;
  }
 }


 // ���� ���������� ������� �� �����
// UAComponent *obj=CI->second->New(CI->second->Name());
 UAContainer* obj=classtemplate->New();
 if(!obj)
  return 0;

 if(!obj->Default())
 {
   delete obj;
   obj=0;
   return 0;
 }

/* if(!obj->Build())
 {
   delete obj;
   obj=0;
   return 0;
 }*/


 // � ������, ���� ������ ��������� ��������������� ��� ����� �� ���������...
 if(!prototype)
 {
  if(!classtemplate->Copy(obj,this))
  {
   delete obj;
   obj=0;
   return 0;
  }
 }
 else
 {
  // � ������, ���� ������ ��������� �� ��������� ��� ����� ����� ��������
  // �������
  if(!dynamic_cast<const UAContainer*>(prototype)->Copy(obj,this))
  {
   delete obj;
   obj=0;
   return 0;
  }
 }

 if(!PushObject(classid,obj))
  {
   delete obj;
   obj=0;
   return 0;
  }

// PushObject(classid,obj);
 obj->SetActivity(true);

 return obj;
}

UAComponent* UAContainerStorage::TakeObject(const NameT &classname, const UAComponent *prototype)
{
 return TakeObject(GetClassId(classname),prototype);
}

// ���������� ������ � ���������
// ��������� ������ ���������� ��� ��������� � ���������
// ���� 'Activity' ������� ������������ � false
// ���� ������ �� ���������� � ��������� - ������������ false
bool UAContainerStorage::ReturnObject(UAComponent *object)
{
 if(!object)
  return false;

 UAContainer *obj=dynamic_cast<UAContainer*>(object);
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
  if(!ReturnObject(obj->GetComponentByIndex(i)))
   return false;

 return true;
}

// ��������� ��� ��������� ������ � ���������
// ���� ������ ��� ����������� ����� ��������� �� ���������� false
bool UAContainerStorage::PushObject(const UId &classid, UAContainer *object)
{
 if(!object || classid == ForbiddenId || object->GetStorage())
  return false;

 USharedPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;
 if(!classtemplate)
  return false;

 UInstancesStorage* instances=ObjectsStorage.Create(classid);
 if(!instances)
 {
  return false;
 }
 object->ObjectIterator=instances->Add(object, true);
 object->SetClass(classid);


// ObjectsStorage[classid][object]=true;//(object->GetOwner())?true:false;
 object->SetStorage(this);
 return true;
}

// ������� ��� ��������� ������ �� ��������� � ����������
// ��� classid
// � ������ ������ ���������� ForbiddenId
UId UAContainerStorage::PopObject(UAContainer *object)
{
 if(!object)
  return ForbiddenId;

 UId classid=object->GetClass();
 UInstancesStorage* instances=ObjectsStorage.Find(classid);
 if(instances)
 {
  instances->Del(object);
 }
 else
  classid=ForbiddenId;

 object->ObjectIterator=0;
 object->SetStorage(0);
 object->SetClass(ForbiddenId);

 return classid;
}

// ���������� ������ � ������ ���������
bool UAContainerStorage::MoveObject(UAContainer *object, UAContainerStorage *newstorage)
{
 if(!newstorage)
  return false;

 return newstorage->PushObject(PopObject(object),object);
}

// ��������� ���������� �� ������ 'object' � ���������
bool UAContainerStorage::CheckObject(const UAContainer *object) const
{
 if(!object)
  return false;

 const UInstancesStorage* instances=ObjectsStorage.Find(object->GetClass());
 if(instances && instances->Find(object))
  return true;

 return false;
}

// ��������� ��������� ����� �������� � ���������
int UAContainerStorage::CalcNumObjects(void) const
{
 int result=0;
 for(int i=0;i<ObjectsStorage.GetSize();i++)
 {
  result+=ObjectsStorage[i].Objects.GetSize();
 }

 return result;
}

int UAContainerStorage::CalcNumObjects(const UId &classid) const
{
 const UInstancesStorage* instances=ObjectsStorage.Find(classid);

 if(instances)
  return instances->GetSize();

 return 0;
}

size_t UAContainerStorage::CalcNumObjects(const string &classname) const
{
 return CalcNumObjects(GetClassId(classname));
}


// �������� ��� ��������� ������� �� ���������
void UAContainerStorage::FreeObjectsStorage(void)
{
 for(int i=0;i<ObjectsStorage.GetSize();i++)
 {
  UInstancesStorage& instances=ObjectsStorage[i].Objects;
  UInstancesStorageElement* current=instances.GetFirst();
  UInstancesStorageElement* next=0;
  while(current != 0)
  {
   next=current->Next;
   if(!current->UseFlag)
   {
    current->Object->SetStorage(0);
    delete current->Object;
    current->Object=0;
    instances.Del(current);
   }
   current=next;
  }
 }
}

// ������� ��� ������� �� ���������
void UAContainerStorage::ClearObjectsStorage(void)
{
 for(int i=0;i<ObjectsStorage.GetSize();i++)
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
 }
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
