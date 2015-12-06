#ifndef RDK_ENGINE_SUPPORT_CPP
#define RDK_ENGINE_SUPPORT_CPP

#include "rdk_engine_support.h"
#include "rdk_cpp_initdll.h"

// Экземпляр менеджера
RDKDllManager DllManager;

//RDK::UEPtr<RDK::UEngine> PEngine=0;
//RDK::UEPtr<RDK::UEnvironment> PEnvironment=0;
//RDK::UEPtr<RDK::UStorage> PStorage=0;

//int SelectedEngineIndex=0;

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
// UGenericMutexSharedLocker lock(GlobalMutex);
// return int(EngineList.size());
 return NumEngines;
}

/// Создает требуемое число пустых движков
int RDKDllManager::SetNumEngines(int num)
{
 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);

  if(num<0)
   return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

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
   #ifdef RDK_MUTEX_DEADLOCK_DEBUG
   MutexList[i]->DebugId=i;
   #endif
  }
  NumEngines=int(EngineList.size());
 }

 SetSelectedChannelIndex(num-1);

 return RDK_SUCCESS;
}


/// Добавляет новый движок в позицию index
/// Если index <0 или >= NumEngines то добавляет в конец
int RDKDllManager::Add(int index)
{
 if(index<0 || index >= GetNumEngines())
  return SetNumEngines(GetNumEngines()+1);

 int old_num=GetNumEngines();
 int num=old_num+1;

 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  EngineList.resize(num,0);
  StorageList.resize(num,0);
  EnvironmentList.resize(num,0);
  MutexList.resize(num,0);
  LockerList.resize(num,0);
 }

 for(int i=int(EngineList.size())-1;i>index;i--)
 {
  UGenericMutexExclusiveLocker lock1(MutexList[i-1]);
  EngineList[i]=EngineList[i-1];
  StorageList[i]=StorageList[i-1];
  EnvironmentList[i]=EnvironmentList[i-1];
  MutexList[i]=MutexList[i-1];
  LockerList[i]=LockerList[i-1];
 }

 EngineList[index]=0;
 StorageList[index]=0;
 EnvironmentList[index]=0;
 MutexList[index]=0;
 LockerList[index]=0;
 MutexList[index]=UCreateMutex();
 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 MutexList[index]->DebugId=index;
 #endif

 SetSelectedChannelIndex(SelectedChannelIndex);
 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  NumEngines=int(EngineList.size());
 }
 return RDK_SUCCESS;
}

/// Удаляет движок из позиции index
int RDKDllManager::Del(int index)
{
 if(index<0 || index >= GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 if(GetNumEngines() == 1)
  return RDK_E_CORE_ZERO_CHANNEL_MUST_EXIST;

 {
  UGenericMutexExclusiveLocker lock(MutexList[index]);
  EngineDestroy(index);
 }
 UDestroyMutex(MutexList[index]);
 for(int i=index+1;i<int(EngineList.size());i++)
 {
//  UGenericMutexLocker lock1(MutexList[i-1]);
  UGenericMutexExclusiveLocker lock2(MutexList[i]);
  EngineList[i-1]=EngineList[i];
  StorageList[i-1]=StorageList[i];
  EnvironmentList[i-1]=EnvironmentList[i];
  MutexList[i-1]=MutexList[i];
  LockerList[i-1]=LockerList[i];
 }

 int new_num_size=0;
 int curr_selected_channel_index=0;

 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  EngineList.resize(EngineList.size()-1);
  StorageList.resize(StorageList.size()-1);
  EnvironmentList.resize(EnvironmentList.size()-1);
  MutexList.resize(MutexList.size()-1);
  LockerList.resize(LockerList.size()-1);
  new_num_size=NumEngines=int(EngineList.size());
  curr_selected_channel_index=SelectedChannelIndex;
 }

 if(curr_selected_channel_index>=new_num_size)
  SetSelectedChannelIndex(curr_selected_channel_index-1);
 else
  SetSelectedChannelIndex(curr_selected_channel_index);
 return RDK_SUCCESS;
}

/// Создаает требуемый движок
/// (если движок уже инициализирован, то не делает ничего
int RDKDllManager::EngineCreate(int index)
{
 if(index<0 || index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 if(EngineList[index])
  return RDK_SUCCESS;

 EngineList[index]=FuncCreateNewEngine();
 if(!EngineList[index])
 {
  EngineDestroy(index);
  return RDK_E_CORE_ENGINE_CREATE_FAIL;
 }

 try
 {
  StorageList[index]=FuncCreateNewStorage();
  if(!StorageList[index])
  {
   EngineDestroy(index);
   return RDK_E_CORE_STORAGE_CREATE_FAIL;
  }

  EnvironmentList[index]=FuncCreateNewEnvironment();
  if(!EnvironmentList[index])
  {
   EngineDestroy(index);
   return RDK_E_CORE_ENVIRONMENT_CREATE_FAIL;
  }

  EngineList[index]->Default();

  EnvironmentList[index]->SetSystemDir(RdkSystemDir);
  if(!EngineList[index]->Init(StorageList[index],EnvironmentList[index]))
  {
   EngineDestroy(index);
   return RDK_E_CORE_ENGINE_INIT_FAIL;
  }

//  if(index == SelectedChannelIndex)
  {
   UGenericMutexExclusiveLocker lock(GlobalMutex);
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
 return RDK_SUCCESS;
}

/// Уничтожает требуемый движок
/// (если движок уже уничтожен, то не делает ничего
int RDKDllManager::EngineDestroy(int index)
{
 if(index<0 || index>=GetNumEngines())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

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
 return RDK_SUCCESS;
}
// --------------------------

// --------------------------
// Методы доступа к каналам
// --------------------------
/// Текущий выбраный канал
int RDKDllManager::GetSelectedChannelIndex(void) const
{
// UGenericMutexSharedLocker lock(GlobalMutex);
 return SelectedChannelIndex;
}

bool RDKDllManager::SetSelectedChannelIndex(int channel_index)
{
// UGenericMutexExclusiveLocker lock(GlobalMutex);
 if(channel_index<0 || channel_index>=GetNumEngines())
  return false;

 SelectedChannelIndex=channel_index;
// ::SelectedEngineIndex=SelectedChannelIndex;
 /// Данные текущего выбранного канала
 Engine=EngineList[channel_index];
 Environment=EnvironmentList[channel_index];
 Storage=StorageList[channel_index];

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

 RDK::UEPtr<RDK::UEnvironment> environment=EnvironmentList[engine_index];
 if(environment)
  return environment->GetModel();

 return 0;
}
// --------------------------


// --------------------------
// Методы доступа к каналам с блокировками
// --------------------------
/// Метод доступ к мьютексу
UGenericMutex* RDKDllManager::GetEngineMutex(void)
{
 if(MutexList.empty())
  return 0;
 return MutexList[0];
}

UGenericMutex* RDKDllManager::GetEngineMutex(int index)
{
 if(index<0 || index>=int(MutexList.size()))
  return 0;

 return MutexList[index];
}

// Возвращает ссылку на указатель управляющего ядра
RDK::UELockPtr<RDK::UEngine> RDKDllManager::GetEngineLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UEngine>(MutexList[SelectedChannelIndex],GetEngine());
#endif
}

RDK::UELockPtr<RDK::UEngine> RDKDllManager::GetEngineLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine(engine_index));
#else
 return RDK::UELockPtr<RDK::UEngine>(MutexList[engine_index],GetEngine(engine_index));
#endif
}

// Возвращает ссылку на указатель среды выполнения
RDK::UELockPtr<RDK::UEnvironment> RDKDllManager::GetEnvironmentLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEnvironment>(0,GetEnvironment());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UEnvironment>(MutexList[SelectedChannelIndex],GetEnvironment());
#endif
}

RDK::UELockPtr<RDK::UEnvironment> RDKDllManager::GetEnvironmentLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEnvironment>(0,GetEnvironment(engine_index));
#else
 return RDK::UELockPtr<RDK::UEnvironment>(MutexList[engine_index],GetEnvironment(engine_index));
#endif
}

// Возвращает ссылку на указатель хранилища
RDK::UELockPtr<RDK::UStorage> RDKDllManager::GetStorageLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UStorage>(0,GetStorage());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UStorage>(MutexList[SelectedChannelIndex],GetStorage());
#endif
}

RDK::UELockPtr<RDK::UStorage> RDKDllManager::GetStorageLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UStorage>(0,GetStorage(engine_index));
#else
 return RDK::UELockPtr<RDK::UStorage>(MutexList[engine_index],GetStorage(engine_index));
#endif
}

// Возвращает указатель на текущую модель
RDK::UELockPtr<RDK::UContainer> RDKDllManager::GetModelLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UContainer>(MutexList[SelectedChannelIndex],GetModel());
#endif
}

RDK::UELockPtr<RDK::UContainer> RDKDllManager::GetModelLock(int engine_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel(engine_index));
#else
 return RDK::UELockPtr<RDK::UContainer>(MutexList[engine_index],GetModel(engine_index));
#endif
}
// --------------------------


#endif
