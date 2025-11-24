#include <climits>
#include "UExceptionLogger.h"

#include "UEnvironment.h"
#include "UContainer.h"
#include "../Utilities/USupport.h"
#include "../System/rdk_system.h"
#include "../../Deploy/Include/rdk_error_codes.h"

#ifdef RDK_USE_GLOG
#include <glog/logging.h>
DECLARE_int32(minloglevel);
DECLARE_int32(v);
#endif


namespace RDK
{

UExceptionLogger::UExceptionLogger()
: DebugMode(false)
, DebugSysEventsMask(0)
, DebuggerMessageFlag(false)
, ChannelIndex(0)
, LogDirectory()
, Environment(0)
, GlobalLogger(0)
, ExceptionHandler(0)
, ExceptionPreprocessor(0)
, ExceptionPostprocessor(0)
{
}

UExceptionLogger::~UExceptionLogger() = default;

bool UExceptionLogger::GetDebugMode(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return DebugMode;
}

bool UExceptionLogger::SetDebugMode(bool value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 if(DebugMode == value)
  return true;
 DebugMode=value;
#ifdef RDK_USE_GLOG
 if(DebugMode)
 {
  FLAGS_minloglevel = google::GLOG_INFO;
  FLAGS_v = 1;
 }
 else
 {
  FLAGS_minloglevel = google::GLOG_WARNING;
  FLAGS_v = 0;
 }
#endif
 return true;
}

unsigned int UExceptionLogger::GetDebugSysEventsMask(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return DebugSysEventsMask;
}

bool UExceptionLogger::SetDebugSysEventsMask(unsigned int value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 DebugSysEventsMask=value;
 return true;
}

bool UExceptionLogger::GetDebuggerMessageFlag(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return DebuggerMessageFlag;
}

bool UExceptionLogger::SetDebuggerMessageFlag(bool value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 DebuggerMessageFlag=value;
 return true;
}

int UExceptionLogger::GetChannelIndex(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return ChannelIndex;
}

bool UExceptionLogger::SetChannelIndex(int value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 ChannelIndex=value;
 return true;
}

bool UExceptionLogger::RegisterEnvironment(UEnvironment* env)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 Environment=env;
 return true;
}

void UExceptionLogger::UnRegisterEnvironment(void)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 Environment=0;
}

UExceptionLogger::PExceptionHandler UExceptionLogger::GetExceptionHandler(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return ExceptionHandler;
}

bool UExceptionLogger::SetExceptionHandler(PExceptionHandler value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 ExceptionHandler=value;
 return true;
}

UExceptionLogger::PExceptionPreprocessor UExceptionLogger::GetExceptionPreprocessor(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return ExceptionPreprocessor;
}

bool UExceptionLogger::SetExceptionPreprocessor(PExceptionPreprocessor value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 ExceptionPreprocessor=value;
 return true;
}

UExceptionLogger::PExceptionPostprocessor UExceptionLogger::GetExceptionPostprocessor(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return ExceptionPostprocessor;
}

bool UExceptionLogger::SetExceptionPostprocessor(PExceptionPostprocessor value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 ExceptionPostprocessor=value;
 return true;
}

void UExceptionLogger::WriteLog(int severity, const std::string &message) const
{
 int channel = ChannelIndex;
 if(channel < 0 && channel != RDK_SYS_MESSAGE && channel != RDK_GLOB_MESSAGE)
  channel = RDK_SYS_MESSAGE;

 RDK::Logging::ChannelLog(channel, severity, message);
}

void UExceptionLogger::ProcessException(const UException &exception) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);

 const UException* processed_exception=&exception;
 UException temp_ex;
 if(Environment && ExceptionPreprocessor)
 {
  if(ExceptionPreprocessor(Environment,Environment->GetModel(), exception,temp_ex))
   processed_exception=&temp_ex;
 }

 WriteLog(processed_exception->GetType(), processed_exception->what());

 if(DebuggerMessageFlag)
 {
  std::string debug_message;
  if(ChannelIndex == RDK_SYS_MESSAGE)
   debug_message = std::string("[S] ") + processed_exception->what();
  else if(ChannelIndex == RDK_GLOB_MESSAGE)
   debug_message = std::string("[G] ") + processed_exception->what();
  else
   debug_message = std::string("[") + sntoa(ChannelIndex) + std::string("] ") + processed_exception->what();
  RdkDebuggerMessage(debug_message);
 }

 if(ExceptionPostprocessor && Environment)
  ExceptionPostprocessor(Environment,Environment->GetModel(), *processed_exception);
}

void UExceptionLogger::ProcessExceptionGlobal(int type, const UException &exception) const
{
 WriteLog(type, exception.what());
}

void UExceptionLogger::LogMessage(int msg_level, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, "", line, error_event_number);
}

void UExceptionLogger::LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, "", method_name, line, error_event_number);
}

void UExceptionLogger::LogMessageEx(int msg_level, const std::string &object_name, const std::string &line, int error_event_number)
{
 if(msg_level == RDK_EX_DEBUG && !GetDebugMode())
  return;

 switch (msg_level)
 {
  case RDK_EX_FATAL:
  case RDK_EX_UNKNOWN:
  {
   EStringFatal ex(line, error_event_number);
   ex.SetObjectName(object_name);
   ProcessException(ex);
  } break;

  case RDK_EX_ERROR:
  {
   EStringError ex(line, error_event_number);
   ex.SetObjectName(object_name);
   ProcessException(ex);
  } break;

  case RDK_EX_WARNING:
  {
   EStringWarning ex(line, error_event_number);
   ex.SetObjectName(object_name);
   ProcessException(ex);
  } break;

  case RDK_EX_INFO:
  {
   EStringInfo ex(line, error_event_number);
   ex.SetObjectName(object_name);
   ProcessException(ex);
  } break;

  case RDK_EX_DEBUG:
  {
   EStringDebug ex(line, error_event_number);
   ex.SetObjectName(object_name);
   ProcessException(ex);
  } break;

  case RDK_EX_APP:
  {
   EStringApp ex(line, error_event_number);
   ex.SetObjectName(object_name);
   ProcessException(ex);
  } break;

  default:
  {
   EStringInfo ex(line, error_event_number);
   ex.SetObjectName(object_name);
   ProcessException(ex);
  } break;
 }
}

void UExceptionLogger::LogMessageEx(int msg_level, const std::string &object_name, const std::string &method_name, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, object_name, method_name + std::string(" - ") + line, error_event_number);
}

std::string UExceptionLogger::GetLogDir(void) const
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 return LogDirectory;
}

bool UExceptionLogger::SetLogDir(const std::string &value)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 if(LogDirectory == value)
  return true;
 LogDirectory=value;
#ifdef RDK_USE_GLOG
 // Do not set FLAGS_log_dir here - we use custom sinks for file output
 // FLAGS_log_dir is kept empty to prevent glog from creating files automatically
#endif
 if(!LogDirectory.empty())
 {
  if(CreateNewDirectory(LogDirectory.c_str()) != 0)
   return false;
 }
 return true;
}

bool UExceptionLogger::RegisterGlobalLogger(UExceptionLogger* global_logger)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 GlobalLogger=global_logger;
 return true;
}

void UExceptionLogger::UnRegisterGlobalLogger(void)
{
 std::lock_guard<std::mutex> lock(LoggerMutex);
 GlobalLogger=0;
}

}

