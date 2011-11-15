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
// Элемент списка существующих объектов определенного класса
//class UInstancesStorageElement
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorageElement::UInstancesStorageElement(void)
{
 // Указатель на предыдущий элемент списка
 Prev=0;

 // Указатель на следующий элемент списка
 Next=0;

 // Указатель на объект
 Object=0;

 // Признак того свободен ли объект
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
// Операторы
// --------------------------
// Оператор присваивания
UInstancesStorageElement& UInstancesStorageElement::operator = (const UInstancesStorageElement &copy)
{
 // Указатель на предыдущий элемент списка
 Prev=copy.Prev;

 // Указатель на следующий элемент списка
 Next=copy.Next;

 // Указатель на объект
 Object=copy.Object;

 // Признак того свободен ли объект
 UseFlag=copy.UseFlag;

 return *this;
}

// Операторы сравнения
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


// Список существующих объектов определенного класса с флагом использования
// <экземпляр объекта, флаг true если занят или false если свободен>
//class UInstancesStorage
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorage::UInstancesStorage(void)
{
 // Указатель на первый элемент списка
 First=0;

 // Указатель на последний элемент списка
 Last=0;

 // Размер списка
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
// Методы управления списком
// --------------------------
// Очищает список
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

// Возвращает размер списка
int UInstancesStorage::GetSize(void) const
{
 return Size;
}

// Добавляет элемент в список и возвращает указатель на добавленый элемент
// Элементы автоматически сортируются по object
// Метод проверят объект на уникальность и обновляет существующую запись если
// такой объект уже существует
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

 // !!! Тут надо доработать - невозможно вставить элемент в начало сущетвующего списка
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

// Удаляет элемент из списка
void UInstancesStorage::Del(UAContainer* object)
{
 UInstancesStorageElement* element=Find(object);
 Del(element);
}

// Удаляет элемент из списка только в том случае, если value является элементом списка
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

// Ищет заданный объект и возвращает указатель на элемент списка с этим объектом
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

// Ищет "свободный" и возвращает указатель на элемент списка с этим объектом
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

// Указатель на первый элемент списка
UInstancesStorageElement* UInstancesStorage::GetFirst(void)
{
 return First;
}

const UInstancesStorageElement* UInstancesStorage::GetFirst(void) const
{
 return First;
}

// Указатель на последний элемент списка
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
// Операторы
// --------------------------
// Оператор присваивания
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
// Единица хранилища образцов классов
//struct UObjectStorageElement
// --------------------------
// Конструкторы и деструкторы
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
// Операторы
// --------------------------
// Оператор присваивания
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
// Конструкторы и деструкторы
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
// Методы управления
// --------------------------
// Очищает хранилище
void UObjectsStorage::Clear(void)
{
 if(Objects)
 {
  delete[] Objects;
  Objects=0;
 }
 Size=RealSize=0;
}

// Возвращает размер массива
int UObjectsStorage::GetSize(void) const
{
 return Size;
}

// Изменяет размер хранилища
// Метод сохраняет старые данные
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

// Ищет класс по Id
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


// Ищет объект по Id и удаляет его из массива
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

// Добавляет новый элемент в конец хранилища
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

// Создает новый элемент в хранилище с заданным id и возвращает указатель на него
// Если такой элемент уже существует то возвращает указатель на этот элемент
UInstancesStorage* UObjectsStorage::Create(const UId &id)
{
 UInstancesStorage* instance=Find(id);
 if(instance)
  return instance;

 UObjectStorageElement classelement;
 classelement.Id=id;
 return &PushBack(classelement)->Objects;
}

// Возвращает указатель на массив классов
UObjectStorageElement* UObjectsStorage::GetObjects(void) const
{
 return Objects;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UObjectsStorage& UObjectsStorage::operator = (const UObjectsStorage &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Objects[i]=copy.Objects[i];

 return *this;
}

// Оператор доступа
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
/*// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UFreeObjectsTableIteratorsVector::UFreeObjectsTableIteratorsVector(void)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;
}

UFreeObjectsTableIteratorsVector::UFreeObjectsTableIteratorsVector(const UFreeObjectsTableIteratorsVector &copy)
{
 // Размер массива изображений
 Size=0;

 // Массив изображений
 Buffer=0;

 // Реальный размер массива
 RealSize=0;

 *this=copy;
}

UFreeObjectsTableIteratorsVector::~UFreeObjectsTableIteratorsVector(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Возвращает размер массива
int UFreeObjectsTableIteratorsVector::GetSize(void) const
{
 return Size;
}

// Изменяет размер массива с сохранением прежних данных
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

// Очищает массив
void UFreeObjectsTableIteratorsVector::Clear(void)
{
 if(Buffer)
 {
  delete []Buffer;
  Buffer=0;
 }
 Size=RealSize=0;
}

// Возвращает указатель на начало данных
PUInstancesStorageElement* UFreeObjectsTableIteratorsVector::GetBuffer(void)
{
 return Buffer;
}
// --------------------------

// --------------------------
// Методы управления свободными объектами
// --------------------------
// Возвращает указатель на первый "свободный" элемент массива, или 0
// если таковых не найдено
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
// Операторы
// --------------------------
// Оператор присваивания
UFreeObjectsTableIteratorsVector& UFreeObjectsTableIteratorsVector::operator = (const UFreeObjectsTableIteratorsVector &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  *Buffer[i]=*copy.Buffer[i];

 return *this;
}

// Оператор доступа
PUInstancesStorageElement UFreeObjectsTableIteratorsVector::operator [] (int index)
{
 return Buffer[index];
}

UInstancesStorageElement& UFreeObjectsTableIteratorsVector::operator () (int index)
{
 return *Buffer[index];
}
// --------------------------

// Хранилище итераторов на свободные объекты заданных классов
//class UFreeObjectsTable

// Methods
// --------------------------
// Конструкторы и деструкторы
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
// Методы управления свободными объектами
// --------------------------
// Возвращает указатель на свободный объект заданного класса
// или 0 если объект не найден
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

// Добавляет заданный элемент в хранилище указателей на свободные объекты
// Возвращает указатель на созданный объект
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
// Методы управления
// --------------------------
// Очищает хранилище
void UFreeObjectsTable::Clear(void)
{
 if(FreeObjects)
 {
  delete[] FreeObjects;
  FreeObjects=0;
 }
 Size=RealSize=0;
}

// Изменяет размер хранилища
// Метод сохраняет старые данные
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

// Возвращает размер массива
int UFreeObjectsTable::GetSize(void) const
{
 return Size;
}

// Ищет класс по Id
UFreeObjectsTableIteratorsVector* UFreeObjectsTable::Find(const UId &id) const
{
 UFreeObjectsTableElement* pclasses=FreeObjects;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
   return &pclasses->Objects;

 return 0;
}

// Ищет класс по Id и удаляет его из массива
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

// Добавляет новый элемент в конец хранилища
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

// Возвращает указатель на массив классов
UFreeObjectsTableElement* UFreeObjectsTable::GetFreeObjects(void) const
{
 return FreeObjects;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UFreeObjectsTable& UFreeObjectsTable::operator = (const UFreeObjectsTable &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  FreeObjects[i]=copy.FreeObjects[i];

 return *this;
}

// Оператор доступа
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
// Методы
// --------------------------
// Конструкторы и деструкторы
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
// Методы управления свойствами
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// Возвращает Id класса по его имени
const UId& UAContainerStorage::GetClassId(const NameT &name) const
{
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  return ForbiddenId;
 else return I->second;
}

// Возвращает имя класса по его Id
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
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище с автоматическим назначением id
// Возвращает id класса
/*UId UAContainerStorage::AddClass(UAComponent *classtemplate)
{
 return UAStorage::AddClass(classtemplate);
} */

// Добавляет образец класса объекта в хранилище
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

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
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

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
void UAContainerStorage::GetClassIdList(UId* buffer, int max_num_classes) const
{
 return UAStorage::GetClassIdList(buffer,max_num_classes);
}

void UAContainerStorage::GetClassIdList(UIdVector &buffer) const
{
 buffer.Resize(GetNumClasses());
 GetClassIdList(&buffer[0],buffer.GetSize());
}

// Возвращает список имен всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
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

// Удаляет все не используемые образцы классов из хранилища
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

// Удаляет все образцы классов из хранилища
// Возвращает false если в хранилище присутствуют объекты
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
// Методы управления хранилищем объектов
// --------------------------
// Извлекает объект из хранилища
// Возвращает указатель на свободный объект по имени класса
// Выбранный объект помечается как занятый в хранилище
// Флаг 'Activity' объекта выставляется в true
// Если свободного объекта не существует он создается и добавляется
// в хранилище
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
  UInstancesStorageElement* element=0;// Заглушка!! instances->FindFree();
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


 // Если свободного объекта не нашли
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


 // В случае, если объект создается непосредственно как копия из хранилища...
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
  // В случае, если объект создается из хранилища как часть более сложного
  // объекта
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

// Возвращает объект в хранилище
// Выбранный объект помечается как свободный в хранилище
// Флаг 'Activity' объекта выставляется в false
// Если объект не существует в хранилище - возвращается false
bool UAContainerStorage::ReturnObject(UAComponent *object)
{
 if(!object)
  return false;

 UAContainer *obj=dynamic_cast<UAContainer*>(object);
 obj->ObjectIterator->UseFlag=false;
 obj->Activity=false;
 obj->SetOwner(0); // возможно это не так? еще не проверено
// FreeObjectsTableIterator tI=FreeObjectsTable.find(object->GetClass());
// if(tI != FreeObjectsTable.end())
// {
//  tI->second.push_back(object->ObjectIterator);
// }

 // Возвращаем все содержимое компонента
 for(int i=0;i<obj->GetNumComponents();i++)
  if(!ReturnObject(obj->GetComponentByIndex(i)))
   return false;

 return true;
}

// Добавляет уже созданный объект в хранилище
// Если объект уже принадлежит иному хранилищу то возвращает false
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

// Выводит уже созданный объект из хранилища и возвращает
// его classid
// В случае ошибки возвращает ForbiddenId
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

// Перемещает объект в другое хранилище
bool UAContainerStorage::MoveObject(UAContainer *object, UAContainerStorage *newstorage)
{
 if(!newstorage)
  return false;

 return newstorage->PushObject(PopObject(object),object);
}

// Проверяет существует ли объект 'object' в хранилище
bool UAContainerStorage::CheckObject(const UAContainer *object) const
{
 if(!object)
  return false;

 const UInstancesStorage* instances=ObjectsStorage.Find(object->GetClass());
 if(instances && instances->Find(object))
  return true;

 return false;
}

// Вычисляет суммарное число объектов в хранилище
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


// Удалаяет все свободные объекты из хранилища
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

// Удаляет все объекты из хранилища
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
// Скрытые методы таблицы соответствий классов
// --------------------------
// Добавляет класс с именем 'name' в таблицу соответствий
UId UAContainerStorage::AddLookupClass(const NameT &name)
{
 if(ClassesLookupTable.find(name) != ClassesLookupTable.end())
  return ForbiddenId;

 ClassesLookupTable.insert(make_pair(name,LastClassId+1));

 return LastClassId+1;
}

// Удаляет класс с именем 'name' из таблицы соотвествий
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
