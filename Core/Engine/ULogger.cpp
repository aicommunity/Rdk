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

/// ���� �� ����� � ������
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

/// ����� ������������
/// 0 - ��������
/// 1 - ��������� ������ ����� � ��������� ���� ����
/// 2 - ��������� ��� ������ � ������ ���� ����
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

/// �������������� ���
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

/// ��������� ������ � ���
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

/// ��������� ����� � ������ � ������� ��������� �������� ����������
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

