#ifndef RDK_ENGINE_SUPPORT_CPP
#define RDK_ENGINE_SUPPORT_CPP

#include "rdk_engine_support.h"
#include "rdk_cpp_initdll.h"
#include "rdk_error_codes.h"
#include "rdk_exceptions.h"

// Экземпляр менеджера
URdkCoreManager RdkCoreManager;

// Менеджер DLL
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URdkCoreManager::URdkCoreManager(void)
{
 FuncCreateNewStorage=0;//&RDK::NewUStorage;
 FuncCreateNewEnvironment=0;//&RDK::NewUAContainerEnvironment;
 FuncCreateNewEngine=0;//&RDK::NewUEngine;
 GlobalMutex=UCreateMutex();
 SelectedChannelIndex=0;
 Engine=0;
 Environment=0;
 Storage=0;
 BufObjectsMode=0;
 GlobalLogger.SetChannelIndex(RDK_SYS_MESSAGE);
}

URdkCoreManager::~URdkCoreManager(void)
{
 for(int i=0;i<NumChannels;i++)
  MCore_ChannelUnInit(i);

 RDK_SYS_TRY
 {
  try
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

   for(size_t i=0;i<LoggerList.size();i++)
	if(LoggerList[i])
	 delete LoggerList[i];
   LoggerList.resize(0);

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
  catch (RDK::UException &exception)
  {
   GlobalLogger.ProcessException(exception);
  }
  catch (std::exception &exception)
  {
   GlobalLogger.ProcessException(RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  GlobalLogger.ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Возвращает имя каталога бинарных файлов
std::string URdkCoreManager::GetSystemDir(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return SystemDir;
}

// Устанавливает имя каталога бинарных файлов
int URdkCoreManager::SetSystemDir(const char *dir)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 SystemDir=dir;
 return RDK_SUCCESS;
}

// Возвращает имя каталога логов
const char* URdkCoreManager::GetLogDir(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return LogDir.c_str();
}

// Устанавливает имя каталога логов
int URdkCoreManager::SetLogDir(const char *dir)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 if(LogDir == dir)
  return RDK_SUCCESS;
 LogDir=dir;
 for(size_t i=0;i<LoggerList.size();i++)
 {
  if(LoggerList[i])
   LoggerList[i]->SetLogDir(LogDir);
 }
 GlobalLogger.SetLogDir(LogDir);
 return RDK_SUCCESS;
}

/// Флаг режима отладки
bool URdkCoreManager::GetDebugMode(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return DebugMode;
}

int URdkCoreManager::SetDebugMode(bool value)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 if(DebugMode == value)
  return RDK_SUCCESS;
 DebugMode=value;
 for(size_t i=0;i<LoggerList.size();i++)
 {
  if(LoggerList[i])
   LoggerList[i]->SetDebugMode(DebugMode);
 }
 GlobalLogger.SetDebugMode(DebugMode);
 return RDK_SUCCESS;
}

int URdkCoreManager::GetBufObjectsMode(void)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 return BufObjectsMode;
}

int URdkCoreManager::SetBufObjectsMode(int value)
{
 UGenericMutexExclusiveLocker lock(GlobalMutex);
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(BufObjectsMode == value)
	return RDK_SUCCESS;

   for(size_t i=0;i<EngineList.size();i++)
   {
	GetEngineLock(i)->SetBufObjectsMode(value);
   }
   BufObjectsMode=value;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   GlobalLogger.ProcessException(exception);
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   GlobalLogger.ProcessException(RDK::UExceptionWrapperStd(exception));
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  GlobalLogger.ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

// Очищает коллекцию глобальных шрифтов
int URdkCoreManager::ClearFonts(void)
{
 Fonts.DelAllFonts();
 return RDK_SUCCESS;
}

// Загружает глобальные шрифты
int RDK_CALL URdkCoreManager::LoadFonts(void)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   // Грузим шрифты
   std::vector<std::string> font_names;
   std::string font_path=SystemDir+"Fonts/";
   RDK::FindFilesList(font_path, "*.fnt", true, font_names);
   GlobalLogger.LogMessage(RDK_EX_DEBUG, std::string("Loading fonts form ")+font_path+"\n");

   ClearFonts();
   RDK::UBitmapFont font;
   for(size_t i=0;i<font_names.size();i++)
   {
	AddFont(font_path+font_names[i]);
    GlobalLogger.LogMessage(RDK_EX_DEBUG, std::string("Loaded font ")+font_names[i]+"\n");
   }
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   GlobalLogger.ProcessException(exception);
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   GlobalLogger.ProcessException(RDK::UExceptionWrapperStd(exception));
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  GlobalLogger.ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

// Загружает новый глобальный шрифт
bool URdkCoreManager::AddFont(const std::string &font_file_name)
{
 int res=RDK_SUCCESS;
 RDK_SYS_TRY
 {
  try
  {
   RDK::UBitmapFont font;
   std::size_t dir_sep_pos=font_file_name.find_last_of("\\/");
   std::string font_name;
   if(dir_sep_pos != std::string::npos)
	font_name=font_file_name.substr(dir_sep_pos+1);
   else
	font_name=font_file_name;
   std::size_t _pos=font_name.find_first_of("_");
   std::size_t _pos2=font_name.find_first_not_of("0123456789",_pos+1);
   if(_pos != std::string::npos)
   {
	std::string font_string_size=font_name.substr(_pos+1,_pos2-_pos-1);
	int size=RDK::atoi(font_string_size);
	if(!font.LoadFromFile(font_name.substr(0,_pos),font_file_name,size))
	 return false;
	return Fonts.AddFont(font.GetName(),size,font);
   }
  }
  catch (RDK::UException &exception)
  {
   GlobalLogger.ProcessException(exception);
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   GlobalLogger.ProcessException(RDK::UExceptionWrapperStd(exception));
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  GlobalLogger.ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  res=RDK_EXCEPTION_CATCHED;
 }

 return false;
}

// Возвращает ссылку на коллекцию шрифтов
RDK::UBitmapFontCollection& URdkCoreManager::GetFonts(void)
{
 return Fonts;
}
// --------------------------

// --------------------------
// Методы управления созданием каналов
// --------------------------
bool URdkCoreManager::SetCoreElementsCreationFunctions(PCreateNewStorage fCreateNewStorage,
							PCreateNewEnvironment fCreateNewEnvironment,
							PCreateNewEngine fCreateNewEngine)
{
 FuncCreateNewStorage=fCreateNewStorage;
 FuncCreateNewEnvironment=fCreateNewEnvironment;
 FuncCreateNewEngine=fCreateNewEngine;

 return true;
}

/// Возвращает число движков
int URdkCoreManager::GetNumChannels(void) const
{
 return NumChannels;
}

/// Создает требуемое число пустых движков
int URdkCoreManager::SetNumChannels(int num)
{
 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);

  if(num<0)
   return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

  for(int i=num;i<int(EngineList.size());i++)
  {
   ChannelDestroy(i);
   UDestroyMutex(MutexList[i]);
  }

  int old_num=EngineList.size();

  EngineList.resize(num,0);
  StorageList.resize(num,0);
  EnvironmentList.resize(num,0);
  MutexList.resize(num,0);
  LockerList.resize(num,0);
  LoggerList.resize(num,0);
  for(int i=old_num;i<num;i++)
  {
   MutexList[i]=UCreateMutex();
   #ifdef RDK_MUTEX_DEADLOCK_DEBUG
   MutexList[i]->DebugId=i;
   #endif
  }
  NumChannels=int(EngineList.size());
 }

 SetSelectedChannelIndex(num-1);

 return RDK_SUCCESS;
}

/// Делает текущим канала с заданным индексом
int URdkCoreManager::SelectChannel(int index)
{
 if(index<0 || index>=NumChannels)
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(SelectedChannelIndex == index)
	return RDK_SUCCESS;

   RDK::UELockPtr<RDK::UEngine> ptr1(GetEngineLock(SelectedChannelIndex));
   RDK::UELockPtr<RDK::UEngine> ptr2(GetEngineLock(index));

   if(!SetSelectedChannelIndex(index))
	res=RDK_E_CORE_CHANNEL_NOT_FOUND;
   else
    res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   GlobalLogger.ProcessException(exception);
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   GlobalLogger.ProcessException(RDK::UExceptionWrapperStd(exception));
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  GlobalLogger.ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  res=RDK_EXCEPTION_CATCHED;
 }


 return res;
}

/// Возвращает индекс текущего выбраного канала
int URdkCoreManager::GetSelectedChannelIndex(void) const
{
 return SelectedChannelIndex;
}

/// Добавляет новый движок в позицию index
/// Если index <0 или >= NumChannels то добавляет в конец
int URdkCoreManager::Add(int index)
{
 if(index<0 || index >= GetNumChannels())
  return SetNumChannels(GetNumChannels()+1);

 int old_num=GetNumChannels();
 int num=old_num+1;

 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  EngineList.resize(num,0);
  StorageList.resize(num,0);
  EnvironmentList.resize(num,0);
  MutexList.resize(num,0);
  LockerList.resize(num,0);
  LoggerList.resize(num,0);
 }

 for(int i=int(EngineList.size())-1;i>index;i--)
 {
  UGenericMutexExclusiveLocker lock1(MutexList[i-1]);
  EngineList[i]=EngineList[i-1];
  StorageList[i]=StorageList[i-1];
  EnvironmentList[i]=EnvironmentList[i-1];
  MutexList[i]=MutexList[i-1];
  LockerList[i]=LockerList[i-1];
  LoggerList[i]=LoggerList[i-1];
 }

 EngineList[index]=0;
 StorageList[index]=0;
 EnvironmentList[index]=0;
 MutexList[index]=0;
 LockerList[index]=0;
 LoggerList[index]=0;
 MutexList[index]=UCreateMutex();
 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 MutexList[index]->DebugId=index;
 #endif

 SetSelectedChannelIndex(SelectedChannelIndex);
 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  NumChannels=int(EngineList.size());
 }
 return RDK_SUCCESS;
}

/// Удаляет движок из позиции index
int URdkCoreManager::Del(int index)
{
 if(index<0 || index >= GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 if(GetNumChannels() == 1)
  return RDK_E_CORE_ZERO_CHANNEL_MUST_EXIST;

 {
  UGenericMutexExclusiveLocker lock(MutexList[index]);
  ChannelDestroy(index);
 }
 UDestroyMutex(MutexList[index]);
 for(int i=index+1;i<int(EngineList.size());i++)
 {
  UGenericMutexExclusiveLocker lock2(MutexList[i]);
  EngineList[i-1]=EngineList[i];
  StorageList[i-1]=StorageList[i];
  EnvironmentList[i-1]=EnvironmentList[i];
  MutexList[i-1]=MutexList[i];
  LockerList[i-1]=LockerList[i];
  LoggerList[i-1]=LoggerList[i];
 }

 int new_num_size=0;
 int curr_selected_channel_index=0;

 {
  UGenericMutexExclusiveLocker lock(GlobalMutex);
  EngineList.resize(EngineList.size()-1);
  StorageList.resize(StorageList.size()-1);
  EnvironmentList.resize(EnvironmentList.size()-1);
  MutexList.resize(MutexList.size()-1);
  LoggerList.resize(LoggerList.size()-1);
  LockerList.resize(LockerList.size()-1);
  new_num_size=NumChannels=int(EngineList.size());
  curr_selected_channel_index=SelectedChannelIndex;
 }

 if(curr_selected_channel_index>=new_num_size)
  SetSelectedChannelIndex(curr_selected_channel_index-1);
 else
  SetSelectedChannelIndex(curr_selected_channel_index);
 return RDK_SUCCESS;
}

/// Создаает требуемый канал
/// (если канал уже инициализирован, то не делает ничего
int URdkCoreManager::ChannelCreate(int index)
{
 if(index<0 || index>=GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 if(EngineList[index])
  return RDK_SUCCESS;

 // TODO: здесь инициализация параметров логгера и его запуск
 LoggerList[index]=new RDK::ULoggerEnv;
 LoggerList[index]->SetLogDir(LogDir);

 EngineList[index]=FuncCreateNewEngine();
 if(!EngineList[index])
 {
  ChannelDestroy(index);
  return RDK_E_CORE_ENGINE_CREATE_FAIL;
 }

 EngineList[index]->SetLogger(LoggerList[index]);

 StorageList[index]=FuncCreateNewStorage();
 if(!StorageList[index])
 {
  ChannelDestroy(index);
  return RDK_E_CORE_STORAGE_CREATE_FAIL;
 }

 EnvironmentList[index]=FuncCreateNewEnvironment();
 if(!EnvironmentList[index])
 {
  ChannelDestroy(index);
  return RDK_E_CORE_ENVIRONMENT_CREATE_FAIL;
 }

 EnvironmentList[index]->SetFonts(Fonts);
 EngineList[index]->Default();

 EnvironmentList[index]->SetSystemDir(SystemDir);
 if(!EngineList[index]->Init(StorageList[index],EnvironmentList[index]))
 {
  ChannelDestroy(index);
  return RDK_E_CORE_ENGINE_INIT_FAIL;
 }

  {
   UGenericMutexExclusiveLocker lock(GlobalMutex);
   /// Данные текущего выбранного канала
   Engine=EngineList[SelectedChannelIndex];
   Environment=EnvironmentList[SelectedChannelIndex];
   Storage=StorageList[SelectedChannelIndex];
   Logger=LoggerList[SelectedChannelIndex];
  }

 return RDK_SUCCESS;
}

/// Уничтожает требуемый движок
/// (если движок уже уничтожен, то не делает ничего
int URdkCoreManager::ChannelDestroy(int index)
{
 if(index<0 || index>=GetNumChannels())
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

 delete LoggerList[index];
 LoggerList[index]=0;
 return RDK_SUCCESS;
}

// Инициализирует канал (функция должна быть вызвана первой!)
// Upd: Функция может быть вызвана после SetNumChannels и SelectChannel
int URdkCoreManager::ChannelInit(int channel_index, int predefined_structure, void* exception_handler)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(channel_index<0 || channel_index>=NumChannels)
    return RDK_E_CORE_CHANNEL_NOT_FOUND;

   res=ChannelUnInit(channel_index);
   if(res != RDK_SUCCESS)
	return res;

   res=ChannelCreate(channel_index);
   if(res != RDK_SUCCESS)
	return res;

   GetEngineLock(channel_index)->SetChannelIndex(channel_index);
   GetEngineLock(channel_index)->SetBufObjectsMode(BufObjectsMode);
   MLog_SetExceptionHandler(channel_index, exception_handler);

   MEnv_SetPredefinedStructure(channel_index, predefined_structure);
   MEnv_CreateStructure(channel_index);
   MEnv_Init(channel_index);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   GlobalLogger.ProcessException(exception);
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   GlobalLogger.ProcessException(RDK::UExceptionWrapperStd(exception));
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  GlobalLogger.ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  res=RDK_EXCEPTION_CATCHED;
 }

 return res;
}

// Деинициализирует канал (функция автоматически вызывается при вызове инициализации)
int URdkCoreManager::ChannelUnInit(int channel_index)
{
 if(channel_index<0 || channel_index>=NumChannels)
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(EngineList[channel_index])
   {
	res = MEnv_UnInit(channel_index);
	if(res==RDK_E_CORE_ENVIRONMENT_UNINIT_FAIL)
	 return res;
   }

   res=ChannelDestroy(channel_index);
  }
  catch (RDK::UException &exception)
  {
   GlobalLogger.ProcessException(exception);
   res=RDK_EXCEPTION_CATCHED;
  }
  catch (std::exception &exception)
  {
   GlobalLogger.ProcessException(RDK::UExceptionWrapperStd(exception));
   res=RDK_EXCEPTION_CATCHED;
  }
 }
 RDK_SYS_CATCH
 {
  GlobalLogger.ProcessException(RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
  res=RDK_EXCEPTION_CATCHED;
 }


 return res;
}
// --------------------------

// --------------------------
// Методы доступа к каналам
// --------------------------
// Возвращает ссылку на указатель управляющего ядра
RDK::UEPtr<RDK::UEngine>& URdkCoreManager::GetEngine(void)
{
 return Engine;
}

RDK::UEPtr<RDK::UEngine> URdkCoreManager::GetEngine(int channel_index)
{
 return EngineList[channel_index];
}

// Возвращает ссылку на указатель среды выполнения
RDK::UEPtr<RDK::UEnvironment>& URdkCoreManager::GetEnvironment(void)
{
 return Environment;
}

RDK::UEPtr<RDK::UEnvironment> URdkCoreManager::GetEnvironment(int channel_index)
{
 return EnvironmentList[channel_index];
}

// Возвращает ссылку на указатель хранилища
RDK::UEPtr<RDK::UStorage>& URdkCoreManager::GetStorage(void)
{
 return Storage;
}

RDK::UEPtr<RDK::UStorage> URdkCoreManager::GetStorage(int channel_index)
{
 return StorageList[channel_index];
}

// Возвращает указатель на текущую модель
RDK::UEPtr<RDK::UContainer> URdkCoreManager::GetModel(void)
{
 if(Environment)
  return Environment->GetModel();

 return 0;
}

RDK::UEPtr<RDK::UContainer> URdkCoreManager::GetModel(int channel_index)
{
 if(channel_index<0 || channel_index>=int(EnvironmentList.size()))
  return 0;

 RDK::UEPtr<RDK::UEnvironment> environment=EnvironmentList[channel_index];
 if(environment)
  return environment->GetModel();

 return 0;
}
// --------------------------


// --------------------------
// Методы доступа к каналам с блокировками
// --------------------------
/// Метод доступа к глобальному лучше
UGenericMutex* URdkCoreManager::GetGlobalMutex(void)
{
 return GlobalMutex;
}

/// Метод доступ к мьютексу
UGenericMutex* URdkCoreManager::GetEngineMutex(void)
{
 if(MutexList.empty())
  return 0;
 return MutexList[0];
}

UGenericMutex* URdkCoreManager::GetEngineMutex(int index)
{
 if(index<0 || index>=int(MutexList.size()))
  return 0;

 return MutexList[index];
}

// Возвращает ссылку на указатель управляющего ядра
RDK::UELockPtr<RDK::UEngine> URdkCoreManager::GetEngineLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UEngine>(MutexList[SelectedChannelIndex],GetEngine());
#endif
}

RDK::UELockPtr<RDK::UEngine> URdkCoreManager::GetEngineLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEngine>(0,GetEngine(channel_index));
#else
 return RDK::UELockPtr<RDK::UEngine>(MutexList[channel_index],GetEngine(channel_index));
#endif
}

// Возвращает ссылку на указатель среды выполнения
RDK::UELockPtr<RDK::UEnvironment> URdkCoreManager::GetEnvironmentLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEnvironment>(0,GetEnvironment());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UEnvironment>(MutexList[SelectedChannelIndex],GetEnvironment());
#endif
}

RDK::UELockPtr<RDK::UEnvironment> URdkCoreManager::GetEnvironmentLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UEnvironment>(0,GetEnvironment(channel_index));
#else
 return RDK::UELockPtr<RDK::UEnvironment>(MutexList[channel_index],GetEnvironment(channel_index));
#endif
}

// Возвращает ссылку на указатель хранилища
RDK::UELockPtr<RDK::UStorage> URdkCoreManager::GetStorageLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UStorage>(0,GetStorage());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UStorage>(MutexList[SelectedChannelIndex],GetStorage());
#endif
}

RDK::UELockPtr<RDK::UStorage> URdkCoreManager::GetStorageLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UStorage>(0,GetStorage(channel_index));
#else
 return RDK::UELockPtr<RDK::UStorage>(MutexList[channel_index],GetStorage(channel_index));
#endif
}

// Возвращает указатель на текущую модель
RDK::UELockPtr<RDK::UContainer> URdkCoreManager::GetModelLock(void)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel());
#else
// UGenericMutexSharedLocker lock(GlobalMutex);
 return RDK::UELockPtr<RDK::UContainer>(MutexList[SelectedChannelIndex],GetModel());
#endif
}

RDK::UELockPtr<RDK::UContainer> URdkCoreManager::GetModelLock(int channel_index)
{
#ifdef RDK_ENGINE_UNLOCKED
 return RDK::UELockPtr<RDK::UContainer>(0,GetModel(channel_index));
#else
 return RDK::UELockPtr<RDK::UContainer>(MutexList[channel_index],GetModel(channel_index));
#endif
}
// --------------------------

// --------------------------
/// Средства логгирования
// --------------------------
// Возвращает ссылку на указатель на логгер
RDK::UEPtr<RDK::ULoggerEnv>& URdkCoreManager::GetLogger(void)
{
 return Logger;
}

RDK::UEPtr<RDK::ULoggerEnv> URdkCoreManager::GetLogger(int channel_index)
{
 if(channel_index == RDK_SYS_MESSAGE)
  return &GlobalLogger;

 return LoggerList[channel_index];
}

/// Возвращает ссылку на глобальный логгер
RDK::UEPtr<RDK::ULoggerEnv> URdkCoreManager::GetGlobalLogger(void)
{
 return &GlobalLogger;
}
// --------------------------

// --------------------------
// Вспомогательные методы управления
// --------------------------
/// Меняет текущий выбраный канал
bool URdkCoreManager::SetSelectedChannelIndex(int channel_index)
{
// UGenericMutexExclusiveLocker lock(GlobalMutex);
 if(channel_index<0 || channel_index>=GetNumChannels())
  return false;

 SelectedChannelIndex=channel_index;
// ::SelectedEngineIndex=SelectedChannelIndex;
 /// Данные текущего выбранного канала
 Engine=EngineList[channel_index];
 Environment=EnvironmentList[channel_index];
 Storage=StorageList[channel_index];
 Logger=LoggerList[channel_index];

 return true;
}
// --------------------------

#endif
