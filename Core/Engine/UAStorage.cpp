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
// Возвращает Id класса по его имени
const UId& UAStorage::GetClassId(const NameT &name) const
{
 map<NameT,UId>::const_iterator I=ClassesLookupTable.find(name);
 if(I == ClassesLookupTable.end())
  throw new EClassNameNotExist(name);
 return I->second;
}

// Возвращает имя класса по его Id
const NameT UAStorage::GetClassName(const UId &id) const
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
  throw new EClassIdAlreadyExist(id);

 if(!classtemplate->Build())
  return ForbiddenId;

 ClassesStorage[id]=classtemplate;
 classtemplate->SetClass(id);
 LastClassId=id;

 ClassesDescription[id]=classtemplate->NewDescription();

 return id;
}

// Добавляет образец класса объекта в хранилище
UId UAStorage::AddClass(UEPtr<UAComponent> classtemplate, const string &classname, const UId &classid)
{
 if(ClassesLookupTable.find(classname) != ClassesLookupTable.end())
  throw new EClassNameAlreadyExist(classname);

 UId id=AddClass(classtemplate,classid);
 ClassesLookupTable[classname]=id;
 ClassesDescription[id]->SetClassName(classname);
 return id;
}

// Удаляет образец класса объекта из хранилища
void UAStorage::DelClass(const UId &classid)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I != ClassesStorage.end())
  ClassesStorage.erase(I);
 else
  throw new EClassIdNotExist(classid);

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
  throw EClassIdNotExist(classid);

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

// Возвращает список имен всех классов хранилища
// Буфер 'buffer' будет очищен от предыдущих значений
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
  throw EClassIdNotExist(classid);

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
  throw EClassIdNotExist(classid);

 return I->second;
}

// Устанавливает XML описание класса
// Класс в хранилище должен существовать
void UAStorage::SetClassDescription(const UId &classid, const UEPtr<UComponentDescription>& description)
{
 UClassesStorageIterator I=ClassesStorage.find(classid);

 if(I == ClassesStorage.end())
  throw EClassIdNotExist(classid);

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

// Сохраняет общее описание всех классов в xml
bool UAStorage::SaveCommonClassesDescription(Serialize::USerStorageXML &xml)
{
 xml.AddNode(sntoa(0));
 if(!UContainerDescription::SaveCommon(xml))
 {
  xml.SelectUp();
  return false;
 }
 xml.SelectUp();
 return true;
}

// Загружает общее описание всех классов из xml
bool UAStorage::LoadCommonClassesDescription(Serialize::USerStorageXML &xml)
{
 if(xml.SelectNode(sntoa(0)))
 {
  UContainerDescription::LoadCommon(xml);
  xml.SelectUp();
 }

 UClassesDescriptionIterator I=ClassesDescription.begin();

 while(I != ClassesDescription.end())
 {
  dynamic_pointer_cast<UContainerDescription>(I->second)->RemoveCommonDuplicatesParameters();
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
void UAStorage::ReturnObject(UEPtr<UAComponent> object)
{
 delete object.Get();
}
// --------------------------

// --------------------------
// Скрытые методы таблицы соответствий классов
// --------------------------
// Добавляет класс с именем 'name' в таблицу соответствий
UId UAStorage::AddLookupClass(const NameT &name)
{
 if(ClassesLookupTable.find(name) != ClassesLookupTable.end())
  throw EClassNameAlreadyExist(name);

 ClassesLookupTable.insert(make_pair(name,LastClassId+1));

 return LastClassId+1;
}

// Удаляет класс с именем 'name' из таблицы соотвествий
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
// Конструкторы и деструкторы
// --------------------------
UAStorage::EClassIdNotExist::EClassIdNotExist(UId id)
 : Id(id)
{

}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UAStorage::EClassIdNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Id=")+sntoa(Id);
}
// --------------------------

// Попытка работы с классом по имени, отсутствующему в хранилище
//class UAStorage::EClassNameNotExist: public EError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAStorage::EClassNameNotExist::EClassNameNotExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UAStorage::EClassNameNotExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------

// Некорректное имя класса
// class UAStorage::EInvalidClassName: public EError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAStorage::EInvalidClassName::EInvalidClassName(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UAStorage::EInvalidClassName::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------



// Класс с заданным именем уже существует
//class UAStorage::EClassNameAlredyExist: public EError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAStorage::EClassNameAlreadyExist::EClassNameAlreadyExist(const std::string &name)
: Name(name)
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UAStorage::EClassNameAlreadyExist::CreateLogMessage(void) const
{
 return Exception::CreateLogMessage()+std::string(" Name=")+Name;
}
// --------------------------
       */
}


#endif

