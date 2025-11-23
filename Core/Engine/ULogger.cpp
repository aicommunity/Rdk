#include <fstream>
#include "ULogger.h"
#include "../../Deploy/Include/rdk_error_codes.h"

namespace RDK {

ULogger::ULogger(void)
{
 LogMutex=UCreateMutex();
 LogEnabledFlag=true;
}


ULogger::~ULogger(void)
{
 Clear();
 if(LogMutex)
  UDestroyMutex(LogMutex);
}

/// Путь до папки с логами
std::string ULogger::GetLogDir(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return LogDir;
}

bool ULogger::SetLogDir(const std::string &value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(LogDir == value)
  return true;

 LogDir=value;
 Clear();
 return true;
}

/// Режим логгирования
/// 0 - отключен
/// 1 - сохранять каждый канал в отдельный файл лога
/// 2 - сохранять все каналы в единый файл лога
std::string ULogger::GetSuffix(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return Suffix;
}

bool ULogger::SetSuffix(const std::string &value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(Suffix == value)
  return true;

 Suffix=value;
 return true;
}

/// Инициализирует лог
/// With glog, this ensures the log directory exists (glog initialization is done in UApplication::Init)
int ULogger::InitLog(void)
{
#ifdef RDK_USE_GLOG
 // With glog, directory creation is handled during glog initialization
 // Just ensure the directory exists if specified
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(!LogDir.Get().empty())
 {
  if(CreateNewDirectory(LogDir.Get().c_str()) != 0)
  {
   LogEnabledFlag=false;
   RdkDebuggerMessage(std::string("Failed to create log directory ")+LogDir.Get());
   return RDK_E_LOGGER_CANT_CREATE_LOG_PATH;
  }
 }
 return RDK_SUCCESS;
#else
 // Old implementation
 Clear();

 UGenericMutexExclusiveLocker lock(LogMutex);
 if(!LogDir.Get().empty())
 {
  if(CreateNewDirectory(LogDir.Get().c_str()) != 0)
  {
   LogEnabledFlag=false;
   RdkDebuggerMessage(std::string("Failed to create log directory ")+LogDir.Get());
   return RDK_E_LOGGER_CANT_CREATE_LOG_PATH;
  }
 }

 return RDK_SUCCESS;
#endif
}

/// Сохраняет строку в лог
/// With glog, this is a compatibility stub - actual logging is done through glog
int ULogger::WriteMessageToFile(const std::string &str)
{
#ifdef RDK_USE_GLOG
 // With glog, logging is handled through ProcessException/ProcessExceptionRaw
 // This method is kept for compatibility but does nothing
 // The message should already be logged via glog
 (void)str; // Suppress unused parameter warning
 return RDK_SUCCESS;
#else
 // Old implementation for fallback
 UGenericMutexExclusiveLocker lock(LogMutex);
 try
 {
  if(!EventsLogFile)
  {
   std::string file_name;
   time_t time_data;
   time(&time_data);
   file_name=RDK::get_text_time(time_data, '.', '_');
   EventsLogFile=new std::ofstream((LogDir.Get()+file_name+Suffix.Get()+".txt").c_str(),std::ios_base::out | std::ios_base::app);

   if(!EventsLogFile->is_open() || EventsLogFile->fail() || EventsLogFile->bad())
   {
	Clear();
	RdkDebuggerMessage(std::string("Failed to open log file ")+LogDir.Get()+file_name+Suffix.Get()+".txt");
	return RDK_E_LOGGER_CANT_CREATE_LOG_FILE;
   }
  }

  *EventsLogFile<<str<<std::endl;
//  EventsLogFile->flush();
  if(EventsLogFile->fail())
  {
   Clear();
   RdkDebuggerMessage(std::string("Failed to write log message"));
   return RDK_E_LOGGER_LOG_FILE_WRITE_ERROR;
  }
  return RDK_SUCCESS;
 }
 catch(...)
 {
  fstream crush_file("CoreLogCrush.txt",ios::out | ios::app);
  crush_file<<"ULogger::WriteMessageToFile unexcepted catch! "<<"Message="<<str<<endl;
 }
 return RDK_UNHANDLED_EXCEPTION;
#endif
}

/// Закрывает файлы с логами и удаляет связанные файловые переменные
int ULogger::Clear(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(EventsLogFile)
 {
  delete EventsLogFile;
  EventsLogFile=0;
 }

 LogEnabledFlag=true;
 return RDK_SUCCESS;
}

/// Возвращает true если файл записи логов открыт
/// With glog, this always returns true as glog handles file creation
bool ULogger::IsLogFileCreated(void) const
{
#ifdef RDK_USE_GLOG
 // With glog, files are always available (glog manages them)
 return true;
#else
 UGenericMutexExclusiveLocker lock(LogMutex);
 return (EventsLogFile)?true:false;
#endif
}


}

