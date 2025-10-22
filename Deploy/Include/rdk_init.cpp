#ifndef RDK_INIT_CPP
#define RDK_INIT_CPP

#include <exception>
#include <sstream>
#include "rdk_init.h"

// Временная декларация, реализована в rdk_remote_stub.cpp
namespace RDK {
  const char* RemoteCallInternal(const char* request, int& return_value, int& channel_index);
}
#include "rdk.h"
#include "rdk_version.h"
//#include "rdk_rpc.cpp"
#include "rdk_engine_support.h"
#include "rdk_exceptions.h"
#include "../../Core/Engine/UEnvException.h"
#include "rdk_error_codes.h"
#include "../../Core/System/UGenericMutex.h"

namespace RDK {


/// ���������� RDK_UNHANDLED_EXCEPTION ���� �� ������� �������� ������ ����������
/// ����� ���������� RDK_EXCEPTION_CATCHED
int RDK_CALL ProcessException(int channel_index, const UException &ex)
{
 std::shared_ptr<ULoggerEnv> logger=RdkCoreManager.GetLogger(channel_index);
 if(!logger)
  return RDK_UNHANDLED_EXCEPTION;
 logger->ProcessException(ex);
 return RDK_EXCEPTION_CATCHED;
}

/// ���������� � ���������� ������� ���������, ���� result != RDK_SUCCESS
/// �������� ������ ���� ������� ���������� �����
void AssertDebugger(int result, const char* function, const char* file, int line)
{
 if(result != 0)
  RdkDebuggerMessage(std::string("Assertion in ")+std::string(function)+std::string(" in ")+extract_file_name(file)+std::string(":")+sntoa(line)+std::string(" code=")+sntoa(result));
}

/// ���������� � ��� ���������, ���� result != RDK_SUCCESS
void AssertLog(int result, const char* function, const char* file, int line)
{
 if(result != 0)
 {
   std::string message(std::string("Assertion in ")+std::string(function)+std::string(" in ")+extract_file_name(file)+std::string(":")+sntoa(line)+std::string(" code=")+sntoa(result));
   if(MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, message.c_str()) != RDK_SUCCESS)
    RdkDebuggerMessage(message);
 }
}

}

/*****************************************************************************/
extern RDK::UStorage* CreateNewStorage(void);

extern RDK::UEnvironment* CreateNewEnvironment(void);

extern RDK::UEngine* CreateNewEngine(void);
/*****************************************************************************/

// ----------------------------
// ������� RPC
// � ������� ���� ������� �������� ������� ����� ������� ����������,
// ������������ ��������� ������
// ----------------------------
/// ��������� ������ � ���������� xml-�������� ������
/// request - xml �������� �������
/// return_value - ������������ �������� ��� ��� �������, ������� ��� �����
/// ��� ��������� ���������� 0
const char* RDK_CALL Core_RemoteCall(const char *request, int &return_value, int &channel_index)
{
 return RDK::RemoteCallInternal(request, return_value, channel_index);
}
// ----------------------------

// ----------------------------
// ������� ����������� ������
// ----------------------------
/// ���������� �������� ������ ����
int RDK_CALL Ver_CoreMajor(void)
{
 return RdkCoreManager.GetVersion().Major;
}

/// ���������� �������� ������ ����
int RDK_CALL Ver_CoreMinor(void)
{
 return RdkCoreManager.GetVersion().Minor;
}

/// ���������� ������ ����� ����
int RDK_CALL Ver_CoreRevision(void)
{
 return RdkCoreManager.GetVersion().Revision;
}

/// ���������� ������ ������ ���� � ���� ������
const char* RDK_CALL Ver_Core(void)
{
 return RdkCoreManager.GetVersion().ToString().c_str();
}

/// ���������� ������ ���� � ����������
/// ���������� >0 ���� ������ ���� ������,
/// ���������� <0 ���� ������ ���� ������,
/// ���������� 0 � ������ ����������.
int RDK_CALL Ver_CoreCompare(int major, int minor, int revision)
{
 return RdkCoreManager.GetVersion().CompareCore(major, minor, revision);
}

/// ���������� ��� ����������� ����
const char* RDK_CALL Ver_CompilerName(void)
{
 return RdkCoreManager.GetVersion().CompilerName.c_str();
}

/// ���������� ������ ����������� ����
const char* RDK_CALL Ver_CompilerVersion(void)
{
 return RdkCoreManager.GetVersion().CompilerVersion.c_str();
}
/*
/// ���������� ������ opencv (���� ������������)
const char* RDK_CALL Ver_OpenCvVersion(void)
{
#ifdef CV_VERSION
 return CV_VERSION;
#else
 return "";
#endif
}*/
// ----------------------------

// ----------------------------
// ������� �����������
// ----------------------------
// ���������� ��������� ���������� ������������
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

// ��������/��������� ��������� ������������
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

/// ���������� ��������� ����� ����������� ������ �����
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

/// ������������� ��������� ����� ����������� ������ �����
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

/// ���������� ����� ��������� ������� ��� �����������
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

/// ������������� ����� ��������� ������� ��� �����������
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

/// ���������� ���� ��������� ������ ���� � ��������
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

/// ������������� ���� ��������� ������ ���� � ��������
int RDK_CALL Log_SetDebuggerMessageFlag(bool value)
{
 if(!RdkCoreManager.GetLogger()->SetDebuggerMessageFlag(value))
  return RDK_E_LOGGER_SET_DEBUGGER_FLAG_FAIL;
 return RDK_SUCCESS;
}

int RDK_CALL MLog_SetDebuggerMessageFlag(int channel_index, bool value)
{
 if(channel_index<RDK_GLOB_MESSAGE|| channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;

 if(!RdkCoreManager.GetLogger(channel_index)->SetDebuggerMessageFlag(value))
  return RDK_E_LOGGER_SET_DEBUGGER_FLAG_FAIL;
 return RDK_SUCCESS;
}

// ���������� ��������-������������ ����������
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

 if(!RdkCoreManager.GetLogger(channel_index)->SetExceptionHandler(reinterpret_cast<RDK::ULoggerEnv::PExceptionHandler>(value)))
  return RDK_E_LOGGER_SET_EXCEPTION_HANDLER_FAIL;

 return RDK_SUCCESS;
}

// ���������� ������ ����� ����
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

// ���������� � ��� ����� ���������
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

// ���������� � ��� ����� ��������� � ����� ������
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

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
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

/// ���������� ����� ������������� ����� ����
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

/// ���������� ����� ����� ����
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


/// ������� ��� ����������� ���������
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
// ������ �������������
// ----------------------------
// ���������� ��� �������� �������� ������
const char* RDK_CALL Core_GetSystemDir(void)
{
 return RdkCoreManager.GetSystemDir();
}

const char* RDK_CALL GetSystemDir(void)
{
 return Core_GetSystemDir();
}

// ������������� ��� �������� �������� ������
int RDK_CALL Core_SetSystemDir(const char *dir)
{
 return RdkCoreManager.SetSystemDir(dir);
}

int RDK_CALL SetSystemDir(const char *dir)
{
 return Core_SetSystemDir(dir);
}

// ���������� ��� �������� �������� ������
const char* RDK_CALL Core_GetLogDir(void)
{
 return RdkCoreManager.GetLogDir();
}

// ������������� ��� �������� �������� ������
int RDK_CALL Core_SetLogDir(const char *dir)
{
 return RdkCoreManager.SetLogDir(dir);
}

// ���������� ���������� ��������� ��������� ����������� ������
bool RDK_CALL Core_GetDebugMode(void)
{
 return RdkCoreManager.GetDebugMode();
}

// ������������� ���������� ��������� ��������� ����������� ������
int RDK_CALL Core_SetDebugMode(bool value)
{
 return RdkCoreManager.SetDebugMode(value);
}

/// ���������� ���� ��������� ������ ���� � ��������
bool RDK_CALL Core_GetDebuggerMessageFlag(void)
{
 return RdkCoreManager.GetDebuggerMessageFlag();
}

/// ������������� ���� ��������� ������ ���� � ��������
int RDK_CALL Core_SetDebuggerMessageFlag(bool value)
{
 return RdkCoreManager.SetDebuggerMessageFlag(value);
}

// ������� ���������� ������
int RDK_CALL Core_ClearFonts(void)
{
 return RdkCoreManager.ClearFonts();
}

// ��������� ���������� ������
int RDK_CALL Core_LoadFonts(void)
{
 return RdkCoreManager.LoadFonts();
}

// ���������� ����� �������
int RDK_CALL Core_GetNumChannels(void)
{
 return RdkCoreManager.GetNumChannels();
}

// ������� ��������� ����� �������
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

// ��������� ������ � ������� ��������� �������
// ���� ������� ����� ��� �������� ��������� ��
// ��������� � �����
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

// ������� ������ �� �������
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

// ���������� ������ �������� ���������� ������
int RDK_CALL Core_GetSelectedChannelIndex(void)
{
 return RdkCoreManager.GetSelectedChannelIndex();
}

// ����������� ������� ��������� �� ������ � �������� �������
// � ������ �������� ������, ��������� ������������� ����������������� �� 0 ������
int RDK_CALL Core_SelectChannel(int index)
{
 return RdkCoreManager.SelectChannel(index);
}

/// ��������� ����� �� ������ ������� UnlockEngine
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
   return RdkCoreManager.LockChannel(index);
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

/// ������������ �����
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
   return RdkCoreManager.UnLockChannel(index);
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
    MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_DEBUG, (std::string("Automatic increase channels quantity to ")+RDK::sntoa(RdkCoreManager.GetSelectedChannelIndex()+1)).c_str());
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

// ���������������� ������ (������� ������������� ���������� ��� ������ �������������)
int RDK_CALL Core_ChannelUnInit(void)
{
 return MCore_ChannelUnInit(Core_GetSelectedChannelIndex());
}

int RDK_CALL MCore_ChannelUnInit(int channel_index)
{
 return RdkCoreManager.ChannelUnInit(channel_index);
}

/// ��������� ��������������� �� ������
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

/// ����� �������� ���������� ��������� ���������� ���
/// ������������ ��������
/// 0 - ���� ���������� ��� ���� �������, ������������ ����� ���
/// 1 - ���������� ���������� � �������������� ������� ������� �������
int RDK_CALL Core_GetBufObjectsMode(void)
{
 return RdkCoreManager.GetBufObjectsMode();
}

int RDK_CALL Core_SetBufObjectsMode(int mode)
{
 return RdkCoreManager.SetBufObjectsMode(mode);
}

/// ������������ �������� ������ ������, �� ��������� ���������
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

/// ���������� ����� �������� ����� ������
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

/// ������ � ��������
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
// ������ ���������� ����������
// ----------------------------
// ���������� ����� ������� � ���������
int RDK_CALL Storage_GetNumClasses(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetNumClasses();
}

// ���������� id ������� � ���������. ������ ������ ���� ��������
int RDK_CALL Storage_GetClassesList(int *buffer)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassesList(buffer);
}

// ���������� ����� ������� � ��������� � ���� ������ ����������� ��������
const char * RDK_CALL Storage_GetClassesNameList(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassesNameList();
}

// ���������� ��� ������ �� ��� id.
const char * RDK_CALL Storage_GetClassName(int id)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassName(id);
}

// ���������� Id ������ �� ��� �����
int RDK_CALL Storage_GetClassId(const char *name)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassId(name);
}

// ������� ������� ������ ������� �� ���������
// ���������� false ���� classid �� ������,
// ��� ������������ ������� ����� ������
int RDK_CALL Storage_DelClass(int classid)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelClass(classid);
}

// �������� ��� ��������� ������� �� ���������
int RDK_CALL Storage_FreeObjectsStorage(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_FreeObjectsStorage();
}

// ������� ��� ������� �� ���������
int RDK_CALL Storage_ClearObjectsStorage(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_ClearObjectsStorage();
}

// ��������� ��������� ����� �������� � ���������
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

// ���������� �������� ������ �� ��� id � ������� xml
const char* RDK_CALL Storage_GetClassDescription(const char* classname)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassDescription(classname);
}

// ������������� �������� ������ �� ��� id, �������� ��� �� ������� xml
int RDK_CALL Storage_SetClassDescription(const char* classname, const char* description)
{
 return RdkCoreManager.GetEngineLock()->Storage_SetClassDescription(classname, description);
}

// ��������� �������� ���� ������� � xml
const char* RDK_CALL Storage_SaveClassesDescription(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveClassesDescription();
}

// ��������� �������� ���� ������� �� xml
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

// ��������� ����� �������� ���� ������� � xml
const char* RDK_CALL Storage_SaveCommonClassesDescription(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveCommonClassesDescription();
}

// ��������� ����� �������� ���� ������� �� xml
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


// ��������� �������� ���� ������� � xml ������� ����� ��������
const char* RDK_CALL Storage_SaveAllClassesDescription(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveAllClassesDescription();
}

// ��������� �������� ���� ������� �� xml ������� ����� ��������
int RDK_CALL Storage_LoadAllClassesDescription(const char* xmltext)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadAllClassesDescription(xmltext);
}

const char* RDK_CALL Storage_GetClassProperties(const char *stringid, unsigned int type_mask)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassProperties(stringid,type_mask);
}

const char* RDK_CALL MStorage_GetClassProperties(int channel_index, const char *stringid, unsigned int type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock()->Storage_GetClassProperties(stringid,type_mask);
}

const char* RDK_CALL Storage_GetClassStructure(const char *stringid, unsigned int type_mask)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassStructure(stringid,type_mask);
}

const char* RDK_CALL MStorage_GetClassStructure(int channel_index, const char *stringid, unsigned int type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock()->Storage_GetClassStructure(stringid,type_mask);
}
// ----------------------------

// ----------------------------
// ������ ���������� ����������� ���������
// ----------------------------
// ���������� ����� ���������
int RDK_CALL Storage_GetNumClassLibraries(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetNumClassLibraries();
}

// ���������� ������ ��������� � ���� ������, ����������� ��������
const char* RDK_CALL Storage_GetClassLibrariesList(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassLibrariesList();
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// library_name - ��� ����������
const char* RDK_CALL Storage_GetLibraryClassNames(const char *library_name)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetLibraryClassNames(library_name);
}

// ���������� ������ ������� ���������� � ���� ������, ����������� ��������
// index - ������ ����������
const char* RDK_CALL Storage_GetLibraryClassNamesByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetLibraryClassNamesByIndex(index);
}

// ���������� ��� ���������� �� �������
const char * RDK_CALL Storage_GetClassLibraryNameByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassLibraryNameByIndex(index);
}

// ���������� ������ ���������� �� �������
const char * RDK_CALL Storage_GetClassLibraryVersionByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_GetClassLibraryVersionByIndex(index);
}

/// ������� ����� runtime-����������
int RDK_CALL Storage_CreateRuntimeCollection(const char *collection_name)
{
 return RdkCoreManager.GetEngineLock()->Storage_CreateRuntimeCollection(collection_name);
}

// ��������� ��������� �� ����� dll-�����
int RDK_CALL Storage_LoadBinaryCollectionFromFile(const char *filename)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadBinaryCollectionFromFile(filename);
}

// ��������� runtime-���������
int RDK_CALL Storage_LoadRuntimeCollectionFromFile(const char *filename)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadRuntimeCollectionFromFile(filename);
}

int RDK_CALL Storage_LoadRuntimeCollectionFromString(const char *buffer)
{
 return RdkCoreManager.GetEngineLock()->Storage_LoadRuntimeCollectionFromString(buffer);
}

// ��������� runtime-���������
int RDK_CALL Storage_SaveRuntimeCollectionToFile(const char *filename)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveRuntimeCollectionToFile(filename);
}

int RDK_CALL Storage_SaveRuntimeCollectionToString(const char *buffer)
{
 return RdkCoreManager.GetEngineLock()->Storage_SaveRuntimeCollectionToString(buffer);
}

// ������� ������������ ���������� �� ������ �� �������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
int RDK_CALL Storage_DelClassLibraryByIndex(int index)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelClassLibraryByIndex(index);
}

// ������� ������������ ���������� �� ������ �� �����
// ��������������� �� ������������ ������ ����� �� ���������� �������.
int RDK_CALL Storage_DelClassLibraryByName(const char *name)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelClassLibraryByName(name);
}

// ������� �� ������ ��� ����������
// ��������������� �� ������������ ������ ����� �� ���������� �������.
int RDK_CALL Storage_DelAllClassLibraries(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_DelAllClassLibraries();
}

// ���������� ������ � Storage ��� ������� �������.
// ������ ��������� �� ������
int RDK_CALL Storage_CreateClass(const char* stringid, const char *classname, const char *collection_name)
{
 return RdkCoreManager.GetEngineLock()->Storage_CreateClass(stringid, classname, collection_name);
}

// ��������� ��������� ������� ���������
// �������� �������������� ���������� ������ � ������� ���������
int RDK_CALL Storage_BuildStorage(void)
{
 return RdkCoreManager.GetEngineLock()->Storage_BuildStorage();
}
// ----------------------------


// ----------------------------
// ������ ���������� ������
// ----------------------------
// ������ ������������� �������� ������ ���������
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

// ���� ��������� �������������
// true - ��������� ������ � �������������
// false - ��������� �� ������
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

// ���������� ��������� �������������
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


// ������� ������� �������������� ���������
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

// ���������� ��������� ���������� ������������
bool RDK_CALL Env_GetEventsLogMode(void)
{
 return Log_GetEventsLogMode();
}

bool RDK_CALL MEnv_GetEventsLogMode(int channel_index)
{
 return MLog_GetEventsLogMode(channel_index);
}

// ��������/��������� ��������� ������������
int RDK_CALL Env_SetEventsLogMode(bool value)
{
 return Log_SetEventsLogMode(value);
}

int RDK_CALL MEnv_SetEventsLogMode(int channel_index, bool value)
{
 return MLog_SetEventsLogMode(channel_index,value);
}

// ������������� �����
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

// ��������������� �����
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

// ��������� �������������� �������� ������ ���������
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

// ���������� ������� ������ ���������
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

// ������� ������ � ��� ����������, ������� ���������, ������� ����� � �������� ���������
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


// �������������� ������
int RDK_CALL Env_ModelInit(const char *stringid)
{
    return RdkCoreManager.GetEngineLock()->Env_ModelInit(stringid);
}

int RDK_CALL MEnv_ModelInit(int channel_index, const char *stringid)
{
    if(channel_index<0 || channel_index>=Core_GetNumChannels())
     return RDK_E_CORE_CHANNEL_NOT_FOUND;
    return RdkCoreManager.GetEngineLock(channel_index)->Env_ModelInit(stringid);
}

// ���������������� ������
int RDK_CALL Env_ModelUnInit(const char *stringid)
{
    return RdkCoreManager.GetEngineLock()->Env_ModelUnInit(stringid);
}

int RDK_CALL MEnv_ModelUnInit(int channel_index, const char *stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Env_ModelUnInit(stringid);
}

// ����� �����
// ���� stringid == 0 �� ��������� ��� ������ �������,
// ����� ��������� ������ ��������� ��������� ������
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

// ������ ���� ������ � �������� �������
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


/// ������ ������ �������� ������������� calc_intervsal ������ � ����������� ��������� ���������
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

// ����� ������ �����
// ���� stringid == 0 �� ���������� ��� ������ �������,
// ����� - ������ ��������� ��������� ������
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

/// ����� ������ ���������� �� �������� �� ���������
/// ���� stringid == 0 �� ���������� ��� ������ �������,
/// ����� - ������ ��������� ��������� ������
/// ���� subcomps == true �� ����� ���������� ��������� ���� �������� ���������
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

// ���������� ���������� ������� ������ �� ��������� ��������
int RDK_CALL Env_IncreaseModelTimeByStep(void)
{
 return RdkCoreManager.GetEngineLock()->Env_IncreaseModelTimeByStep();
}

/// ������������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
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

/// ���������� ����������� �������� ������� ����� ������ ������� (��)
/// �������� ������� ����� ������������ �� ��� ���, ���� ����� ��������� � ������
/// ��������� �������� �� ������ ������ ��� ��� ��������
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


// �����, ����������� �� ��������� RT-������
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

/// �����, ����������� � ������ �� ���� ����� RTCalculate;
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

/// ������������������ RT ������� (��������� RTModelCalcTime/RTLastDuration)
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

// ���������� ��� �������� �������� ��� �������� ������
const char* RDK_CALL Env_GetCurrentDataDir(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetCurrentDataDir();
}

// ������������� ��� �������� �������� ��� �������� ������
int RDK_CALL Env_SetCurrentDataDir(const char *dir)
{
 return RdkCoreManager.GetEngineLock()->Env_SetCurrentDataDir(dir);
}

int RDK_CALL MEnv_SetCurrentDataDir(int channel_index, const char *dir)
{
 return RdkCoreManager.GetEngineLock(channel_index)->Env_SetCurrentDataDir(dir);
}

/// ���������� ��������� ����� ����������� ������ �����
bool RDK_CALL Env_GetDebugMode(void)
{
 return Log_GetDebugMode();
}

bool RDK_CALL MEnv_GetDebugMode(int channel_index)
{
 return MLog_GetDebugMode(channel_index);
}

/// ������������� ��������� ����� ����������� ������ �����
int RDK_CALL Env_SetDebugMode(bool value)
{
 return Log_SetDebugMode(value);
}

int RDK_CALL MEnv_SetDebugMode(int channel_index, bool value)
{
 return MLog_SetDebugMode(channel_index, value);
}

/// ���������� ����� ��������� ������� ��� �����������
unsigned int RDK_CALL Env_GetDebugSysEventsMask(void)
{
 return Log_GetDebugSysEventsMask();
}

unsigned int RDK_CALL MEnv_GetDebugSysEventsMask(int channel_index)
{
 return MLog_GetDebugSysEventsMask(channel_index);
}

/// ������������� ����� ��������� ������� ��� �����������
int RDK_CALL Env_SetDebugSysEventsMask(unsigned int value)
{
 return Log_SetDebugSysEventsMask(value);
}

int RDK_CALL MEnv_SetDebugSysEventsMask(int channel_index, unsigned int value)
{
 return MLog_SetDebugSysEventsMask(channel_index, value);
}


/// ���������� ���� ��������� ������ ���� � ��������
bool RDK_CALL Env_GetDebuggerMessageFlag(void)
{
 return Log_GetDebuggerMessageFlag();
}

bool RDK_CALL MEnv_GetDebuggerMessageFlag(int channel_index)
{
 return MLog_GetDebuggerMessageFlag(channel_index);
}

/// ������������� ���� ��������� ������ ���� � ��������
bool RDK_CALL Env_SetDebuggerMessageFlag(bool value)
{
 return Log_SetDebuggerMessageFlag(value);
}

bool RDK_CALL MEnv_SetDebuggerMessageFlag(int channel_index, bool value)
{
 return MLog_SetDebuggerMessageFlag(channel_index, value);
}

// ***********************************************
// ������ ���������� ������� �����������
// !!! ��������� ������ ������ �� ���
// ������, ������������ � ����������� �� ���������� id !!!
// ***********************************************
// ������������� ������� ��������� (��������� ������������ ����� - ������)
int RDK_CALL Env_SelectCurrentComponent(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Env_SelectCurrentComponent(stringid);
}

// ���������� ������� ��������� � ��������� �� ��������� (������)
int RDK_CALL Env_ResetCurrentComponent(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Env_ResetCurrentComponent(stringid);
}

// ������ ������� ��������� �� ��� �������� (������ �� ������� �����)
// ���� ��� �� ������� ������, �� �� ������ ������
int RDK_CALL Env_UpCurrentComponent(void)
{
 return RdkCoreManager.GetEngineLock()->Env_UpCurrentComponent();
}

// ������ ������� ��������� �� ��� �������� �� ������������ ������ �����������
// (����� �� N ������� ���� ������������ �������� ����������)
int RDK_CALL Env_DownCurrentComponent(const char *stringid)
{
 return RdkCoreManager.GetEngineLock()->Env_DownCurrentComponent(stringid);
}

// ���������� ������� ��� �������� ����������
const char* RDK_CALL Env_GetCurrentComponentName(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetCurrentComponentName();
}

// ���������� ������� ��������� id �������� ����������
const char* RDK_CALL Env_GetCurrentComponentId(void)
{
 return RdkCoreManager.GetEngineLock()->Env_GetCurrentComponentId();
}
// ***********************************************

/// ���������� ��������� � ���� � ������ ��������� ������
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
// ������ ���������� �������
// ----------------------------
// ������� ������
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

// ������� ����� ������ �� ����� ������ � ���������
// �������������� ������� ������������ ������
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

// ������� ������
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


// ���������, ���������� �� ������
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

// ���������, ���������� �� � ������ ��������� � ������ stringid)
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

// ��������� � ��������� ��������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'classname'
// ���� stringid - ������ ������, �� ��������� � ���� ������
// ���������� ��� ���������� � ������ ������
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

// ������� �� ���������� ���������� ������ � ��������������� 'stringid' ���������
// ���������� � �������� 'name'
// ���� stringid - ������ ������, �� ������� �� ����� ������
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

// ������� ����� ��� ������������� ���������� 'stringid'
// ���������� ��� ���������� ����������
int RDK_CALL Model_CloneComponent(const char* component_name, const char* new_name)
{
 return RdkCoreManager.GetEngineLock()->Model_CloneComponent(component_name,new_name);
}

int RDK_CALL MModel_CloneComponent(int channel_index, const char* component_name, const char* new_name)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_CHANNEL_NOT_FOUND;
 return RdkCoreManager.GetEngineLock(channel_index)->Model_CloneComponent(component_name,new_name);
}

/// ���������� ���������� � ������ ���������
/// ���� comp �� ����������� ����� ����������, ��� target ����� �������� ��
/// ����� ���������� storage, ��� target �� ����� ������� � ���� ���������
/// �� ���������� false � �� ������ ������
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

// ���������� ����� ���� ��������� � ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ����� ���� ��������� ������
int RDK_CALL Model_GetNumComponents(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_GetNumComponents(stringid);
}

// ���������� ������ ���� id ��������� ���������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ������ ���� id ������
int RDK_CALL Model_GetComponentsList(const char* stringid, int *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentsList(stringid, buffer);
}

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ','
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

// ���������� ������, ���������� ������ ���� ���� ��������� ��������� ���������� 'stringid'
// ����� ����������� �������� ',' � ������� ��� ������ 'class_name'
// ���� find_all == true �� ����� ������� � �� ���� ��������������
const char* RDK_CALL Model_FindComponentsByClassName(const char* stringid, const char* class_name, bool find_all)
{

 return RdkCoreManager.GetEngineLock()->Model_FindComponentsByClassName(stringid,class_name,find_all);
}

// ���������� ��������� � ������� �������� index ��� ������ 'name' ����� ���
// ���� �� ������ �� �������� ����� ���������
// ����������� ��� ��������� ������� ������� ���������
// ���� �������� 'step' ������� �� ������� �������, �� ��������� ���������������
// �� ��� �������
int RDK_CALL Model_ChangeComponentPosition(const char* stringid, int step)
{

 return RdkCoreManager.GetEngineLock()->Model_ChangeComponentPosition(stringid, step);
}

int RDK_CALL MModel_ChangeComponentPosition(int channel_index, const char* stringid, int step)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_ChangeComponentPosition(stringid, step);
}

// ���������� xml-������ ������� ��������������� ���� ����������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ���������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ����������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ����������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* RDK_CALL Model_GetConnectorsList(const char* stringid,
						  int sublevel, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetConnectorsList(stringid, sublevel, owner_level_stringid);
}

// ���������� xml-������ ������� ��������������� ���� ��������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// �������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� ��������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* RDK_CALL Model_GetItemsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetItemsList(stringid, sublevel, owner_level_stringid);
}

// ���������� xml-������ ������� ��������������� ���� �������� ����.
// 'sublevel' ����������� ����� ������� ����������� �������� ��� �������
// ������� ����� ��������� � ������.
// ���� 'sublevel' == -2, �� ���������� �������������� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� �������������� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� �������������� �������� ������ ���� ����
// ��������������� ������� ������ �� ������������.
const char* RDK_CALL Model_GetNetsList(const char* stringid,
							int sublevel, const char* owner_level_stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_GetNetsList(stringid, sublevel, owner_level_stringid);
}

// ���������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
const char* RDK_CALL Model_GetComponentName(const char* stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentName(stringid);
}

// ���������� ������� ��� ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
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

// ���������� ������� id ���������� �� ��������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������
// ������ ���������� � ������������� ������ dll
// ��� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ��� ����������� �� ������ �������� ����������
const char* RDK_CALL Model_GetComponentLongId(const char* stringid, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentLongId(stringid,owner_level_stringid);
}

// ���������� ��� ������ ���������� � ��������� �� �������� 'stringid'
// ���� stringid - ������ ������, �� ���������� ��� ������ ������
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


// ���������� ������ ������� ���������� ����������� ��������
const char* RDK_CALL Model_GetComponentPropertiesList(const char* stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesList(stringid,type_mask);
}

// ���������� ������ ���� � �������� ������� ���������� ����������� ��������
// ������ ������� ����� ��� ���_��������:������_�����(������)
const char* RDK_CALL Model_GetComponentPropertiesLookupList(const char* stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesLookupList(stringid,type_mask);
}

// ���������� �������� ���������� �� ��������������
const char * RDK_CALL Model_GetComponentProperties(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentProperties(stringid,type_mask);
}

// ���������� �������� ���������� �� �������������� � ����������
const char * RDK_CALL Model_GetComponentPropertiesEx(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesEx(stringid, type_mask);
}

// ���������� ���������� �������� ���������� �� ��������������
const char * RDK_CALL Model_GetComponentSelectedProperties(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// ���������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
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

// ������������� �������� ���������� �� ��������������
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

// ������������� �������� �������� ���������� �� �������������� ���������� � ����� ��������
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

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
int RDK_CALL Model_SetGlobalComponentPropertyValue(const char *stringid, const char* class_stringid, const char *paramname, const char *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetGlobalComponentPropertyValue(stringid,class_stringid, paramname,buffer);
}

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
int RDK_CALL Model_SetGlobalOwnerComponentPropertyValue(const char *stringid, const char* class_stringid, const char* class_owner_stringid, const char *paramname, const char *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetGlobalOwnerComponentPropertyValue(stringid, class_stringid, class_owner_stringid, paramname,buffer);
}

// ���������� ��������� void* �� ������ �������� ����������
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

// �������� ������ 'data' � �������� �������� ����������
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

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
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

// ���������� ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedParameters(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// ���������� ��������� ���������� �� �������������� � ����������
const char * RDK_CALL Model_GetComponentParametersEx(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPropertiesEx(stringid, type_mask & 0xFFFFFF01);
}

// ���������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
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

// ������������� ��������� ���������� �� ��������������
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

// ������������� �������� ��������� ���������� �� �������������� ���������� � ����� ���������
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
/*
// ��������� ��������� ���������� ���� � ������
int RDK_CALL Model_CreateLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{
 return RdkCoreManager.GetEngineLock()->Model_CreateLink(stringid1, output_number, stringid2, input_number);
}*/

int RDK_CALL Model_CreateLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return RdkCoreManager.GetEngineLock()->Model_CreateLink(stringid1, item_property_name, stringid2, connector_property_name);
}

int RDK_CALL MModel_CreateLinkByName(int channel_index, const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
  if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 RdkCoreManager.GetEngineLock(channel_index)->Model_CreateLink(stringid1, item_property_name, stringid2, connector_property_name);
 return 0;
}

int RDK_CALL Model_CreateLinkByNameEx(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name, int connector_c_index)
{
 return RdkCoreManager.GetEngineLock()->Model_CreateLink(stringid1, item_property_name, stringid2, connector_property_name,connector_c_index);
}

int RDK_CALL Model_BreakLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakLink(stringid1, item_property_name, stringid2, connector_property_name);
}

// ��������� ��� �����
int RDK_CALL Model_BreakAllLinks(void)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakAllLinks();
}


// ��������� ��� ������� � �������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentLinks(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakAllComponentLinks(stringid);
}

// ��������� ��� ������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentInputLinks(const char* stringid)
{
 return RdkCoreManager.GetEngineLock()->Model_BreakAllComponentInputLinks(stringid);
}

int RDK_CALL MModel_BreakAllComponentInputLinks(int channel_index, const char* stringid)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 RdkCoreManager.GetEngineLock(channel_index)->Model_BreakAllComponentInputLinks(stringid);
 return 0;
}

// ��������� ��� �������� ����� ���������� ����������
int RDK_CALL Model_BreakAllComponentOutputLinks(const char* stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_BreakAllComponentOutputLinks(stringid);
}

// ���������, ���������� �� ������� �����
bool RDK_CALL Model_CheckLink(const char* stringid1, int output_number, const char* stringid2, int input_number)
{

 return RdkCoreManager.GetEngineLock()->Model_CheckLink(stringid1, output_number, stringid2, input_number);
}

bool RDK_CALL Model_CheckLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name)
{
 return RdkCoreManager.GetEngineLock()->Model_CheckLink(stringid1, item_property_name, stringid2, connector_property_name);
}

/// ����������� ��� ����� ������������ � ������ ���������� 1 �� ����� ���������� 2
int RDK_CALL Model_SwitchOutputLinks(const char* item_name_1, const char* item_property_name1, const char* item_name_2, const char* item_property_name2)
{
 return RdkCoreManager.GetEngineLock()->Model_SwitchOutputLinks(item_name_1, item_property_name1, item_name_2, item_property_name2);
}

int RDK_CALL MModel_SwitchOutputLinks(int channel_index, const char* item_name_1, const char* item_property_name1, const char* item_name_2, const char* item_property_name2)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SwitchOutputLinks(item_name_1, item_property_name1, item_name_2, item_property_name2);
}

// ��������� ��� ����� ������ ���������� stringid � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * RDK_CALL Model_GetComponentInternalLinks(const char* stringid, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInternalLinks(stringid, owner_level_stringid);
}

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� � ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����������� ������� �������� ����������
int RDK_CALL Model_SetComponentInternalLinks(const char* stringid, const char* buffer, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentInternalLinks(stringid,buffer, owner_level_stringid);
}

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * RDK_CALL Model_GetComponentInputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputLinks(stringid,owner_level_stringid, sublevel);
}

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char * RDK_CALL Model_GetComponentOutputLinks(const char* stringid, const char* owner_level_stringid, int sublevel)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputLinks(stringid,owner_level_stringid, sublevel);
}

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
// ����� ����������� �� ������ ���������� owner_level_stringid
// ���� owner_level_stringid �� �����, �� ����� ����������� �� ������ �������� ����������
const char* RDK_CALL Model_GetComponentPersonalLinks(const char* stringid, const char* owner_level_stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentPersonalLinks(stringid,owner_level_stringid);
}

// ���������� ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
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

// ���������� ���������� ������ ��������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
const char * RDK_CALL Model_GetComponentSelectedState(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentSelectedProperties(stringid);
}

// ���������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
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

// ������������� ��������� ���������� �� ��������������
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

// ������������� �������� ���������� ��������� ���������� �� �������������� ���������� � ����� ����������
int RDK_CALL Model_SetComponentStateValue(const char *stringid, const char *statename, const char *buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_SetComponentPropertyValue(stringid,statename,buffer);
}

// ���������� ����� ������ � ����������
int RDK_CALL Model_GetComponentNumInputs(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentNumInputs(stringid);
}

// ���������� ������ ����� ���������� � ����� ���������
int RDK_CALL Model_GetComponentInputDataSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputDataSize(stringid, index);
}

// ���������� ������ �������� ����� � ������
int RDK_CALL Model_GetComponentInputElementSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputElementSize(stringid, index);
}

// ���������� ������ ����� ���������� � ������ ���������
int RDK_CALL Model_GetComponentInputByteSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ����� ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDK_CALL Model_GetComponentInputData(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentInputData(stringid, index);
}

// ���������� ����� ������� � ����������
int RDK_CALL Model_GetComponentNumOutputs(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentNumOutputs(stringid);
}

// ���������� ������ ������ ���������� � ����� ���������
int RDK_CALL Model_GetComponentOutputDataSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputDataSize(stringid, index);
}

// ���������� ������ �������� ������ � ������
//int RDK_CALL Model_GetComponentOutputElementSize(const char *stringid, int index)
//{
//
// return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputElementSize(stringid, index);
//}

// ���������� ������ ������ ���������� � ������ ���������
int RDK_CALL Model_GetComponentOutputByteSize(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputByteSize(stringid, index);
}

// ���������� ��������� �� ������ ������ ��� �� ������ ����
// ������ ��� ������!
unsigned char* RDK_CALL Model_GetComponentOutputData(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputData(stringid, index);
}

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
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

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
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

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
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

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
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

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentProperties(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
int RDK_CALL Model_SaveComponentPropertiesToFile(const char *stringid, const char* file_name, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentPropertiesToFile(stringid, file_name, type_mask);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentProperties(const char *stringid, char* buffer)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponentProperties(stringid, buffer);
}

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
int RDK_CALL Model_LoadComponentPropertiesFromFile(const char *stringid, const char* file_name)
{

 return RdkCoreManager.GetEngineLock()->Model_LoadComponentPropertiesFromFile(stringid, file_name);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentParameters(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL MModel_SaveComponentParameters(int channel_index, const char *stringid, unsigned int type_mask)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF01);
}

// ��������� ��� ��������� ���������� � ��� �������� ��������� �� xml
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

// ��������� ��������� ���������� � ��� �������� ��������� � xml
const char * RDK_CALL Model_SaveComponentState(const char *stringid, unsigned int type_mask)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentProperties(stringid, type_mask & 0xFFFFFF02);
}

// ��������� ��������� ���������� � ��� �������� ��������� �� xml
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


// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
const char* RDK_CALL Model_SaveComponentDrawInfo(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_SaveComponentDrawInfo(stringid);
}

// ��������� ����� ����� ������ �� ���������
unsigned int RDK_CALL Model_GetDefaultTimeStep(void)
{

 return RdkCoreManager.GetEngineLock()->Model_GetDefaultTimeStep();
}

int RDK_CALL Model_SetDefaultTimeStep(unsigned int value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetDefaultTimeStep(value);
}

int RDK_CALL MModel_SetDefaultTimeStep(int channel_index, unsigned int value)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetDefaultTimeStep(value);
}

// ��������� ����� ����� ����������
unsigned int RDK_CALL Model_GetTimeStep(const char *stringid)
{

 return RdkCoreManager.GetEngineLock()->Model_GetTimeStep(stringid);
}

int RDK_CALL Model_SetTimeStep(const char *stringid, unsigned int value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetTimeStep(stringid, value);
}

// ������������� ��� ����� ���������� � ���� ��� �������� ���������
int RDK_CALL Model_SetGlobalTimeStep(const char *stringid, unsigned int value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetGlobalTimeStep(stringid, value);
}

int RDK_CALL MModel_SetGlobalTimeStep(int channel_index, const char *stringid, unsigned int value)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return 0;

 return RdkCoreManager.GetEngineLock(channel_index)->Model_SetGlobalTimeStep(stringid, value);
}

// ���������� ������� ����� ������
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

// ������������� ������� ����� ������
int RDK_CALL Model_SetTime(unsigned long long value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetTime(value);
}

// ���������� �������� �����
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

// ������������� �������� �����
int RDK_CALL Model_SetRealTime(unsigned long long value)
{

 return RdkCoreManager.GetEngineLock()->Model_SetRealTime(value);
}

// ����������� �������� ����� �� �������� ��������
int RDK_CALL Model_IncreaseRealTime(unsigned long long value)
{

 return RdkCoreManager.GetEngineLock()->Model_IncreaseRealTime(value);
}

// ���������� ���������� ��� � �������� �������
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

// ������������� ����� ������� ���������� ������
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

// ���������� ����� ������� ���������� ��� ������� ������� �������� ��������� (��)
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

// ���������� �����, ����������� �� ��������� �������
// (������ �� ������� ������� �������� ��������) (��)
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

// ���������� ���������� ��������������, ������ ���������
// ������� ������������ ������� � ���������� ������� ���� �����
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

// �����, ��������� ����� ����� ���������� ���������� �����
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
// ������ ���������� ������������
// ----------------------------
// ���������� ��������-������������ ����������
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

// ���������� ������ ����� ����
const char* RDK_CALL Engine_GetLog(int &error_level)
{
 return Log_GetLog(error_level);
}

const char* RDK_CALL MEngine_GetLog(int channel_index, int &error_level)
{
 return MLog_GetLog(channel_index, error_level);
}

// ���������� � ��� ����� ���������
int RDK_CALL Engine_LogMessage(int log_level, const char *message)
{
 return Log_LogMessage(log_level, message);
}

int RDK_CALL MEngine_LogMessage(int channel_index, int log_level, const char *message)
{
 return MLog_LogMessage(channel_index, log_level, message);
}

// ���������� � ��� ����� ��������� � ����� ������
int RDK_CALL Engine_LogMessageEx(int log_level, const char *message, int error_event_number)
{
 return Log_LogMessageEx(log_level, message, error_event_number);
}

int RDK_CALL MEngine_LogMessageEx(int channel_index, int log_level, const char *message, int error_event_number)
{
 return MLog_LogMessageEx(channel_index, log_level, message, error_event_number);
}

// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
// ���� ��������
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

/// ���������� ����� ������������� ����� ����
int RDK_CALL Engine_GetNumUnreadLogLines(void)
{
 return Log_GetNumUnreadLogLines();
}

int RDK_CALL MEngine_GetNumUnreadLogLines(int channel_index)
{
 return MLog_GetNumUnreadLogLines(channel_index);
}

/// ���������� ����� ����� ����
int RDK_CALL Engine_GetNumLogLines(void)
{
 return Log_GetNumLogLines();
}

int RDK_CALL MEngine_GetNumLogLines(int channel_index)
{
 return MLog_GetNumLogLines(channel_index);
}


/// ������� ��� ����������� ���������
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
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
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

// ������ ����� ������� �����
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

// ������ ����� ������ �����
int RDK_CALL Env_GetNumInputImages(void)
{

 return RdkCoreManager.GetEngineLock()->Env_GetNumInputImages();
}

// ������ ����� ������� �����
int RDK_CALL Env_GetNumOutputImages(void)
{

 return RdkCoreManager.GetEngineLock()->Env_GetNumInputImages();
}

// ������ ���������� �� ��������� (������� ����������)
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

// ������ ������ �����������
void RDK_CALL Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{

 return RdkCoreManager.GetEngineLock()->Env_SetInputImage(number, image, width, height,cmodel);
}

// ������ ���� ��������� �������� ����������� ������ �������������� ���
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


// ���������� ���������� �� ��������� (������� ����������)
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

// ���������� ������� �������� ����������
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
// ������ ���������� ����������� �������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
// ������������ �������� ����� ����������� ��� RDK::MDMatrix*
// ���� ����� �� �������� ������ ������ ����, �� ���������� 0
const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrix(const char *stringid, const char *property_name)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputAsMatrix(stringid, property_name);
}

const /* RDK::MDMatrix* */void* RDK_CALL Model_GetComponentOutputAsMatrixByIndex(const char *stringid, int  index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentOutputAsMatrix(stringid, index);
}

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
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

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
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

// ���������� ��������� �� ���� � �������� 'index' ���������� 'id'
const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInput(const char *stringid, const char *property_name)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapInput(stringid, property_name);
}

const /*RDK::UBitmap* */ void* RDK_CALL Model_GetComponentBitmapInputByIndex(const char *stringid, int index)
{

 return RdkCoreManager.GetEngineLock()->Model_GetComponentBitmapInput(stringid, index);
}

/// �������� ������ � ���������� ����������� ������ � �������� 'index' ���������� 'id'
/// � �������� bmp_param
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

/// �������� ����������� ������ � �������� 'index' ���������� 'id'
/// ����� ������������, ��� bmp ��� ����� ���������� ������ ��� ���������� ���������� �������
int RDK_CALL Model_CopyComponentBitmapOutput(const char *stringid, const char *property_name, /*RDK::UBitmap* */ void* bmp)
{
 return RdkCoreManager.GetEngineLock()->Model_CopyComponentBitmapOutput(stringid, property_name, (RDK::UBitmap*)bmp);
}

RDK_LIB_TYPE int RDK_CALL MModel_CopyComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, /*RDK::UBitmap**/void* bmp)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
 {
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;
 }

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

// �������� ����������� ������ � �������� 'index' ���������� 'id'
int RDK_CALL Model_SetComponentBitmapOutput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

  return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapOutput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

  return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapOutputUnsafe(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

  return RdkCoreManager.GetEngine(channel_index)->Model_SetComponentBitmapOutput(stringid, property_name, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}

int RDK_CALL Model_SetComponentBitmapOutputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

  return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapOutputByIndex(int channel_index, const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

  return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentBitmapOutput(stringid, index, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}

// �������� ����������� ����� � �������� 'index' ���������� 'id'
int RDK_CALL Model_SetComponentBitmapInput(const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
  return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}

int RDK_CALL MModel_SetComponentBitmapInput(int channel_index, const char *stringid, const char *property_name, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{
 if(channel_index<0 || channel_index>=Core_GetNumChannels())
  return RDK_E_CORE_INCORRECT_CHANNELS_NUMBER;

  return RdkCoreManager.GetEngineLock(channel_index)->Model_SetComponentBitmapInput(stringid, property_name, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}

int RDK_CALL Model_SetComponentBitmapInputByIndex(const char *stringid, int index, const /*RDK::UBitmap* */ void* const bmp, bool reflect)
{

  return RdkCoreManager.GetEngineLock()->Model_SetComponentBitmapInput(stringid, index, reinterpret_cast<const RDK::UBitmap*>(bmp),reflect);
}
// --------------------------

/* ************************************************************************** */




// ----------------------------
// ���������� ������ �������������
// ----------------------------
// ���������� ���������� ����������
// ������ ���� ������ � ���������� ����������� ����������������� ��
int RDK_CALL ExceptionDispatcher(void *exception)
{
 if(!RdkCoreManager.GetEngine())
  return 1;

 if(!exception)
  return 2;

 RDK::UException *exc=reinterpret_cast<RDK::UException*>(exception);
 RdkCoreManager.GetEngineLock()->ProcessException(*exc);

 return RDK_SUCCESS;
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

