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
 UId id=classid;
 if(id == ForbiddenId)
  id=LastClassId+1;

 if(ClassesStorage.find(id) != ClassesStorage.end())
  throw new EClassIdDontExist(id);

 if(!classtemplate->Build())
  return ForbiddenId;

 ClassesStorage[id]=classtemplate;
 classtemplate->SetClass(id);
 LastClassId=id;

 ClassesDescription[id]=classtemplate->NewDescription();

 return id;
}

// Удаляет образец класса объекта из хранилища
void UAStorage::DelClass(const UId &classid)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  ClassesStorage.erase(I);
 else
  throw new EClassIdDontExist(classid);

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

 if(I == ClassesStorage.end())
  throw EClassIdDontExist(classid);

 return I->second;
}

// Возвращает число классов
int UAStorage::GetNumClasses(void) const
{
 return ClassesStorage.size();
}

// Возвращает список идентификаторов всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
void UAStorage::GetClassIdList(std::vector<UId> &buffer) const
{
 buffer.resize(0);
 buffer.reserve(ClassesStorage.size());
 for(UClassesStorageCIterator I = ClassesStorage.begin(), J=ClassesStorage.end(); I != J; ++I)
  buffer.push_back(I->first);
}

// Удаляет все образцы классов из хранилища
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
// Методы управления хранилищем объектов
// --------------------------
// Извлекает объект из хранилища
// Возвращает указатель на свободный объект по имени класса
// Выбранный объект помечается как занятый в хранилище
// Флаг 'Activity' объекта выставляется в true
// Если свободного объекта не существует он создается и добавляется
// в хранилище
UEPtr<UAComponent> UAStorage::TakeObject(const UId &classid, const UEPtr<UAComponent> prototype)
{
 UEPtr<UAComponent> classtemplate=ClassesStorage.find(classid)->second;
 UEPtr<UAComponent> obj;

 if(!classtemplate)
  throw EClassIdDontExist(classid);

 obj=classtemplate->New();

 obj->SetClass(classid);
 obj->Default();
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

 if(I == ClassesDescription.end())
  throw EClassIdDontExist(classid);

 return I->second;
}

// Устанавливает XML описание класса
// Класс в хранилище должен существовать
void UAStorage::SetClassDescription(const UId &classid, const UEPtr<UComponentDescription>& description)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I == ClassesStorage.end())
  throw EClassIdDontExist(classid);

 ClassesDescription[classid]=description;
}

// Сохраняет описание класса в xml
void UAStorage::SaveClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml)
{
 GetClassDescription(classid)->Save(xml);
}

// Загружает описание класса из xml
void UAStorage::LoadClassDescription(const UId &classid,
										Serialize::USerStorageXML &xml)
{
 GetClassDescription(classid)->Load(xml);
}

// Сохраняет описание всех классов в xml
void UAStorage::SaveClassesDescription(Serialize::USerStorageXML &xml)
{
 for(UClassesDescriptionCIterator I = ClassesDescription.begin(), J=ClassesDescription.end(); I != J; ++I)
 {
  xml.AddNode(sntoa(I->first));
  I->second->Save(xml);
  xml.SelectUp();
 }
}

// Загружает описание всех классов из xml
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
// --------------------------

// --------------------------
// Скрытые методы управления хранилищем объектов
// --------------------------
// Возвращает объект в хранилище
// В текущей реализации всегда удаляет объект и возвращает true
void UAStorage::ReturnObject(UEPtr<UAComponent> object)
{
 delete object.Get();
}
// --------------------------
/* *************************************************************************** */

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAStorage::EClassIdDontExist::EClassIdDontExist(UId id)
 : Id(id)
{

}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UAStorage::EClassIdDontExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Id=")+sntoa(Id);
}
// --------------------------
}


#endif

