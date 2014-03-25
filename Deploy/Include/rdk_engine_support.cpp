#ifndef RDK_ENGINE_SUPPORT_CPP
#define RDK_ENGINE_SUPPORT_CPP

#include "rdk_engine_support.h"


// Экземпляр менеджера
RDKDllManager DllManager;

RDK::UEPtr<RDK::UEngine> PEngine=0;
RDK::UEPtr<RDK::UEnvironment> PEnvironment=0;
RDK::UEPtr<RDK::UStorage> PStorage=0;

int SelectedEngineIndex=0;

std::string RdkSystemDir;

int BufObjectsMode=0;


// Менеджер DLL
// --------------------------
// Конструкторы и деструкторы
// --------------------------
RDKDllManager::RDKDllManager(void)
{
 FuncCreateNewStorage=0;//&RDK::NewUStorage;
 FuncCreateNewEnvironment=0;//&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=0;//&RDK::NewUEngine;
 GlobalMutex=UCreateMutex();
}

RDKDllManager::~RDKDllManager(void)
{
 EngineUnInit();

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

 for(size_t i=0;i<MutexList.size();i++)
  if(MutexList[i])
   UDestroyMutex(MutexList[i]);

 MutexList.resize(0);

 LockerList.resize(0);

 if(GlobalMutex)
 {
  UDestroyMutex(GlobalMutex);
  GlobalMutex=0;
 }
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

/// Возвращает число движков
int RDKDllManager::GetNumEngines(void) const
{
 return int(EngineList.size());
}

/// Создает требуемое число пустых движков
int RDKDllManager::SetNumEngines(int num)
{
 if(num<0)
  return 1;

 for(int i=num;i<int(EngineList.size());i++)
 {
  EngineDestroy(i);
  UDestroyMutex(MutexList[i]);
 }

 int old_num=EngineList.size();

 EngineList.resize(num,0);
 StorageList.resize(num,0);
 EnvironmentList.resize(num,0);
 MutexList.resize(num,0);
 LockerList.resize(num,0);
 for(int i=old_num;i<num;i++)
 {
  MutexList[i]=UCreateMutex();
 }
 return 0;
}

/// Создаает требуемый движок
/// (если движок уже инициализирован, то не делает ничего
int RDKDllManager::EngineCreate(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(EngineList[index])
  return 0;

 EngineList[index]=FuncCreateNewEngine();
 if(!EngineList[index])
 {
  EngineDestroy(index);
  return 2;
 }

 try
 {
  StorageList[index]=FuncCreateNewStorage();
  if(!StorageList[index])
  {
   EngineDestroy(index);
   return 3;
  }

  EnvironmentList[index]=FuncCreateNewEnvironment();
  if(!EnvironmentList[index])
  {
   EngineDestroy(index);
   return 4;
  }

  EngineList[index]->Default();

  EnvironmentList[index]->SetSystemDir(RdkSystemDir);
  if(!EngineList[index]->Init(StorageList[index],EnvironmentList[index]))
  {
   EngineDestroy(index);
   return 5;
  }
 }
 catch (RDK::UException &exception)
 {
  EngineList[index]->ProcessException(exception);
 }
 return 0;
}

/// Уничтожает требуемый движок
/// (если движок уже уничтожен, то не делает ничего
int RDKDllManager::EngineDestroy(int index)
{
 if(index<0 || index>=GetNumEngines())
  return 1;

 if(EngineList[index])
 {
  if(EngineList[index] == PEngine)
   PEngine=0;
  delete EngineList[index];
  EngineList[index]=0;
 }

 if(EnvironmentList[index])
 {
  if(EnvironmentList[index] == PEnvironment)
   PEnvironment=0;
  delete EnvironmentList[index];
  EnvironmentList[index]=0;
 }

 if(StorageList[index])
 {
  if(StorageList[index] == PStorage)
   PStorage=0;
  delete StorageList[index];
  StorageList[index]=0;
 }
 return 0;
}
// --------------------------

#endif
