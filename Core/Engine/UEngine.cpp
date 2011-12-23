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
#include "UEnvException.h"
#include "UXMLEnvSerialize.h"

namespace RDK{

// Инициализация dll
DLLPDllInit DLLDllInit=0;

// Указатель на функцию возвращающую число хранилищ в библиотеке
DLLPGetNumStorages DLLGetNumStorages=0;

// Указатель на функцию возвращающую число сред в библиотеке
DLLPGetNumEnvironments DLLGetNumEnvironments=0;

// Указатель на функцию возвращающую число движков в библиотеке
DLLPGetNumEngines DLLGetNumEngines=0;

// Возвращает хранилище по индексу
DLLPGetStorage DLLGetStorage=0;

// Возвращает среду по индексу
DLLPGetEnvironment DLLGetEnvironment=0;

// Возвращает движок по индексу
DLLPGetEngine DLLGetEngine=0;

// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
DLLPAddNewStorage DLLAddNewStorage=0;

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
DLLPAddNewEnvironment DLLAddNewEnvironment=0;

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
DLLPAddNewEngine DLLAddNewEngine=0;


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngine::UEngine(void)
{
 Runned=-1;
 Storage=0;
 Environment=0;
 FuncCreateNewStorage=0;
 FuncCreateNewEnvironment=0;
}

UEngine::~UEngine(void)
{
 Stop();
}
// --------------------------

// --------------------------
// Методы управления параметрами инициализации
// --------------------------
// Указатели на функции создания экземпляров хранилища и среды
UEngine::PCreateNewStorage UEngine::GetFuncCreateNewStorage(void) const
{
 return FuncCreateNewStorage;
}

bool UEngine::SetFuncCreateNewStorage(UEngine::PCreateNewStorage value)
{
 if(FuncCreateNewStorage == value)
  return true;

 FuncCreateNewStorage=value;
 return true;
}

UEngine::PCreateNewEnvironment UEngine::GetFuncCreateNewEnvironment(void) const
{
 return FuncCreateNewEnvironment;
}

bool UEngine::SetFuncCreateNewEnvironment(UEngine::PCreateNewEnvironment value)
{
 if(FuncCreateNewEnvironment == value)
  return true;

 FuncCreateNewEnvironment=value;
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
// Методы доступа к переменным состояния
// --------------------------
// Флаг работы системы
int UEngine::IsRunned(void)
{
 return Runned;
}
// --------------------------


// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает указатель на среду
UAContainerEnvironment* UEngine::GetEnvironment(void)
{
 return Environment;
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Инициализирует данные движка
bool UEngine::Init(void)
{
 if(!Options.LoadFromFile(OptionsFileName))
 {
  if(!Default())
   return false;
 }

 StorageIndex=atoi(Options("General","StorageIndex",sntoa(StorageIndex).c_str()));
 EnvironmentIndex=atoi(Options("General","EnvironmentIndex",sntoa(EnvironmentIndex).c_str()));
/*
 if(LoadDll(Options(MainLibrarySectionName,MainLibraryName,"").c_str()))
  return false;
  */
 if(StorageIndex>=0)
  Storage=DLLGetStorage(StorageIndex);
 else
  Storage=DLLAddNewStorage();

 if(!Storage)
  return false;

 if(EnvironmentIndex>=0)
  Environment=DLLGetEnvironment(EnvironmentIndex);
 else
 {
//  if(LoadPredefinedLibraries())
//   return false;
  if(LoadClasses())
   return false;
  if(LoadLibraries())
   return false;
  Environment=DLLAddNewEnvironment(Storage,true,&ClassesList, &LibrariesList);
 }
/*
 Storage=FuncCreateNewStorage();
 Storage=CreateStorage(Storage);
 Environment=FuncCreateNewEnvironment();
 if(LoadPredefinedLibraries())
  return false;
 if(LoadLibraries())
  return false;
 CreateEnvironment(Environment, Storage, true, &LibrariesList);

 if(!Environment)
  return false;
  */
 if(!Storage || !Environment || Environment->GetStorage() != Storage)
 {
  return false;
 }

 return true;
}

// Деинициализирует данные движка
// и сохраняет текущие настройки
bool UEngine::UnInit(void)
{
 if(!Stop())
  return false;

 Options("General","StorageIndex",sntoa(StorageIndex));
 Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

 if(!Options.SaveToFile(OptionsFileName))
  return false;

 return true;
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
 catch (UException * exception)
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
  Storage->GetClassIdList(buffer,Storage->GetNumClasses());
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
}

 // Возвращает имя класса по его id.
const char * UEngine::Storage_GetClassName(int id) const
{
 try
 {
  TempString=Storage->GetClassName(id);
 }
 catch (UException * exception)
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
  return Storage->GetClassId(name);
 }
 catch (UException * exception)
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
  return Storage->DelClass(classid);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Удалаяет все свободные объекты из хранилища
void UEngine::Storage_FreeObjectsStorage(void)
{
 try
 {
  Storage->FreeObjectsStorage();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
}

// Удаляет все объекты из хранилища
void UEngine::Storage_ClearObjectsStorage(void)
{
 try
 {
  Storage->ClearObjectsStorage();
 }
 catch (UException * exception)
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
 catch (UException * exception)
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
 catch (UException * exception)
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
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает описание класса по его id в формате xml
const char* UEngine::Storage_GetClassDescription(int classid)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Create(sntoa(classid));
  Storage->SaveClassDescription(classid,xml);
  xml.SelectUp();
  xml.Save(TempString);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Устанавливает описание класса по его id, считывая его из формата xml
bool UEngine::Storage_SetClassDescription(int classid, const char* description)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Load(description, sntoa(classid));
  return Storage->LoadClassDescription(classid,xml);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Сохраняет описание всех классов в xml
const char* UEngine::Storage_SaveClassesDescription(void)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Create("ClassesDescription");
  Storage->SaveClassesDescription(xml);
  xml.SelectUp();
  xml.Save(TempString);
 }
 catch (UException * exception)
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
  Serialize::USerStorageXML xml;
  xml.Load(xmltext, "ClassesDescription");
  return Storage->LoadClassesDescription(xml);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Сохраняет общее описание всех классов в xml
const char* UEngine::Storage_SaveCommonClassesDescription(void)
{
 try
 {
  Serialize::USerStorageXML xml;
  xml.Create("ClassesDescription");
  Storage->SaveCommonClassesDescription(xml);
  xml.SelectUp();
  xml.Save(TempString);
 }
 catch (UException * exception)
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
  Serialize::USerStorageXML xml;
  xml.Load(xmltext, "ClassesDescription");
  return Storage->LoadCommonClassesDescription(xml);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}


// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int UEngine::Env_GetPredefinedStructure(void) const
{
 try
 {
  return Environment->GetPredefinedStructure();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

bool UEngine::Env_SetPredefinedStructure(int value)
{
 try
 {
  return Environment->SetPredefinedStructure(value);
 }
 catch (UException * exception)
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
 catch (UException * exception)
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
 catch (UException * exception)
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
 catch (UException * exception)
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
  return Environment->Init();
 }
 catch (UException * exception)
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
  return Environment->UnInit();
 }
 catch (UException * exception)
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
  return Environment->CreateStructure();
 }
 catch (UException * exception)
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
  return Environment->DestroyStructure();
 }
 catch (UException * exception)
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
  Environment->DestroyModel();
  Storage->ClearObjectsStorage();
  Storage->ClearClassesStorage();
  Environment->DelAllClassLibraries();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
}

// Загружает библиотеку по имени dll-файла
int UEngine::Env_LoadStorageLibrary(const char *filename)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Env_DelClassLibraryByIndex(int index)
{
 try
 {
  return Environment->DelClassLibrary(index);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Env_DelClassLibraryByName(const char *name)
{
 try
 {
  return Environment->DelClassLibrary(name);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Env_DelAllClassLibraries(void)
{
 try
 {
  return Environment->DelAllClassLibraries();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool UEngine::Env_BuildStorage(void)
{
 try
 {
  return Environment->BuildStorage();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return false;
}

// Возвращает число библиотек
int UEngine::Env_GetNumClassLibraries(void) const
{
 try
 {
  return Environment->GetNumClassLibraries();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает имя библиотеки по индексу
const char * UEngine::Env_GetClassLibraryName(int index)
{
 try
 {
  TempString=Environment->GetClassLibraryName(index);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает версию библиотеки по индексу
const char * UEngine::Env_GetClassLibraryVersion(int index)
{
 try
 {
  TempString=Environment->GetClassLibraryVersion(index);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}


// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
int UEngine::Env_CreateClass(const char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
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
   RDK::operator<<(id,stringid);
   Environment->SetModelCalculationComponent(id);
  }

  if(!Environment->Calculate())
   return 1;
 }
 catch (UException * exception)
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
  if(Environment->DestroyModel())
   return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return -1;
}

// Создает новую модель по id класса в хранилище
// Предварительно удаляет существующую модель
int UEngine::Model_Create(int classid)
{
 try
 {
  if(Environment->CreateModel(classid))
   return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return -1;
}

// Очищает модель
int UEngine::Model_Clear(void)
{
 try
 {
  UEPtr<RDK::UAContainer> model=dynamic_pointer_cast<RDK::UAContainer>(Environment->GetModel());

  if(!model)
   return -2;

  model->DelAllComponents();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
int UEngine::Model_AddComponent(char* stringid, int classid)
{
 try
 {
  UEPtr<RDK::UAContainer> destcont=FindComponent(stringid);

  UEPtr<RDK::UAContainer> cont=dynamic_pointer_cast<RDK::UAContainer>(Storage->TakeObject(classid));

  if(!cont)
   return -3;

  if(!destcont)
   return -4;

  return destcont->AddComponent(cont);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
int UEngine::Model_DelComponent(char* stringid, int id)
{
 try
 {
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  if(!destcont->DelComponent(id))
   return -5;
 }
 catch (UException * exception)
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
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  return destcont->GetNumComponents();
 }
 catch (UException * exception)
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
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return -4;

  destcont->GetComponentsList(buffer);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* UEngine::Model_GetComponentName(const char* stringid)
{
 try
 {
  RDK::UAContainer* destcont=FindComponent(stringid);

  if(!destcont)
   return 0;

  return destcont->GetName().c_str();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает параметры компонента по идентификатору
const char* UEngine::Model_GetComponentParameters(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return 0;

  RDK::ULongId id;
  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(RDK::operator<<(id,stringid));
  if(!cont)
   return 0;

  XmlStorage.Create(cont->GetLongName(model,namebuffer));
  XmlStorage.AddNode("Parameters");

  if(!Model_GetComponentParameters(cont,&XmlStorage))
   return 0;

  XmlStorage.SelectUp();
  TempString="";
  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает выборочные параметры компонента по идентификатору
const char* UEngine::Model_GetComponentSelectedParameters(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает параметры компонента по идентификатору с описаниями
const char* UEngine::Model_GetComponentParametersEx(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return 0;

  RDK::ULongId id;
  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(RDK::operator<<(id,stringid));
  if(!cont)
   return 0;

  XmlStorage.Create(cont->GetLongName(model,namebuffer));
  XmlStorage.AddNode("Parameters");

  if(!Model_GetComponentParametersEx(cont,&XmlStorage))
   return 0;

  XmlStorage.SelectUp();
  TempString="";
  XmlStorage.Save(TempString);
  return TempString.c_str();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// устанавливает параметры компонента по идентификатору
bool UEngine::Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return false;

  RDK::ULongId id;
  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(RDK::operator<<(id,stringid));
  if(!cont)
   return false;

  XmlStorage.Load(buffer, cont->GetLongName(model,namebuffer));
  XmlStorage.SelectNode("Parameters");

  if(!Model_SetComponentParameters(cont,&XmlStorage))
   return false;
  XmlStorage.SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Связывает выбранные контейнеры друг с другом
int UEngine::Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  longid1<<stringid1;
  longid2<<stringid2;
  bool res=model->CreateLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Разрывает выбранную связь
int UEngine::Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 try
 {
  if(!stringid1)
   return -10;

  if(!stringid2)
   return -11;

  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return -2;

  RDK::ULongId longid1, longid2;
  longid1<<stringid1;
  longid2<<stringid2;
  bool res=model->BreakLink(longid1,output_number,longid2,input_number);
  if(!res)
   return -3;
 }
 catch (UException * exception)
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
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return -2;

  model->BreakLinks();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Разрывает все входные и выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Разрывает все входные связи выбранного контейнера
int UEngine::Model_BreakAllComponentInputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Разрывает все выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentOutputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
const char* UEngine::Model_GetComponentInternalLinks(char* stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("Links");

  if(!Model_GetComponentInternalLinks(cont,&XmlStorage))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
int UEngine::Model_SetComponentInternalLinks(char* stringid,const char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"Links");

  if(!Model_SetComponentInternalLinks(cont,&XmlStorage))
   return -4;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
const char * UEngine::Model_GetComponentInputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
const char * UEngine::Model_GetComponentOutputLinks(char* stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает состояние компонента по идентификатору
const char * UEngine::Model_GetComponentState(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return 0;

  RDK::ULongId id;
  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(RDK::operator<<(id,stringid));
  if(!cont)
   return 0;

  XmlStorage.Create(cont->GetLongName(model,namebuffer));
  XmlStorage.AddNode("State");

  if(!Model_GetComponentState(cont,&XmlStorage))
   return 0;
  XmlStorage.SelectUp();

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
const char * UEngine::Model_GetComponentSelectedState(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return 0;
}

// Устанавливает состояние компонента по идентификатору
bool UEngine::Model_SetComponentState(const char *stringid, const char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

  if(!model)
   return false;

  RDK::ULongId id;
  string namebuffer;

  UEPtr<RDK::UAContainer> cont=model->GetComponentL(RDK::operator<<(id,stringid));
  if(!cont)
   return false;

  XmlStorage.Load(buffer, cont->GetLongName(model,namebuffer));
  XmlStorage.SelectNode("State");

  if(!Model_SetComponentState(cont,&XmlStorage))
   return false;
  XmlStorage.SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// Возвращает число входов у компонента
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер входа компонента в числе элементов
int UEngine::Model_GetComponentInputSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
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
 catch (UException * exception)
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
 catch (UException * exception)
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
 catch (UException * exception)
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
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает размер выхода компонента в числе элементов
int UEngine::Model_GetComponentOutputSize(const char *stringid, int index)
{
 try
 {
  return 0;
 }
 catch (UException * exception)
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
  return 0;
 }
 catch (UException * exception)
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
 catch (UException * exception)
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
  return 0;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char *  UEngine::Model_SaveComponent(const char *stringid)
{
 std::string str;
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("Save");

  if(!Model_SaveComponent(cont,&XmlStorage))
   return 0;

  XmlStorage.Save(str);
 // strcpy(buffer,str.c_str());
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return str.c_str();
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponent(const char *stringid, char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"Save");
  XmlStorage.SelectNode(0);

  if(!Model_LoadComponent(cont,&XmlStorage))
   return -4;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * UEngine::Model_SaveComponentParameters(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("SaveParameters");

  if(!Model_SaveComponentParameters(cont,&XmlStorage))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Загружает все параметры компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"SaveParameters");
  XmlStorage.SelectNode(0);

  if(!Model_LoadComponentParameters(cont,&XmlStorage))
   return -4;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Сохраняет состояние компонента и его дочерних компонент в xml
const char * UEngine::Model_SaveComponentState(const char *stringid)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return 0;

  XmlStorage.Create("SaveState");

  if(!Model_SaveComponentState(cont,&XmlStorage))
   return 0;

  TempString="";
  XmlStorage.Save(TempString);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return TempString.c_str();
}

// Загружает состояние компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentState(const char *stringid, char* buffer)
{
 try
 {
  UEPtr<RDK::UANet> cont=dynamic_pointer_cast<RDK::UANet>(FindComponent(stringid));

  if(!cont)
   return -3;

  XmlStorage.Load(buffer,"SaveState");
  XmlStorage.SelectNode(0);

  if(!Model_LoadComponentState(cont,&XmlStorage))
   return -4;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return 0;
}
// --------------------------

// --------------------------
// Скрытые методы управления средой
// --------------------------
// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->GetProperty(I->second.Id,serstorage);
   ++I;
  }
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentSelectedParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возвращает параметры компонента по идентификатору с описаниями
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentParametersEx(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  UEPtr<UContainerDescription> descr=dynamic_pointer_cast<UContainerDescription>(Storage->GetClassDescription(cont->GetClass()));

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->GetProperty(I->second.Id,serstorage);

   if(descr)
   {
	std::string paramname=I->second.Property->GetName();
	if(serstorage->SelectNode(paramname))
	{
	 serstorage->SetNodeAttribute("Header",descr->GetParameter(paramname).Header);
	 serstorage->SelectUp();
	}
   }
   ++I;
  }
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}


// устанавливает параметры компонента по идентификатору
bool UEngine::Model_SetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  std::string name;

  RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetProperty(I->second.Id,serstorage);
   ++I;
  }
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
int UEngine::Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  ULinksList linkslist;
  cont->GetLinks(linkslist, cont);

  *serstorage<<linkslist;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }
 return true;
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
int UEngine::Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  ULinksList linkslist;
  *serstorage>>linkslist;

  cont->BreakLinks();
  cont->CreateLinks(linkslist);
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
int UEngine::Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
int UEngine::Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  RDK::UAContainer::VariableMapT props=cont->GetStateList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->GetState(I->second.Id,serstorage);
   ++I;
  }
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentSelectedState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Устанавливает состояние компонента по идентификатору
bool UEngine::Model_SetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  std::string name;

  RDK::UAContainer::VariableMapT props=cont->GetStateList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetState(I->second.Id,serstorage);
   ++I;
  }
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
int UEngine::Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",RDK::sntoa(cont->GetClass()));
  serstorage->AddNode("Parameters");
  if(!Model_GetComponentParameters(cont, serstorage))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Links");
  if(!Model_GetComponentInternalLinks(cont,serstorage))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!Model_SaveComponent(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
	return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
  if(cont->GetClass() != id)
   return false;

  serstorage->SelectNode("Parameters");
  if(!Model_SetComponentParameters(cont, serstorage))
   return false;
  serstorage->SelectUp();

  cont->DelAllComponents();

  serstorage->SelectNode("Components");
  UAStorage* storage=cont->GetStorage();
  for(int i=0;i<serstorage->GetNumNodes();i++)
  {
   serstorage->SelectNode(i);
   std::string nodename=serstorage->GetNodeName();
   id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
   UEPtr<UANet> newcont=dynamic_pointer_cast<UANet>(storage->TakeObject(id));
   if(!newcont)
	return false;
   if(cont->AddComponent(static_pointer_cast<UAContainer>(newcont)) == ForbiddenId)
	return false;

   if(!Model_LoadComponent(newcont,serstorage))
	return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();

  serstorage->SelectNode("Links");
  if(!Model_SetComponentInternalLinks(cont,serstorage))
   return false;
  serstorage->SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
int UEngine::Model_SaveComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",RDK::sntoa(cont->GetClass()));
  serstorage->AddNode("Parameters");
  if(!Model_GetComponentParameters(cont, serstorage))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!Model_SaveComponentParameters(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
	return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Загружает все параметры компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
  if(cont->GetClass() != id)
   return false;

  serstorage->SelectNode("Parameters");
  if(!Model_SetComponentParameters(cont, serstorage))
   return false;
  serstorage->SelectUp();

  serstorage->SelectNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!serstorage->SelectNode(cont->GetComponentByIndex(i)->GetName()))
	continue;
   std::string nodename=serstorage->GetNodeName();

   if(!Model_LoadComponentParameters(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
	return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Сохраняет состояние компонента и его дочерних компонент в xml
int UEngine::Model_SaveComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  serstorage->AddNode(cont->GetName());
  serstorage->SetNodeAttribute("Class",RDK::sntoa(cont->GetClass()));
  serstorage->AddNode("State");
  if(!Model_GetComponentState(cont, serstorage))
   return false;
  serstorage->SelectUp();

  serstorage->AddNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!Model_SaveComponentState(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
	return false;
  }
  serstorage->SelectUp();

  serstorage->SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}

// Загружает состояние компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 try
 {
  if(!cont || !serstorage)
   return false;

  UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
  if(cont->GetClass() != id)
   return false;

  serstorage->SelectNode("State");
  if(!Model_SetComponentState(cont, serstorage))
   return false;
  serstorage->SelectUp();

  serstorage->SelectNode("Components");
  for(int i=0;i<cont->GetNumComponents();i++)
  {
   if(!serstorage->SelectNode(cont->GetComponentByIndex(i)->GetName()))
	continue;
   std::string nodename=serstorage->GetNodeName();

   if(!Model_LoadComponentState(dynamic_pointer_cast<RDK::UANet>(cont->GetComponentByIndex(i)),serstorage))
	return false;
   serstorage->SelectUp();
  }
  serstorage->SelectUp();
 }
 catch (UException * exception)
 {
  ProcessException(exception);
 }

 return true;
}
// --------------------------

// --------------------------
// Методы управления исключениями
// --------------------------
// Обрабатывает возникшее исключение
void UEngine::ProcessException(UException *exception) const
{
 if(!exception)
  throw exception;

 USharedPtr<UException> ptr=exception;
 ExceptionsLog.push_back(ptr);

 TempLogString+=CreateLogMessage(exception);
 TempLogString+="\r\n";

// if(exception->GetType() == 1)
//  throw exception;
}

// Формирует строку лога об исключении
string UEngine::CreateLogMessage(UException *exception) const
{
 string result;

 if(!exception)
  return result;

 result+=sntoa(exception->GetNumber());
 result+=" ";
 result+=sntoa(exception->GetTime());
 result+=" ";
 result+=sntoa(exception->GetType());
 result+="> ";
 result+=typeid(exception).name();

 UIContainerException *iexception=dynamic_cast<UIContainerException*>(exception);

 if(iexception)
 {
  // Короткое имя компонента в котором сгенерировано исключение
  result+=" Name=";
  result+=iexception->Name;

  // Короткий идентификатор компонента в котором сгенерировано исключение
//  result+=" Id=";
//  result+=iexception->Id;

  // Полное имя владельца компонента в котором сгенерировано исключение
  result+=" OwnerName=";
  result+=iexception->OwnerName;

  // Полный идентификатор владельца компонента в котором сгенерировано исключение
//  result+=" OwnerId=";
//  result+=iexception->OwnerId;

  if(iexception->MainOwnerName != iexception->OwnerName)
  {
   // Полное имя главного владельца компонента в котором сгенерировано исключение
   result+=" MainOwnerName=";
   result+=iexception->MainOwnerName;
  }

  // Полный идентификатор главного владельца компонента в котором сгенерировано исключение
//  result+=" MainOwnerId=";
//  result+=iexception->MainOwnerId;
 }


 return result;
}

// Возвращает массив зарегистрированных исключений
const vector<USharedPtr<UException> > UEngine::GetExceptionsLog(void) const
{
 return ExceptionsLog;
}


// Возвращает массив строк лога
const char* UEngine::GetLog(void) const
{
 return TempLogString.c_str();
}

// --------------------------



// --------------------------
// Методы внутреннего управления движком
// --------------------------
// Загружает набор предустановленных библиотек
int UEngine::LoadPredefinedLibraries(void)
{
 return 0;
}

// Загружает набор классов
int UEngine::LoadClasses(void)
{
 vector<string> variables;
 int res=0;
 UAContainer *dllclass=0;

 Options.GetVariableList(ComponentClassesSectionName, variables);

 ClassesList.clear();
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
 UALibrary *library=0;

 Options.GetVariableList(ComponentLibrariesSectionName, variables);

 LibrariesList.clear();
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
UEPtr<UAContainer> UEngine::FindComponent(const char *stringid)
{
 UEPtr<RDK::UANet> model=dynamic_pointer_cast<RDK::UANet>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::ULongId longid;

 UEPtr<RDK::UAContainer> cont;
 if(!stringid)
  cont=model;
 else
 {
  longid<<stringid;
  if(!longid.GetSize() || longid[0] == ForbiddenId)
   cont=model;
  else
   cont=dynamic_pointer_cast<RDK::UAContainer>(model->GetComponentL(longid));
 }

 return cont;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool UEngine::ADefault(void)
{
 // Имя файла инициализации
 OptionsFileName="options.ini";

 // Имя секции выбора основной библиотеки
 MainLibrarySectionName="MainDLL";

 // Имя переменной основной библиотеки
 MainLibraryName="MainLibraryName";
 Options(MainLibrarySectionName,MainLibraryName,string("nmsdk.dll"));

 // Имя секции выбора библиотек компонент
 ComponentLibrariesSectionName="ComponentLibraries";

 ComponentClassesSectionName="ComponentClasses";

 // Индекс используемого хранилища в библиотеке
 // Если < 0, то новое хранилище будет создано
 StorageIndex=-1;
 Options("General","StorageIndex",sntoa(StorageIndex));

 // Индекс используемой среды в библиотеке
 // Если < 0, то новая среда будет создана
 EnvironmentIndex=-1;
 Options("General","EnvironmentIndex",sntoa(EnvironmentIndex));

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
