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

 if(ClassesStorage.find(id) != ClassesStorage.end())
  return ForbiddenId;

 if(!classtemplate->Build())
  return ForbiddenId;

 ClassesStorage[id]=classtemplate;
 classtemplate->SetClass(id);
 LastClassId=id;

 return id;
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
bool UAStorage::DelClass(const UId &classid)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  ClassesStorage.erase(I);

// delete eraseclass;

 return true;
}

// Проверяет наличие образца класса объекта в хранилище
bool UAStorage::CheckClass(const UId &classid) const
{
 if(ClassesStorage.find(classid) == ClassesStorage.end())
  return false;

 return true;
}

// Возвращает образец класса
UESharedPtr<UAComponent> UAStorage::GetClass(const UId &classid) const
{
 UClassesStorageCIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  return I->second;

 return UESharedPtr<UAComponent>();
}

// Возвращает число классов
int UAStorage::GetNumClasses(void) const
{
 return ClassesStorage.size();
}

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
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

// Удаляет все образцы классов из хранилища
bool UAStorage::ClearClassesStorage(void)
{
/* UClassStorageElement* classes=ClassesStorage.GetClasses();
 for(int i=0;i<ClassesStorage.GetSize();i++,classes++)
 {
  if(classes->Class)
   delete classes->Class;
 }*/
 ClassesStorage.clear();
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
UESharedPtr<UAComponent> UAStorage::TakeObject(const UId &classid, const UAComponent *prototype)
{
 UESharedPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;
 UESharedPtr<UAComponent> obj;

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

// Возвращает объект в хранилище
// В текущей реализации всегда удаляет объект и возвращает true
bool UAStorage::ReturnObject(UEPtr<UAComponent> object)
{
// if(object)
//  delete object;

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
/* *************************************************************************** */

}


#endif

