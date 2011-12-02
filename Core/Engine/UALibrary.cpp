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

#include "UALibrary.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UALibrary::UALibrary(const string &name, const string &version)
{
 Name=name;
 Version=version;
}

UALibrary::~UALibrary(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к параметрам библиотеки
// --------------------------
// Возвращает имя библиотеки
const string& UALibrary::GetName(void) const
{
 return Name;
}

// Возвращает версию библиотеки
const string& UALibrary::GetVersion(void) const
{
 return Version;
}
// --------------------------

// --------------------------
// Методы доступа к данным загрузки
// --------------------------
// Содержит имена всех успешно загруженных образцов
const vector<string>& UALibrary::GetComplete(void) const
{
 return Complete;
}

// Содержит имена всех не загруженных образцов
const vector<string>& UALibrary::GetIncomplete(void) const
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
int UALibrary::Upload(UAStorage *storage)
{
 /*Complete.clear();
 Incomplete.clear();
 UALibrary::Upload(storage);
 return Complete.size();
 */
 int count=0;

 Storage=dynamic_cast<UAContainerStorage*>(storage);

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
bool UALibrary::UploadClass(const UId &classid, UEPtr<UAComponent> cont)
{
 return false;//UALibrary::UploadClass(classid, cont);
}

bool UALibrary::UploadClass(const string &name, UEPtr<UAComponent> cont)
{
 if(!cont)
  return false;

 if(name.size() == 0)
 {
  delete cont;
  return false;
 }

 UAContainerStorage *storage=dynamic_cast<UAContainerStorage *>(Storage);
 if(!storage)
 {
  if(!Storage->AddClass(cont,name))
  {
   Incomplete.push_back(name);
   delete cont;
   return false;
  }
 }
 else
 {
  if(!storage->AddClass(cont,name))
  {
   Incomplete.push_back(name);
   delete cont;
   return false;
  }
 }

 Complete.push_back(name);
 return true;
}



}

#endif


