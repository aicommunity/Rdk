/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UALIBRARY_CPP
#define UALIBRARY_CPP

#include "ULibrary.h"

namespace RDK {

/// Список статически загруженных библиотек
std::list<ULibrary*> ULibrary::LibraryList;


// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULibrary::ULibrary(const string &name, const string &version)
{
 Name=name;
 Version=version;

 AddUniqueLibrary(this);
}

ULibrary::~ULibrary(void)
{
}
// --------------------------

// --------------------------
// Методы управления статически загруженными библиотеками
// --------------------------
/// Возвращает коллекцию статически загруженных библиотек
const std::list<ULibrary*>& ULibrary::GetLibraryList(void)
{
 return LibraryList;
}

/// Очищает коллекцию библиотек
void ULibrary::ClearLibraryList(void)
{
 LibraryList.clear();
}

/// Проверяет наличие библиотеки по ее имени
bool ULibrary::CheckLibrary(const std::string &name)
{
 std::list<ULibrary*>::const_iterator I=LibraryList.begin();
 std::list<ULibrary*>::const_iterator J=LibraryList.end();
 for(;I!=J;++I)
 {
  if((*I)->GetName() == name)
   return true;
 }
 return false;
}

/// Добавляет библиотеку в список, если библиотеки с таким именем еще нет в списке
bool ULibrary::AddUniqueLibrary(ULibrary* const lib)
{
 if(!lib)
  return false;

 if(CheckLibrary(lib->GetName()))
  return false;  /// Заглушка!! здесь может быть warning исключение

 LibraryList.push_back(lib);
 return true;
}
// --------------------------

// --------------------------
// Методы доступа к параметрам библиотеки
// --------------------------
// Возвращает имя библиотеки
const string& ULibrary::GetName(void) const
{
 return Name;
}

// Возвращает версию библиотеки
const string& ULibrary::GetVersion(void) const
{
 return Version;
}
// --------------------------

// --------------------------
// Методы доступа к данным загрузки
// --------------------------
// Содержит имена всех успешно загруженных образцов
const vector<string>& ULibrary::GetComplete(void) const
{
 return Complete;
}

// Содержит имена всех не загруженных образцов
const vector<string>& ULibrary::GetIncomplete(void) const
{
 return Incomplete;
}
// --------------------------

// --------------------------
// Методы доступа к данным библиотеки
// --------------------------
// Заполняет заданное хранилище набором образцов классов.
// Если класс с заданным именем уже существует, то он пропускается.
// Возвращает число реально загруженных классов.
int ULibrary::Upload(UStorage *storage)
{
 /*Complete.clear();
 Incomplete.clear();
 ULibrary::Upload(storage);
 return Complete.size();
 */
 int count=0;

 Storage=storage;

 if(!Storage)
  return 0;

 // ClassSamples.clear();
 Complete.clear();
 Incomplete.clear();
 CreateClassSamples(Storage);
 count=Complete.size();

 Storage=0;
 return count;
}
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Добавляет в хранилище очередной класс
bool ULibrary::UploadClass(const UId &classid, UEPtr<UComponent> cont)
{
 return false;//ULibrary::UploadClass(classid, cont);
}

bool ULibrary::UploadClass(const string &name, UEPtr<UComponent> cont)
{
 if(!cont)
  return false;

 if(name.size() == 0)
 {
  delete cont;
  return false;
 }

 if(!Storage->AddClass(cont,name))
 {
  Incomplete.push_back(name);
  delete cont;
  return false;
 }

 Complete.push_back(name);
 return true;
}



}

#endif


