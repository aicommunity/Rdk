#include "limits.h"
#include "ULoggerEnv.h"
#include "UContainer.h"
#include "UEnvironment.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULoggerEnv::ULoggerEnv(void)
{
 Environment=0;
 CurrentExceptionsLogSize=0;
 ExceptionHandler=0;
 ExceptionPreprocessor=0;
 ExceptionPostprocessor=0;

 LastReadExceptionLogIndex=0;
 MaxExceptionsLogSize=100000;

 LastErrorLevel=INT_MAX;
 DebugMode=false;
 ChannelIndex=0;

 LogIndex=1;

 EventsLogMode=false;

 DebugSysEventsMask=RDK_SYS_DEBUG_CALC | RDK_SYS_DEBUG_PROPERTIES;

 DebuggerMessageFlag=false;
 SetSuffix(std::string(" Ch")+sntoa(ChannelIndex,2));
}

ULoggerEnv::~ULoggerEnv(void)
{
 UnRegisterEnvironment();
 UnRegisterGlobalLogger();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Флаг включения режима отладки
bool ULoggerEnv::GetDebugMode(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return DebugMode;
}

bool ULoggerEnv::SetDebugMode(bool value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(DebugMode == value)
  return true;
 DebugMode=value;
 return true;
}

/// Маска системных событий для логирования
unsigned int ULoggerEnv::GetDebugSysEventsMask(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return DebugSysEventsMask;
}

bool ULoggerEnv::SetDebugSysEventsMask(unsigned int value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(DebugSysEventsMask == value)
  return true;

 DebugSysEventsMask=value;
 return true;
}

/// Возвращает флаг включения вывода лога в отладчик
bool ULoggerEnv::GetDebuggerMessageFlag(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return DebuggerMessageFlag;
}

/// Устанавливает флаг включения вывода лога в отладчик
bool ULoggerEnv::SetDebuggerMessageFlag(bool value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(DebuggerMessageFlag == value)
  return true;

 DebuggerMessageFlag=value;
 return true;
}

/// Флаг включения внутренней регистрации событий в лог-файл
/// true - регистрация включена
bool ULoggerEnv::GetEventsLogMode(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return EventsLogMode;
}

bool ULoggerEnv::SetEventsLogMode(bool value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 EventsLogMode=value;
 return true;
}

/// Индекс текущего канала в многоканальной библиотеке
int ULoggerEnv::GetChannelIndex(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return ChannelIndex;
}

bool ULoggerEnv::SetChannelIndex(int value)
{
 {
  UGenericMutexExclusiveLocker lock(LogMutex);
  if(ChannelIndex == value)
   return true;
  ChannelIndex=value;
 }
 if(value >=0)
  SetSuffix(std::string(" Ch")+sntoa(value,2));
 else
 if(value == -1)
  SetSuffix(std::string(" Sys"));
 else
  SetSuffix(std::string(" Glob"));
 Clear();
 return true;
}
// --------------------------

// --------------------------
// Методы управления исключениями
// --------------------------
/// Регистрация среды выполнения
bool ULoggerEnv::RegisterEnvironment(UEnvironment* env)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(Environment == env)
  return true;
 Environment=env;
 return true;
}

void ULoggerEnv::UnRegisterEnvironment(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 Environment=0;
}

/// Регистрация глобального логгера
bool ULoggerEnv::RegisterGlobalLogger(ULoggerEnv* global_logger)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(GlobalLogger == global_logger)
  return true;
 GlobalLogger=global_logger;
 return true;
}

void ULoggerEnv::UnRegisterGlobalLogger(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 GlobalLogger=0;
}

// Обрабатывает возникшее исключение
void ULoggerEnv::ProcessException(const UException &exception) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);

 const UException* processed_exception=&exception;
 UException temp_ex;
 if(Environment && ExceptionPreprocessor)
 {
  if(ExceptionPreprocessor(Environment,Environment->GetModel(), exception,temp_ex))
   processed_exception=&temp_ex;
 }

 UException result_message(*processed_exception);
 std::string ch_prefix;
 if(ChannelIndex>=0)
  ch_prefix=sntoa(ChannelIndex);
 else
  ch_prefix="S";
 result_message.SetMessage(ch_prefix+std::string("> ")+processed_exception->what());

 ProcessExceptionRaw(processed_exception->GetType(), result_message);
 if(GlobalLogger)
 {
  GlobalLogger->ProcessExceptionGlobal(processed_exception->GetType(), result_message);
 }
/* if(LastErrorLevel>processed_exception->GetType())
  LastErrorLevel=processed_exception->GetType();
 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize > MaxExceptionsLogSize)
 {
  int erase_size=CurrentExceptionsLogSize - MaxExceptionsLogSize-1;
  if(int(LogList.size())>erase_size)
  {
   for(int i=0;i<erase_size;i++)
	LogList.erase(LogList.begin());
  }
  else
   LogList.clear();
 }

 LogList[LogIndex++]=result_message;

 if(EventsLogMode) // Если включено, то сохраняем события в файл
 {
  const_cast<ULoggerEnv* const>(this)->LogMessage(result_message.GetMessage());  // TODO: Проверить на RDK_SUCCESS
 }*/

 if(DebuggerMessageFlag)
  RdkDebuggerMessage(result_message.GetMessage());

 if(ExceptionPostprocessor && Environment)
  ExceptionPostprocessor(Environment,Environment->GetModel(), *processed_exception); // TODO: Нет проверки возвращаемого значения

 if(ExceptionHandler)
  ExceptionHandler(ChannelIndex);
}

/// Обрабатывает возникшее исключение (Внутренний метод)
void ULoggerEnv::ProcessExceptionRaw(int type, const UException &exception) const
{
 if(LastErrorLevel>type)
  LastErrorLevel=type;
 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize > MaxExceptionsLogSize)
 {
  int erase_size=CurrentExceptionsLogSize - MaxExceptionsLogSize-1;
  if(int(LogList.size())>erase_size)
  {
   for(int i=0;i<erase_size;i++)
    LogList.erase(LogList.begin());
  }
  else
   LogList.clear();
 }

 LogList[LogIndex++]=exception;

 if(EventsLogMode) // Если включено, то сохраняем события в файл
 {
  const_cast<ULoggerEnv* const>(this)->WriteMessageToFile(exception.GetMessage());  // TODO: Проверить на RDK_SUCCESS
 }

}

/// Обрабатывает возникшее исключение в режиме гобального логгера
void ULoggerEnv::ProcessExceptionGlobal(int type, const UException &exception) const
{
 ProcessExceptionRaw(type,exception);
 if(ExceptionHandler)
  ExceptionHandler(ChannelIndex);
}


// Возвращает массив строк лога
const char* ULoggerEnv::GetLog(int &error_level) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 TempString.clear();
 std::map<unsigned, UException >::const_iterator I,J;
 I=LogList.begin(); J=LogList.end();
 for(;I != J;++I)
 {
  TempString+=I->second.GetMessage();
  TempString+="/r/n";
 }
 error_level=LastErrorLevel;
 LastErrorLevel=INT_MAX;
 return TempString.c_str();
}

/// Возвращает число строк лога
int ULoggerEnv::GetNumLogLines(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 return int(LogList.size());
}

/// Возвращает строку лога с индексом i
const char* ULoggerEnv::GetLogLine(int i, int &error_level, int &number, time_t &time) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 std::map<unsigned, UException >::const_iterator I=LogList.find(i);

 if(I == LogList.end())
 {
  TempString.clear();
  return TempString.c_str();
 }

 TempString=I->second.GetMessage();
 error_level=I->second.GetType();
 number=I->second.GetNumber();
 time=I->second.GetTime();
 return TempString.c_str();
}

/// Возвращает число непрочитанных строк лога
int ULoggerEnv::GetNumUnreadLogLines(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);

 std::map<unsigned, UException >::const_iterator I=LogList.find(LastReadExceptionLogIndex);
 if(I == LogList.end())
  return int(LogList.size());

 int size=0;
 for(;I!=LogList.end();++I)
  ++size;
 return size;
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* ULoggerEnv::GetUnreadLog(int &error_level, int &number, time_t &time)
{
 UGenericMutexSharedLocker lock(LogMutex);
 TempString.clear();
 error_level=INT_MAX;
 number=0;
 time=time_t();

 if(LogList.empty())
  return TempString.c_str();

 if(LastReadExceptionLogIndex == 0)
 {
  LastReadExceptionLogIndex=LogList.begin()->first;
  TempString=LogList.begin()->second.GetMessage();
  error_level=LogList.begin()->second.GetType();
  number=LogList.begin()->second.GetNumber();
  time=LogList.begin()->second.GetTime();
 }
 else
 {
  std::map<unsigned, UException >::const_iterator I=LogList.find(LastReadExceptionLogIndex);
  if(I != LogList.end())
  {
   ++I;
   if(I != LogList.end())
   {
	LastReadExceptionLogIndex=I->first;
	TempString=I->second.GetMessage();
	error_level=I->second.GetType();
	number=I->second.GetNumber();
	time=I->second.GetTime();
	return TempString.c_str();
   }
  }
  else
   LastReadExceptionLogIndex=LogList.begin()->first;
 }

 error_level=RDK_EX_UNKNOWN;
 return TempString.c_str();
}

// Управление функцией-обработчиком исключений
ULoggerEnv::PExceptionHandler ULoggerEnv::GetExceptionHandler(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return ExceptionHandler;
}

bool ULoggerEnv::SetExceptionHandler(PExceptionHandler value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(ExceptionHandler == value)
  return true;

 ExceptionHandler=value;
 return true;
}

// Управление функцией-предобработчиком исключений
ULoggerEnv::PExceptionPreprocessor ULoggerEnv::GetExceptionPreprocessor(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return ExceptionPreprocessor;
}

bool ULoggerEnv::SetExceptionPreprocessor(PExceptionPreprocessor value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(ExceptionPreprocessor == value)
  return true;
 ExceptionPreprocessor=value;
 return true;
}

/// Управление функцией-постобработки исключений
ULoggerEnv::PExceptionPostprocessor ULoggerEnv::GetExceptionPostprocessor(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return ExceptionPostprocessor;
}

bool ULoggerEnv::SetExceptionPostprocessor(PExceptionPostprocessor value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(ExceptionPostprocessor == value)
  return true;

 ExceptionPostprocessor=value;
 return true;
}

// Максимальное число хранимых исключений
// Если 0, то неограниченно
int ULoggerEnv::GetMaxExceptionsLogSize(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 return MaxExceptionsLogSize;
}

void ULoggerEnv::SetMaxExceptionsLogSize(int value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(MaxExceptionsLogSize == value)
  return;

 MaxExceptionsLogSize=value;
 if(MaxExceptionsLogSize>0 && CurrentExceptionsLogSize>MaxExceptionsLogSize)
 {
  //ExceptionsLog.erase(ExceptionsLog.begin(), ExceptionsLog.begin()+int(ExceptionsLog.size())-MaxExceptionsLogSize);
  CurrentExceptionsLogSize=MaxExceptionsLogSize;
 }
}

/// Очищает лог
void ULoggerEnv::ClearLog(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 LastReadExceptionLogIndex=0;
 CurrentExceptionsLogSize=0;
 LastErrorLevel=INT_MAX;
 LogList.clear();
 LogIndex=1;
}

/// Очищает лог прочитанных сообщений
void ULoggerEnv::ClearReadLog(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 std::map<unsigned, UException >::iterator I=LogList.find(LastReadExceptionLogIndex);
 if(I != LogList.end())
 {
  ++I;
  std::map<unsigned, UException >::iterator J=LogList.begin(),K;
  while(J != I)
  {
   K=J; ++K;
   LogList.erase(J);
   J=K;
  }
 }
 if(!LogList.empty())
  LastReadExceptionLogIndex=LogList.begin()->first;
 else
  LastReadExceptionLogIndex=0;
 CurrentExceptionsLogSize=LogList.size();
 LastErrorLevel=INT_MAX;
// TempLogString.clear();
}
  /*
void ULoggerEnv::LogMessage(const std::string &str)
{
 return ULogger::LogMessage(str);
}   */

// Вызов обработчика исключений среды для простой записи данных в лог
void ULoggerEnv::LogMessage(int msg_level, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, "", line, error_event_number);
}

void ULoggerEnv::LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, "", method_name, line, error_event_number);
}

void ULoggerEnv::LogMessageEx(int msg_level, const std::string &object_name, const std::string &line, int error_event_number)
{
 switch (msg_level)
 {
 case RDK_EX_FATAL:
 {
  EStringFatal exception(line,error_event_number);
 }
 break;

 case RDK_EX_ERROR:
 {
  EStringError exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;

 case RDK_EX_WARNING:
 {
  EStringWarning exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;

 case RDK_EX_INFO:
 {
  EStringInfo exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;

 case RDK_EX_DEBUG:
 {
  if(DebugMode)
  {
   EStringDebug exception(line,error_event_number);
   exception.SetObjectName(object_name);
   ProcessException(exception);
  }
 }
 break;

 case RDK_EX_APP:
 {
  EStringApp exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;
 }
}

void ULoggerEnv::LogMessageEx(int msg_level, const std::string &object_name, const std::string &method_name, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, object_name, method_name+std::string(" - ")+line, error_event_number);
}


/// Сброс логирования
void ULoggerEnv::Reset(void)
{
 if(EventsLogMode)
 {
  Clear();
  InitLog();
 }
}

/// Функция обеспечивает закрытие текущего файла логов и создание нового
void ULoggerEnv::RecreateEventsLogFile(void)
{
 Clear();
// std::string log_dir;
// if(EngineControl && EngineControl->GetApplication())
//  log_dir=EngineControl->GetApplication()->CalcCurrentLogDir();
// else
//  log_dir="EventsLog/";
//
// Logger.SetLogDir(log_dir);
 if(InitLog() != 0)
 {
//  EventsLogFlag=false;
  return;
 }
// else
// {
//  EventsLogFilePath=log_dir;
// }

 /// Сохраняем лог в файл если это необходимо
// if(!LogFlag)
//  EventsLogFlag=false;
// else
//  EventsLogFlag=true;

// CalculationNotInProgress->set();
}

/// Закрывает текущий лог
//void ULoggerEnv::CloseEventsLogFile(void)
//{
// if(!CalculationNotInProgress)
//  return;
// if(!CalculationNotInProgress->wait(100))
//  return;
// CalculationNotInProgress->reset();
//
// Logger.Clear();
// CalculationNotInProgress->set();
//}


}

