#include <fstream>
#include "ULogger.h"
#include "../../Deploy/Include/rdk_error_codes.h"

namespace RDK {

ULogger::ULogger(void)
{
 LogEnabledFlag=true;
}


ULogger::~ULogger(void)
{
 Clear();
}

/// Путь до папки с логами
std::string ULogger::GetLogPath(void) const
{
 return LogPath;
}

bool ULogger::SetLogPath(const std::string &value)
{
 if(LogPath == value)
  return true;

 LogPath=value;
 return true;
}

/// Режим логгирования
/// 0 - отключен
/// 1 - сохранять каждый канал в отдельный файл лога
/// 2 - сохранять все каналы в единый файл лога
std::string ULogger::GetSuffix(void) const
{
 return Suffix;
}

bool ULogger::SetSuffix(const std::string &value)
{
 if(Suffix == value)
  return true;

 Suffix=value;
 return true;
}

/// Инициализирует лог
int ULogger::InitLog(void)
{
 Clear();

 if(!LogPath.Get().empty())
 {
  if(CreateNewDirectory(LogPath.Get().c_str()) != 0)
  {
   LogEnabledFlag=false;
   return RDK_E_LOGGER_CANT_CREATE_LOG_PATH;
  }
 }

 return RDK_SUCCESS;
}

/// Сохраняет строку в лог
int ULogger::LogMessage(const std::string &str)
{
 if(!EventsLogFile)
 {
  std::string file_name;
  time_t time_data;
  time(&time_data);
  file_name=RDK::get_text_time(time_data, '.', '_');
  EventsLogFile=new std::ofstream((LogPath.Get()+file_name+Suffix.Get()+".txt").c_str(),std::ios_base::out | std::ios_base::app);

  if(!EventsLogFile->is_open())
  {
   Clear();
   return RDK_E_LOGGER_CANT_CREATE_LOG_FILE;
  }
 }

 *EventsLogFile<<str<<std::endl;
 EventsLogFile->flush();
 if(EventsLogFile->fail())
 {
  Clear();
  return RDK_E_LOGGER_LOG_FILE_WRITE_ERROR;
 }

 return RDK_SUCCESS;
}

/// Закрывает файлы с логами и удаляет связанные файловые переменные
int ULogger::Clear(void)
{
 if(EventsLogFile)
 {
  delete EventsLogFile;
  EventsLogFile=0;
 }

 LogEnabledFlag=true;
 return RDK_SUCCESS;
}


}

