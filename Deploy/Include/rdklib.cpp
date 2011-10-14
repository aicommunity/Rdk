#ifndef RDK_LIB_CPP
#define RDK_LIB_CPP

#include "rdklib.h"
#include "rdkdll_loader.h"

// Экземпляр менеджера
RDKDllManager DllManager;

// Инициализация dll
bool DllInit(void* pfstorage,void* pfenvironment,void* pfengine)
{
 // Инициализация dll
 RDK::DLLDllInit=DllInit;

 // Указатель на функцию возвращающую число хранилищ в библиотеке
 RDK::DLLGetNumStorages=GetNumStorages;

 // Указатель на функцию возвращающую число сред в библиотеке
 RDK::DLLGetNumEnvironments=GetNumEnvironments;

 // Указатель на функцию возвращающую число движков в библиотеке
 RDK::DLLGetNumEngines=GetNumEngines;

 // Возвращает хранилище по индексу
 RDK::DLLGetStorage=GetStorage;

 // Возвращает среду по индексу
 RDK::DLLGetEnvironment=GetEnvironment;

 // Возвращает движок по индексу
 RDK::DLLGetEngine=GetEngine;

 // Создает новое хранилище и помещает в конец массива
 // Возвращает указатель на хранилище
 RDK::DLLAddNewStorage=AddNewStorage;

 // Создает новую среду и помещает в конец массива
 // Возвращает указатель на среду
 RDK::DLLAddNewEnvironment=AddNewEnvironment;

 // Создает новый движок и помещает в конец массива
 // Возвращает указатель на движок
 RDK::DLLAddNewEngine=AddNewEngine;

 return DllManager.Init(reinterpret_cast<RDKDllManager::PCreateNewStorage>(pfstorage),
                        reinterpret_cast<RDKDllManager::PCreateNewEnvironment>(pfenvironment),
                        reinterpret_cast<RDKDllManager::PCreateNewEngine>(pfengine));
}

// Возвращает число хранилищ в библиотеке
size_t GetNumStorages(void)
{
 return DllManager.StorageList.size();
}

// Возвращает число сред в библиотеке
size_t GetNumEnvironments(void)
{
 return DllManager.EnvironmentList.size();
}

// Возвращает число движков в библиотеке
size_t GetNumEngines(void)
{
 return DllManager.EngineList.size();
}

// Возвращает хранилище по индексу
RDK::UAContainerStorage* GetStorage(size_t i)
{
 if(i>=DllManager.StorageList.size())
  return 0;

 return DllManager.StorageList[i];
}

// Возвращает среду по индексу
RDK::UAContainerEnvironment* GetEnvironment(size_t i)
{
 if(i>=DllManager.EnvironmentList.size())
  return 0;

 return DllManager.EnvironmentList[i];
}

// Возвращает движок по индексу
RDK::UEngine* GetEngine(size_t i)
{
 if(i>=DllManager.EngineList.size())
  return 0;

 return DllManager.EngineList[i];
}


// Создает новое хранилище и помещает в конец массива
// Возвращает указатель на хранилище
RDK::UAContainerStorage* AddNewStorage(void)
{
 if(!DllManager.FuncCreateNewStorage)
  return 0;

 RDK::UAContainerStorage* storage=DllManager.FuncCreateNewStorage();
 if(storage)
 {
  CreateStorage(storage);
  DllManager.StorageList.push_back(storage);
 }
 return storage;
}

// Создает новую среду и помещает в конец массива
// Возвращает указатель на среду
RDK::UAContainerEnvironment* AddNewEnvironment(RDK::UAContainerStorage *storage,bool isinit, std::list<RDK::UAContainer*>* external_classes, std::list<RDK::UALibrary*>* external_libs)
{
 if(!DllManager.FuncCreateNewEnvironment)
  return 0;

 RDK::UAContainerEnvironment* environment=DllManager.FuncCreateNewEnvironment();
 if(environment)
 {
  CreateEnvironment(environment, storage,isinit,external_classes, external_libs);
  DllManager.EnvironmentList.push_back(environment);
 }
 return environment;
}

// Создает новый движок и помещает в конец массива
// Возвращает указатель на движок
RDK::UEngine* AddNewEngine(void)
{
 if(!DllManager.FuncCreateNewEngine)
  return 0;

 RDK::UEngine* engine=DllManager.FuncCreateNewEngine();
 if(engine)
 {
  DllManager.EngineList.push_back(engine);
 }
 return engine;
}


// Менеджер DLL
// --------------------------
// Конструкторы и деструкторы
// --------------------------
RDKDllManager::RDKDllManager(void)
{
 FuncCreateNewStorage=&RDK::NewUStorage;
 FuncCreateNewEnvironment=&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=&RDK::NewUEngine;
}

RDKDllManager::~RDKDllManager(void)
{
 for(size_t i=0;i<EnvironmentList.size();i++)
  if(EnvironmentList[i])
   delete EnvironmentList[i];

 EnvironmentList.resize(0);

 for(size_t i=0;i<StorageList.size();i++)
  if(StorageList[i])
   delete StorageList[i];

 StorageList.resize(0);

 for(size_t i=0;i<EngineList.size();i++)
  if(EngineList[i])
   delete EngineList[i];

 EngineList.resize(0);
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
bool RDKDllManager::Init(PCreateNewStorage fCreateNewStorage,
                            PCreateNewEnvironment fCreateNewEnvironment,
                            PCreateNewEngine fCreateNewEngine)
{
 FuncCreateNewStorage=fCreateNewStorage;
 FuncCreateNewEnvironment=fCreateNewEnvironment;
 FuncCreateNewEngine=fCreateNewEngine;

 return true;
}
// --------------------------


#endif

