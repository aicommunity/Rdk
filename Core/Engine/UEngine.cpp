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
//#include "../../Deploy/Include/rdk_init.h"
//#include "../../Deploy/Include/rdkdll_loader.h"


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
 return Storage->GetNumClasses();
}

// Возвращает id классов в хранилище. Память должна быть выделена
void UEngine::Storage_GetClassesList(int *buffer) const
{
 Storage->GetClassIdList(buffer,Storage->GetNumClasses());
}

 // Возвращает имя класса по его id.
const char * UEngine::Storage_GetClassName(int id) const
{
 TempString=Storage->GetClassName(id);
 return TempString.c_str();
}

// Возвращает Id класса по его имени
int UEngine::Storage_GetClassId(const char *name) const
{
 return Storage->GetClassId(name);
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
bool UEngine::Storage_DelClass(int classid)
{
 return Storage->DelClass(classid);
}

// Удалаяет все свободные объекты из хранилища
void UEngine::Storage_FreeObjectsStorage(void)
{
 Storage->FreeObjectsStorage();
}

// Удаляет все объекты из хранилища
void UEngine::Storage_ClearObjectsStorage(void)
{
 Storage->ClearObjectsStorage();
}

// Вычисляет суммарное число объектов в хранилище
int UEngine::Storage_CalcNumObjects(void) const
{
 return Storage->CalcNumObjects();
}

int UEngine::Storage_CalcNumObjectsById(int classid) const
{
 return Storage->CalcNumObjects(classid);
}

int UEngine::Storage_CalcNumObjectsByName(const char* classname) const
{
 return Storage->CalcNumObjects(classname);
}


// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int UEngine::Env_GetPredefinedStructure(void) const
{
 return Environment->GetPredefinedStructure();
}

bool UEngine::Env_SetPredefinedStructure(int value)
{
 return Environment->SetPredefinedStructure(value);
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool UEngine::Env_IsStoragePresent(void) const
{
 return Environment->IsStoragePresent();
}

// Возвращает состояние инициализации
bool UEngine::Env_IsInit(void) const
{
 return Environment->IsInit();
}

// Признак наличия сформированной структуры
bool UEngine::Env_IsStructured(void) const
{
 return Environment->IsStructured();
}

// Инициализация среды
bool UEngine::Env_Init(void)
{
 return Environment->Init();
}

// Деинициализация среды
bool UEngine::Env_UnInit(void)
{
 return Environment->UnInit();
}

// Формирует предварительно заданную модель обработки
bool UEngine::Env_CreateStructure(void)
{
 return Environment->CreateStructure();
}

// Уничтожает текущую модель обработки
bool UEngine::Env_DestroyStructure(void)
{
 return Environment->DestroyStructure();
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
void UEngine::Env_Destroy(void)
{
 Environment->DestroyModel();
 Storage->ClearObjectsStorage();
 Storage->ClearClassesStorage();
 Environment->DelAllClassLibraries();
}

// Загружает библиотеку по имени dll-файла
int UEngine::Env_LoadStorageLibrary(const char *filename)
{
 return 0;
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Env_DelClassLibraryByIndex(int index)
{
 return Environment->DelClassLibrary(index);
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Env_DelClassLibraryByName(const char *name)
{
 return Environment->DelClassLibrary(name);
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UEngine::Env_DelAllClassLibraries(void)
{
 return Environment->DelAllClassLibraries();
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool UEngine::Env_BuildStorage(void)
{
 return Environment->BuildStorage();
}

// Возвращает число библиотек
int UEngine::Env_GetNumClassLibraries(void) const
{
 return Environment->GetNumClassLibraries();
}

// Возвращает имя библиотеки по индексу
const char * UEngine::Env_GetClassLibraryName(int index)
{
 TempString=Environment->GetClassLibraryName(index);
 return TempString.c_str();
}

// Возвращает версию библиотеки по индексу
const char * UEngine::Env_GetClassLibraryVersion(int index)
{
 TempString=Environment->GetClassLibraryVersion(index);
 return TempString.c_str();
}


// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
// Возвращает id нового класса в хранилище
int UEngine::Env_CreateClass(const char* stringid)
{
 return 0;
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int UEngine::Env_Calculate(const char* stringid)
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

 return 0;
}

// Методы управления моделью
// ----------------------------
// Удаляет модель
int UEngine::Model_Destroy(void)
{
 if(Environment->DestroyModel())
  return 0;

 return -1;
}

// Создает новую модель по id класса в хранилище
// Предварительно удаляет существующую модель
int UEngine::Model_Create(int classid)
{
 if(Environment->CreateModel(classid))
  return 0;

 return -1;
}

// Очищает модель
int UEngine::Model_Clear(void)
{
 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 model->DelAllComponents();
 return 0;
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр контейнера с заданным 'classid'
// если stringid - пустая строка, то добавляет в саму модель
int UEngine::Model_AddComponent(char* stringid, int classid)
{
/* if(!stringid)
  return -10;

 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* cont=Storage->TakeObject(classid);

 if(!cont)
  return -3;

 RDK::UAContainer* destcont=0;
 RDK::ULongId id;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(id,stringid));
 else
  destcont=model;
                */
 RDK::UAContainer* destcont=FindComponent(stringid);

 RDK::UAContainer* cont=Storage->TakeObject(classid);

 if(!cont)
  return -3;

 if(!destcont)
  return -4;

 return destcont->AddComponent(cont);
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр контейнера с заданным 'id'
// если stringid - пустая строка, то удаляет из самой модели
int UEngine::Model_DelComponent(char* stringid, int id)
{         /*
 if(!stringid)
  return -10;

 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
*/
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return -4;

 if(!destcont->DelComponent(id))
  return -5;

 return 0;
}

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
int UEngine::Model_GetNumComponents(char* stringid)
{      /*
 if(!stringid)
  return -10;

 RDK::UAContainer *model=dynamic_cast<RDK::UAContainer *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
         */
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return -4;

 return destcont->GetNumComponents();
}

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
int UEngine::Model_GetComponentsList(char* stringid, int *buffer)
{
/* if(!stringid)
  return -10;

 if(!buffer)
  return -20;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
*/
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return -4;

 destcont->GetComponentsList(buffer);

 return 0;
}

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* UEngine::Model_GetComponentName(char* stringid)
{
/* if(!stringid)
  return 0;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::UAContainer* destcont=0;
 RDK::ULongId longid;
 if(stringid[0] != '\0')
  destcont=model->GetComponentL(RDK::operator<<(longid,stringid));
 else
  destcont=model;
    */
 RDK::UAContainer* destcont=FindComponent(stringid);

 if(!destcont)
  return 0;

 return destcont->GetName().c_str();
}

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char* UEngine::Model_GetComponentParameters(const char *stringid)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return 0;

 XmlStorage.Create("Parameters");

 if(!Model_GetComponentParameters(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);
// strcpy(buffer,str.c_str());
 return TempString.c_str();
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char* UEngine::Model_GetComponentSelectedParameters(const char *stringid)
{
 return 0;
}

// устанавливает параметры компонента по идентификатору
bool UEngine::Model_SetComponentParameters(const char *stringid, const char* buffer)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return false;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return false;

 XmlStorage.Load(buffer,"Parameters");

 if(!Model_SetComponentParameters(cont,&XmlStorage))
  return false;

 return true;
}

// Связывает выбранные контейнеры друг с другом
int UEngine::Model_CreateLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 if(!stringid1)
  return -10;

 if(!stringid2)
  return -11;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::ULongId longid1, longid2;
 longid1<<stringid1;
 longid2<<stringid2;
 bool res=model->CreateLink(longid1,output_number,longid2,input_number);
 if(!res)
  return -3;

 return 0;
}

// Разрывает выбранную связь
int UEngine::Model_BreakLink(char* stringid1, int output_number, char* stringid2, int input_number)
{
 if(!stringid1)
  return -10;

 if(!stringid2)
  return -11;

 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 RDK::ULongId longid1, longid2;
 longid1<<stringid1;
 longid2<<stringid2;
 bool res=model->BreakLink(longid1,output_number,longid2,input_number);
 if(!res)
  return -3;

 return 0;
}

// Разрывает все связи
int UEngine::Model_BreakAllLinks(void)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return -2;

 model->BreakLinks();

 return 0;
}

// Разрывает все входные и выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentLinks(char* stringid)
{
 return 0;
}

// Разрывает все входные связи выбранного контейнера
int UEngine::Model_BreakAllComponentInputLinks(char* stringid)
{
 return 0;
}

// Разрывает все выходные связи выбранного контейнера
int UEngine::Model_BreakAllComponentOutputLinks(char* stringid)
{
 return 0;
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
const char* UEngine::Model_GetComponentInternalLinks(char* stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("Links");

 if(!Model_GetComponentInternalLinks(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);

 return TempString.c_str();
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
int UEngine::Model_SetComponentInternalLinks(char* stringid,const char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"Links");

 if(!Model_SetComponentInternalLinks(cont,&XmlStorage))
  return -4;

 return 0;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
const char * UEngine::Model_GetComponentInputLinks(char* stringid)
{
 return 0;
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
const char * UEngine::Model_GetComponentOutputLinks(char* stringid)
{
 return 0;
}

// Возвращает состояние компонента по идентификатору
const char * UEngine::Model_GetComponentState(const char *stringid)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return 0;

 XmlStorage.Create("State");

 if(!Model_GetComponentState(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);
 return TempString.c_str();
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
const char * UEngine::Model_GetComponentSelectedState(const char *stringid)
{
 return 0;
}

// Устанавливает состояние компонента по идентификатору
bool UEngine::Model_SetComponentState(const char *stringid, const char* buffer)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return false;

 RDK::ULongId id;
// id.Add(2);

// RDK::UBAbstract* cont=dynamic_cast<RDK::UBAbstract*>(model->GetComponentL(id));
 RDK::UAContainer* cont=model->GetComponentL(RDK::operator<<(id,stringid));
 if(!cont)
  return false;

 XmlStorage.Load(buffer,"State");

 if(!Model_SetComponentState(cont,&XmlStorage))
  return false;

 return true;
}

// Возвращает число входов у компонента
int UEngine::Model_GetComponentNumInputs(const char *stringid)
{
 return 0;
}

// Возвращает размер входа компонента в числе элементов
int UEngine::Model_GetComponentInputSize(const char *stringid, int index)
{
 return 0;
}

// Возвращает размер элемента входа в байтах
int UEngine::Model_GetComponentInputElementSize(const char *stringid, int index)
{
 return 0;
}

// Возвращает размер входа компонента в байтах элементов
int UEngine::Model_GetComponentInputByteSize(const char *stringid, int index)
{
 return 0;
}

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
unsigned char* UEngine::Model_GetComponentInputData(const char *stringid, int index)
{
 return 0;
}

// Возвращает число выходов у компонента
int UEngine::Model_GetComponentNumOutputs(const char *stringid)
{
 return 0;
}

// Возвращает размер выхода компонента в числе элементов
int UEngine::Model_GetComponentOutputSize(const char *stringid, int index)
{
 return 0;
}

// Возвращает размер элемента выхода в байтах
int UEngine::Model_GetComponentOutputElementSize(const char *stringid, int index)
{
 return 0;
}

// Возвращает размер выхода компонента в байтах элементов
int UEngine::Model_GetComponentOutputByteSize(const char *stringid, int index)
{
 return 0;
}

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
unsigned char* UEngine::Model_GetComponentOutputData(const char *stringid, int index)
{
 return 0;
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char *  UEngine::Model_SaveComponent(const char *stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("Save");

 if(!Model_SaveComponent(cont,&XmlStorage))
  return 0;

 std::string str;
 XmlStorage.Save(str);
// strcpy(buffer,str.c_str());

 return str.c_str();
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponent(const char *stringid, char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"Save");
 XmlStorage.SelectNode(0);

 if(!Model_LoadComponent(cont,&XmlStorage))
  return -4;

 return 0;
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * UEngine::Model_SaveComponentParameters(const char *stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("SaveParameters");

 if(!Model_SaveComponentParameters(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);

 return TempString.c_str();
}

// Загружает все параметры компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentParameters(const char *stringid, char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"SaveParameters");
 XmlStorage.SelectNode(0);

 if(!Model_LoadComponentParameters(cont,&XmlStorage))
  return -4;

 return 0;
}

// Сохраняет состояние компонента и его дочерних компонент в xml
const char * UEngine::Model_SaveComponentState(const char *stringid)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return 0;

 XmlStorage.Create("SaveState");

 if(!Model_SaveComponentState(cont,&XmlStorage))
  return 0;

 TempString="";
 XmlStorage.Save(TempString);

 return TempString.c_str();
}

// Загружает состояние компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentState(const char *stringid, char* buffer)
{
 RDK::UANet *cont=dynamic_cast<RDK::UANet *>(FindComponent(stringid));

 if(!cont)
  return -3;

 XmlStorage.Load(buffer,"SaveState");
 XmlStorage.SelectNode(0);

 if(!Model_LoadComponentState(cont,&XmlStorage))
  return -4;

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

 return true;
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentSelectedParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// устанавливает параметры компонента по идентификатору
bool UEngine::Model_SetComponentParameters(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 std::string name;
// name=XmlStorage.GetNodeName();
// if(name != cont->GetName())
//  return false;

 RDK::UAContainer::VariableMapT props=cont->GetPropertiesList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetProperty(I->second.Id,serstorage);
   ++I;
  }

 return true;
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
int UEngine::Model_GetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 ULinksList linkslist;
 cont->GetLinks(linkslist, cont);

 *serstorage<<linkslist;

 return true;
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
int UEngine::Model_SetComponentInternalLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 ULinksList linkslist;
 *serstorage>>linkslist;

 cont->BreakLinks();
 cont->CreateLinks(linkslist);

 return true;
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
int UEngine::Model_GetComponentInputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
int UEngine::Model_GetComponentOutputLinks(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
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

 return true;
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
bool UEngine::Model_GetComponentSelectedState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 return true;
}

// Устанавливает состояние компонента по идентификатору
bool UEngine::Model_SetComponentState(RDK::UAContainer* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

 std::string name;
// name=XmlStorage.GetNodeName();
// if(name != cont->GetName())
//  return false;

 RDK::UAContainer::VariableMapT props=cont->GetStateList();

  RDK::UAContainer::VariableMapCIteratorT I,J;

  I=props.begin();
  J=props.end();
  while(I != J)
  {
   cont->SetState(I->second.Id,serstorage);
   ++I;
  }

 return true;
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
int UEngine::Model_SaveComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
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
  if(!Model_SaveComponent(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
 }
 serstorage->SelectUp();

 serstorage->SelectUp();

 return true;
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int UEngine::Model_LoadComponent(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
{
 if(!cont || !serstorage)
  return false;

// serstorage->SelectNode(cont->GetName());
 UId id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
 if(cont->GetClass() != id)
  return false;

 serstorage->SelectNode("Parameters");
 if(!Model_SetComponentParameters(cont, serstorage))
  return false;
 serstorage->SelectUp();

// cont->SetName(serstorage->GetNodeName());

 cont->DelAllComponents();

 serstorage->SelectNode("Components");
 UAStorage* storage=cont->GetStorage();
 for(int i=0;i<serstorage->GetNumNodes();i++)
 {
  serstorage->SelectNode(i);
  std::string nodename=serstorage->GetNodeName();
  id=RDK::atoi(serstorage->GetNodeAttribute("Class"));
  UANet *newcont=dynamic_cast<UANet*>(storage->TakeObject(id));
  if(!newcont)
   return false;
  if(cont->AddComponent(newcont) == ForbiddenId)
   return false;

//  newcont->SetName(serstorage->GetNodeName());
//  serstorage->SelectUp();
  if(!Model_LoadComponent(newcont,serstorage))
   return false;
  serstorage->SelectUp();
 }
 serstorage->SelectUp();

 serstorage->SelectNode("Links");
 if(!Model_SetComponentInternalLinks(cont,serstorage))
  return false;
 serstorage->SelectUp();

// serstorage->SelectUp();

 return true;
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
int UEngine::Model_SaveComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
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
  if(!Model_SaveComponentParameters(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
 }
 serstorage->SelectUp();

 serstorage->SelectUp();

 return true;
}

// Загружает все параметры компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentParameters(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
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

  if(!Model_LoadComponentParameters(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
  serstorage->SelectUp();
 }
 serstorage->SelectUp();

 return true;
}

// Сохраняет состояние компонента и его дочерних компонент в xml
int UEngine::Model_SaveComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
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
  if(!Model_SaveComponentState(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
 }
 serstorage->SelectUp();

 serstorage->SelectUp();

 return true;
}

// Загружает состояние компонента и его дочерних компонент из xml
int UEngine::Model_LoadComponentState(RDK::UANet* cont, RDK::Serialize::USerStorageXML *serstorage)
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

  if(!Model_LoadComponentState(dynamic_cast<RDK::UANet*>(cont->GetComponentByIndex(i)),serstorage))
   return false;
  serstorage->SelectUp();
 }
 serstorage->SelectUp();

 return true;

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
UAContainer* UEngine::FindComponent(const char *stringid)
{
 RDK::UANet *model=dynamic_cast<RDK::UANet *>(Environment->GetModel());

 if(!model)
  return 0;

 RDK::ULongId longid;

 RDK::UAContainer *cont=0;
 if(!stringid)
  cont=model;
 else
 {
  longid<<stringid;
  if(!longid.GetSize() || longid[0] == ForbiddenId)
   cont=model;
  else
   cont=dynamic_cast<RDK::UAContainer *>(model->GetComponentL(longid));
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
