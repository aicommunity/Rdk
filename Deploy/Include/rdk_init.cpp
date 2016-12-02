#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include <exception>
#include "rdk_init.h"
#include "rdk.h"
//#include "rdk_rpc.cpp"
#include "rdk_engine_support.h"
#include "rdk_exceptions.h"
#include "../../Core/Engine/UEnvException.h"
#include "rdk_error_codes.h"
#include "../../Core/System/UGenericMutex.h"

namespace RDK {


/// Возвращает RDK_UNHANDLED_EXCEPTION если не удалось записать данные исключения
/// иначе возвращает RDK_EXCEPTION_CATCHED
int RDK_CALL ProcessException(int channel_index, const UException &ex)
{
 UEPtr<ULoggerEnv> logger=RdkCoreManager.GetLogger(channel_index);
 if(!logger)
  return RDK_UNHANDLED_EXCEPTION;
 logger->ProcessException(ex);
 return RDK_EXCEPTION_CATCHED;
}
/*
// Очищает коллекцию глобальных шрифтов
void ClearClobalFonts(void)
{
 GlobalFonts.DelAllFonts();
}

// Загружает новый глобальный шрифт
bool AddGlobalFont(const std::string &font_file_name)
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
	return RDK::GlobalFonts.AddFont(font.GetName(),size,font);
   }
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return false;
}
      */
}

/*****************************************************************************/
extern RDK::UStorage* CreateNewStorage(void);

extern RDK::UEnvironment* CreateNewEnvironment(void);

extern RDK::UEngine* CreateNewEngine(void);
/*****************************************************************************/

// ----------------------------
// Функции RPC
// С помощью этих функций возможно вызвать любую функицю библиотеки,
// возвращающую строковые данные
// ----------------------------
/// Выполняет запрос и возвращает xml-описание ответа
/// request - xml описание запроса
/// return_value - возвращаемое значение для тех функций, которые его имеют
/// для остальных возвращает 0
const char* RDK_CALL Core_RemoteCall(const char *request, int &return_value, int &channel_index)
{
 return RDK::RemoteCallInternal(request, return_value, channel_index);
}
// ----------------------------

// ----------------------------
// Функции логирования
// ----------------------------
// Возвращает состояние внутренего логгирования
bool RDK_CALL Log_GetEventsLogMode(void)
{
 return RdkCoreManager.GetLogger()->GetEventsLogMode();
}

bool RDK_CALL MLog_GetEventsLogMode(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return false;

 return RdkCoreManager.GetLogger(channel_index)->GetEventsLogMode();
}

// Включает/выключает внутренне логгирование
int RDK_CALL Log_SetEventsLogMode(bool value)
{
 return RdkCoreManager.GetLogger()->SetEventsLogMode(value);
}

int RDK_CALL MLog_SetEventsLogMode(int channel_index, bool value)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetLogger(channel_index)->SetEventsLogMode(value);
}

/// Возвращает состояние флага отладочного режима среды
bool RDK_CALL Log_GetDebugMode(void)
{
 return RdkCoreManager.GetLogger()->GetDebugMode();
}

bool RDK_CALL MLog_GetDebugMode(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return false;
 return RdkCoreManager.GetLogger(channel_index)->GetDebugMode();
}

/// Устанавливает состояние флага отладочного режима среды
int RDK_CALL Log_SetDebugMode(bool value)
{
 return RdkCoreManager.GetLogger()->SetDebugMode(value);
}

int RDK_CALL MLog_SetDebugMode(int channel_index, bool value)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetLogger(channel_index)->SetDebugMode(value);
}

/// Возвращает маску системных событий для логирования
unsigned int RDK_CALL Log_GetDebugSysEventsMask(void)
{
 return RdkCoreManager.GetLogger()->GetDebugSysEventsMask();
}

unsigned int RDK_CALL MLog_GetDebugSysEventsMask(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetLogger(channel_index)->GetDebugSysEventsMask();
}

/// Устанавливает маску системных событий для логирования
int RDK_CALL Log_SetDebugSysEventsMask(unsigned int value)
{
 return RdkCoreManager.GetLogger()->SetDebugSysEventsMask(value);
}

int RDK_CALL MLog_SetDebugSysEventsMask(int channel_index, unsigned int value)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetLogger(channel_index)->SetDebugSysEventsMask(value);
}

/// Возвращает флаг включения вывода лога в отладчик
bool RDK_CALL Log_GetDebuggerMessageFlag(void)
{
 return RdkCoreManager.GetLogger()->GetDebuggerMessageFlag();
}

bool RDK_CALL MLog_GetDebuggerMessageFlag(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetLogger(channel_index)->GetDebuggerMessageFlag();
}

/// Устанавливает флаг включения вывода лога в отладчик
bool RDK_CALL Log_SetDebuggerMessageFlag(bool value)
{
 return RdkCoreManager.GetLogger()->SetDebuggerMessageFlag(value);
}

bool RDK_CALL MLog_SetDebuggerMessageFlag(int channel_index, bool value)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetLogger(channel_index)->SetDebuggerMessageFlag(value);
}

// Управление функцией-обработчиком исключений
void* RDK_CALL Log_GetExceptionHandler(void)
{
 return (void*)RdkCoreManager.GetLogger()->GetExceptionHandler();
}

void* RDK_CALL MLog_GetExceptionHandler(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;

 return (void*)RdkCoreManager.GetLogger(channel_index)->GetExceptionHandler();
}

int RDK_CALL Log_SetExceptionHandler(void* value)
{
 return RdkCoreManager.GetLogger()->SetExceptionHandler(reinterpret_cast<RDK::ULoggerEnv::PExceptionHandler>(value));
}

int RDK_CALL MLog_SetExceptionHandler(int channel_index, void* value)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetLogger(channel_index)->SetExceptionHandler(reinterpret_cast<RDK::ULoggerEnv::PExceptionHandler>(value));
}

// Возвращает массив строк лога
const char* RDK_CALL Log_GetLog(int &error_level)
{
 return RdkCoreManager.GetLogger()->GetLog(error_level);
}

const char* RDK_CALL MLog_GetLog(int channel_index, int &error_level)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetLogger(channel_index)->GetLog(error_level);
}

// Записывает в лог новое сообщение
int RDK_CALL Log_LogMessage(int log_level, const char *message)
{
 RdkCoreManager.GetLogger()->LogMessage(log_level, message);
 return RDK_SUCCESS;
}

int RDK_CALL MLog_LogMessage(int channel_index, int log_level, const char *message)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 RdkCoreManager.GetLogger(channel_index)->LogMessage(log_level, message);
 return RDK_SUCCESS;
}

// Записывает в лог новое сообщение с кодом ошибки
int RDK_CALL Log_LogMessageEx(int log_level, const char *message, int error_event_number)
{
 RdkCoreManager.GetLogger()->LogMessage(log_level, message,error_event_number);
 return RDK_SUCCESS;
}

int RDK_CALL MLog_LogMessageEx(int channel_index, int log_level, const char *message, int error_event_number)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 RdkCoreManager.GetLogger(channel_index)->LogMessage(log_level, message,error_event_number);
 return RDK_SUCCESS;
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* RDK_CALL Log_GetUnreadLog(int &error_level, int &number, unsigned long long &time)
{
 time_t read_time;
 const char* res=RdkCoreManager.GetLogger()->GetUnreadLog(error_level, number, read_time);
 time=read_time;
 return res;
}

const char* RDK_CALL MLog_GetUnreadLog(int channel_index, int &error_level, int &number, unsigned long long &time)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;
 time_t read_time;
 const char* res=RdkCoreManager.GetLogger(channel_index)->GetUnreadLog(error_level, number, read_time);
 time=read_time;
 return res;
}

const char* RDK_CALL Log_GetUnreadLogUnsafe(int &error_level, int &number, unsigned long long &time)
{
 time_t read_time;
 const char* res=RdkCoreManager.GetLogger()->GetUnreadLog(error_level, number, read_time);
 time=read_time;
 return res;
}

const char* RDK_CALL MLog_GetUnreadLogUnsafe(int channel_index, int &error_level, int &number, unsigned long long &time)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;
 time_t read_time;
 const char* res=RdkCoreManager.GetLogger(channel_index)->GetUnreadLog(error_level, number, read_time);
 time=read_time;
 return res;
}

/// Возвращает число непрочитанных строк лога
int RDK_CALL Log_GetNumUnreadLogLines(void)
{
 return RdkCoreManager.GetLogger()->GetNumUnreadLogLines();
}

int RDK_CALL MLog_GetNumUnreadLogLines(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetLogger(channel_index)->GetNumUnreadLogLines();
}

/// Возвращает число строк лога
int RDK_CALL Log_GetNumLogLines(void)
{
 return RdkCoreManager.GetLogger()->GetNumLogLines();
}

int RDK_CALL MLog_GetNumLogLines(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetLogger(channel_index)->GetNumLogLines();
}


/// Очищает лог прочитанных сообщений
int RDK_CALL Log_ClearReadLog(void)
{
 RdkCoreManager.GetLogger()->ClearReadLog();
 return RDK_SUCCESS;
}

int RDK_CALL MLog_ClearReadLog(int channel_index)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;
 RdkCoreManager.GetLogger(channel_index)->ClearReadLog();
 return RDK_SUCCESS;
}
// ----------------------------

// ----------------------------
// Методы инициализации
// ----------------------------
// Возвращает имя каталога бинарных файлов
const char* RDK_CALL Core_GetSystemDir(void)
{
 return RdkCoreManager.GetSystemDir().c_str();
}

const char* RDK_CALL GetSystemDir(void)
{
 return Core_GetSystemDir();
}

// Устанавливает имя каталога бинарных файлов
int RDK_CALL Core_SetSystemDir(const char *dir)
{
 return RdkCoreManager.SetSystemDir(dir);
}

int RDK_CALL SetSystemDir(const char *dir)
{
 return Core_SetSystemDir(dir);
}

// Возвращает имя каталога бинарных файлов
const char* RDK_CALL Core_GetLogDir(void)
{
 return RdkCoreManager.GetLogDir();
}

// Устанавливает имя каталога бинарных файлов
int RDK_CALL Core_SetLogDir(const char *dir)
{
 return RdkCoreManager.SetLogDir(dir);
}

// Возвращает глобальную настройку включения отладочного режима
bool RDK_CALL Core_GetDebugMode(void)
{
 return RdkCoreManager.GetDebugMode();
}

// Устанавливает глобальную настройку включения отладочного режима
int RDK_CALL Core_SetDebugMode(bool value)
{
 return RdkCoreManager.SetDebugMode(value);
}

// Очищает глобальные шрифты
int RDK_CALL Core_ClearFonts(void)
{
 return RdkCoreManager.ClearFonts();
}

// Загружает глобальные шрифты
int RDK_CALL Core_LoadFonts(void)
{
 return RdkCoreManager.LoadFonts();
}

// Возвращает число движков
int RDK_CALL Core_GetNumChannels(void)
{
 return RdkCoreManager.GetNumChannels();
}

// Создает требуемое число движков
// num > 0
int RDK_CALL Core_SetNumChannels(int num)
{
 if(num<=0)
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 if(num == Core_GetNumChannels())
  return RDK_SUCCESS;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!RdkCoreManager.SetCoreElementsCreationFunctions(reinterpret_cast<URdkCoreManager::PCreateNewStorage>(CreateNewStorage),
						reinterpret_cast<URdkCoreManager::PCreateNewEnvironment>(CreateNewEnvironment),
						reinterpret_cast<URdkCoreManager::PCreateNewEngine>(CreateNewEngine)))
	return RDK_E_CORE_INIT_FAIL;

   res=RdkCoreManager.SetNumChannels(num);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL SetNumChannels(int num)
{
 return Core_SetNumChannels(num);
}

// Добавляет движок в позицию заданного индекса
// Если позиция лежит вне пределов диапазона то
// добавляет в конец
int RDK_CALL Core_AddChannel(int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!RdkCoreManager.SetCoreElementsCreationFunctions(reinterpret_cast<URdkCoreManager::PCreateNewStorage>(CreateNewStorage),
						reinterpret_cast<URdkCoreManager::PCreateNewEnvironment>(CreateNewEnvironment),
						reinterpret_cast<URdkCoreManager::PCreateNewEngine>(CreateNewEngine)))
	return RDK_E_CORE_INIT_FAIL;

   res=RdkCoreManager.Add(index);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

// Удаляет движок по индексу
int RDK_CALL Core_DelChannel(int index)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   res=RdkCoreManager.Del(index);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

// Возвращает индекс текущего выбранного движка
int RDK_CALL Core_GetSelectedChannelIndex(void)
{
 return RdkCoreManager.GetSelectedChannelIndex();
}

// Настраивает обычный интерфейс на работу с заданным движком
// В случае удаления движка, интерфейс автоматически перенастраивается на 0 движок
int RDK_CALL Core_SelectChannel(int index)
{
 return RdkCoreManager.SelectChannel(index);
}

/// Блокирует канал до вызова функции UnlockEngine
int RDK_CALL Core_LockChannel(void)
{
 return MCore_LockChannel(RdkCoreManager.GetSelectedChannelIndex());
}

int RDK_CALL MCore_LockChannel(int index)
{
 if(index<0 || index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(RdkCoreManager.LockerList[index])
	return RDK_SUCCESS;

   if(!RdkCoreManager.LockerList[index])
	RdkCoreManager.LockerList[index]=new UGenericMutexExclusiveLocker(RdkCoreManager.MutexList[index]);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

/// Разблокирует канал
int RDK_CALL Core_UnLockChannel(void)
{
 return MCore_UnLockChannel(RdkCoreManager.GetSelectedChannelIndex());
}

int RDK_CALL MCore_UnLockChannel(int index)
{
 if(index<0 || index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(!RdkCoreManager.LockerList[index])
	return RDK_SUCCESS;

   delete RdkCoreManager.LockerList[index];
   RdkCoreManager.LockerList[index]=0;
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL Core_ChannelInit(int predefined_structure, void* exception_handler)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(Core_GetNumChannels()<=RdkCoreManager.GetSelectedChannelIndex())
   {
	res=SetNumChannels(RdkCoreManager.GetSelectedChannelIndex()+1);

	if(res != RDK_SUCCESS)
	 return res;
   }

   res=MCore_ChannelInit(RdkCoreManager.GetSelectedChannelIndex(), predefined_structure, exception_handler);
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }

 return res;
}

int RDK_CALL MCore_ChannelInit(int channel_index, int predefined_structure, void* exception_handler)
{
 return RdkCoreManager.ChannelInit(channel_index, predefined_structure, exception_handler);
}

// Деинициализирует движок (функция автоматически вызывается при вызове инициализации)
int RDK_CALL Core_ChannelUnInit(void)
{
 return MCore_ChannelUnInit(Core_GetSelectedChannelIndex());
}

int RDK_CALL MCore_ChannelUnInit(int channel_index)
{
 return RdkCoreManager.ChannelUnInit(channel_index);
}

/// Проверяет инициализирован ли движок
bool RDK_CALL Core_IsChannelInit(void)
{
 return (RdkCoreManager.GetEngine())?true:false;
}

bool RDK_CALL MCore_IsChannelInit(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return false;

 return (RdkCoreManager.GetEngine(channel_index))?true:false;
}

/// Режим создания внутренних временных переменных для
/// возвращаемых значений
/// 0 - одна переменная для всех методов, возвращающих такой тип
/// 1 - уникальные переменные с необходимостью вызвова функции очистки
int RDK_CALL Core_GetBufObjectsMode(void)
{
 return RdkCoreManager.GetBufObjectsMode();
}

int RDK_CALL Core_SetBufObjectsMode(int mode)
{
 return RdkCoreManager.SetBufObjectsMode(mode);
}

/// Высвобождает буферную строку движка, по заданному указателю
int RDK_CALL Engine_FreeBufString(const char *pointer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RdkCoreManager.GetEngineLock()->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int RDK_CALL MEngine_FreeBufString(int channel_index,const char *pointer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   if(channel_index<0 || channel_index>=Core_GetNumChannels())
	return RDK_E_CORE_CHANNEL_NOT_FOUND;
   RdkCoreManager.GetEngineLock(channel_index)->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(channel_index,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(channel_index,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(channel_index,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int RDK_CALL Engine_FreeBufStringUnsafe(const char *pointer)
{
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RdkCoreManager.GetEngine()->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(RDK_SYS_MESSAGE,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

int RDK_CALL MEngine_FreeBufStringUnsafe(int channel_index,const char *pointer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 int res=RDK_UNHANDLED_EXCEPTION;
 RDK_SYS_TRY
 {
  try
  {
   RdkCoreManager.GetEngineLock(channel_index)->DestroyTempString(pointer);
   res=RDK_SUCCESS;
  }
  catch (RDK::UException &exception)
  {
   res=ProcessException(channel_index,exception);
  }
  catch (std::exception &exception)
  {
   res=ProcessException(channel_index,RDK::UExceptionWrapperStd(exception));
  }
 }
 RDK_SYS_CATCH
 {
  res=ProcessException(channel_index,RDK::UExceptionWrapperSEH(GET_SYSTEM_EXCEPTION_DATA));
 }
 return res;
}

/// Возвращает число буферных строк движка
int RDK_CALL Engine_GetNumBufStrings(void)
{
 return RdkCoreManager.GetEngineLock()->GetNumTempStrings();
}

int RDK_CALL MEngine_GetNumBufStrings(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock()->GetNumTempStrings();
}

/// Доступ к мьютексу
void* RDK_CALL Engine_GetMutex(void)
{
 return RdkCoreManager.GetEngineMutex();
}

void* RDK_CALL MEngine_GetMutex(int index)
{
 return RdkCoreManager.GetEngineMutex(index);
}
// ----------------------------

// --------------------------
// Методы управления хранилищем
// ----------------------------
// Возвращает число классов в хранилище
int RDK_CALL Storage_GetNumClasses(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetNumClasses();
}

// Возвращает id классов в хранилище. Память должна быть выделена
int RDK_CALL Storage_GetClassesList(int *buffer)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassesList(buffer);
}

// Возвращает имена классов в хранилище в виде строки разделенной запятыми
const char * RDK_CALL Storage_GetClassesNameList(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassesNameList();
}

// Возвращает имя класса по его id.
const char * RDK_CALL Storage_GetClassName(int id)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassName(id);
}

// Возвращает Id класса по его имени
int RDK_CALL Storage_GetClassId(const char *name)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassId(name);
}

// Удаляет образец класса объекта из хранилища
// Возвращает false если classid не найден,
// или присутствуют объекты этого класса
int RDK_CALL Storage_DelClass(int classid)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelClass(classid);
}

// Удалаяет все свободные объекты из хранилища
int RDK_CALL Storage_FreeObjectsStorage(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_FreeObjectsStorage();
}

// Удаляет все объекты из хранилища
int RDK_CALL Storage_ClearObjectsStorage(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_ClearObjectsStorage();
}

// Вычисляет суммарное число объектов в хранилище
int RDK_CALL Storage_CalcNumObjects(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_CalcNumObjects();
}

int RDK_CALL Storage_CalcNumObjectsById(int classid)
{
 return RdkCoreManager.GetEngineLock()->Storage_CalcNumObjectsById(classid);
}

int RDK_CALL Storage_CalcNumObjectsByName(const char* classname)
{
 return RdkCoreManager.GetEngineLock()->Storage_CalcNumObjectsByName(classname);
}

// Возвращает описание класса по его id в формате xml
const char* RDK_CALL Storage_GetClassDescription(const char* classname)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassDescription(classname);
}

// Устанавливает описание класса по его id, считывая его из формата xml
int RDK_CALL Storage_SetClassDescription(const char* classname, const char* description)
{
 return RdkCoreManager.GetEngineLock()->Storage_SetClassDescription(classname, description);
}

// Сохраняет описание всех классов в xml
const char* RDK_CALL Storage_SaveClassesDescription(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveClassesDescription();
}

// Загружает описание всех классов из xml
int RDK_CALL Storage_LoadClassesDescription(const char* xmltext)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadClassesDescription(xmltext);
}

int RDK_CALL MStorage_LoadClassesDescription(int channel_index, const char* xmltext)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetEngineLock(channel_index)->Storage_LoadClassesDescription(xmltext);
}

// Сохраняет общее описание всех классов в xml
const char* RDK_CALL Storage_SaveCommonClassesDescription(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveCommonClassesDescription();
}

// Загружает общее описание всех классов из xml
int RDK_CALL Storage_LoadCommonClassesDescription(const char* xmltext)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadCommonClassesDescription(xmltext);
}

int RDK_CALL MStorage_LoadCommonClassesDescription(int channel_index, const char* xmltext)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetEngineLock(channel_index)->Storage_LoadCommonClassesDescription(xmltext);
}


// Сохраняет описание всех классов в xml включая общее описание
const char* RDK_CALL Storage_SaveAllClassesDescription(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveAllClassesDescription();
}

// Загружает описание всех классов из xml включая общее описание
int RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadAllClassesDescription(xmltext);
}
// ----------------------------

// ----------------------------
// Методы управления коллекциями компонент
// ----------------------------
// Возвращает число библиотек
int RDK_CALL Storage_GetNumClassLibraries(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetNumClassLibraries();
}

// Возвращает список библиотек в виде строки, разделенной запятыми
const char* RDK_CALL Storage_GetClassLibrariesList(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassLibrariesList();
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// library_name - имя библиотеки
const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetLibraryClassNames(library_name);
}

// Возвращает список классов библиотеки в виде строки, разделенной запятыми
// index - индекс библиотеки
const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetLibraryClassNamesByIndex(index);
}

// Возвращает имя библиотеки по индексу
const char * RDK_CALL Storage_GetClassLibraryNameByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassLibraryNameByIndex(index);
}

// Возвращает версию библиотеки по индексу
const char * RDK_CALL Storage_GetClassLibraryVersionByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassLibraryVersionByIndex(index);
}

/// Создает новую runtime-библиотеку
int RDK_CALL Storage_CreateRuntimeCollection(const char *collection_name)
{
 return RdkCoreManager.GetEngineLock()->Storage_CreateRuntimeCollection(collection_name);
}

// Загружает коллекцию по имени dll-файла
int RDK_CALL Storage_LoadBinaryCollectionFromFile(const char *filename)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadBinaryCollectionFromFile(filename);
}

// Загружает runtime-коллекцию
int RDK_CALL Storage_LoadRuntimeCollectionFromFile(const char *filename)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadRuntimeCollectionFromFile(filename);
}

int RDK_CALL Storage_LoadRuntimeCollectionFromString(const char *buffer)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadRuntimeCollectionFromString(buffer);
}

// Сохраняет runtime-коллекцию
int RDK_CALL Storage_SaveRuntimeCollectionToFile(const char *filename)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveRuntimeCollectionToFile(filename);
}

int RDK_CALL Storage_SaveRuntimeCollectionToString(const char *buffer)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveRuntimeCollectionToString(buffer);
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти лежит на вызывающей стороне.
int RDK_CALL Storage_DelClassLibraryByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelClassLibraryByIndex(index);
}

// Удаляет подключенную библиотеку из списка по имени
// Ответственность за освобождение памяти лежит на вызывающей стороне.
int RDK_CALL Storage_DelClassLibraryByName(const char *name)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelClassLibraryByName(name);
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
int RDK_CALL Storage_DelAllClassLibraries(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelAllClassLibraries();
}

// Перемещает объект в Storage как образец классов.
// Объект удаляется из модели
int RDK_CALL Storage_CreateClass(const char* stringid, const char *classname, const char *collection_name)
{
 return RdkCoreManager.GetEngineLock()->Storage_CreateClass(stringid, classname, collection_name);
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
int RDK_CALL Storage_BuildStorage(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_BuildStorage();
}
// ----------------------------


// ----------------------------
// Методы управления средой
// ----------------------------
// Индекс предарительно заданной модели обработки
int RDK_CALL Env_GetPredefinedStructure(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetPredefinedStructure();
}

int RDK_CALL MEnv_GetPredefinedStructure(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_GetPredefinedStructure();
}

int RDK_CALL Env_SetPredefinedStructure(int value)
{
 return RdkCoreManager.GetEngineLock()->Env_SetPredefinedStructure(value);
}

int RDK_CALL MEnv_SetPredefinedStructure(int channel_index, int value)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetPredefinedStructure(value);
}

// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool RDK_CALL Env_IsStoragePresent(void)
{
 return RdkCoreManager.GetEngineLock()->Env_IsStoragePresent();
}

bool RDK_CALL MEnv_IsStoragePresent(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return false;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_IsStoragePresent();
}

// Возвращает состояние инициализации
bool RDK_CALL Env_IsInit(void)
{
 return RdkCoreManager.GetEngineLock()->Env_IsInit();
}

bool RDK_CALL MEnv_IsInit(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return false;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_IsInit();
}


// Признак наличия сформированной структуры
bool RDK_CALL Env_IsStructured(void)
{
 return RdkCoreManager.GetEngineLock()->Env_IsStructured();
}

bool RDK_CALL MEnv_IsStructured(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return false;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_IsStructured();
}

// Возвращает состояние внутренего логгирования
bool RDK_CALL Env_GetEventsLogMode(void)
{
 return Log_GetEventsLogMode();
}

bool RDK_CALL MEnv_GetEventsLogMode(int channel_index)
{
 return MLog_GetEventsLogMode(channel_index);
}

// Включает/выключает внутренне логгирование
int RDK_CALL Env_SetEventsLogMode(bool value)
{
 return Log_SetEventsLogMode(value);
}

int RDK_CALL MEnv_SetEventsLogMode(int channel_index, bool value)
{
 return MLog_SetEventsLogMode(channel_index,value);
}

// Инициализация среды
int RDK_CALL Env_Init(void)
{
 return RdkCoreManager.GetEngineLock()->Env_Init();
}

int RDK_CALL MEnv_Init(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_Init();
}

// Деинициализация среды
int RDK_CALL Env_UnInit(void)
{
 return RdkCoreManager.GetEngineLock()->Env_UnInit();
}

int RDK_CALL MEnv_UnInit(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_UnInit();
}

// Формирует предварительно заданную модель обработки
int RDK_CALL Env_CreateStructure(void)
{
 return RdkCoreManager.GetEngineLock()->Env_CreateStructure();
}

int RDK_CALL MEnv_CreateStructure(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_CreateStructure();
}

// Уничтожает текущую модель обработки
int RDK_CALL Env_DestroyStructure(void)
{
 return RdkCoreManager.GetEngineLock()->Env_DestroyStructure();
}

int RDK_CALL MEnv_DestroyStructure(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_DestroyStructure();
}

// Удаляет модель и все библиотеки, очищает хранилище, приводя среду в исходное состояние
int RDK_CALL Env_Destroy(void)
{
 return RdkCoreManager.GetEngineLock()->Env_Destroy();
}

int RDK_CALL MEnv_Destroy(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_Destroy();
}

// Метод счета
// Если stringid == 0 то вычисляет всю модель целиком,
// иначе вычисляет только указанный компонент модели
int RDK_CALL Env_Calculate(const char* stringid)
{
#ifdef RDK_UNSAFE_CALCULATE
 return RdkCoreManager.GetEngine()->Env_Calculate(stringid);
#endif
 return RdkCoreManager.GetEngineLock()->Env_Calculate(stringid);
}

int RDK_CALL MEnv_Calculate(int channel_index, const char* stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
#ifdef RDK_UNSAFE_CALCULATE
 return RdkCoreManager.GetEngine(channel_index)->Env_Calculate(stringid);
#endif
 return RdkCoreManager.GetEngineLock(channel_index)->Env_Calculate(stringid);
}

int RDK_CALL MEnv_CalculateUnsafe(int channel_index, const char* stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngine(channel_index)->Env_Calculate(stringid);
}

// Расчет всей модели в реальном времени
int RDK_CALL Env_RTCalculate(void)
{
 return RdkCoreManager.GetEngineLock()->Env_RTCalculate();
}

int RDK_CALL MEnv_RTCalculate(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_RTCalculate();
}


/// Расчет модели порциями длительностью calc_intervsal секунд с максимально возможной скоростью
int RDK_CALL Env_FastCalculate(double calc_interval)
{
 return RdkCoreManager.GetEngineLock()->Env_FastCalculate(calc_interval);
}

int RDK_CALL MEnv_FastCalculate(int channel_index, double calc_interval)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_FastCalculate(calc_interval);
}

// Метод сброса счета
// Если stringid == 0 то сбрасывает всю модель целиком,
// иначе - только указанный компонент модели
int RDK_CALL Env_Reset(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Env_Reset(stringid);
}

int RDK_CALL MEnv_Reset(int channel_index, const char* stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_Reset(stringid);
}

/// Метод сброса параметров на значения по умолчанию
/// Если stringid == 0 то сбрасывает всю модель целиком,
/// иначе - только указанный компонент модели
/// Если subcomps == true то также сбрасывает параметры всех дочерних компонент
int RDK_CALL Env_Default(const char* stringid, bool subcomps)
{
 return RdkCoreManager.GetEngineLock()->Env_Default(stringid,subcomps);
}

int RDK_CALL MEnv_Default(int channel_index, const char* stringid, bool subcomps)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_Default(stringid,subcomps);
}

// Производит увеличение времени модели на требуемую величину
int RDK_CALL Env_IncreaseModelTimeByStep(void)
{
 return RdkCoreManager.GetEngineLock()->Env_IncreaseModelTimeByStep();
}

/// Устанавливает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
int RDK_CALL Env_SetMinInterstepsInterval(unsigned long long value)
{
 return RdkCoreManager.GetEngineLock()->Env_SetMinInterstepsInterval(value);
}

int RDK_CALL MEnv_SetMinInterstepsInterval(int channel_index, unsigned long long value)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetMinInterstepsInterval(value);
}

/// Возвращает минимальный интервал времени между шагами расчета (мс)
/// Итерации расчета будут пропускаться до тех пор, пока время прошедшее с начала
/// последней итерации не станет больше чем эта величина
unsigned long long RDK_CALL Env_GetMinInterstepsInterval(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetMinInterstepsInterval();
}

unsigned long long RDK_CALL Env_GetMinInterstepsInterval(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_GetMinInterstepsInterval();
}


// Время, потраченное на последний RT-расчет
double RDK_CALL Env_GetRTLastDuration(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetRTLastDuration();
}

double RDK_CALL MEnv_GetRTLastDuration(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0.0;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_GetRTLastDuration();
}

/// Время, расчитанное в модели за один вызов RTCalculate;
double RDK_CALL Env_GetRTModelCalcTime(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetRTModelCalcTime();
}

double RDK_CALL MEnv_GetRTModelCalcTime(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0.0;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_GetRTModelCalcTime();
}

/// Производительность RT расчета (отношение RTModelCalcTime/RTLastDuration)
double RDK_CALL Env_CalcRTPerformance(void)
{
 return RdkCoreManager.GetEngineLock()->Env_CalcRTPerformance();
}

double RDK_CALL MEnv_CalcRTPerformance(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0.0;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_CalcRTPerformance();
}

// Возвращает имя текущего каталога для хранения данных
const char* RDK_CALL Env_GetCurrentDataDir(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetCurrentDataDir();
}

// Устанавливает имя текущего каталога для хранения данных
int RDK_CALL Env_SetCurrentDataDir(const char *dir)
{
 return RdkCoreManager.GetEngineLock()->Env_SetCurrentDataDir(dir);
}

int RDK_CALL MEnv_SetCurrentDataDir(int channel_index, const char *dir)
{
 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetCurrentDataDir(dir);
}

/// Возвращает состояние флага отладочного режима среды
bool RDK_CALL Env_GetDebugMode(void)
{
 return Log_GetDebugMode();
}

bool RDK_CALL MEnv_GetDebugMode(int channel_index)
{
 return MLog_GetDebugMode(channel_index);
}

/// Устанавливает состояние флага отладочного режима среды
int RDK_CALL Env_SetDebugMode(bool value)
{
 return Log_SetDebugMode(value);
}

int RDK_CALL MEnv_SetDebugMode(int channel_index, bool value)
{
 return MLog_SetDebugMode(channel_index, value);
}

/// Возвращает маску системных событий для логирования
unsigned int RDK_CALL Env_GetDebugSysEventsMask(void)
{
 return Log_GetDebugSysEventsMask();
}

unsigned int RDK_CALL MEnv_GetDebugSysEventsMask(int channel_index)
{
 return MLog_GetDebugSysEventsMask(channel_index);
}

/// Устанавливает маску системных событий для логирования
int RDK_CALL Env_SetDebugSysEventsMask(unsigned int value)
{
 return Log_SetDebugSysEventsMask(value);
}

int RDK_CALL MEnv_SetDebugSysEventsMask(int channel_index, unsigned int value)
{
 return MLog_SetDebugSysEventsMask(channel_index, value);
}


/// Возвращает флаг включения вывода лога в отладчик
bool RDK_CALL Env_GetDebuggerMessageFlag(void)
{
 return Log_GetDebuggerMessageFlag();
}

bool RDK_CALL MEnv_GetDebuggerMessageFlag(int channel_index)
{
 return MLog_GetDebuggerMessageFlag(channel_index);
}

/// Устанавливает флаг включения вывода лога в отладчик
bool RDK_CALL Env_SetDebuggerMessageFlag(bool value)
{
 return Log_SetDebuggerMessageFlag(value);
}

bool RDK_CALL MEnv_SetDebuggerMessageFlag(int channel_index, bool value)
{
 return MLog_SetDebuggerMessageFlag(channel_index, value);
}

// ***********************************************
// Методы управления текущим компонентом
// !!! Следующие методы влияют на все
// методы, обращающиеся к компонентам по строковому id !!!
// ***********************************************
// Устанавливает текущий компонент (адресация относительно корня - модели)
int RDK_CALL Env_SelectCurrentComponent(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Env_SelectCurrentComponent(stringid);
}

// Сбрасывает текущий компонент в состояние по умолчению (модель)
int RDK_CALL Env_ResetCurrentComponent(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Env_ResetCurrentComponent(stringid);
}

// Меняет текущий компонент на его родителя (подъем на уровень вверх)
// Если уже на верхнем уровне, то не делает ничего
int RDK_CALL Env_UpCurrentComponent(void)
{
 return RdkCoreManager.GetEngineLock()->Env_UpCurrentComponent();
}

// Меняет текущий компонент на его дочерний на произвольном уровне вложенности
// (спуск на N уровней вниз относительно текущего компонента)
int RDK_CALL Env_DownCurrentComponent(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Env_DownCurrentComponent(stringid);
}

// Возвращает длинное имя текущего компонента
const char* RDK_CALL Env_GetCurrentComponentName(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetCurrentComponentName();
}

// Возвращает длинный строковой id текущего компонента
const char* RDK_CALL Env_GetCurrentComponentId(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetCurrentComponentId();
}
// ***********************************************

/// Инициирует извещение о сбое в работе источника данных
int RDK_CALL Env_CallSourceController(void)
{
 return RdkCoreManager.GetEngineLock()->Env_CallSourceController();
}

int RDK_CALL MEnv_CallSourceController(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_CallSourceController();
}
// --------------------------

// --------------------------
// Методы управления моделью
// ----------------------------
// Удаляет модель
int RDK_CALL Model_Destroy(void)
{
 return RdkCoreManager.GetEngineLock()->Model_Destroy();
}

int RDK_CALL MModel_Destroy(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_Destroy();
}

// Создает новую модель по имени класса в хранилище
// Предварительно удаляет существующую модель
int RDK_CALL Model_Create(const char *classname)
{
 return RdkCoreManager.GetEngineLock()->Model_Create(classname);
}

int RDK_CALL MModel_Create(int channel_index, const char *classname)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_Create(classname);
}

// Очищает модель
int RDK_CALL Model_Clear(void)
{
 return RdkCoreManager.GetEngineLock()->Model_Clear();
}

int RDK_CALL MModel_Clear(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_Clear();
}


// Проверяет, существует ли модель
bool RDK_CALL Model_Check(void)
{
 if(!RdkCoreManager.GetEngine())
  return false;
 return RdkCoreManager.GetEngineLock()->Model_Check();
}

bool RDK_CALL MModel_Check(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return false;

 if(!RdkCoreManager.GetEngine(channel_index))
  return false;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_Check();
}

// Проверяет, существует ли в модели компонент с именем stringid)
bool RDK_CALL Model_CheckComponent(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_CheckComponent(stringid);
}

bool RDK_CALL MModel_CheckComponent(int channel_index, const char* stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return false;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_CheckComponent(stringid);
}

// Добавляет в выбранный контейнер модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'classname'
// если stringid - пустая строка, то добавляет в саму модель
// Возвращает имя компонента в случае успеха
const char* RDK_CALL Model_AddComponent(const char* stringid, const char *classname)
{
 return RdkCoreManager.GetEngineLock()->Model_AddComponent(stringid, classname);
}

const char* RDK_CALL MModel_AddComponent(int channel_index, const char* stringid, const char *classname)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_AddComponent(stringid, classname);
}

// Удаляет из выбранного контейнера модели с идентификатором 'stringid' экземпляр
// контейнера с заданным 'name'
// если stringid - пустая строка, то удаляет из самой модели
int RDK_CALL Model_DelComponent(const char* stringid, const char *name)
{
 return RdkCoreManager.GetEngineLock()->Model_DelComponent(stringid, name);
}

int RDK_CALL MModel_DelComponent(int channel_index, const char* stringid, const char *name)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_DelComponent(stringid, name);
}

/// Перемещает компоненту в другой компонент
/// Если comp не принадлежит этому компоненту, или target имеет отличный от
/// этого компонента storage, или target не может принять в себя компонент
/// то возвращает false и не делает ничего
int RDK_CALL Model_MoveComponent(const char* component, const char* target)
{
 return RdkCoreManager.GetEngineLock()->Model_MoveComponent(component, target);
}

int RDK_CALL MModel_MoveComponent(int channel_index, const char* component, const char* target)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_MoveComponent(component, target);
}

// Возвращает число всех компонент в заданного компоненте 'stringid'
// если stringid - пустая строка, то возвращает число всех компонент модели
int RDK_CALL Model_GetNumComponents(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_GetNumComponents(stringid);
}

// Возвращает массив всех id заданного компонента 'stringid'
// если stringid - пустая строка, то возвращает массив всех id модели
int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentsList(stringid, buffer);
}

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ','
const char* RDK_CALL Model_GetComponentsNameList(const char* stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentsNameList(stringid);
}

const char* RDK_CALL MModel_GetComponentsNameList(int channel_index, const char* stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentsNameList(stringid);
}

// Возвращает строку, содержащую список имен всех компонент заданного компонента 'stringid'
// имена разделяются сипволом ',' и имеющих имя класса 'class_name'
// Если find_all == true то поиск ведется и во всех сабкомпонентах
const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{

 return RdkCoreManager.GetEngineLock()->Model_FindComponentsByClassName(stringid,class_name,find_all);
}

// Перемещает компонент с текущим индексом index или именем 'name' вверх или
// вниз по списку на заданное число элементов
// Применяется для изменения порядка расчета компонент
// Если значение 'step' выводит за границы массива, то компонент устанавливается
// на эту границу
int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step)
{

 return RdkCoreManager.GetEngineLock()->Model_ChangeComponentPosition(stringid, step);
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
const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetConnectorsList(stringid, sublevel, owner_level_stringid);
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
const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetItemsList(stringid, sublevel, owner_level_stringid);
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
const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_GetNetsList(stringid, sublevel, owner_level_stringid);
}

// Возвращает имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
const char* RDK_CALL Model_GetComponentName(const char* stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentName(stringid);
}

// Возвращает длинное имя компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
const char* RDK_CALL Model_GetComponentLongName(const char* stringid, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentLongName(stringid,owner_level_stringid);
}

const char* RDK_CALL MModel_GetComponentLongName(int channel_index, const char* stringid, const char* owner_level_stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentLongName(stringid,owner_level_stringid);
}

// Возвращает длинный id компонента по заданному 'stringid'
// если stringid - пустая строка, то возвращает имя модели
// Память выделяется и освобождается внутри dll
// Имя формируется до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имя формируется до уровня текущего компонента
const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentLongId(stringid,owner_level_stringid);
}

// Возвращает имя класса компонента в хранилище по длинному 'stringid'
// если stringid - пустая строка, то возвращает имя класса модели
const char* RDK_CALL Model_GetComponentClassName(const char* stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentClassName(stringid);
}

const char* RDK_CALL MModel_GetComponentClassName(int channel_index, const char* stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentClassName(stringid);
}


// Возвращает список свойств компонента разделенный запятыми
const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesList(stringid,type_mask);
}

// Возвращает список имен и индексов свойств компонента разделенный запятыми
// каждый элемент имеет вид имя_свойства:индекс_входа(выхода)
const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesLookupList(stringid,type_mask);
}

// Возвращает свойства компонента по идентификатору
const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentProperties(stringid,type_mask);
}

// Возвращает свойства компонента по идентификатору с описаниями
const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesEx(stringid, type_mask);
}

// Возвращает выборочные свойства компонента по идентификатору
const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// Возвращает значение свойства компонента по идентификатору компонента и имени свойства
const char * RDK_CALL Model_GetComponentPropertyValue(const char *stringid, const char *paramname)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertyValue(stringid,paramname);
}

const char * RDK_CALL MModel_GetComponentPropertyValue(int channel_index, const char *stringid, const char *paramname)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;


 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentPropertyValue(stringid,paramname);
}

// Устанавливает свойства компонента по идентификатору
int RDK_CALL Model_SetComponentProperties(const char *stringid, const char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentProperties(stringid, buffer);
}

RDK_LIB_TYPE int RDK_CALL MModel_SetComponentProperties(int engine_index, const char *stringid, const char* buffer)
{
 if(engine_index<0 || engine_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(engine_index)->Model_SetComponentProperties(stringid,buffer);
}

// Устанавливает значение свойства компонента по идентификатору компонента и имени свойства
int RDK_CALL Model_SetComponentPropertyValue(const char *stringid, const char *paramname, const char *buffer)
{
 return RdkCoreManager.GetEngineLock()->Model_SetComponentPropertyValue(stringid,paramname,buffer);
}

int RDK_CALL MModel_SetComponentPropertyValue(int channel_index, const char *stringid, const char *paramname, const char *buffer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentPropertyValue(stringid,paramname,buffer);
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// включая этот компонент
int RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetGlobalComponentPropertyValue(stringid,class_stringid, paramname,buffer);
}

// Устанавливает значение свойства всем дочерним компонентам компонента stringid, производным от класса class_stringid
// и владельцем, производным от класса 'class_owner_stringid' включая этот компонент
int RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetGlobalOwnerComponentPropertyValue(stringid, class_stringid, class_owner_stringid, paramname,buffer);
}

// Возвращает указатель void* на данные свойства компонента
const void* RDK_CALL Model_GetComponentPropertyData(const char *stringid, const char *property_name)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertyData(stringid, property_name);
}

const void* RDK_CALL MModel_GetComponentPropertyData(int channel_index, const char *stringid, const char *property_name)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentPropertyData(stringid, property_name);
}

// Копирует данные 'data' в заданное свойство компонента
int RDK_CALL Model_SetComponentPropertyData(const char *stringid, const char *property_name, const void *data)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentPropertyData(stringid, property_name, data);
}

int RDK_CALL MModel_SetComponentPropertyData(int channel_index, const char *stringid, const char *property_name, const void *data)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentPropertyData(stringid, property_name, data);
}

// Возвращает параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentParameters(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

const char * RDK_CALL MModel_GetComponentParameters(int channel_index, const char *stringid, unsigned int type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentProperties(stringid,type_mask & 0xFFFFFF01);
}

// Возвращает выборочные параметры компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// Возвращает параметры компонента по идентификатору с описаниями
const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesEx(stringid, type_mask & 0xFFFFFF01);
}

// Возвращает значение параметра компонента по идентификатору компонента и имени параметра
const char * RDK_CALL Model_GetComponentParameterValue(const char *stringid, const char *paramname)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertyValue(stringid,paramname);
}

const char * RDK_CALL MModel_GetComponentParameterValue(int channel_index, const char *stringid, const char *paramname)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentPropertyValue(stringid,paramname);
}

// Устанавливает параметры компонента по идентификатору
int RDK_CALL Model_SetComponentParameters(const char *stringid, const char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentParameters(int channel_index, const char *stringid, const char* buffer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentProperties(stringid, buffer);
}

// Устанавливает значение параметра компонента по идентификатору компонента и имени параметра
int RDK_CALL Model_SetComponentParameterValue(const char *stringid, const char *paramname, const char *buffer)
{

 RdkCoreManager.GetEngineLock()->Model_SetComponentPropertyValue(stringid,paramname,buffer);
 return 0;
}

int RDK_CALL MModel_SetComponentParameterValue(int channel_index, const char *stringid, const char *paramname, const char *buffer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentPropertyValue(stringid,paramname,buffer);
 return 0;
}

// Связывает выбранные контейнеры друг с другом
int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return RdkCoreManager.GetEngineLock()->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}

int RDK_CALL Model_CreateLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return RdkCoreManager.GetEngineLock()->Model_CreateLink(stringid1, item_property_name, stringid2, connector_property_name);
}

int RDK_CALL Model_CreateLinkByNameEx(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name, int connector_c_index)
{
 return RdkCoreManager.GetEngineLock()->Model_CreateLink(stringid1, item_property_name, stringid2, connector_property_name,connector_c_index);
}

// Связывает все компоненты выбранного компонента по возрастанию id в формате: 0 выход к 0 входу
int RDK_CALL Model_ChainLinking(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_ChainLinking(stringid);
}

// Связывает все компоненты выбранного компонента параллельно, подключая их к необходимому числу выходов модели
// Используется для тестирования производительности
int RDK_CALL Model_ParallelLinking(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_ParallelLinking(stringid);
}

// Разрывает выбранную связь
int RDK_CALL Model_BreakLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakLink(stringid1, output_number, stringid2, input_number);
}

int RDK_CALL Model_BreakLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakLink(stringid1, item_property_name, stringid2, connector_property_name);
}

// Разрывает все связи
int RDK_CALL Model_BreakAllLinks(void)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakAllLinks();
}

// Разрывает связь ко входу connector_index коннектора 'connectorid'
int RDK_CALL Model_BreakConnectorLink(const char* connectorname, int connector_index)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakConnectorLink(connectorname, connector_index);
}

// Разрывает все входные и выходные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentLinks(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakAllComponentLinks(stringid);
}

// Разрывает все входные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakAllComponentInputLinks(stringid);
}

// Разрывает все выходные связи выбранного контейнера
int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_BreakAllComponentOutputLinks(stringid);
}

// Проверяет, существует ли заданна связь
bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{

 return RdkCoreManager.GetEngineLock()->Model_CheckLink(stringid1, output_number, stringid2, input_number);
}

bool RDK_CALL Model_CheckLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return RdkCoreManager.GetEngineLock()->Model_CheckLink(stringid1, item_property_name, stringid2, connector_property_name);
}

// Возращает все связи внутри компонента stringid в виде xml в буфер buffer
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInternalLinks(stringid, owner_level_stringid);
}

// Устанавливает все связи внутри компонента stringid из строки xml в буфере buffer
// Имена применяются с уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то применяется уровень текущего компонента
int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentInternalLinks(stringid,buffer, owner_level_stringid);
}

// Возращает все входные связи к компоненту stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputLinks(stringid,owner_level_stringid, sublevel);
}

// Возращает все выходные связи из компонента stringid в виде xml в буфер buffer
// если 'sublevel' == -2, то возвращает связи всех элементов включая
// все вложенные сети и сам опрашиваемый компонент.
// если 'sublevel' == -1, то возвращает связи всех подсетей включая
// все вложенные сети.
// если 'sublevel' == 0, то возвращает связи подсетей только этой сети
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputLinks(stringid,owner_level_stringid, sublevel);
}

// Возращает все связи между двумя компонентами в виде xml в буфер buffer
// включая связи этого компонента
// если 'sublevel' == -1, то возвращает также все связи между объектом и любым дочерним компонентом
// второго объекта. Работает симметрично в обе стороны.
// если 'sublevel' == 0, то возвращает связи только между этими объектами
// Имена формируются до уровня компонента owner_level_stringid
// Если owner_level_stringid не задан, то имена формируются до уровня текущего компонента
const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPersonalLinks(stringid,owner_level_stringid);
}

// Возвращает состояние компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentState(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

const char * RDK_CALL MModel_GetComponentState(int channel_index, const char *stringid, unsigned int type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// Возвращает выборочные данные состояния компонента по идентификатору
// Память для buffer должна быть выделена!
const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// Возвращает значение переменной состояния компонента по идентификатору компонента и имени переменной
const char * RDK_CALL Model_GetComponentStateValue(const char *stringid, const char *statename)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertyValue(stringid,statename);
}

const char * RDK_CALL MModel_GetComponentStateValue(int channel_index, const char *stringid, const char *statename)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetComponentPropertyValue(stringid,statename);
}

// Устанавливает состояние компонента по идентификатору
int RDK_CALL Model_SetComponentState(const char *stringid, const char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_SetComponentState(int channel_index, const char *stringid, const char* buffer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentProperties(stringid, buffer);
}

// Устанавливает значение переменной состояния компонента по идентификатору компонента и имени переменной
int RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentPropertyValue(stringid,statename,buffer);
}

// Возвращает число входов у компонента
int RDK_CALL Model_GetComponentNumInputs(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentNumInputs(stringid);
}

// Возвращает размер входа компонента в числе элементов
int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputDataSize(stringid, index);
}

// Возвращает размер элемента входа в байтах
int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputElementSize(stringid, index);
}

// Возвращает размер входа компонента в байтах элементов
int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputByteSize(stringid, index);
}

// Возвращает указатель на данные входа как на массив байт
// Только для чтения!
unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputData(stringid, index);
}

// Возвращает число выходов у компонента
int RDK_CALL Model_GetComponentNumOutputs(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentNumOutputs(stringid);
}

// Возвращает размер выхода компонента в числе элементов
int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputDataSize(stringid, index);
}

// Возвращает размер элемента выхода в байтах
//int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index)
//{
//
// return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputElementSize(stringid, index);
//}

// Возвращает размер выхода компонента в байтах элементов
int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputByteSize(stringid, index);
}

// Возвращает указатель на данные выхода как на массив байт
// Только для чтения!
unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputData(stringid, index);
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
const char * RDK_CALL Model_SaveComponent(const char *stringid, unsigned int params_type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponent(stringid, params_type_mask);
}

const char * RDK_CALL MModel_SaveComponent(int channel_index, const char *stringid, unsigned int params_type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SaveComponent(stringid, params_type_mask);
}

// Сохраняет все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния в xml
int RDK_CALL Model_SaveComponentToFile(const char *stringid, const char* file_name, unsigned int params_type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentToFile(stringid, file_name, params_type_mask);
}

int RDK_CALL MModel_SaveComponentToFile(int channel_index, const char *stringid, const char* file_name, unsigned int params_type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SaveComponentToFile(stringid, file_name, params_type_mask);
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int RDK_CALL Model_LoadComponent(const char *stringid, const char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponent(stringid, buffer);
}

int RDK_CALL MModel_LoadComponent(int channel_index, const char *stringid, const char* buffer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_LoadComponent(stringid, buffer);
}

// Загружает все внутренние данные компонента, и всех его дочерних компонент, исключая
// переменные состояния из xml
int RDK_CALL Model_LoadComponentFromFile(const char *stringid, const char* file_name)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponentFromFile(stringid, file_name);
}

int RDK_CALL MModel_LoadComponentFromFile(int channel_index, const char *stringid, const char* file_name)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_LoadComponentFromFile(stringid, file_name);
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask);
}

// Сохраняет все свойства компонента и его дочерних компонент в xml
int RDK_CALL Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentPropertiesToFile(stringid, file_name, type_mask);
}

// Загружает все свойства компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponentProperties(stringid, buffer);
}

// Загружает все свойства компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponentPropertiesFromFile(stringid, file_name);
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// Сохраняет все параметры компонента и его дочерних компонент в xml
const char * RDK_CALL MModel_SaveComponentParameters(int channel_index, const char *stringid, unsigned int type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// Загружает все параметры компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentParameters(const char *stringid, const char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_LoadComponentParameters(int channel_index, const char *stringid, const char* buffer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_LoadComponentProperties(stringid, buffer);
}

// Сохраняет состояние компонента и его дочерних компонент в xml
const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// Загружает состояние компонента и его дочерних компонент из xml
int RDK_CALL Model_LoadComponentState(const char *stringid, char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponentProperties(stringid, buffer);
}

int RDK_CALL MModel_LoadComponentState(int channel_index, const char *stringid, char* buffer)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_LoadComponentProperties(stringid, buffer);
}


// Сохраняет внутренние данные компонента, и его _непосредственных_ дочерних компонент, исключая
// переменные состояния в xml
const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentDrawInfo(stringid);
}

// Управляет шагом счета модели по умолчанию
unsigned int RDK_CALL Model_GetDefaultTimeStep(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetDefaultTimeStep();
}

int RDK_CALL Model_SetDefaultTimeStep(unsigned int value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetDefaultTimeStep(value);
}

// Управляет шагом счета компонента
unsigned int RDK_CALL Model_GetTimeStep(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetTimeStep(stringid);
}

int RDK_CALL Model_SetTimeStep(const char *stringid, unsigned int value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetTimeStep(stringid, value);
}

// Устанавливает шаг счета компонента и всех его дочерних компонент
int RDK_CALL Model_SetGlobalTimeStep(const char *stringid, unsigned int value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetGlobalTimeStep(stringid, value);
}

// Возвращает текущее время модели
unsigned long long RDK_CALL Model_GetTime(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetTime();
}

unsigned long long RDK_CALL MModel_GetTime(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetTime();
}

double RDK_CALL Model_GetDoubleTime(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetDoubleTime();
}

double RDK_CALL MModel_GetDoubleTime(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0.0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetDoubleTime();
}

// Устанавливает текущее время модели
int RDK_CALL Model_SetTime(unsigned long long value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetTime(value);
}

// Возвращает реальное время
unsigned long long RDK_CALL Model_GetRealTime(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetRealTime();
}

double RDK_CALL Model_GetDoubleRealTime(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetDoubleRealTime();
}

double RDK_CALL MModel_GetDoubleRealTime(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0.0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetDoubleRealTime();
}

// Устанавливает реальное время
int RDK_CALL Model_SetRealTime(unsigned long long value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetRealTime(value);
}

// Увеличивает реальное время на заданную величину
int RDK_CALL Model_IncreaseRealTime(unsigned long long value)
{

 return RdkCoreManager.GetEngineLock()->Model_IncreaseRealTime(value);
}

// Возвращает мгновенный шаг в реальном времени
unsigned long long RDK_CALL Model_GetRealTimeStep(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetRealTimeStep();
}

double RDK_CALL Model_GetDoubleRealTimeStep(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetDoubleRealTimeStep();
}

double RDK_CALL Model_GetDoubleSourceTime(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetDoubleSourceTime();
}

double RDK_CALL MModel_GetDoubleSourceTime(int channel_index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0.0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetDoubleSourceTime();
}

// Устанавливает время внешних источников данных
int RDK_CALL Model_SetDoubleSourceTime(double value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetDoubleSourceTime(value);
}

int RDK_CALL MModel_SetDoubleSourceTime(int channel_index, double value)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetDoubleSourceTime(value);
}

int RDK_CALL Model_SetDoubleSourceTimeAll(double value)
{
 int res=RDK_SUCCESS;
 for(int i=0;i<Core_GetNumChannels();i++)
 {
  int temp_res=RdkCoreManager.GetEngineLock(i)->Model_SetDoubleSourceTime(value);
  if(temp_res != RDK_SUCCESS)
   res=temp_res;
 }
 return res;
}

// Возвращает время расчета компонента без времени расчета дочерних компонент (мс)
unsigned long long RDK_CALL Model_GetStepDuration(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_GetStepDuration(stringid);
}

unsigned long long RDK_CALL MModel_GetStepDuration(int channel_index, const char *stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetStepDuration(stringid);
}

// Возвращает время, затраченное на обработку объекта
// (вместе со времени обсчета дочерних объектов) (мс)
unsigned long long RDK_CALL Model_GetFullStepDuration(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetFullStepDuration(stringid);
}

unsigned long long RDK_CALL MModel_GetFullStepDuration(int channel_index, const char *stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetFullStepDuration(stringid);
}

// Возвращает мгновенное быстродействие, равное отношению
// полного затраченного времени к ожидаемому времени шага счета
double RDK_CALL Model_GetInstantPerformance(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_GetInstantPerformance(stringid);
}

double RDK_CALL MModel_GetInstantPerformance(int channel_index, const char *stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0.0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetInstantPerformance(stringid);
}

// Время, прошедшее между двумя последними итерациями счета
unsigned long long RDK_CALL Model_GetInterstepsInterval(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_GetInterstepsInterval(stringid);
}

unsigned long long RDK_CALL MModel_GetInterstepsInterval(int channel_index, const char *stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_GetInterstepsInterval(stringid);
}
// --------------------------

// --------------------------
// Методы управления исключениями
// ----------------------------
// Управление функцией-обработчиком исключений
void* RDK_CALL Engine_GetExceptionHandler(void)
{

 return Log_GetExceptionHandler();
}

void* RDK_CALL MEngine_GetExceptionHandler(int channel_index)
{
 return MLog_GetExceptionHandler(channel_index);
}

int RDK_CALL Engine_SetExceptionHandler(void* value)
{
 return Log_SetExceptionHandler(value);
}

int RDK_CALL MEngine_SetExceptionHandler(int channel_index, void* value)
{
 return MLog_SetExceptionHandler(channel_index, value);
}

// Возвращает массив строк лога
const char* RDK_CALL Engine_GetLog(int &error_level)
{
 return Log_GetLog(error_level);
}

const char* RDK_CALL MEngine_GetLog(int channel_index, int &error_level)
{
 return MLog_GetLog(channel_index, error_level);
}

// Записывает в лог новое сообщение
int RDK_CALL Engine_LogMessage(int log_level, const char *message)
{
 return Log_LogMessage(log_level, message);
}

int RDK_CALL MEngine_LogMessage(int channel_index, int log_level, const char *message)
{
 return MLog_LogMessage(channel_index, log_level, message);
}

// Записывает в лог новое сообщение с кодом ошибки
int RDK_CALL Engine_LogMessageEx(int log_level, const char *message, int error_event_number)
{
 return Log_LogMessageEx(log_level, message, error_event_number);
}

int RDK_CALL MEngine_LogMessageEx(int channel_index, int log_level, const char *message, int error_event_number)
{
 return MLog_LogMessageEx(channel_index, log_level, message, error_event_number);
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* RDK_CALL Engine_GetUnreadLog(int &error_level, int &number, unsigned long long &time)
{
 return Log_GetUnreadLog(error_level, number, time);
}

const char* RDK_CALL MEngine_GetUnreadLog(int channel_index, int &error_level, int &number, unsigned long long &time)
{
 return MLog_GetUnreadLog(channel_index, error_level, number, time);
}

const char* RDK_CALL Engine_GetUnreadLogUnsafe(int &error_level, int &number, unsigned long long &time)
{
 return Log_GetUnreadLogUnsafe(error_level, number, time);
}

const char* RDK_CALL MEngine_GetUnreadLogUnsafe(int channel_index, int &error_level, int &number, unsigned long long &time)
{
 return MLog_GetUnreadLogUnsafe(channel_index, error_level, number, time);
}

/// Возвращает число непрочитанных строк лога
int RDK_CALL Engine_GetNumUnreadLogLines(void)
{
 return Log_GetNumUnreadLogLines();
}

int RDK_CALL MEngine_GetNumUnreadLogLines(int channel_index)
{
 return MLog_GetNumUnreadLogLines(channel_index);
}

/// Возвращает число строк лога
int RDK_CALL Engine_GetNumLogLines(void)
{
 return Log_GetNumLogLines();
}

int RDK_CALL MEngine_GetNumLogLines(int channel_index)
{
 return MLog_GetNumLogLines(channel_index);
}


/// Очищает лог прочитанных сообщений
int RDK_CALL Engine_ClearReadLog(void)
{
 return Log_ClearReadLog();
}

int RDK_CALL MEngine_ClearReadLog(int channel_index)
{
 return MLog_ClearReadLog(channel_index);
}
// ----------------------------


// --------------------------
// Методы управления средой
// --------------------------
// Задает число входов среды
void RDK_CALL Env_SetNumInputImages(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_SetNumInputImages(number);
}

void RDK_CALL MEnv_SetNumInputImages(int channel_index, int number)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetNumInputImages(number);
}

// Задает число выходов среды
void RDK_CALL Env_SetNumOutputImages(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_SetNumOutputImages(number);
}

void RDK_CALL MEnv_SetNumOutputImages(int channel_index, int number)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetNumOutputImages(number);
}

// Задает число входов среды
int RDK_CALL Env_GetNumInputImages(void)
{

 return RdkCoreManager.GetEngineLock()->Env_GetNumInputImages();
}

// Задает число выходов среды
int RDK_CALL Env_GetNumOutputImages(void)
{

 return RdkCoreManager.GetEngineLock()->Env_GetNumInputImages();
}

// Задает разрешение по умолчанию (рабочее разрешение)
void RDK_CALL Env_SetInputRes(int number, int width, int height)
{

 return RdkCoreManager.GetEngineLock()->Env_SetInputRes(number, width, height);
}

void RDK_CALL MEnv_SetInputRes(int channel_index, int number, int width, int height)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetInputRes(number, width, height);
}

// Задает данные изображения
void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{

 return RdkCoreManager.GetEngineLock()->Env_SetInputImage(number, image, width, height,cmodel);
}

// Задает флаг отражения входного изображения вокруг горизонтальной оси
RDK_LIB_TYPE void Env_SetReflectionXFlag(bool value)
{

 return RdkCoreManager.GetEngineLock()->Env_SetReflectionXFlag(value);
}

RDK_LIB_TYPE void MEnv_SetReflectionXFlag(int channel_index, bool value)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return;

 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetReflectionXFlag(value);
}


// Возвращает разрешение по умолчанию (рабочее разрешение)
int RDK_CALL Env_GetInputImageWidth(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_GetInputImageWidth(number);
}

int RDK_CALL Env_GetInputImageHeight(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_GetInputImageHeight(number);
}

int RDK_CALL Env_GetInputImageColorModel(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_GetInputImageColorModel(number);
}

// Возвращает текущее выходное разрешение
int RDK_CALL Env_GetOutputImageWidth(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_GetOutputImageWidth(number);
}

int RDK_CALL Env_GetOutputImageHeight(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_GetOutputImageHeight(number);
}

int RDK_CALL Env_GetOutputImageColorModel(int number)
{

 return RdkCoreManager.GetEngineLock()->Env_GetOutputImageColorModel(number);
}

unsigned char* RDK_CALL Env_GetInputImage(int index)
{

 return RdkCoreManager.GetEngineLock()->Env_GetInputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImage(int index)
{

 return RdkCoreManager.GetEngineLock()->Env_GetOutputImage(index);
}

unsigned char* RDK_CALL Env_GetOutputImageY8(int index)
{

 return RdkCoreManager.GetEngineLock()->Env_GetOutputImageY8(index);
}
// --------------------------

// --------------------------
// Методы управления графической моделью
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
// возвращаемое значение имеет фактический тип RDK::MDMatrix*
// если выход не содержит данных такого типа, то возвращает 0
const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputAsMatrix(stringid, property_name);
}

const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int  index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputAsMatrix(stringid, index);
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutput(const char *stringid, const char *property_name)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutput(stringid, property_name);
}

const /* RDK::UBitmap* */ void* RDK_CALL Model_GetComponentOutputByIndex(const char *stringid, int index)
{
 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutput(stringid, index);
}
const /* RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentOutput(int channel_index, const char *stringid, const char *property_name)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutput(stringid, property_name);
}

const /* RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentOutputByIndex(int channel_index,const char *stringid, int index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutput(stringid, index);
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutput(const char *stringid, const char *property_name)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapOutputByIndex(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, index);
}

const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutput(int channel_index,const char *stringid, const char *property_name)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, property_name);

}
const /*RDK::UBitmap* */ void* RDK_CALL MModel_GetComponentBitmapOutputByIndex(int channel_index,const char *stringid, int index)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;
 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapOutput(stringid, index);
}

// Возвращает указатель на вход с индексом 'index' компонента 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapInput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapInput(stringid, index);
}

/// Копирует данные о разрешении изображения выхода с индексом 'index' компонента 'id'
/// в стрктуру bmp_param
int RDK_CALL Model_CopyComponentBitmapOutputHeader(const char *stringid, const char *property_name, /*RDK::UBitmapParam* */ void* bmp_param)
{
 return RdkCoreManager.GetEngineLock()->Model_CopyComponentBitmapOutputHeader(stringid, property_name, (RDK::UBitmapParam*)bmp_param);
}

int RDK_CALL MModel_CopyComponentBitmapOutputHeader(int channel_index, const char *stringid, const char *property_name, /*RDK::UBitmapParam* */ void* bmp_param)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_CopyComponentBitmapOutputHeader(stringid, property_name, (RDK::UBitmapParam*)bmp_param);
}

int RDK_CALL Model_CopyComponentBitmapOutputHeaderByIndex(const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param)
{
 return RdkCoreManager.GetEngineLock()->Model_CopyComponentBitmapOutputHeaderByIndex(stringid, index, (RDK::UBitmapParam*)bmp_param);
}

int RDK_CALL MModel_CopyComponentBitmapOutputHeaderByIndex(int channel_index, const char *stringid, int index, /*RDK::UBitmapParam* */ void* bmp_param)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_CopyComponentBitmapOutputHeaderByIndex(stringid, index, (RDK::UBitmapParam*)bmp_param);
}

/// Копирует изображение выхода с индексом 'index' компонента 'id'
/// метод предполагает, что bmp уже имеет выделенную память под изобржение требуемого размера
int RDK_CALL Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, /*RDK::UBitmap* */ void* bmp)
{
 return RdkCoreManager.GetEngineLock()->Model_CopyComponentBitmapOutput(stringid, property_name, (RDK::UBitmap*)bmp);
}

RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, /*RDK::UBitmap**/void* bmp)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

  return RdkCoreManager.GetEngineLock(channel_index)->Model_CopyComponentBitmapOutput(stringid, property_name, (RDK::UBitmap*)bmp);
}

int RDK_CALL Model_CopyComponentBitmapOutputByIndex(const char *stringid, int index, /*RDK::UBitmap* */ void* bmp)
{
 return RdkCoreManager.GetEngineLock()->Model_CopyComponentBitmapOutput(stringid, index, (RDK::UBitmap*)bmp);
}

int RDK_CALL MModel_CopyComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index, /*RDK::UBitmap* */ void* bmp)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_CopyComponentBitmapOutput(stringid, index, (RDK::UBitmap*)bmp);
}

// Замещает изображение выхода с индексом 'index' компонента 'id'
int RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapOutputUnsafe(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngine(channel_index)->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

int RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

// Замещает изображение входа с индексом 'index' компонента 'id'
int RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapInput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}

int RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapInput(stringid, index, reinterpret_cast<const RDK::UBitmap* const >(bmp),reflect);
}
// --------------------------

/* ************************************************************************** */




// ----------------------------
// Внутренние методы инициализации
// ----------------------------
// Обработчик исключений библиотеки
// Должен быть вызван в глобальном обработчике пользовательского ПО
int RDK_CALL ExceptionDispatcher(void *exception)
{
 if(!RdkCoreManager.GetEngine())
  return 1;

 if(!exception)
  return 2;

 RDK::UException *exc=reinterpret_cast<RDK::UException*>(exception);
 RdkCoreManager.GetEngineLock()->ProcessException(*exc);

 return 0;
}
// ----------------------------

#ifndef _MSC_VER
namespace boost {

void tss_cleanup_implemented(void)
{
	/*
	This function's sole purpose is to cause a link
error in cases where
	automatic tss cleanup is not implemented by
Boost.Threads as a
	reminder that user code is responsible for calling
the necessary
	functions at the appropriate times (and for
implementing an a
	tss_cleanup_implemented() function to eliminate the
linker's
	missing symbol error).
	If Boost.Threads later implements automatic tss
cleanup in cases
	where it currently doesn't (which is the plan), the
duplicate
	symbol error will warn the user that their custom
solution is no
	longer needed and can be removed.
	*/
}

}
#endif

void tss_cleanup_implemented(void)
{
	/*
	This function's sole purpose is to cause a link
error in cases where
	automatic tss cleanup is not implemented by
Boost.Threads as a
	reminder that user code is responsible for calling
the necessary
	functions at the appropriate times (and for
implementing an a
	tss_cleanup_implemented() function to eliminate the
linker's
	missing symbol error).
	If Boost.Threads later implements automatic tss
cleanup in cases
	where it currently doesn't (which is the plan), the
duplicate
	symbol error will warn the user that their custom
solution is no
	longer needed and can be removed.
	*/
}


#endif

