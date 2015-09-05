/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UEngine_CPP
#define UEngine_CPP

#include "UEngine.h"
//#include "UEnvException.h"
#include "UXMLEnvSerialize.h"
//#include "Libraries/IO/UFileIO.h"
#include "../Application/UIVisualController.h"

// --------------------------------------
// Объявления дополнительных функций
// --------------------------------------
// Возвращает хранилище по индексу
extern RDK::UStorage* GetStorage(size_t i);

// Возвращает среду по индексу
extern RDK::UEnvironment*  GetEnvironment(size_t i);

// Возвращает движок по индексу
extern RDK::UEngine*  GetEngine(size_t i);

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
extern RDK::UStorage*  AddNewStorage(void);

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
extern RDK::UEnvironment*  AddNewEnvironment(void);

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
extern RDK::UEngine*  AddNewEngine(void);
// --------------------------------------

namespace RDK{

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngine::UEngine(void)
 : Storage(0), Environment(0)
{
 Runned=-1;
 ChannelIndex=0;
 BufObjectsMode=0;
// CurrentExceptionsLogSize=0;
// ExceptionHandler=0;

// LastReadExceptionLogIndex=-1;
// MaxExceptionsLogSize=1000;
}

UEngine::~UEngine(void)
{
 Stop();
}
// --------------------------

// --------------------------
// Методы управления параметрами инициализации
// --------------------------
/// Режим создания внутренних временных переменных для
/// возвращаемых значений
/// 0 - одна переменная для всех методов, возвращающих такой тип
/// 1 - уникальные переменные с необходимостью вызвова функции очистки
int UEngine::GetBufObjectsMode(void) const
{
 return BufObjectsMode;
}

bool UEngine::SetBufObjectsMode(int mode)
{
 if(BufObjectsMode == mode)
  return true;
 BufObjectsMode=mode;
 TempStrings.clear();
 return true;
}

// Имя файла инициализации
const string& UEngine::GetOptionsFileName(void) const
{
 return OptionsFileName;
}

bool UEngine::SetOptionsFileName(const string& value)
{
 if(OptionsFileName == value)
  return true;

 OptionsFileName=value;
 return true;
}
// --------------------------

// --------------------------
// Методы управления временными переменными
// --------------------------
/// Создает в списке временных строку новую строку
/// и возвращает ссылку на нее
std::string& UEngine::CreateTempString(void) const
{
 switch (BufObjectsMode)
 {
 case 0:
 {
  if(TempStrings.empty())
  {
   UEPtr<string> pstr=new std::string;
   TempStrings.push_back(pstr);
  }
  return *TempStrings.front();
 }
 break;

 case 1:
 {
  UEPtr<string> pstr=new std::string;
  TempStrings.push_back(pstr);
  return *TempStrings.back();
 }
 break;
 }
 return DummyTempString;
}

/// Возвращает временную строку
/// по указателю на ее данные
std::string& UEngine::FindTempString(const char *str_data) const
{
 std::list<UEPtr<std::string> >::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  if((*I)->c_str() == str_data)
   return **I;

 }
 return DummyTempString;
}

/// Удаляет временную строку
/// по указателю на ее данные
void UEngine::DestroyTempString(const char *str_data) const
{
 if(!str_data)
  return;

 if(BufObjectsMode == 0)
  return;

 std::list<UEPtr<std::string> >::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  if(*I && (*I)->c_str() == str_data)
  {
   delete *I;
   TempStrings.erase(I);
   break;
  }
 }
}

/// Удаляет временную строку
/// по ссылке на нее
void UEngine::DestroyTempString(const std::string &ref) const
{
 if(BufObjectsMode == 0)
  return;

 std::list<UEPtr<std::string> >::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  if(**I == ref)
  {
   delete *I;
   TempStrings.erase(I);
   break;
  }
 }
}

/// Удаляет все временные строк
void UEngine::ClearAllTempStrings(void) const
{
 if(BufObjectsMode == 0)
  return;

 std::list<UEPtr<std::string> >::iterator I,J;
 I=TempStrings.begin();
 J=TempStrings.end();
 for(;I!=J;++I)
 {
  delete *I;
 }
 TempStrings.clear();
}

/// Возвращает число временных строк
int UEngine::GetNumTempStrings(void) const
{
 if(BufObjectsMode == 0)
  return 0;

 return int(TempStrings.size());
}
// --------------------------

// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// Флаг работы системы
int UEngine::IsRunned(void)
{
 return Runned;
}

/// Индекс текущего канала в многоканальной библиотеке
int UEngine::GetChannelIndex(void) const
{
 return ChannelIndex;
}

bool UEngine::SetChannelIndex(int value)
{
 if(ChannelIndex == value)
  return true;

 ChannelIndex=value;
 if(Environment)
  Environment->SetChannelIndex(ChannelIndex);
 return true;
}
// --------------------------


// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает указатель на среду
UEnvironment* UEngine::GetEnvironment(void)
{
 return Environment;
}

// Возвращает указатель на модель
UContainer* UEngine::GetModel(void)
{
 return Environment->GetModel();
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Инициализирует данные движка
void UEngine::Init(void)
{
 // Заглушка
}

bool UEngine::Init(UEPtr<UStorage> storage, UEPtr<UEnvironment> env)
{
// if(!Options.LoadFromFile(OptionsFileName))
// {
  if(!Default())
   return false;
// }

 AccessCache.clear();

// StorageIndex=atoi(Options("General","StorageIndex",sntoa(StorageIndex).c_str()));
// EnvironmentIndex=atoi(Options("General","EnvironmentIndex",sntoa(EnvironmentIndex).c_str()));

// if(StorageIndex>=0)
//  Storage=DLLGetStorage(StorageIndex);
// else
//  Storage=AddNewStorage();

 Storage=storage;
 Environment=env;

 Environment->ClearLog();
 Environment->SetChannelIndex(ChannelIndex);
 CreateStorage();

 if(!Storage)
  return false;

// if(EnvironmentIndex>=0)
//  Environment=DLLGetEnvironment(EnvironmentIndex);
// else
// {
  LibrariesList.clear();
  ClassesList.clear();
  if(LoadPredefinedLibraries())
   return false;
  if(LoadClasses())
   return false;
  if(LoadLibraries())
   return false;
//  Environment=AddNewEnvironment();
  CreateEnvironment(true,&ClassesList, &LibrariesList);
// }

 if(!Storage || !Environment || Environment->GetStorage() != Storage)
 {
  return false;
 }
/*
 UFileIO FileIO;
 FileIO.Default();
 FileIO.SetDirection(0);
 FileIO.SetFileName(ClassesDescriptionFileName);
 FileIO.Calculate();
 if(!FileIO.GetDataString().empty())
 {
  Storage_LoadCommonClassesDescription(FileIO.GetDataString().c_str());
  Storage_LoadClassesDescription(FileIO.GetDataString().c_str());
 }
  */

 return true;
}

// Деинициализирует данные движка
// и сохраняет текущие настройки
void UEngine::UnInit(void)
{
 AccessCache.clear();
 if(!Stop())
  return;

/* Options("General","StorageIndex",sntoa(StorageIndex));
 Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

 if(!Options.SaveToFile(OptionsFileName))
  return false;
  */
}

// Запускает систему
bool UEngine::Start(void)
{
 if(IsRunned() == 1)
 {
  return true;
 }

 if(!IsReady())
  if(!Reset())
  {
   return false;
  }


 Runned=1;
 return true;
}

// Приостанавливает систему
bool UEngine::Pause(void)
{
 if(IsRunned() == 0)
 {
  return true;
 }

 if(!IsReady())
  if(!Reset())
  {
   return false;
  }

 Runned=0;
 return true;
}

// Останавливает систему
bool UEngine::Stop(void)
{
 if(IsRunned() == -1)
 {
  return true;
 }

 Runned=-1;
 return true;
}
// --------------------------

// --------------------------
// Методы управления средой
// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
int UEngine::Storage_GetNumClasses(void)
{
 try
 {
  return Storage->GetNumClasses();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает id классов в хранилище. Память должна быть выделена
void UEngine::Storage_GetClassesList(int *buffer) const
{
 try
 {
  std::vector<UId> temp;
  Storage->GetClassIdList(temp);
  if(temp.size())
   memcpy(buffer,&temp[0],temp.size()*sizeof(UId));
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}


// Возвращает имена классов в хранилище в виде строки, разделенной запятыми
const char* UEngine::Storage_GetClassesNameList(void) const
{
 std::string& TempString=CreateTempString();
 try
 {
  std::vector<std::string> temp;
  Storage->GetClassNameList(temp);
  for(size_t i=0;i<temp.size();i++)
  {
   TempString+=temp[i];
   if(i<temp.size()-1)
    TempString+=",";
  }
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}


 // Возвращает имя класса по его id.
const char * UEngine::Storage_GetClassName(int id) const
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString=Storage->FindClassName(id);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает Id класса по его имени
int UEngine::Storage_GetClassId(const char *name) const
{
 try
 {
  return Storage->FindClassId(name);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
bool UEngine::Storage_DelClass(int classid)
{
 try
 {
  AccessCache.clear();
  Storage->DelClass(classid);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return true;
}

// Удалаяет все свободные объекты из хранилища
void UEngine::Storage_FreeObjectsStorage(void)
{
 try
 {
  AccessCache.clear();
  Storage->FreeObjectsStorage();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Удаляет все объекты из хранилища
void UEngine::Storage_ClearObjectsStorage(void)
{
 try
 {
  AccessCache.clear();
  Storage->ClearObjectsStorage();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Вычисляет суммарное число объектов в хранилище
int UEngine::Storage_CalcNumObjects(void) const
{
 try
 {
  return Storage->CalcNumObjects();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

int UEngine::Storage_CalcNumObjectsById(int classid) const
{
 try
 {
  return Storage->CalcNumObjects(classid);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

int UEngine::Storage_CalcNumObjectsByName(const char* classname) const
{
 try
 {
  return Storage->CalcNumObjects(classname);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает описание класса по его id в формате xml
const char* UEngine::Storage_GetClassDescription(const char* classname)
{
 std::string& TempString=CreateTempString();
 try
 {
  USerStorageXML xml;
  xml.Create(classname);
  Storage->SaveClassDescription(classname,xml);
  xml.SelectUp();
  xml.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Устанавливает описание класса по его id, считывая его из формата xml
bool UEngine::Storage_SetClassDescription(const char* classname, const char* description)
{
 try
 {
  USerStorageXML xml;
  xml.Load(description, classname);
  Storage->LoadClassDescription(classname,xml);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return true;
}

// Сохраняет описание всех классов в xml
const char* UEngine::Storage_SaveClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 try
 {
  USerStorageXML xml;
  xml.Create("Root");
  xml.AddNode("ClassesDescription");
  Storage->SaveClassesDescription(xml);
  xml.SelectRoot();
  xml.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Загружает описание всех классов из xml
bool UEngine::Storage_LoadClassesDescription(const char* xmltext)
{
 try
 {
  USerStorageXML xml;
  xml.Load(xmltext, "Root");
  if(!xml.SelectNode("ClassesDescription"))
   return false;
  Storage->LoadClassesDescription(xml);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return true;
}

// Сохраняет общее описание всех классов в xml
const char* UEngine::Storage_SaveCommonClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 try
 {
  USerStorageXML xml;
  xml.Create("Root");
  xml.AddNode("CommonClassesDescription");
  Storage->SaveCommonClassesDescription(xml);
  xml.SelectRoot();
  xml.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Загружает общее описание всех классов из xml
bool UEngine::Storage_LoadCommonClassesDescription(const char* xmltext)
{
 try
 {
  USerStorageXML xml;
  xml.Load(xmltext, "Root");
  if(!xml.SelectNode("CommonClassesDescription"))
   return false;
  return Storage->LoadCommonClassesDescription(xml);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Сохраняет описание всех классов в xml включая общее описание
const char* UEngine::Storage_SaveAllClassesDescription(void)
{
 std::string& TempString=CreateTempString();
 try
 {
  USerStorageXML xml;
  xml.Create("Root");
  xml.AddNode("CommonClassesDescription");
  Storage->SaveCommonClassesDescription(xml);
  xml.SelectUp();
  xml.AddNode("ClassesDescription");
  Storage->SaveClassesDescription(xml);
  xml.SelectRoot();
  xml.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Загружает описание всех классов из xml включая общее описание
bool UEngine::Storage_LoadAllClassesDescription(const char* xmltext)
{
 try
 {
  USerStorageXML xml;
  xml.Load(xmltext, "Root");
  if(!xml.SelectNode("CommonClassesDescription"))
   return false;
  if(!Storage->LoadCommonClassesDescription(xml))
   return false;
  xml.SelectUp();
  if(!xml.SelectNode("ClassesDescription"))
   return false;
  Storage->LoadClassesDescription(xml);
  return true;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}
// ----------------------------

// ----------------------------
// Методы управления коллекциями компонент
// ----------------------------
// Возвращает число библиотек
int UEngine::Storage_GetNumClassLibraries(void) const
{
 try
 {
  return Storage->GetNumCollections();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}


// Возвращает список библиотек в виде строки, разделенной запятыми
const char * UEngine::Storage_GetClassLibrariesList(void) const
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString.clear();
  for(int i=0;i<Storage->GetNumCollections();i++)
  {
   TempString+=Storage->GetCollectionName(i);
   if(i<Storage->GetNumCollections()-1)
    TempString+=",";
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// library_name - имя библиотеки
const char * UEngine::Storage_GetLibraryClassNames(const char *library_name) const
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString.clear();
  UEPtr<ULibrary> lib=Storage->GetCollection(library_name);
  if(lib)
  {
   const vector<string> &classes=lib->GetComplete();
   for(int i=0;i<int(classes.size());i++)
   {
	TempString+=classes[i];
    if(i<int(classes.size())-1)
     TempString+=",";
   }
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// index - индекс библиотеки
const char * UEngine::Storage_GetLibraryClassNamesByIndex(int index) const
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString.clear();
  UEPtr<ULibrary> lib=Storage->GetCollection(index);
  if(lib)
  {
   const vector<string> &classes=lib->GetComplete();
   for(int i=0;i<int(classes.size());i++)
   {
    TempString+=classes[i];
    if(i<int(classes.size())-1)
     TempString+=",";
   }
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}


// Возвращает имя библиотеки по индексу
const char * UEngine::Storage_GetClassLibraryNameByIndex(int index)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString=Storage->GetCollectionName(index);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает версию библиотеки по индексу
const char * UEngine::Storage_GetClassLibraryVersionByIndex(int index)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString=Storage->GetCollectionVersion(index);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Storage_DelClassLibraryByIndex(int index)
{
 try
 {
  AccessCache.clear();
  return Storage->DelCollection(index);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Storage_DelClassLibraryByName(const char *name)
{
 try
 {
  AccessCache.clear();
  return Storage->DelCollection(name);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Storage_DelAllClassLibraries(void)
{
 try
 {
  AccessCache.clear();
  return Storage->DelAllCollections();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Загружает коллекцию по имени dll-файла
int UEngine::Storage_LoadBinaryCollectionFromFile(const char *filename)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Загружает runtime-коллекцию
int UEngine::Storage_LoadRuntimeCollectionFromFile(const char *filename)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

int UEngine::Storage_LoadRuntimeCollectionFromString(const char *buffer)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Сохраняет runtime-коллекцию
int UEngine::Storage_SaveRuntimeCollectionToFile(const char *filename)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

int UEngine::Storage_SaveRuntimeCollectionToString(const char *buffer)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

/// Создает новую runtime-библиотеку
int UEngine::Storage_CreateRuntimeCollection(const char *collection_name)
{
 try
 {
  if(Storage->CreateRuntimeCollection(collection_name))
   return 0;
  return 74615;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
int UEngine::Storage_CreateClass(const char* stringid, const char *class_name, const char *collection_name)
{
 try
 {
  UEPtr<UContainer> sample=FindComponent(stringid);
  UEPtr<URuntimeLibrary> library=dynamic_pointer_cast<URuntimeLibrary>(Storage->GetCollection(collection_name));

  if(!sample)
   return 74616;

  if(!library)
   return 74617;

  if(!Storage->AddClassToCollection(class_name, sample,library))
   return 74618;

  AccessCache.clear();
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool UEngine::Storage_BuildStorage(void)
{
 try
 {
  AccessCache.clear();
  return Storage->BuildStorage();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}
// ----------------------------


// ----------------------------
// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int UEngine::Env_GetPredefinedStructure(void) const
{
 try
 {
  return Environment->GetPredefinedStructure();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

bool UEngine::Env_SetPredefinedStructure(int value)
{
 try
 {
  AccessCache.clear();
  return Environment->SetPredefinedStructure(value);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool UEngine::Env_IsStoragePresent(void) const
{
 try
 {
  return Environment->IsStoragePresent();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Возвращает состояние инициализации
bool UEngine::Env_IsInit(void) const
{
 try
 {
  return Environment->IsInit();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Признак наличия сформированной структуры
bool UEngine::Env_IsStructured(void) const
{
 try
 {
  return Environment->IsStructured();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Инициализация среды
bool UEngine::Env_Init(void)
{
 try
 {
  AccessCache.clear();
  Environment->Init();
   return true;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Деинициализация среды
bool UEngine::Env_UnInit(void)
{
 try
 {
  AccessCache.clear();
  Environment->UnInit();
  return true;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Формирует предварительно заданную модель обработки
bool UEngine::Env_CreateStructure(void)
{
 try
 {
  AccessCache.clear();
  return Environment->CreateStructure();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Уничтожает текущую модель обработки
bool UEngine::Env_DestroyStructure(void)
{
 try
 {
  AccessCache.clear();
  return Environment->DestroyStructure();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
void UEngine::Env_Destroy(void)
{
 try
 {
  AccessCache.clear();
  Environment->DestroyModel();
  Storage->ClearObjectsStorage();
  Storage->ClearClassesStorage();
  Storage->DelAllCollections();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int UEngine::Env_Calculate(const char* stringid)
{
 try
 {
  RDK::ULongId id;
  if(!stringid)
  {
   Environment->SetModelCalculationComponent(id);
  }
  else
  {
   RDK::UContainer* destcont=FindComponent(stringid);
   if(destcont)
    destcont->GetLongId(Environment->GetModel(),id);
//   id.DecodeFromString(stringid);
    Environment->SetModelCalculationComponent(id);
  }

  if(!Environment->Calculate())
   throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   // return 1;
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Расчет всей модели в реальном времени
void UEngine::Env_RTCalculate(void)
{
 try
 {
  Environment->RTCalculate();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
}


// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
// иначе - только указанный компонент модели
int UEngine::Env_Reset(const char* stringid)
{
 try
 {
  RDK::ULongId id;
  if(!stringid)
  {
   Environment->SetModelCalculationComponent(id);
   if(!Environment->Reset())
	throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
  }
  else
  {
   RDK::UContainer* destcont=FindComponent(stringid);
   if(destcont)
   {
	if(!destcont->Reset())
	 throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
   }
   else
	return 100000;
  }
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

/// Метод сброса параметров на значения по умолчанию
/// Если stringid == 0 то сбрасывает всю модель целиком,
/// иначе - только указанный компонент модели
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
int UEngine::Env_Default(const char* stringid, bool subcomps)
{
 try
 {
  RDK::UContainer* destcont;
  if(!stringid)
  {
   destcont=GetModel();
  }
  else
  {
   destcont=FindComponent(stringid);
  }

  if(!destcont->DefaultAll(destcont,subcomps))
   throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}



// Производит увеличение времени модели на требуемую величину
void UEngine::Env_IncreaseModelTimeByStep(void)
{
 try
 {
  Environment->IncreaseModelTimeByStep();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
}

/// Устанавливает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
int UEngine::Env_SetMinInterstepsInterval(unsigned long long value)
{
 try
 {
  if(!Environment->SetMinInterstepsInterval(value))
   throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
//   return -2000;
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

/// Возвращает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
unsigned long long UEngine::Env_GetMinInterstepsInterval(void) const
{
 try
 {
  return Environment->GetMinInterstepsInterval();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Время, потраченное на последний RT-расчет
double UEngine::Env_GetRTLastDuration(void) const
{
 return Environment->GetRTLastDuration();
}

/// Время, расчитанное в модели за один вызов RTCalculate;
double UEngine::Env_GetRTModelCalcTime(void) const
{
 return Environment->GetRTModelCalcTime();
}

/// Производительность RT расчета (отношение RTModelCalcTime/RTLastDuration)
double UEngine::Env_CalcRTPerformance(void) const
{
 return Environment->CalcRTPerformance();
}

// !!! Следующие методы управления текущим компонентом влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// Устанавливает текущий компонент (адресация относительно корня - модели)
int UEngine::Env_SelectCurrentComponent(const char *stringid)
{
 try
 {
  Environment->ResetCurrentComponent();

  // Если первый символ - цифра, то декодруем как строковой id
  if(stringid[0]>=0x30 && stringid[0]<=0x39)
  {
   ULongId longid;
   longid.DecodeFromString(stringid);
   Environment->SelectCurrentComponent(longid);
  }
  else // ...иначе декодируем как строковое имя
  {
   Environment->SelectCurrentComponent(stringid);
  }
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Сбрасывает текущий компонент в состояние по умолчению (модель)
int UEngine::Env_ResetCurrentComponent(const char *stringid)
{
 try
 {
  Environment->ResetCurrentComponent();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Меняет текущий компонент на его родителя (подъем на уровень вверх)
// Если уже на верхнем уровне, то не делает ничего
int UEngine::Env_UpCurrentComponent(void)
{
 try
 {
  Environment->UpCurrentComponent();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
// (спуск на N уровней вниз относительно текущего компонента)
int UEngine::Env_DownCurrentComponent(const char *stringid)
{
 try
 {
  // Если первый символ - цифра, то декодруем как строковой id
  if(stringid[0]>=0x30 && stringid[0]<=0x39)
  {
   ULongId longid;
   longid.DecodeFromString(stringid);
   Environment->DownCurrentComponent(longid);
  }
  else // ...иначе декодируем как строковое имя
  {
   Environment->DownCurrentComponent(stringid);
  }
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает длинное имя текущего компонента
const char* UEngine::Env_GetCurrentComponentName(void) const
{
 std::string& TempString=CreateTempString();
 try
 {
  Environment->GetCurrentComponent()->GetLongName(Environment->GetModel(),TempString);
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает длинный строковой id текущего компонента
const char* UEngine::Env_GetCurrentComponentId(void) const
{
 std::string& TempString=CreateTempString();
 try
 {
  ULongId longid;
  Environment->GetCurrentComponent()->GetLongId(Environment->GetModel(),longid);
  longid.EncodeToString(TempString);
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает имя текущего каталога для хранения данных
const char* UEngine::Env_GetCurrentDataDir(void) const
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString=Environment->GetCurrentDataDir();
  return TempString.c_str();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

// Устанавливает имя текущего каталога для хранения данных
int UEngine::Env_SetCurrentDataDir(const char *dir)
{
 try
 {
  Environment->SetCurrentDataDir(dir);
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает имя каталога бинарных файлов
const char* UEngine::Env_GetSystemDir(void) const
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString=Environment->GetSystemDir().c_str();
  return TempString.c_str();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

// Устанавливает имя каталога бинарных файлов
int UEngine::Env_SetSystemDir(const char *dir)
{
 try
 {
  Environment->SetSystemDir(dir);
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

/// Возвращает состояние флага отладочного режима среды
int UEngine::Env_GetDebugMode(void) const
{
 try
 {
  return Environment->GetDebugMode();
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

/// Устанавливает состояние флага отладочного режима среды
int UEngine::Env_SetDebugMode(bool value)
{
 try
 {
  return Environment->SetDebugMode(value);
 }
 catch (RDK::UException &exception)
 {
  ProcessException(exception);
 }
 return -1;
}

// Задает число входов среды
void UEngine::Env_SetNumInputImages(int number)
{
 //GetEnvironment()->SetNumInputImages(number);
}

// Задает число выходов среды
void UEngine::Env_SetNumOutputImages(int number)
{
 //GetEnvironment()->SetNumOutputImages(number);
}

// Возвращает число входов среды
int UEngine::Env_GetNumInputImages(void)
{
 return 0;//return GetEnvironment()->GetNumInputImages();
}

// Возвращает число выходов среды
int UEngine::Env_GetNumOutputImages(void)
{
 return 0;//return GetEnvironment()->GetNumOutputImages();
}

// Задает разрешение по умолчанию (рабочее разрешение)
void UEngine::Env_SetInputRes(int number, int width, int height)
{
 //GetEnvironment()->SetInputImageRes(number,width,height);
}

// Задает флаг отражения входного изображения вокруг горизонтальной оси
void UEngine::Env_SetReflectionXFlag(bool value)
{
 //GetEnvironment()->SetReflectionXFlag(value);
}

// Возвращает разрешение по умолчанию (рабочее разрешение)
int UEngine::Env_GetInputImageWidth(int number)
{
 return 0;//return GetEnvironment()->GetInputImageWidth(number);
}

int UEngine::Env_GetInputImageHeight(int number)
{
 return 0;//return GetEnvironment()->GetInputImageHeight(number);
}

int UEngine::Env_GetInputImageColorModel(int number)
{
 return 0;//return GetEnvironment()->GetInputImageColorModel(number);
}

// Возвращает текущее выходное разрешение
int UEngine::Env_GetOutputImageWidth(int number)
{
 return 0;// return GetEnvironment()->GetOutputImageWidth(number);
}

int UEngine::Env_GetOutputImageHeight(int number)
{
 return 0;// return GetEnvironment()->GetOutputImageHeight(number);
}

int UEngine::Env_GetOutputImageColorModel(int number)
{
 return 0;// return GetEnvironment()->GetOutputImageColorModel(number);
}

void UEngine::Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
/* TempBmp.AttachBuffer(width,height,image,RDK::UBMColorModel(cmodel));
 BResizeEdges.SetNewWidth(GetEnvironment()->GetInputImageWidth(number));
 BResizeEdges.SetNewHeight(GetEnvironment()->GetInputImageHeight(number));
 BResizeEdges(TempBmp,TempBmp2);
 GetEnvironment()->SetInputImage(number,TempBmp2);
 TempBmp.DetachBuffer();*/
}

unsigned char* UEngine::Env_GetInputImage(int index)
{
 return 0;// return GetEnvironment()->GetInputImage(index).GetData();
}

unsigned char* UEngine::Env_GetOutputImage(int index)
{
 return 0;// return GetEnvironment()->GetOutputImage(index).GetData();
}

unsigned char* UEngine::Env_GetOutputImageY8(int index)
{
 return 0;// return GetEnvironment()->GetOutputImage(index).GetData();
}


/// Инициирует извещение о сбое в работе источника данных
bool UEngine::Env_CallSourceController(void)
{
 try
 {
  if(!Environment)
   return false;
  return Environment->CallSourceController();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}


// Методы управления моделью
// ----------------------------
// Удаляет модель
int UEngine::Model_Destroy(void)
{
 try
 {
  AccessCache.clear();
  if(!Environment->DestroyModel())
   throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
//   return -1;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
int UEngine::Model_Create(const char *classname)
{
 try
 {
  AccessCache.clear();
  if(!Environment->CreateModel(classname))
   throw EFunctionReturnFalse(__FILE__,__FUNCTION__,__LINE__);
//   return -1;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Очищает модель
int UEngine::Model_Clear(void)
{
 try
 {
  AccessCache.clear();
  UEPtr<RDK::UContainer> model=dynamic_pointer_cast<RDK::UContainer>(Environment->GetModel());

  if(!model)
   return -2;

  model->DelAllComponents();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Проверяет, существует ли модель
bool UEngine::Model_Check(void)
{
 try
 {
  UEPtr<RDK::UContainer> model=dynamic_pointer_cast<RDK::UContainer>(Environment->GetModel());

  if(!model)
   return false;

  return true;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Проверяет, существует ли в модели компонент с именем stringid)
bool UEngine::Model_CheckComponent(const char* stringid) const
{
 try
 {
  UEPtr<RDK::UContainer> destcont=FindComponent(stringid);
  if(destcont)
   return true;
 }
 catch (UContainer::EComponentNameNotExist &exception)
 {
  return false;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'classname'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает имя компонента в случае успеха
const char* UEngine::Model_AddComponent(const char* stringid, const char *classname)
{
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UContainer> destcont=FindComponent(stringid);

  UEPtr<RDK::UContainer> cont=dynamic_pointer_cast<RDK::UContainer>(Storage->TakeObject(classname));

  if(!cont)
  {
   DestroyTempString(TempString);
   return 0;
  }

  if(!destcont)
  {
   DestroyTempString(TempString);
   return 0;
  }

  if(destcont->AddComponent(cont))
  {
   TempString=cont->GetName();
  }
  else
   TempString.clear();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'name'
// если stringid - пустая строка, то удаляет из самой модели
int UEngine::Model_DelComponent(const char* stringid, const char *name)
{
 try
 {
  RDK::UContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  destcont->DelComponent(name);
  AccessCache.clear();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}


/// Перемещает компоненту в другой компонент
/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
int UEngine::Model_MoveComponent(const char* component, const char* target)
{
 try
 {
  RDK::UContainer* comp=FindComponent(component);
  RDK::UContainer* target_comp=FindComponent(target);

  if(!comp)
   return -4;

  if(!target_comp)
   return -5;

  if(!comp->GetOwner())
   return -6;

  if(comp == target_comp)
   return -7;

  if(!comp->GetOwner()->MoveComponent(comp,target_comp))
   return -8;

  AccessCache.clear();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}


// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
int UEngine::Model_GetNumComponents(const char* stringid)
{
 try
 {
  RDK::UContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return 0;

  return destcont->GetNumComponents();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
int UEngine::Model_GetComponentsList(const char* stringid, int *buffer)
{
 try
 {
  RDK::UContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  std::vector<int> tempbuffer;

  destcont->GetComponentsList(tempbuffer);
  if(tempbuffer.size())
   memcpy(buffer,&tempbuffer[0],tempbuffer.size()*sizeof(int));
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ','
const char* UEngine::Model_GetComponentsNameList(const char* stringid)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString.clear();
  RDK::UContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return TempString.c_str();

  std::vector<std::string> tempbuffer;

  destcont->GetComponentsList(tempbuffer);
  for(int i=0;i<int(tempbuffer.size());i++)
  {
   TempString+=tempbuffer[i];
   if(i<int(tempbuffer.size())-1)
    TempString+=",";
  }
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 DestroyTempString(TempString);
 return 0;
}

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ',' и имеющих имя класса 'class_name'
// Если find_all == true то поиск ведется и во всех сабкомпонентах
const char* UEngine::Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString.clear();

  if(!class_name || !strlen(class_name))
   return TempString.c_str();

  RDK::UContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return TempString.c_str();

  int class_id=Storage->FindClassId(class_name);

  if(class_id == ForbiddenId)
   return TempString.c_str();

  std::vector<std::string> tempbuffer;
  destcont->GetComponentsNameByClassName(class_name, tempbuffer, find_all);
  size_t numComp=tempbuffer.size();

  for(size_t i=0; i<numComp; i++)
  {
   TempString+=tempbuffer[i];
   if( int(i) < int(tempbuffer.size())-1 )
	 TempString+=",";
  }

  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

/* Старый вариант, не работает поиск в субкомпанентах
const char* UEngine::Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{
 try
 {
  TempString.clear();

  if(!class_name || !strlen(class_name))
   return TempString.c_str();

  RDK::UContainer* destcont=FindComponent(stringid);

  std::vector<std::string> tempbuffer;

  if(!destcont)
   return TempString.c_str();

  destcont->GetComponentsList(tempbuffer);
  std::string name;
  int class_id=Storage->FindClassId(class_name);

  if(class_id == ForbiddenId)
   return TempString.c_str();

  for(int i=0;i<destcont->GetNumComponents();i++)
  {
   if(destcont->GetComponentByIndex(i)->GetClass() == class_id)
   {
	TempString+=destcont->GetComponentByIndex(i)->GetLongName(destcont,name);
	if(i<int(tempbuffer.size())-1)
	 TempString+=",";
   }
  }
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}
*/

// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
int UEngine::Model_ChangeComponentPosition(const char* stringid, int step)
{
 try
 {
  RDK::UContainer* destcont=FindComponent(stringid);

  if(destcont == Environment->GetModel())
   return 0;

  if(!destcont)
   return 1;

  if(!destcont->GetOwner())
   return 2;

  if(destcont->GetOwner()->ChangeComponentPosition(destcont->GetName(),step))
   return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 3;
}


// Возвращает xml-список длинных идентификаторов всех коннекторов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// коннекторы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех коннекторов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы коннекторов только этой сети
// Предварительная очистка буфера не производится.
const char* UEngine::Model_GetConnectorsList(const char* stringid,
                          int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<RDK::UContainer> owner_level=FindComponent(owner_level_stringid);

  TempString="";
  if(!cont)
   return TempString.c_str();


  ULongIdVector buffer;
  cont->GetConnectorsList(buffer,sublevel,owner_level);

  XmlStorage.Create("Connectors");
  XmlStorage<<buffer;
  XmlStorage.SelectRoot();

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает xml-список длинных идентификаторов всех элементов сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// элементы будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех элементов включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы элементов только этой сети
// Предварительная очистка буфера не производится.
const char* UEngine::Model_GetItemsList(const char* stringid,
                            int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<RDK::UContainer> owner_level=FindComponent(owner_level_stringid);

  TempString="";
  if(!cont)
   return TempString.c_str();


  ULongIdVector buffer;
  cont->GetItemsList(buffer,sublevel,owner_level);

  XmlStorage.Create("Items");
  XmlStorage<<buffer;

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает xml-список длинных идентификаторов всех подсетей сети.
// 'sublevel' опеределяет число уровней вложенности подсетей для которых
// подсети будут добавлены в список.
// если 'sublevel' == -2, то возвращает идентификаторы всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает идентификаторы всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает идентификаторы подсетей только этой сети
// Предварительная очистка буфера не производится.
const char* UEngine::Model_GetNetsList(const char* stringid,
                            int sublevel, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<RDK::UContainer> owner_level=FindComponent(owner_level_stringid);

  TempString="";
  if(!cont)
   return TempString.c_str();

  ULongIdVector buffer;
  cont->GetNetsList(buffer,sublevel,owner_level);

  XmlStorage.Create("Nets");
  XmlStorage<<buffer;

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();

}


// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* UEngine::Model_GetComponentName(const char* stringid)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UContainer> destcont=FindComponent(stringid);

  if(!destcont)
   return TempString.c_str();

  TempString=destcont->GetName();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}


// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
const char* UEngine::Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UContainer> destcont=FindComponent(stringid);
  UEPtr<RDK::UContainer> owner_level=FindComponent(owner_level_stringid);

  if(!destcont)
   return TempString.c_str();

  return destcont->GetLongName(owner_level,TempString).c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
const char* UEngine::Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UContainer> destcont=FindComponent(stringid);
  UEPtr<RDK::UContainer> owner_level=FindComponent(owner_level_stringid);

  if(!destcont)
   return TempString.c_str();

  ULongId id;

  destcont->GetLongId(owner_level,id);
  id.EncodeToString(TempString);
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает имя класса компонента в хранилище по длинному 'stringid'
// если stringid - пустая строка, то возвращает имя класса модели
const char* UEngine::Model_GetComponentClassName(const char* stringid)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UContainer> destcont=FindComponent(stringid);

  if(!destcont)
   return TempString.c_str();

  TempString=Storage->FindClassName(destcont->GetClass());
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает список свойств компонента разделенный запятыми
const char* UEngine::Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);

  if(!cont)
   return TempString.c_str();

  RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
    if(TempString.size()>0)
     TempString+=",";
	TempString+=I->first;
//    TempString+=":";
//    TempString+=sntoa(I->second.Property->GetMinRange());
   }
   ++I;
  }

  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

const char* UEngine::Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);

  if(!cont)
   return TempString.c_str();

  RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
    if(TempString.size()>0)
     TempString+=",";
    TempString+=I->first;
    TempString+=":";
    TempString+=sntoa(0);
   }
   ++I;
  }

  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}


// Возвращает свойства компонента по идентификатору
const char* UEngine::Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return TempString.c_str();

  XmlStorage.Create(cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  XmlStorage.AddNode(UVariable::GetPropertyTypeNameByType(type_mask));

  if(!cont->GetComponentProperties(cont,&XmlStorage, type_mask))
  {
   DestroyTempString(TempString);
   return 0;
  }

  XmlStorage.SelectUp();
  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

// Возвращает выборочные свойства компонента по идентификатору
const char* UEngine::Model_GetComponentSelectedProperties(const char *stringid)
{
 std::string& TempString=CreateTempString();
 try
 {
  DestroyTempString(TempString);
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

// Возвращает свойства компонента по идентификатору с описаниями
const char* UEngine::Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
  if(!cont)
   return TempString.c_str();
  std::string comp_name;
  USerStorageXML xml;
  xml.Create(cont->GetLongName(Environment->GetCurrentComponent(),comp_name));
  xml.AddNode(UVariable::GetPropertyTypeNameByType(type_mask));

  if(!cont->GetComponentPropertiesEx(cont,&xml, type_mask))
  {
   DestroyTempString(TempString);
   return 0;
  }

  xml.SelectUp();
  xml.Save(TempString);
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

// Возвращает значение свойства компонента по идентификатору компонента и имени свойства
const char * UEngine::Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
{
 std::string& TempString=CreateTempString();
 try
 {
  TempString="";
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  if(!cont)
   return TempString.c_str();

  cont->GetPropertyValue(paramname,TempString);
  return TempString.c_str();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 DestroyTempString(TempString);
 return 0;
}

// устанавливает свойства компонента по идентификатору
int UEngine::Model_SetComponentProperties(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
  if(!cont)
   return 1;

  XmlStorage.Load(buffer, cont->GetLongName(Environment->GetCurrentComponent(),CompName));
  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(XmlStorage.SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
    if(cont->SetComponentProperties(cont,&XmlStorage))
     return 2;

    XmlStorage.SelectUp();
   }
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Устанавливает значение свойства компонента по идентификатору компонента и имени свойства
void UEngine::Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  if(!cont)
   return;

  cont->SetPropertyValue(paramname,buffer);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}


// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
void UEngine::Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));
  if(!cont)
   return;

  UId classid=Storage->FindClassId(class_stringid);
  if(classid == ForbiddenId)
   return;

  cont->SetGlobalComponentPropertyValue(cont, classid, paramname, buffer);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
void UEngine::Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));
  if(!cont)
   return;

  UId classid=Storage->FindClassId(class_stringid);
  UId owner_classid=Storage->FindClassId(class_owner_stringid);
  if(classid == ForbiddenId || owner_classid == ForbiddenId)
   return;

  cont->SetGlobalOwnerComponentPropertyValue(cont, classid, owner_classid, paramname, buffer);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Возвращает указатель void* на данные свойства компонента
const void* UEngine::Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  return iproperty->GetMemoryArea();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Копирует данные 'data' в заданное свойство компонента
int UEngine::Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data)
{
 try
 {
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  if(!iproperty->ReadFromMemory(data))
   return 1;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Связывает выбранные контейнеры друг с другом
int UEngine::Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
   longid1.DecodeFromString(stringid1);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid1);
   longid1=cont->GetLongId(model);
  }

  if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
   longid2.DecodeFromString(stringid2);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid2);
   longid2=cont->GetLongId(model);
  }
  bool res=model->CreateLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

int UEngine::Model_CreateLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;
/*
  std::string longid1, longid2;
  if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
   longid1.DecodeFromString(stringid1);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid1);
   longid1=cont->GetLongName(model);
  }

  if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
   longid2.DecodeFromString(stringid2);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid2);
   longid2=cont->GetLongName(model);
  } */
  bool res=model->CreateLink(stringid1,item_property_name,stringid2,connector_property_name);
  if(!res)
   return -3;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}


// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
int UEngine::Model_ChainLinking(const char* stringid)
{
 try
 {
  UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

  cont->BreakLinks(cont);

  if(cont->GetNumComponents() == 0)
   return 0;

  ULongId id1,id2;


  UEPtr<UNet> item=dynamic_pointer_cast<UNet>(cont->GetComponentByIndex(0));
  item->GetLongId(cont,id1);

  // Подключаем выходы модели
  int minsize=cont->GetNumOutputs();
  if(minsize>item->GetNumInputs())
   minsize=item->GetNumInputs();
  for(int j=0;j<minsize;j++)
   cont->CreateLink(ForbiddenId,j,id1,j);

  for(int i=1;i<cont->GetNumComponents();i++)
  {
   UEPtr<UNet> connector=dynamic_pointer_cast<UNet>(cont->GetComponentByIndex(i));
   connector->GetLongId(cont,id2);
   minsize=item->GetNumOutputs();
   if(minsize>connector->GetNumInputs())
    minsize=connector->GetNumInputs();
   for(int j=0;j<minsize;j++)
    cont->CreateLink(id1,j,id2,j);
   id1=id2;
   item=connector;
  }
 /*   // Заглушка! Непонятно, будет ли это нужно когда-нибудь
  // Подключаем входы модели
  minsize=cont->GetNumInputs();
  if(minsize>item->GetNumOutputs())
   minsize=item->GetNumOutputs();
  for(int j=0;j<minsize;j++)
   cont->CreateLink(id1,j,ForbiddenId,j);
   */
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
int UEngine::Model_ParallelLinking(const char* stringid)
{
 try
 {
  UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

  cont->BreakLinks(cont);

  if(cont->GetNumComponents() == 0)
   return 0;

  ULongId id1,id2;


  UEPtr<UNet> item=cont;//dynamic_pointer_cast<UNet>(cont->GetComponentByIndex(0));
  //item->GetLongId(cont,id1);

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   UEPtr<UNet> connector=dynamic_pointer_cast<UNet>(cont->GetComponentByIndex(i));
   connector->GetLongId(cont,id2);
   int minsize=item->GetNumOutputs();
   if(minsize>connector->GetNumInputs())
    minsize=connector->GetNumInputs();
   for(int j=0;j<minsize;j++)
    cont->CreateLink(id1,j,id2,j);
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Разрывает выбранную связь
int UEngine::Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
   longid1.DecodeFromString(stringid1);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid1);
   longid1=cont->GetLongId(model);
  }

  if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
   longid2.DecodeFromString(stringid2);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid2);
   longid2=cont->GetLongId(model);
  }

  bool res=model->BreakLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

int UEngine::Model_BreakLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;
  /*
  RDK::ULongId longid1, longid2;
  if(stringid1[0]>=0x30 && stringid1[0]<=0x39)
   longid1.DecodeFromString(stringid1);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid1);
   longid1=cont->GetLongId(model);
  }

  if(stringid2[0]>=0x30 && stringid2[0]<=0x39)
   longid2.DecodeFromString(stringid2);
  else
  {
   UEPtr<UContainer> cont=FindComponent(stringid2);
   longid2=cont->GetLongId(model);
  }
*/
  bool res=model->BreakLink(stringid1,item_property_name,stringid2,connector_property_name);
  if(!res)
   return -3;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Разрывает все связи
int UEngine::Model_BreakAllLinks(void)
{
 try
 {
  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;

  model->BreakLinks();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Разрывает связь ко входу connector_index коннектора 'connectorid'
int UEngine::Model_BreakConnectorLink(const char* connectorname, int connector_index)
{
 try
 {
  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

  if(!model)
   return -2;

  model->BreakConnectorLink(connectorname, connector_index);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Разрывает все входные и выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentLinks(const char* stringid)
{
 try
 {
  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!model)
   return -2;

  model->BreakLinks();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Разрывает все входные связи выбранного контейнера
int UEngine::Model_BreakAllComponentInputLinks(const char* stringid)
{
 try
 {
  UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!model)
   return -2;

  model->DisconnectAllItems();
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Разрывает все выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentOutputLinks(const char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Проверяет, существует ли заданна связь
bool UEngine::Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 try
 {
  UEPtr<RDK::UADItem> cont1;
  UEPtr<RDK::UConnector> cont2;
  try
  {
   cont1=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid1));
   cont2=dynamic_pointer_cast<RDK::UConnector>(FindComponent(stringid2));
  }
  catch (UException &exception)// Заглушка!! здесь другое исключение
  {
   return false;
  }
  if(!cont1 || !cont2)
   return false;

  return cont1->CheckLink(cont2,output_number,input_number);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

bool UEngine::Model_CheckLink(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 try
 {
  UEPtr<RDK::UItem> cont1;
  UEPtr<RDK::UConnector> cont2;
  try
  {
   cont1=dynamic_pointer_cast<RDK::UItem>(FindComponent(stringid1));
   cont2=dynamic_pointer_cast<RDK::UConnector>(FindComponent(stringid2));
  }
  catch (UException &exception)// Заглушка!! здесь другое исключение
  {
   return false;
  }
  if(!cont1 || !cont2)
   return false;

  return cont1->CheckLink(cont2,item_property_name,connector_property_name, -1); // TODO: -1 заменить на параметр
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char* UEngine::Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
  UEPtr<RDK::UNet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(cont->GetComponentInternalLinks(cont,&XmlStorage,owner))
   return TempString.c_str();

  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются с уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то применяется уровень текущего компонента
int UEngine::Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
  UEPtr<RDK::UNet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"Links");

  if(!cont->SetComponentInternalLinks(cont,&XmlStorage,owner))
   return -4;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * UEngine::Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
  UEPtr<RDK::UNet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(!cont->GetComponentInputLinks(cont,&XmlStorage,owner,sublevel))
  {
   DestroyTempString(TempString);
   return 0;
  }

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * UEngine::Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
  UEPtr<RDK::UNet> owner;
  if(owner_level_stringid)
   dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(!cont->GetComponentOutputLinks(cont,&XmlStorage,owner, sublevel))
  {
   DestroyTempString(TempString);
   return 0;
  }

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char* UEngine::Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));
  UEPtr<RDK::UNet> owner;
  if(owner_level_stringid)
   owner=dynamic_pointer_cast<RDK::UNet>(FindComponent(owner_level_stringid));

  TempString="";
  if(!cont)
   return TempString.c_str();

  XmlStorage.Create("Links");

  if(cont->GetComponentPersonalLinks(cont,&XmlStorage,owner))
   return TempString.c_str();

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает число входов у компонента
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 try
 {
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetNumInputs();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер входа компонента в числе элементов
int UEngine::Model_GetComponentInputDataSize(const char *stringid, int index)
{
 try
 {
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetInputDataSize(index);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер элемента входа в байтах
int UEngine::Model_GetComponentInputElementSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер входа компонента в байтах элементов
int UEngine::Model_GetComponentInputByteSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
unsigned char* UEngine::Model_GetComponentInputData(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает число выходов у компонента
int UEngine::Model_GetComponentNumOutputs(const char *stringid)
{
 try
 {
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetNumOutputs();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер выхода компонента в числе элементов
int UEngine::Model_GetComponentOutputDataSize(const char *stringid, int index)
{
 try
 {
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetOutputDataSize(index);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер элемента выхода в байтах
int UEngine::Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 try
 {
  UEPtr<RDK::UADItem> cont=dynamic_pointer_cast<RDK::UADItem>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetOutputDataElementSize(index);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер выхода компонента в байтах элементов
int UEngine::Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
unsigned char* UEngine::Model_GetComponentOutputData(const char *stringid, int index)
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return 0;

  return cont->GetOutputData(index).UChar;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char *  UEngine::Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.DelNode();
  XmlStorage.Create("Save");
  XmlStorage.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

  if(!cont->SaveComponent(cont,&XmlStorage, true, params_type_mask))
  {
   DestroyTempString(TempString);
   return 0;
  }

  XmlStorage.Save(TempString);
 // strcpy(buffer,str.c_str());
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
int UEngine::Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask)
{ /*
 const char* save_data=Model_SaveComponent(stringid,params_type_mask);

 try
 {
  if(!save_data)
   return 871121;

  UFileIO file;
  file.Default();
  file.SetDirection(1);
  file.SetFileName(file_name);
  file.SetBinFlag(0);
  file.SetDataString(save_data);
  file.Calculate();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }*/
 return 0;   
}


// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponent(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 try
 {
  AccessCache.clear();
  XmlStorage.Load(buffer,"Save");
  XmlStorage.SelectNode(0);

  Environment->Reset();

  if(!Environment->GetModel())
  {
   std::string name=XmlStorage.GetNodeAttribute("Class");
   UId id=Storage->FindClassId(name);
   UEPtr<UNet> cont=dynamic_pointer_cast<RDK::UNet>(Environment->GetModel()).Get();
   if(!cont || cont->GetClass() != id)
   {
	Model_Destroy();
	Model_Create(name.c_str());
	cont=dynamic_pointer_cast<RDK::UNet>(Environment->GetModel()).Get();
   }

   if(!cont->LoadComponent(cont,&XmlStorage,true))
	return -4;
  }
  else
  {
   if(XmlStorage.GetNodeAttribute("ModelName") != Environment->GetModel()->GetName())
	throw EErrorEngineModelNameDontMatch(XmlStorage.GetNodeAttribute("ModelName"), Environment->GetModel()->GetName());
	//return -10;

   UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

   if(!cont)
	return -3;

   if(!cont->LoadComponent(cont,&XmlStorage,true))
	return -4;
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponentFromFile(const char *stringid, const char* file_name)
{    /*
 try
 {
  UFileIO file;
  file.Default();
  file.SetDirection(0);
  file.SetFileName(file_name);
  file.SetBinFlag(0);
  file.Calculate();
  if(file.GetDataString().empty())
   return 871122;
  return Model_LoadComponent(stringid, file.GetDataString().c_str());
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }*/
 return 0;     
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
const char * UEngine::Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{
 RDK::USerStorageXML XmlStorage;
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
  {
   DestroyTempString(TempString);
   return 0;
  }

  XmlStorage.DelNode();
  XmlStorage.Create("SaveProperties");
  XmlStorage.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

  if(!cont->SaveComponentProperties(cont,&XmlStorage, type_mask))
  {
   DestroyTempString(TempString);
   return 0;
  }

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
int UEngine::Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask)
{  /*
 const char* save_data=Model_SaveComponentProperties(stringid,type_mask);

 try
 {
  if(!save_data)
   return 871121;

  UFileIO file;
  file.Default();
  file.SetDirection(1);
  file.SetFileName(file_name);
  file.SetBinFlag(0);
  file.SetDataString(save_data);
  file.Calculate();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }*/
 return 0;       
}

// Загружает все свойства компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentProperties(const char *stringid, const char* buffer)
{
 RDK::USerStorageXML XmlStorage;
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"SaveProperties");
  if(XmlStorage.GetNodeAttribute("ModelName") != Environment->GetModel()->GetName())
	throw EErrorEngineModelNameDontMatch(XmlStorage.GetNodeAttribute("ModelName"), Environment->GetModel()->GetName());
//   return -10;

  XmlStorage.SelectNode(0);

  if(!cont->LoadComponentProperties(cont,&XmlStorage))
   return -4;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name)
{/*
 try
 {
  UFileIO file;
  file.Default();
  file.SetDirection(0);
  file.SetFileName(file_name);
  file.SetBinFlag(0);
  file.Calculate();
  if(file.GetDataString().empty())
   return 871122;
  return Model_LoadComponentProperties(stringid, file.GetDataString().c_str());
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }*/
 return 0;   
}

// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
const char* UEngine::Model_SaveComponentDrawInfo(const char *stringid)
{
 std::string& TempString=CreateTempString();
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
  {
   DestroyTempString(TempString);
   return 0;
  }
  USerStorageXML xml;
  xml.DelNode();
  xml.Create("Save");
  xml.SetNodeAttribute("ModelName",Environment->GetModel()->GetName());

  if(!cont->SaveComponentDrawInfo(cont,&xml))
  {
   DestroyTempString(TempString);
   return 0;
  }

  xml.Save(TempString);
 // strcpy(buffer,str.c_str());
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}



// Управляет шагом счета модели по умолчанию
unsigned int UEngine::Model_GetDefaultTimeStep(void) const
{
 return DefaultTimeStep;
}

void UEngine::Model_SetDefaultTimeStep(unsigned int value)
{
 DefaultTimeStep=value;
}

// Управляет шагом счета компонента
unsigned int UEngine::Model_GetTimeStep(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  return cont->GetTimeStep();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

void UEngine::Model_SetTimeStep(const char *stringid, unsigned int value)
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  cont->SetTimeStep(value);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Устанавливает шаг счета компонента и всех его дочерних компонент
void UEngine::Model_SetGlobalTimeStep(const char *stringid, unsigned int value)
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  cont->SetGlobalTimeStep(value);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Возвращает текущее время модели
unsigned long long UEngine::Model_GetTime(void)
{
 return Environment->GetTime().GetTime();
}

double UEngine::Model_GetDoubleTime(void)
{
 return Environment->GetTime().GetDoubleTime();
}

// Устанавливает текущее время модели
bool UEngine::Model_SetTime(unsigned long long value)
{
 return Environment->GetTime().SetTime(value);
}

// Возвращает реальное время
unsigned long long UEngine::Model_GetRealTime(void)
{
 return Environment->GetTime().GetRealTime();
}

double UEngine::Model_GetDoubleRealTime(void)
{
 return Environment->GetTime().GetDoubleRealTime();
}

// Устанавливает реальное время
bool UEngine::Model_SetRealTime(unsigned long long value)
{
 return Environment->GetTime().SetRealTime(value);
}

// Увеличивает реальное время на заданную величину
bool UEngine::Model_IncreaseRealTime(unsigned long long value)
{
 return Environment->GetTime().IncreaseRealTime(value);
}

// Возвращает мгновенный шаг в реальном времени
unsigned long long UEngine::Model_GetRealTimeStep(void)
{
 return Environment->GetTime().GetRealTimeStep();
}

double UEngine::Model_GetDoubleRealTimeStep(void)
{
 return Environment->GetTime().GetDoubleRealTimeStep();
}

double UEngine::Model_GetDoubleSourceTime(void) const
{
 return Environment->GetTime().GetSourceCurrentGlobalTime();
}

// Устанавливает время внешних источников данных в днях
bool UEngine::Model_SetDoubleSourceTime(double value)
{
 return Environment->GetTime().SetSourceCurrentGlobalTime(value);
}

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
unsigned long long UEngine::Model_GetStepDuration(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  return cont->GetStepDuration();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
unsigned long long UEngine::Model_GetFullStepDuration(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  return cont->GetFullStepDuration();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}


// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double UEngine::Model_GetInstantPerformance(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  return cont->GetInstantPerformance();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Время, прошедшее между двумя последними итерациями счета
unsigned long long UEngine::Model_GetInterstepsInterval(const char *stringid) const
{
 try
 {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  return cont->GetInterstepsInterval();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}
// --------------------------

// --------------------------
// Скрытые методы управления средой
// --------------------------
/// Метод сброса параметров на значения по умолчанию
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
/*
bool UEngine::Env_Default(RDK::UContainer* cont, bool subcomps)
{
 if(!cont)
  return false;

 if(!cont->Default())
  return false;

 bool res=true;
 if(subcomps)
 {
  for(int i=0;i<cont->GetNumComponents();i++)
   res &= Env_Default(cont->GetComponentByIndex(i),subcomps);
 }
 return res;
} */
         /*
// Возвращает свойства компонента по идентификатору
bool UEngine::Model_GetComponentProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 cont->GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

    std::string paramname=I->first;//I->second.Property->GetName();
    if(serstorage->SelectNode(paramname))
    {
     serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
     serstorage->SelectUp();
    }
   }
   ++I;
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возвращает выборочные свойства компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentSelectedProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возвращает свойства компонента по идентификатору с описаниями
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentPropertiesEx(RDK::UContainer* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  UEPtr<UContainerDescription> descr=dynamic_pointer_cast<UContainerDescription>(Storage->GetClassDescription(Storage->FindClassName(cont->GetClass())));

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   if(I->second.CheckMask(type_mask))
   {
	try
	{
	 cont->GetProperty(I->first,serstorage);
	}
	catch(UIProperty::EPropertyError &exception)
	{
	 ProcessException(exception);
	}

	std::string paramname=I->first;//I->second.Property->GetName();
    if(serstorage->SelectNode(paramname))
    {
     serstorage->SetNodeAttribute("PType",sntoa(I->second.Type));
     if(descr)
	 {
	  serstorage->SetNodeAttribute("Header",descr->GetDescription(paramname).Header);
     }
	 serstorage->SelectUp();
    }
   }
   ++I;
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}


// устанавливает свойства компонента по идентификатору
int UEngine::Model_SetComponentProperties(RDK::UContainer* cont, RDK::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  std::string name;

  RDK::UContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   try
   {
	cont->SetProperty(I->first,serstorage);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	ProcessException(exception);
   }
   ++I;
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}
		*/
		/*
// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
void UEngine::Model_SetGlobalComponentPropertyValue(RDK::UContainer* cont, UId classid, const char *paramname, const char *buffer)
{
 try
 {
  if(!cont || classid == ForbiddenId)
   return;

  if(cont->GetClass() == classid)
  {
   try
   {
	cont->SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	ProcessException(exception);
   }
  }

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   Model_SetGlobalComponentPropertyValue(cont->GetComponentByIndex(i), classid, paramname, buffer);
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
void UEngine::Model_SetGlobalOwnerComponentPropertyValue(RDK::UContainer* cont, UId classid, UId owner_classid, const char *paramname, const char *buffer)
{
 try
 {
  if(!cont || classid == ForbiddenId)
   return;

  if(cont->GetClass() == classid && cont->GetOwner() && cont->GetOwner()->GetClass() == owner_classid)
  {
  {
   try
   {
    cont->SetPropertyValue(paramname,buffer);
   }
   catch(UIProperty::EPropertyError &exception)
   {
	ProcessException(exception);
   }
  }
  }

  for(int i=0;i<cont->GetNumComponents();i++)
  {
   Model_SetGlobalOwnerComponentPropertyValue(cont->GetComponentByIndex(i), classid, owner_classid, paramname, buffer);
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}
	   /*
// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UEngine::Model_GetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   cont->GetLinks(linkslist, owner_level);
  else
   cont->GetLinks(linkslist, cont);


  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются до уровня компонента owner_level
// Если owner_level не задан, то имена применяются до уровня текущего компонента
int UEngine::Model_SetComponentInternalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  *serstorage>>linkslist;

  cont->BreakLinks();
  cont->CreateLinks(linkslist, owner_level);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UEngine::Model_GetComponentInputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetInputLinks(linkslist, cont);

  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UEngine::Model_GetComponentOutputLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
//  cont->GetOutputLinks(linkslist, cont);

  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возращает все внешние связи c компонентом cont и его дочерними компонентами в виде xml в буфер buffer
// Информация о связях формируется относительно владельца компонента cont!
// Имена формируются до уровня компонента owner_level
// Если owner_level не задан, то имена формируются до уровня текущего компонента
int UEngine::Model_GetComponentPersonalLinks(RDK::UNet* cont, RDK::USerStorageXML *serstorage, RDK::UNet* owner_level)
{
 try
 {
  if(!cont || !serstorage)
   return 1;

  UStringLinksList linkslist;
  if(owner_level)
   cont->GetLinks(linkslist, owner_level, true, cont);
  else
   cont->GetLinks(linkslist, cont->GetOwner(), true, cont);

  *serstorage<<linkslist;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
bool UEngine::Model_SaveComponent(RDK::UNet* cont, RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(cont->GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(ptParameter));
  if(!Model_GetComponentProperties(cont, serstorage,params_type_mask))
   return false;
  serstorage->SelectUp();

  if(links)
  {
   serstorage->AddNode("Links");
   if(Model_GetComponentInternalLinks(cont,serstorage,0))
    return false;
   serstorage->SelectUp();
  }

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!Model_SaveComponent(dynamic_pointer_cast<RDK::UNet>(cont->GetComponentByIndex(i)),serstorage,false,params_type_mask))
    return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
bool UEngine::Model_LoadComponent(RDK::UNet* cont, RDK::USerStorageXML *serstorage, bool links)
{
 try
 {
  if(!serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);

  if(!cont) // Создаем модель
  {
   Model_Create(name.c_str());
   cont=dynamic_pointer_cast<RDK::UNet>(Environment->GetModel()).Get();
  }
  else
  if(cont->GetClass() != id)
   return false;

  cont->SetName(serstorage->GetNodeName());

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(Model_SetComponentProperties(cont, serstorage))
	  return false;
	}
	catch(UException &exception)
	{
	 ProcessException(exception);
    }
    serstorage->SelectUp();
   }
  }

  cont->DelAllComponents();

  if(!serstorage->SelectNode("Components"))
   return false;
  UStorage* storage=cont->GetStorage();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   name=serstorage->GetNodeAttribute("Class");
   try
   {
    id=Storage->FindClassId(name);
	UEPtr<UNet> newcont=dynamic_pointer_cast<UNet>(storage->TakeObject(id));
	if(!newcont)
	 continue;
	if(cont->AddComponent(static_pointer_cast<UContainer>(newcont)) == ForbiddenId)
	 continue;

//   if(!Model_LoadComponent(newcont,serstorage))
	if(!Model_LoadComponent(newcont,serstorage,false))
	 return false;
   }
   catch(UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  if(links)
  {
   serstorage->SelectNode("Links");
   if(!Model_SetComponentInternalLinks(cont,serstorage,0))
	return false;
   serstorage->SelectUp();
  }
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
bool UEngine::Model_SaveComponentProperties(RDK::UNet* cont, RDK::USerStorageXML *serstorage, unsigned int type_mask)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(cont->GetClass()));
  serstorage->AddNode(UVariable::GetPropertyTypeNameByType(type_mask));
  if(!Model_GetComponentProperties(cont, serstorage,type_mask))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   try
   {
	if(!Model_SaveComponentProperties(dynamic_pointer_cast<RDK::UNet>(cont->GetComponentByIndex(i)),serstorage,type_mask))
	 return false;
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Загружает все свойства компонента и его дочерних компонент из xml
bool UEngine::Model_LoadComponentProperties(RDK::UNet* cont, RDK::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  std::string name=serstorage->GetNodeAttribute("Class");
  UId id=Storage->FindClassId(name);
  if(cont->GetClass() != id)
   return false;

  for(unsigned int i=0, mask=1;i<7;i++, mask<<=1)
  {
   if(serstorage->SelectNode(UVariable::GetPropertyTypeNameByType(mask)))
   {
	try
	{
	 if(Model_SetComponentProperties(cont, serstorage))
	  return false;
	}
	catch (UException &exception)
	{
	 ProcessException(exception);
    }
    serstorage->SelectUp();
   }
  }

  serstorage->SelectNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!serstorage->SelectNode(cont->GetComponentByIndex(i)->GetName()))
	continue;
   std::string nodename=serstorage->GetNodeName();
   try
   {
	if(!Model_LoadComponentProperties(dynamic_pointer_cast<RDK::UNet>(cont->GetComponentByIndex(i)),serstorage))
	 return false;
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
   serstorage->SelectUp();
  }
  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
}
   */    /*
// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
bool UEngine::Model_SaveComponentDrawInfo(RDK::UNet* cont, RDK::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",Storage->FindClassName(cont->GetClass()));

  serstorage->AddNode("Links");

  UStringLinksList linkslist;
//  cont->GetLinks(linkslist, cont);

  for(int i=0;i<cont->GetNumComponents();i++)
   static_pointer_cast<UNet>(cont->GetComponentByIndex(i))->GetLinks(linkslist, cont,true,cont->GetComponentByIndex(i));
  *serstorage<<linkslist;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   XmlStorage.AddNode(cont->GetComponentByIndex(i)->GetName());
   serstorage->SetNodeAttribute("Class",Storage->FindClassName(cont->GetComponentByIndex(i)->GetClass()));
   XmlStorage.AddNode("Parameters");
   try
   {
	if(!cont->GetComponentProperties(cont->GetComponentByIndex(i),serstorage,ptParameter|pgAny))
	 return false;
   }
   catch (UException &exception)
   {
	ProcessException(exception);
   }
   XmlStorage.SelectUp();
   XmlStorage.SelectUp();
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return true;
} */
// --------------------------

// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
// если выход не содержит данных такого типа, то возвращает 0
const /* RDK::MDMatrix* */void* UEngine::Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name)
{
 try
 {
  UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

  if(!cont)
   return 0;

  // Ищем указатель на выходные данные
  UIProperty* output_property=0;
//  cont->FindOutputProperty(index, output_property);
  output_property=cont->FindProperty(property_name);
  if(!output_property)
   return 0;

  if(output_property->GetLanguageType() == typeid(MDMatrix<double>) ||
     output_property->GetLanguageType() == typeid(MDVector<double>))
  {
   return output_property->GetPointer(0);
  }

  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

const /* RDK::MDMatrix* */void* UEngine::Model_GetComponentOutputAsMatrix(const char *stringid, int index)
{
 try
 {
  UEPtr<UNet> cont=dynamic_pointer_cast<UNet>(FindComponent(stringid));

  if(!cont)
   return 0;

  // Ищем указатель на выходные данные
  UIProperty* output_property=0;
  cont->FindOutputProperty(index, output_property);
  if(!output_property)
   return 0;

  if(output_property->GetLanguageType() == typeid(MDMatrix<double>) ||
     output_property->GetLanguageType() == typeid(MDVector<double>))
  {
   return output_property->GetPointer(0);
  }

  return 0;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* UEngine::Model_GetComponentOutput(const char *stringid, const char *property_name)
{
 try
 {
  if(!property_name)
   return 0;

  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
  if(!property)
   return 0;
  return (const RDK::UBitmap*)iproperty->GetMemoryArea();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
/*
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumOutputs())
   return 0;

  return cont->GetOutputs()[index];
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;   */
}

const RDK::UBitmap* UEngine::Model_GetComponentOutput(const char *stringid, int index)
{
 try {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumOutputs())
   return 0;

  UIProperty* iproperty=0;
  cont->FindOutputProperty(index, iproperty);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
  if(!property)
   return 0;
  return (const RDK::UBitmap*)property->GetMemoryArea();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* UEngine::Model_GetComponentBitmapOutput(const char *stringid, const char *property_name)
{
/*
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumOutputs())
   return 0;

  return cont->GetOutputs()[index];
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;   */
 return Model_GetComponentOutput(stringid, property_name);
}

const RDK::UBitmap* UEngine::Model_GetComponentBitmapOutput(const char *stringid, int index)
{
 try {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumOutputs())
   return 0;

  UIProperty* iproperty=0;
  cont->FindOutputProperty(index, iproperty);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
  if(!property)
   return 0;
  return (const RDK::UBitmap*)property->GetMemoryArea();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

/// Копирует данные о разрешении изображения выхода с индексом 'index' компонента 'id'
/// в стрктуру bmp_param
int UEngine::Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, RDK::UBitmapParam* bmp_param)
{
 if(!bmp_param)
  return 12011;

 const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, property_name);
 if(!temp_bmp)
  return 12012;

 bmp_param->Width=temp_bmp->GetWidth();
 bmp_param->Height=temp_bmp->GetHeight();
 bmp_param->ColorModel=temp_bmp->GetColorModel();

 return 0;
}

int UEngine::Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, RDK::UBitmapParam* bmp_param)
{
 if(!bmp_param)
  return 12011;

 const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, index);
 if(!temp_bmp)
  return 12012;

 bmp_param->Width=temp_bmp->GetWidth();
 bmp_param->Height=temp_bmp->GetHeight();
 bmp_param->ColorModel=temp_bmp->GetColorModel();

 return 0;
}

/// Копирует изображение выхода с индексом 'index' компонента 'id'
/// метод предполагает, что bmp уже имеет выделенную память под изобржение требуемого размера
int UEngine::Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, RDK::UBitmap* bmp)
{
 if(!bmp)
  return 12011;

 const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, property_name);
 if(!temp_bmp)
  return 12012;

 if(temp_bmp->GetWidth() != bmp->GetWidth() || temp_bmp->GetHeight() != bmp->GetHeight() ||
	temp_bmp->GetColorModel() != bmp->GetColorModel())
  return 12013;

 memcpy(bmp->GetData(),temp_bmp->GetData(),bmp->GetByteLength());
 return 0;
}

int UEngine::Model_CopyComponentBitmapOutput(const char *stringid, int index, RDK::UBitmap* bmp)
{
 if(!bmp)
  return 12011;

 const RDK::UBitmap *temp_bmp=Model_GetComponentBitmapOutput(stringid, index);
 if(!temp_bmp)
  return 12012;

 if(temp_bmp->GetWidth() != bmp->GetWidth() || temp_bmp->GetHeight() != bmp->GetHeight() ||
	temp_bmp->GetColorModel() != bmp->GetColorModel())
  return 12013;

 memcpy(bmp->GetData(),temp_bmp->GetData(),bmp->GetByteLength());
 return 0;
}



// Возвращает указатель на вход с индексом 'index' компонента 'id'
const RDK::UBitmap* UEngine::Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{
 try
 {
  if(!property_name)
   return 0;

  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
  if(!property)
   return 0;
  return (const RDK::UBitmap*)property->GetMemoryArea();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

 return 0;
/*
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumInputs())
   return 0;

  return cont->GetInputs()[index];
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;  */
}

const RDK::UBitmap* UEngine::Model_GetComponentBitmapInput(const char *stringid, int index)
{
 try {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumInputs())
   return 0;

  UIProperty* iproperty=0;
  cont->FindInputProperty(index, iproperty);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
  if(!property)
   return 0;
  return (const RDK::UBitmap*)property->GetMemoryArea();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Замещает изображение выхода с индексом 'index' компонента 'id'
void UEngine::Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const RDK::UBitmap* bmp, bool reflect)
{
 try{
  if(!bmp || !property_name)
   return;
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
  if(!property)
   return;

  if(reflect)
  {
   const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
   property->SetData(TempBmp);
  }
  else
   property->SetData(*bmp);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }

// Model_SetComponentPropertyData(stringid, "Output", bmp);

/*
 try{
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return;

  if(index<0 || index >= cont->GetNumOutputs())
   return;

  UBitmap *output=cont->GetOutputs()[index];

  if(!output)
   return;

  if(reflect)
   const_cast<UBitmap*>(bmp)->ReflectionX(output);
  else
   *output=*bmp;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 } */
}

void UEngine::Model_SetComponentBitmapOutput(const char *stringid, int index, const RDK::UBitmap* bmp, bool reflect)
{
 try{
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return;

  if(index<0 || index >= cont->GetNumOutputs())
   return;

  UIProperty* iproperty=0;
  cont->FindOutputProperty(index, iproperty);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
  if(!property)
   return;

  if(reflect)
  {
   const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
   property->SetData(TempBmp);
  }
  else
   property->SetData(*bmp);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Замещает изображение входа с индексом 'index' компонента 'id'
void UEngine::Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const RDK::UBitmap* const bmp, bool reflect)
{
 try{
  if(!bmp || !property_name)
   return;
  UEPtr<RDK::UContainer> cont=FindComponent(stringid);
  UEPtr<UIProperty> iproperty=cont->FindProperty(property_name);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_pointer_cast<UVBaseDataProperty<UBitmap> >(iproperty);
  if(!property)
   return;

  if(reflect)
  {
   const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
   property->SetData(TempBmp);
  }
  else
   property->SetData(*bmp);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
/*
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return;

  if(index<0 || index >= cont->GetNumInputs())
   return;

  UBitmap *input=cont->GetInputs()[index];

  if(!input)
   return;

  if(reflect)
   const_cast<UBitmap*>(bmp)->ReflectionX(input);
  else
   *input=*bmp;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }*/
}

void UEngine::Model_SetComponentBitmapInput(const char *stringid, int index, const RDK::UBitmap* const bmp, bool reflect)
{
 try {
  UEPtr<RDK::UNet> cont=dynamic_pointer_cast<RDK::UNet>(FindComponent(stringid));

  if(!cont)
   return;

  if(index<0 || index >= cont->GetNumInputs())
   return;


  UIProperty* iproperty=0;
  cont->FindInputProperty(index, iproperty);
  UEPtr<UVBaseDataProperty<UBitmap> > property=dynamic_cast<UVBaseDataProperty<UBitmap>* >(iproperty);
  if(!property)
   return;

  if(reflect)
  {
   const_cast<UBitmap*>(bmp)->ReflectionX(&TempBmp);
   property->SetData(TempBmp);
  }
  else
   property->SetData(*bmp);
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// --------------------------
// Методы управления исключениями
// --------------------------
// Обрабатывает возникшее исключение
void UEngine::ProcessException(UException &exception) const
{
/*
// if(!exception)
//  throw exception;

// USharedPtr<UException> ptr=exception;
// ExceptionsLog.push_back(ptr);

 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize > MaxExceptionsLogSize)
 {
//  ExceptionsLog.erase(ExceptionsLog.begin());
  size_t i=TempLogString.find_first_of("\n");
  if(i != string::npos)
  {
   TempLogString.erase(0,i);
  }
 }
 TempLogString+=exception.CreateLogMessage();
 TempLogString+="\r\n";

 if(ExceptionHandler)
  ExceptionHandler();
  */
 if(Environment)
  Environment->ProcessException(exception);
}


// Возвращает массив зарегистрированных исключений
/*const vector<USharedPtr<UException> > UEngine::GetExceptionsLog(void) const
{
 return ExceptionsLog;
}*/


// Возвращает массив строк лога
const char* UEngine::GetLog(int &error_level) const
{
 std::string& TempString=CreateTempString();
 TempString=Environment->GetLog(error_level);
 return TempString.c_str();
// return TempLogString.c_str();
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* UEngine::GetUnreadLog(int &error_level)
{
/*
 if(LastReadExceptionLogIndex<=0)
 {
  LastReadExceptionLogIndex=TempLogString.size();
  return TempLogString.c_str();
 }

 if(LastReadExceptionLogIndex<int(TempLogString.size()))
 {
  TempString=TempLogString.substr(LastReadExceptionLogIndex);
  LastReadExceptionLogIndex=TempLogString.size();
  return TempString.c_str();
 }

 TempString="";
 return TempString.c_str();  */
 std::string& TempString=CreateTempString();
 TempString=Environment->GetUnreadLog(error_level);
 return TempString.c_str();
}

/// Записывает в лог новое сообщение
int UEngine::Engine_LogMessage(int log_level, const char *message)
{
 if(!Environment)
  return 100001;

 Environment->LogMessage(log_level,message);
 return 0;
}


// Управление функцией-обработчиком исключений
UEnvironment::PExceptionHandler UEngine::GetExceptionHandler(void) const
{
 return Environment->GetExceptionHandler();
// return ExceptionHandler;
}

bool UEngine::SetExceptionHandler(UEnvironment::PExceptionHandler value)
{
/*
 if(ExceptionHandler == value)
  return true;

 ExceptionHandler=value;
 return true;
 */
 return Environment->SetExceptionHandler(value);
}

// Максимальное число хранимых исключений
// Если 0, то неограниченно
int UEngine::GetMaxExceptionsLogSize(void) const
{
 return Environment->GetMaxExceptionsLogSize();
// return MaxExceptionsLogSize;
}

void UEngine::SetMaxExceptionsLogSize(int value)
{
/*
 if(MaxExceptionsLogSize == value)
  return;

 MaxExceptionsLogSize=value;
 if(MaxExceptionsLogSize>0 && CurrentExceptionsLogSize>MaxExceptionsLogSize)
 {
  //ExceptionsLog.erase(ExceptionsLog.begin(), ExceptionsLog.begin()+int(ExceptionsLog.size())-MaxExceptionsLogSize);
  CurrentExceptionsLogSize=MaxExceptionsLogSize;
 }
 */
 Environment->SetMaxExceptionsLogSize(value);
}


/// Возвращает число непрочитанных строк лога
int UEngine::GetNumUnreadLogLines(void) const
{
 return Environment->GetNumUnreadLogLines();
}

/// Возвращает число строк лога
int UEngine::GetNumLogLines(void) const
{
 return Environment->GetNumLogLines();
}

/// Очищает лог прочитанных сообщений
void UEngine::ClearReadLog(void)
{
 return Environment->ClearReadLog();
}
// --------------------------



// --------------------------
// Методы внутреннего управления движком
// --------------------------

// Создает пустое хранилище и возвращает указатель на него
void UEngine::CreateStorage(void)
{
}

// Создает среду и возвращает указатель на нее.
// Если задано хранилище 'storage', то связывает его со средой.
// Если флаг 'isinit' == true, то инициализирует хранилище стандартными библиотеками
// Если указатель на массив external_libs != 0, дополнительно инициализирует хранилище этими бибилиотеками
void UEngine::CreateEnvironment(bool isinit, list<UContainer*>* external_classes, list<ULibrary*>* external_libs)
{
 if(!Storage)
  return;

 if(!Environment)
  return;

 Environment->Default();

 if(!Environment->SetStorage(Storage) || !isinit)
  return;

 if(external_classes != 0)
 {
  list<UContainer*>::iterator I,J;
  I=external_classes->begin();
  J=external_classes->end();
  while(I != J)
  {
   Storage->AddClass(*I);
   ++I;
  }
 }

 if(external_libs != 0)
 {
  list<ULibrary*>::iterator I,J;
  I=external_libs->begin();
  J=external_libs->end();
  while(I != J)
  {
   Storage->AddCollection(*I);
   ++I;
  }
 }

 Storage->BuildStorage();
}

// Загружает набор предустановленных библиотек
int UEngine::LoadPredefinedLibraries(void)
{
 RdkLoadPredefinedLibraries(LibrariesList);

 return 0;
}

// Загружает набор классов
int UEngine::LoadClasses(void)
{
 vector<string> variables;
 int res=0;
 UContainer *dllclass=0;

 Options.GetVariableList(ComponentClassesSectionName, variables);

 for(size_t i=0;i<variables.size();i++)
 {
  //dllclass=LoadUClass(Options(ComponentClassesSectionName,variables[i],""),variables[i]);
  if(!dllclass)
   res|=1;
  ClassesList.push_back(dllclass);
 }
 return res;
}

// Загружает набор библиотек и добавляет указатели на нее в массив external_libs
// возвращает 0 в случае успеха
// Список имен библиотек берется из ini-файла 'inifile', секция 'section'
int UEngine::LoadLibraries(void)
{
 vector<string> variables;
 int res=0;
 ULibrary *library=0;

 Options.GetVariableList(ComponentLibrariesSectionName, variables);

 for(size_t i=0;i<variables.size();i++)
 {
  //library=LoadULibrary(Options(ComponentLibrariesSectionName,variables[i],""),variables[i]);
  if(!library)
   res|=1;
  LibrariesList.push_back(library);
 }
 return res;
}
// --------------------------

// --------------------------
// Методы внутреннего управления консолью
// --------------------------
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет поиск компонента по длинному строковому id
// Если строковое id не задано, то возвращает указатель на модель
UEPtr<UContainer> UEngine::FindComponent(const char *stringid) const
{
/* if(stringid && !AccessCache.empty())
 {
  std::map<std::string, UEPtr<UContainer> >::iterator I=AccessCache.find(stringid);
  if(I != AccessCache.end())
   return I->second;
 }
   */
 UEPtr<RDK::UNet> model=dynamic_pointer_cast<RDK::UNet>(Environment->GetCurrentComponent());

 if(!model)
  return 0;

 RDK::ULongId longid;

 UEPtr<RDK::UContainer> cont;
 if(!stringid)
  cont=model;
 else
 {
  // Если первый символ - цифра, то декодруем как строковой id
  if(stringid[0]>=0x30 && stringid[0]<=0x39)
  {
   longid.DecodeFromString(stringid);
   if(!longid.GetSize() || longid[0] == ForbiddenId)
    cont=model;
   else
    cont=dynamic_pointer_cast<RDK::UContainer>(model->GetComponentL(longid));
  }
  else // ...иначе декодируем как имя
  {
   if(strlen(stringid) == 0)
    cont=model;
   else
    cont=dynamic_pointer_cast<RDK::UContainer>(model->GetComponentL(stringid));
  }

 }

// if(stringid)
//  AccessCache[stringid]=cont;
 return cont;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool UEngine::ADefault(void)
{
 // Имя файла инициализации
 OptionsFileName="options.ini";

 // Имя файла описаний параметров классов
 ClassesDescriptionFileName="ClassesDescription.xml";

 // Имя секции выбора основной библиотеки
// MainLibrarySectionName="MainDLL";

 // Имя переменной основной библиотеки
// MainLibraryName="MainLibraryName";
// Options(MainLibrarySectionName,MainLibraryName,string("nmsdk.dll"));

 // Имя секции выбора библиотек компонент
 ComponentLibrariesSectionName="ComponentLibraries";

 ComponentClassesSectionName="ComponentClasses";

 // Индекс используемого хранилища в библиотеке
 // Если < 0, то новое хранилище будет создано
// StorageIndex=-1;
// Options("General","StorageIndex",sntoa(StorageIndex));

 // Индекс используемой среды в библиотеке
 // Если < 0, то новая среда будет создана
// EnvironmentIndex=-1;
// Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UEngine::ABuild(void)
{
/* if(!Environment)
  return false;

 return Environment->Build();
*/
 return true;
}

// Сброс процесса счета.
bool UEngine::AReset(void)
{
/* if(!Environment)
  return false;

 return Environment->Reset(); */

 return true;
}

// Выполняет расчет этого объекта
bool UEngine::ACalculate(void)
{
/* if(!Environment)
  return false;

 return Environment->Calculate();*/
 return true;
}
// --------------------------

}
#endif
