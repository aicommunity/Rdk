#ifndef RDK_ENGINE_SUPPORT_CPP
#define RDK_ENGINE_SUPPORT_CPP

#include "rdk_engine_support.h"


// Экземпляр менеджера
RDKDllManager DllManager;

//RDK::UEPtr<RDK::UEngine> PEngine=0;
//RDK::UEPtr<RDK::UEnvironment> PEnvironment=0;
//RDK::UEPtr<RDK::UStorage> PStorage=0;

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
 SelectedChannelIndex=0;
 Engine=0;
 Environment=0;
 Storage=0;
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
// Методы управления созданием каналов
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

 if(SelectedChannelIndex<=int(EngineList.size()))
  SetSelectedChannelIndex(int(EngineList.size()-1));

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

  if(index == SelectedChannelIndex)
  {
   /// Данные текущего выбранного канала
   Engine=EngineList[SelectedChannelIndex];
   Environment=EnvironmentList[SelectedChannelIndex];
   Storage=StorageList[SelectedChannelIndex];
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
  if(EngineList[index] == Engine)
   Engine=0;
  delete EngineList[index];
  EngineList[index]=0;
 }

 if(EnvironmentList[index])
 {
  if(EnvironmentList[index] == Environment)
   Environment=0;
  delete EnvironmentList[index];
  EnvironmentList[index]=0;
 }

 if(StorageList[index])
 {
  if(StorageList[index] == Storage)
   Storage=0;
  delete StorageList[index];
  StorageList[index]=0;
 }
 return 0;
}
// --------------------------

// --------------------------
// Методы доступа к каналам
// --------------------------
/// Текущий выбраный канал
int RDKDllManager::GetSelectedChannelIndex(void) const
{
 return SelectedChannelIndex;
}

bool RDKDllManager::SetSelectedChannelIndex(int channel_index)
{
 if(channel_index<0 || channel_index>=int(EngineList.size()))
  return false;

 SelectedChannelIndex=channel_index;
 /// Данные текущего выбранного канала
 Engine=EngineList[SelectedChannelIndex];
 Environment=EnvironmentList[SelectedChannelIndex];
 Storage=StorageList[SelectedChannelIndex];

 return true;
}

// Возвращает ссылку на указатель управляющего ядра
RDK::UEPtr<RDK::UEngine>& RDKDllManager::GetEngine(void)
{
 return Engine;
}

RDK::UEPtr<RDK::UEngine> RDKDllManager::GetEngine(int engine_index)
{
 return EngineList[engine_index];
}

// Возвращает ссылку на указатель среды выполнения
RDK::UEPtr<RDK::UEnvironment>& RDKDllManager::GetEnvironment(void)
{
 return Environment;
}

RDK::UEPtr<RDK::UEnvironment> RDKDllManager::GetEnvironment(int engine_index)
{
 return EnvironmentList[engine_index];
}

// Возвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UStorage>& RDKDllManager::GetStorage(void)
{
 return Storage;
}

RDK::UEPtr<RDK::UStorage> RDKDllManager::GetStorage(int engine_index)
{
 return StorageList[engine_index];
}

// Возвращает указатель на текущую модель
RDK::UEPtr<RDK::UContainer> RDKDllManager::GetModel(void)
{
 if(Environment)
  return Environment->GetModel();

 return 0;
}

RDK::UEPtr<RDK::UContainer> RDKDllManager::GetModel(int engine_index)
{
 if(engine_index<0 || engine_index>=int(EnvironmentList.size()))
  return 0;

 UEPtr<UEnvironment> environment=EnvironmentList[engine_index];
 if(environment)
  return environment->GetModel();

 return 0;
}
// --------------------------


// --------------------------
// Методы доступа к каналам с блокировками
// --------------------------
// Возвращает ссылку на указатель управляющего ядра
RDK::UELockPtr<RDK::UEngine> RDKDllManager::GetEngineLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UEngine>(0,GetEngine());
#else
 return UELockPtr<RDK::UEngine>(MutexList[SelectedChannelIndex],GetEngine());
#endif
}

RDK::UELockPtr<RDK::UEngine> RDKDllManager::GetEngineLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UEngine>(0,GetEngine(engine_index));
#else
 return UELockPtr<RDK::UEngine>(MutexList[engine_index],GetEngine(engine_index));
#endif
}

// Возвращает ссылку на указатель среды выполнения
RDK::UELockPtr<RDK::UEnvironment> RDKDllManager::GetEnvironmentLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UEnvironment>(0,GetEnvironment());
#else
 return UELockPtr<RDK::UEnvironment>(MutexList[SelectedChannelIndex],GetEnvironment());
#endif
}

RDK::UELockPtr<RDK::UEnvironment> RDKDllManager::GetEnvironmentLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UEnvironment>(0,GetEnvironment(engine_index));
#else
 return UELockPtr<RDK::UEnvironment>(MutexList[engine_index],GetEnvironment(engine_index));
#endif
}

// Возвращает ссылку на указатель хранилища
RDK::UELockPtr<RDK::UStorage> RDKDllManager::GetStorageLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UStorage>(0,GetStorage());
#else
 return UELockPtr<RDK::UStorage>(MutexList[SelectedChannelIndex],GetStorage());
#endif
}

RDK::UELockPtr<RDK::UStorage> RDKDllManager::GetStorageLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UStorage>(0,GetStorage(engine_index));
#else
 return UELockPtr<RDK::UStorage>(MutexList[engine_index],GetStorage(engine_index));
#endif
}

// Возвращает указатель на текущую модель
RDK::UELockPtr<RDK::UContainer> RDKDllManager::GetModelLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UContainer>(0,GetModel());
#else
 return UELockPtr<RDK::UContainer>(MutexList[SelectedChannelIndex],GetModel());
#endif
}

RDK::UELockPtr<RDK::UContainer> RDKDllManager::GetModelLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return UELockPtr<RDK::UContainer>(0,GetModel(engine_index));
#else
 return UELockPtr<RDK::UContainer>(MutexList[engine_index],GetModel(engine_index));
#endif
}
// --------------------------


#endif
