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
// class UInstancesStorageElement
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInstancesStorageElement::UInstancesStorageElement(void)
 : UseFlag(false)
{
}

UInstancesStorageElement::UInstancesStorageElement(const UInstancesStorageElement &copy)
 : Object(copy.Object), UseFlag(copy.UseFlag)
{
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
// Добавляет образец класса объекта в хранилище
UId UAContainerStorage::AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid)
{
 if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  return ForbiddenId;
 UAContainerStorage *storage=dynamic_cast<UAContainerStorage*>(classtemplate->GetStorage());
 UObjectsStorageIterator temp=ObjectsStorage.find(classid);
 if(storage)
  storage->PopObject(dynamic_pointer_cast<UAContainer>(classtemplate));

 UId id=UAStorage::AddClass(classtemplate,classid);
 if(id != ForbiddenId)
 {
  ClassesLookupTable[classname]=id;
 }

 return id;
}

UId UAContainerStorage::AddClass(UEPtr<UAComponent> classtemplate, const UId &classid)
{
 return UAStorage::AddClass(classtemplate, classid);
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
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
   UObjectsStorageCIterator temp=ObjectsStorage.find(I->first);
   if(temp != ObjectsStorage.end() && temp->second.size() == 0)
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
   UObjectsStorageIterator temp=ObjectsStorage.find(I->first);
   if(temp != ObjectsStorage.end() && temp->second.size() != 0)
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
UEPtr<UAComponent> UAContainerStorage::TakeObject(const UId &classid, const UAComponent *prototype)
{
 UClassesStorageIterator tmplI=ClassesStorage.find(classid);
 if(tmplI == ClassesStorage.end())
  return 0;

 UClassStorageElement tmpl=tmplI->second;
// UAContainer* classtemplate=dynamic_cast<UAContainer*>(tmpl.operator ->());
 UEPtr<UAContainer> classtemplate=dynamic_pointer_cast<UAContainer>(tmpl);

 if(!classtemplate)
  return 0;

 UObjectsStorageIterator instances=ObjectsStorage.find(classid);
 if(instances != ObjectsStorage.end())
 {
  UInstancesStorageElement* element=0;// Заглушка!! instances->FindFree();
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
   UEPtr<UAContainer> obj=element->Object;

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


 // Если свободного объекта не нашли
// UAComponent *obj=CI->second->New(CI->second->Name());
 UEPtr<UAContainer> obj=classtemplate->New();
 if(!obj)
  return 0;

 if(!obj->Default())
 {
//   delete obj;
//   obj=0;
   return 0;
 }

 // В случае, если объект создается непосредственно как копия из хранилища...
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
  // В случае, если объект создается из хранилища как часть более сложного
  // объекта
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

UEPtr<UAComponent> UAContainerStorage::TakeObject(const NameT &classname, const UAComponent *prototype)
{
 return TakeObject(GetClassId(classname),prototype);
}

// Проверяет существует ли объект 'object' в хранилище
bool UAContainerStorage::CheckObject(UEPtr<UAContainer> object) const
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

// Вычисляет суммарное число объектов в хранилище
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


// Удалаяет все свободные объекты из хранилища
void UAContainerStorage::FreeObjectsStorage(void)
{
 UObjectsStorageIterator instances;
 for(instances=ObjectsStorage.begin();instances != ObjectsStorage.end();++instances)
 {
  list<UInstancesStorageElement>::iterator elementI,elementJ;
  for(elementI=instances->second.begin(); elementI!= instances->second.end();)
  {
   if(!elementI->UseFlag)
   {
	elementJ=elementI; ++elementJ;
	UEPtr<UAContainer> object=elementI->Object;
	PopObject(instances,elementI);
	delete object;
//	instances->second.erase(elementI);
	elementI=elementJ;
   }
   else
    ++elementI;
  }
 }
}

// Удаляет все объекты из хранилища
void UAContainerStorage::ClearObjectsStorage(void)
{
 UObjectsStorageIterator instances;
 for(instances=ObjectsStorage.begin();instances != ObjectsStorage.end();++instances)
 {
  list<UInstancesStorageElement>::iterator elementI,elementJ;
  for(elementI=instances->second.begin(); elementI!= instances->second.end();++elementI)
  {
/*	elementJ=elementI; ++elementJ;
	UEPtr<UAContainer> object=elementI->Object;
	PopObject(instances,elementI);
	delete object;
//	instances->second.erase(elementI);
	elementI=elementJ;*/
	elementI->Object->Free();
  }
 }

 //ObjectsStorage.clear();
 FreeObjectsStorage();

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
// Скрытые методы управления хранилищем объектов
// Выводит уже созданный объект из хранилища и возвращает
// его classid
// --------------------------
// Добавляет уже созданный объект в хранилище
// Если объект уже принадлежит иному хранилищу то возвращает false
bool UAContainerStorage::PushObject(const UId &classid, UEPtr<UAContainer> object)
{
 if(!object || classid == ForbiddenId || object->GetStorage())
  return false;

 UEPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;
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

// Выводит уже созданный объект из хранилища и возвращает
// его classid
// В случае ошибки возвращает ForbiddenId
UId UAContainerStorage::PopObject(UEPtr<UAContainer> object)
{
 if(!object)
  return ForbiddenId;

 UObjectsStorageIterator instances=ObjectsStorage.find(object->GetClass());
 if(instances != ObjectsStorage.end())
 {
  list<UInstancesStorageElement>::iterator elementI;
  for(elementI=instances->second.begin(); elementI!= instances->second.end();++elementI)
  {
   if(elementI->Object == object)
   {
	return PopObject(instances, elementI);
   }
  }
 }

 return ForbiddenId;
}

// Перемещает объект в другое хранилище
bool UAContainerStorage::MoveObject(UEPtr<UAContainer> object, UAContainerStorage *newstorage)
{
 if(!newstorage)
  return false;

 return newstorage->PushObject(PopObject(object),object);
}

// Возвращает объект в хранилище
// Выбранный объект помечается как свободный в хранилище
// Флаг 'Activity' объекта выставляется в false
// Если объект не существует в хранилище - возвращается false
bool UAContainerStorage::ReturnObject(UEPtr<UAComponent> object)
{
 if(!object)
  return false;

 UEPtr<UAContainer> obj=dynamic_pointer_cast<UAContainer>(object);
 obj->ObjectIterator->UseFlag=false;
 obj->Activity=false;
 obj->BreakOwner();
 //obj->SetOwner(0); // возможно это не так? еще не проверено
// FreeObjectsTableIterator tI=FreeObjectsTable.find(object->GetClass());
// if(tI != FreeObjectsTable.end())
// {
//  tI->second.push_back(object->ObjectIterator);
// }
	  /*
 // Возвращаем все содержимое компонента
 for(int i=0;i<obj->GetNumComponents();i++)
 {
  UEPtr<UAComponent> temp=static_pointer_cast<UAComponent>(obj->GetComponentByIndex(i));
  if(!ReturnObject(temp))
   return false;
 }    */
 return true;
}

// В случае ошибки возвращает ForbiddenId
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
