#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include "rdk_init.h"
#include "rdk_gengine.h"
//#include "rdkdll_loader.h"

// Экземпляр менеджера
RDKDllManager DllManager;

extern RDK::UEngine* PEngine;
extern RDK::UAEnvironment* PEnvironment;
extern RDK::UAStorage* PStorage;

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


RDK::UAStorage* CreateNewStorage(void)
{
 return new RDK::UAStorage;
}

extern RDK::UAEnvironment* CreateNewEnvironment(void)
{
 return new RDK::UAEnvironment;
}

extern RDK::UEngine* CreateNewEngine(void)
{
 return new RDK::UEngine;
}

// Деинициализация движка
extern int Engine_Destroy(void);

// Инициализация движка
int Engine_Create(RDK::UEngine *engine, const char *inifilename, void *pCreateNewStorage, void *pCreateNewEnvironment)
{
 if(!engine)
  return 1;

 if(PEngine != engine)
 {
  Engine_Destroy();

  PEngine=engine;
 }

 if(inifilename)
 {
  if(!PEngine->SetOptionsFileName(inifilename))
  {
   Engine_Destroy();
   return 2;
  }

  if(!PEngine->SetFuncCreateNewStorage(reinterpret_cast<RDK::UEngine::PCreateNewStorage>(pCreateNewStorage)))
   return 10;

  if(!PEngine->SetFuncCreateNewEnvironment(reinterpret_cast<RDK::UEngine::PCreateNewEnvironment>(pCreateNewEnvironment)))
   return 11;
 }
 return 0;
}


int LoadEngine(void *create_storage, void *create_environment, void *create_engine)
{
 if(!DllInit(create_storage, create_environment, create_engine))
  return -2;

 RDK::UEngine* pengine=dynamic_cast<RDK::UEngine*>(AddNewEngine());

 if(!pengine)
  return -3;

 pengine->Default();

 if(Engine_Create(pengine, "options.ini",(void*)AddNewStorage,(void*)AddNewEnvironment))
  return -4;

 return 0;
}


int RDK_EngineInit(int predefined_structure)
{
 LoadEngine((void*)CreateNewStorage, (void*)CreateNewEnvironment, (void*)CreateNewEngine);
 RDKInit();
 RDKLoadPredefinedLibraries();

 RDKEnv_SetPredefinedStructure(predefined_structure);
 RDKEnv_CreateStructure();

 return 0;
}

RDK::UAStorage* CreateNewGStorage(void)
{
 return new RDK::UBAStorage;
}

RDK::UAEnvironment* CreateNewGEnvironment(void)
{
 return new RDK::UBAEnvironment;
}

RDK::UEngine* CreateNewGEngine(void)
{
 return new RDK::UBEngine;
}

int RDK_GraphicalEngineInit(int predefined_structure, int num_inputs,
		int num_outputs, int input_width, int input_height)
{
 LoadEngine((void*)CreateNewGStorage, (void*)CreateNewGEnvironment, (void*)CreateNewGEngine);
 RDKInit();
 RDKLoadPredefinedLibraries();

 // Задает число входов среды
 RDKEnv_SetNumInputImages(num_inputs);
 RDKEnv_SetNumOutputImages(num_outputs);

 // Задает разрешение по умолчанию (рабочее разрешение)
 for(int i=0;i<num_inputs;i++)
  RDKEnv_SetInputRes(i, input_width, input_height);

 RDKEnv_SetPredefinedStructure(predefined_structure);
 RDKEnv_CreateStructure();

 return 0;
}


#endif

