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

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UClassesStorage::UClassesStorage(void)
{
 Classes=0;
 Size=RealSize=0;
}

UClassesStorage::UClassesStorage(const UClassesStorage &copy)
{
 Classes=0;
 Size=RealSize=0;
 *this=copy;
}

UClassesStorage::~UClassesStorage(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает хранилище
void UClassesStorage::Clear(void)
{
 if(Classes)
 {
  delete[] Classes;
  Classes=0;
 }
 Size=RealSize=0;
}

// Возвращает размер массива
int UClassesStorage::GetSize(void) const
{
 return Size;
}

// Изменяет размер хранилища
// Метод сохраняет старые данные
void UClassesStorage::Resize(int newsize)
{
 int newrealsize=0;
 if(newsize < 8)
  newrealsize=16;
 else
  newrealsize=newsize<<2;
 if(RealSize<newsize || !Classes)
 {
  UClassStorageElement* newbuffer=new UClassStorageElement[newrealsize];
  memcpy(newbuffer,Classes,sizeof(UClassStorageElement)*Size);

  if(Classes)
   delete []Classes;
  Classes=newbuffer;
  RealSize=newrealsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Ищет класс по Id
UAComponent* UClassesStorage::Find(const UId &id) const
{
 UClassStorageElement* pclasses=Classes;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
   return pclasses->Class;

 return 0;
}

// Ищет класс по Id и удаляет его из массива
// Возвращает указатель на удаленный класс
UAComponent* UClassesStorage::Erase(const UId &id)
{
 UClassStorageElement* pclasses=Classes;
 for(int i=0;i<Size;i++,pclasses++)
  if(pclasses->Id == id)
  {
   UAComponent *result=pclasses->Class;

   if(i != Size-1)
    memmove(pclasses,pclasses+1,sizeof(UClassStorageElement)*(Size-i));
   Resize(Size-1);

   return result;
  }

 return 0;
}

// Добавляет новый элемент в конец хранилища
void UClassesStorage::PushBack(const UClassStorageElement &classelement)
{
 Resize(Size+1);
 Classes[Size-1]=classelement;
}

void UClassesStorage::PushBack(const UId &id, UAComponent *component)
{
 UClassStorageElement classelement;
 classelement.Id=id;
 classelement.Class=component;
 PushBack(classelement);
}

// Возвращает указатель на массив классов
UClassStorageElement* UClassesStorage::GetClasses(void) const
{
 return Classes;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UClassesStorage& UClassesStorage::operator = (const UClassesStorage &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Classes[i]=copy.Classes[i];

 return *this;
}

// Оператор доступа
UClassStorageElement& UClassesStorage::operator [] (int i)
{
 return Classes[i];
}
// --------------------------

/* *************************************************************************** */
// Class UAStorage
/* *************************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
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
// Методы управления свойствами
// --------------------------
// Возвращает последний использованный Id классов
UId UAStorage::GetLastClassId(void) const
{
 return LastClassId;
}
// --------------------------

// --------------------------
// Методы доступа к таблицам соотвествий
// --------------------------
// --------------------------

// --------------------------
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
UId UAStorage::AddClass(UAComponent *classtemplate, const UId &classid)
{
 if(!classtemplate)
  return ForbiddenId;

 UId id=classid;
 if(id == ForbiddenId)
  id=LastClassId+1;

 if(ClassesStorage.Find(id))
  return ForbiddenId;

 ClassesStorage.PushBack(id,classtemplate);
 classtemplate->SetClass(id);
 LastClassId=id;

 return id;
}

// Добавляет образец класса объекта в хранилище с автоматическим назначением id
// Возвращает id класса
/*UId UAStorage::AddClass(UAComponent *classtemplate)
{
 return AddClass(LastClassId+1,classtemplate);
}  */

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
bool UAStorage::DelClass(const UId &classid)
{
 UAComponent *eraseclass=ClassesStorage.Erase(classid);

 delete eraseclass;

 return true;
}

// Проверяет наличие образца класса объекта в хранилище
bool UAStorage::CheckClass(const UId &classid) const
{
 if(ClassesStorage.Find(classid) == 0)
  return false;

 return true;
}

// Возвращает образец класса
UAComponent* const UAStorage::GetClass(const UId &classid) const
{
 return ClassesStorage.Find(classid);
}

// Возвращает число классов
int UAStorage::GetNumClasses(void) const
{
 return ClassesStorage.GetSize();
}

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
void UAStorage::GetClassIdList(UId* buffer, int max_num_classes) const
{
 if(!buffer || !max_num_classes || !ClassesStorage.GetSize())
  return;

 int size=(max_num_classes<ClassesStorage.GetSize())?max_num_classes:ClassesStorage.GetSize();

 UClassStorageElement* classes=ClassesStorage.GetClasses();
 for(int i=0;i<size;i++)
 {
  *buffer++=(*classes++).Id;
 }
}

// Удаляет все образцы классов из хранилища
bool UAStorage::ClearClassesStorage(void)
{
 UClassStorageElement* classes=ClassesStorage.GetClasses();
 for(int i=0;i<ClassesStorage.GetSize();i++,classes++)
 {
  if(classes->Class)
   delete classes->Class;
 }
 ClassesStorage.Clear();
 return true;
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
UAComponent* UAStorage::TakeObject(const UId &classid, const UAComponent *prototype)
{
 UAComponent* classtemplate=ClassesStorage.Find(classid);

 if(!classtemplate)
  return 0;

 UAComponent *obj=classtemplate->New();
 if(!obj)
  return 0;

 obj->SetClass(classid);
 obj->Default();
 return obj;
}

// Возвращает объект в хранилище
// В текущей реализации всегда удаляет объект и возвращает true
bool UAStorage::ReturnObject(UAComponent *object)
{
 if(object)
  delete object;

 return true;
}


// Возвращает Id класса, отвечающий объекту 'object'
UId UAStorage::FindClass(const UAComponent *object) const
{
 if(!object)
  return ForbiddenId;

 return object->GetClass();
}
// --------------------------

// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
// --------------------------
/* *************************************************************************** */

}


#endif

