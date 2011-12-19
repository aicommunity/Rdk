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
// Методы управления хранилищем классов
// --------------------------
// Добавляет образец класса объекта в хранилище
// Возвращает id класса
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

 ClassesDescription[id]=classtemplate->NewDescription();

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
UEPtr<UAComponent> UAStorage::GetClass(const UId &classid) const
{
 UClassesStorageCIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  return I->second;

 return UEPtr<UAComponent>();
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
 UClassesStorageCIterator I;
 for(I=ClassesStorage.begin();I != ClassesStorage.end();++I)
 {
  if(I->second)
   delete I->second.Get();
 }
 ClassesStorage.clear();

 UClassesDescriptionCIterator J;
 for(J=ClassesDescription.begin();J != ClassesDescription.end();++J)
 {
  if(J->second)
   delete J->second.Get();
 }
 ClassesDescription.clear();
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

// Возвращает Id класса, отвечающий объекту 'object'
UId UAStorage::FindClass(UEPtr<UAComponent> object) const
{
 if(!object)
  return ForbiddenId;

 return object->GetClass();
}
// --------------------------

// --------------------------
// Методы управления описанием классов
// --------------------------
// Возвращает XML описание класса
const UEPtr<UComponentDescription> UAStorage::GetClassDescription(const UId &classid) const
{
 UClassesDescriptionCIterator I=ClassesDescription.find(classid);

// if(I == ClassesDescription.end())
//  return ClassesDescription[classid]; // Заглушка! Здесь проверка и исключение

 return I->second;
}

// Устанавливает XML описание класса
// Класс в хранилище должен существовать
bool UAStorage::SetClassDescription(const UId &classid, const UEPtr<UComponentDescription>& description)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I == ClassesStorage.end())
  return false;

 ClassesDescription[classid]=description;
 return true;
}

// Сохраняет описание класса в xml
bool UAStorage::SaveClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml)
{
// xml.AddNode(sntoa(classid));

 GetClassDescription(classid)->Save(xml);

// xml.SelectUp();
 return true;
}

// Загружает описание класса из xml
bool UAStorage::LoadClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml)
{
// if(!xml.SelectNode(sntoa(classid)))
//  return false;

 GetClassDescription(classid)->Load(xml);

// xml.SelectUp();
 return true;
}

// Сохраняет описание всех классов в xml
bool UAStorage::SaveClassesDescription(Serialize::USerStorageXML &xml)
{
 UClassesDescriptionIterator I=ClassesDescription.begin();

 while(I != ClassesDescription.end())
 {
  xml.AddNode(sntoa(I->first));
  I->second->Save(xml);
  xml.SelectUp();
  ++I;
 }

 return true;
}

// Загружает описание всех классов из xml
bool UAStorage::LoadClassesDescription(Serialize::USerStorageXML &xml)
{
 UClassesDescriptionIterator I=ClassesDescription.begin();

 while(I != ClassesDescription.end())
 {
  if(!xml.SelectNode(sntoa(I->first)))
  {
   ++I;
   continue;
  }
  I->second->Load(xml);
  xml.SelectUp();
  ++I;
 }

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления хранилищем объектов
// --------------------------
// Возвращает объект в хранилище
// В текущей реализации всегда удаляет объект и возвращает true
bool UAStorage::ReturnObject(UEPtr<UAComponent> object)
{
 delete object.Get();

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления описанием классов
// --------------------------
// Формирует прототип XML описания для заданного класса
// Класс в хранилище должен существовать
// Очищает уже созданные поля описания
/*bool UAStorage::GenerateClassDescription(UClassesStorageIterator classI,
										Serialize::USerStorageXML &xml)
{
 // Заполняем xml
 xml.Create("Class");
 xml.SetNodeAttribute("Id",sntoa(classI->first));
 xml.AddNode("Header");
 xml.SelectUp();
 xml.AddNode("Description");
 xml.SelectUp();
 return true;
}            */
// --------------------------

/* *************************************************************************** */

}


#endif

