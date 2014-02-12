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
//std::list<ULibrary*> ULibrary::LibraryList;


// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULibrary::ULibrary(const string &name, const string &version, int type)
: Name(name), Version(version), Type(type), Storage(0)
{
// if(!LibraryList)
//  LibraryList=new std::list<ULibrary*>;


// AddUniqueLibrary(this);
}

ULibrary::~ULibrary(void)
{
// RemoveLibrary(this);

/* if(LibraryList && LibraryList->empty())
 {
  delete LibraryList.Get();
  LibraryList=0;
 }*/
}
// --------------------------

// --------------------------
// Методы управления статически загруженными библиотеками
// --------------------------
/*
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

/// Удаляет библиотеку из списка.
/// Если список становится пуст, то уничтожает его
void ULibrary::RemoveLibrary(ULibrary* const lib)
{
 std::list<ULibrary*>::iterator I=LibraryList.begin();
 std::list<ULibrary*>::iterator J=LibraryList.end();
 for(;I!=J;++I)
 {
  if((*I) == lib)
  {
   LibraryList.erase(I);
   break;
  }
 }
} */
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

/// Тип библиотеки
/// 0 - Внутренняя библиотека (собрана вместе с ядром)
/// 1 - Внешняя библиотека (загружена из внешней dll)
/// 2 - Библиотека, созданная во время выполнения
int ULibrary::GetType(void) const
{
 return Type;
}

/// Зависимости библиотеки от других библиотек
const std::vector<pair<string, string> > ULibrary::GetDependencies(void) const
{
 return Dependencies;
}
// --------------------------

// --------------------------
// Методы доступа к данным загрузки
// --------------------------
/// Возвращает true если коллекция предоставляет класс с таким именем
bool ULibrary::IsClassNamePresent(const std::string &class_name) const
{
 for(size_t i=0;i<ClassesList.size();i++)
  if(ClassesList[i] == class_name)
   return true;
 return false;
}

/// Имена классов библиотеки
const vector<string>& ULibrary::GetClassesList(void) const
{
 return ClassesList;
}

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

// if(!Complete.empty())
//  return int(Complete.size());

 // ClassSamples.clear();
// Complete.clear();
 Incomplete.clear();
 CreateClassSamples(Storage);
 count=int(Complete.size());

 Storage=0;
 return count;
}
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
/// Проверяет зависимости библиотеки от других библиотек
/// и возвращает список недостающих библиотек
/// Возвращает true если все необходимые библиотеки уже загружены
bool ULibrary::CheckDependencies(UStorage *storage, std::vector<pair<string, string> > &dependencies) const
{
 if(!storage)
  return false;

 if(Dependencies.empty())
  return true;

 dependencies.clear();
 int num_libraries=storage->GetNumCollections();
 for(size_t i=0;i<Dependencies.size();i++)
 {
  bool dep_found=false;
  for(int j=0;j<num_libraries;j++)
  {
   UEPtr<ULibrary> lib=storage->GetCollection(j);
   if(lib && lib->GetName() == Dependencies[i].first
	&& (Dependencies[i].second.empty() || Dependencies[i].second == lib->GetVersion()))
   {
	dep_found=true;
	break;
   }
  }
  if(!dep_found)
   dependencies.push_back(Dependencies[i]);
 }
 if(dependencies.empty())
  return true;

 return false;
}

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

 if(Storage->CheckClass(name))
  return true;

 std::vector<std::string>::iterator I;
 if(!Storage->AddClass(cont,name))
 {
  if(find(Incomplete.begin(),Incomplete.end(),name) == Incomplete.end())
   Incomplete.push_back(name);
  I=find(ClassesList.begin(),ClassesList.end(),name);
  if(I != ClassesList.end())
   ClassesList.erase(I);
  I=find(Complete.begin(),Complete.end(),name);
  if(I != Complete.end())
   Complete.erase(I);

  delete cont;
  return false;
 }

 if(find(ClassesList.begin(),ClassesList.end(),name) == ClassesList.end())
  ClassesList.push_back(name);
 if(find(Complete.begin(),Complete.end(),name) == Complete.end())
  Complete.push_back(name);
 I=find(Incomplete.begin(),Incomplete.end(),name);
 if(I != Incomplete.end())
  Incomplete.erase(I);

 return true;
}

/// Удаление заданного класса из списка успешно загруженных
/// Класс переносится в незагруженные (Incomplete)
void ULibrary::RemoveClassFromCompletedList(const string &name)
{
 std::vector<std::string>::iterator I;

 I=find(ClassesList.begin(),ClassesList.end(),name);
 if(I != ClassesList.end())
  ClassesList.erase(I);

 I=find(Complete.begin(),Complete.end(),name);
 if(I != Complete.end())
  Complete.erase(I);

 I=find(Incomplete.begin(),Incomplete.end(),name);
 if(I == Incomplete.end())
  Incomplete.push_back(name);
}
// --------------------------




// --------------------------
// Конструкторы и деструкторы
// --------------------------
URuntimeLibrary::URuntimeLibrary(const string &name, const string &version)
 : ULibrary(name,version,2)
{

}

URuntimeLibrary::~URuntimeLibrary(void)
{

}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
/// Описание компонент библиотеки
const USerStorageXML& URuntimeLibrary::GetClassesStructure(void) const
{
 return ClassesStructure;
}

bool URuntimeLibrary::SetClassesStructure(const USerStorageXML& xml)
{
 ClassesStructure=xml;
 return true;
}

bool URuntimeLibrary::SetClassesStructure(const std::string &buffer)
{
 ClassesStructure.Load(buffer,"Library");
 return true;
}

/// Обновляет структуру классов в соответствии с хранилищем
bool URuntimeLibrary::UpdateClassesStructure(void)
{
 return true;
}
// --------------------------

// --------------------------
/// Создает компонент из описания xml
UEPtr<UContainer> URuntimeLibrary::CreateClassSample(UStorage *storage, USerStorageXML &xml)
{
 UEPtr<UNet> cont;

 if(!storage)
  return 0;

 std::string class_name=xml.GetNodeAttribute("Class");
 cont=dynamic_pointer_cast<UNet>(storage->TakeObject(class_name));
 if(!cont)
  return 0;

 if(!cont->LoadComponent(cont,&xml,true))
 {
  storage->ReturnObject(cont);
  return 0;
 }

 return cont;
}

// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
void URuntimeLibrary::CreateClassSamples(UStorage *storage)
{
 ClassesStructure.SelectRoot();
 int num_classes=ClassesStructure.GetNumNodes();
 for(int i=0;i<num_classes;i++)
 {
  try
  {
   ClassesStructure.SelectNode(i);
   UEPtr<UContainer> cont=CreateClassSample(storage, ClassesStructure);
   UploadClass(std::string("T")+cont->GetName(),cont);
  }
  catch(UException &exception)
  {
   ClassesStructure.SelectUp();
  }
  ClassesStructure.SelectUp();
 }
}
// --------------------------


}

#endif


