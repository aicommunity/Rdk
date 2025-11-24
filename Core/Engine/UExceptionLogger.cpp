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

namespace
{
std::string BuildSeverityPrefix(int type)
{
 switch(type)
 {
 case RDK_EX_FATAL:
  return "[FATAL] ";
 case RDK_EX_ERROR:
  return "[ERROR] ";
 case RDK_EX_WARNING:
  return "[WARNING] ";
 case RDK_EX_DEBUG:
  return "[DEBUG] ";
 case RDK_EX_APP:
  return "[APP] ";
 default:
  return std::string();
 }
}
}

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
 RDK_LOG_BY_LEVEL(severity, message);
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

 std::string ch_prefix;
 if(ChannelIndex>=0)
  ch_prefix=sntoa(ChannelIndex);
 else
  ch_prefix="S";

 std::string message = BuildSeverityPrefix(processed_exception->GetType()) + ch_prefix + std::string("> ") + processed_exception->what();

 WriteLog(processed_exception->GetType(), message);

 if(GlobalLogger && GlobalLogger!=this)
 {
  GlobalLogger->ProcessExceptionGlobal(processed_exception->GetType(), *processed_exception);
 }

 if(DebuggerMessageFlag)
  RdkDebuggerMessage(message);

 if(ExceptionPostprocessor && Environment)
  ExceptionPostprocessor(Environment,Environment->GetModel(), *processed_exception);
}

void UExceptionLogger::ProcessExceptionGlobal(int type, const UException &exception) const
{
 std::string ch_prefix;
 if(ChannelIndex>=0)
  ch_prefix=sntoa(ChannelIndex);
 else
  ch_prefix="G";
 std::string message = BuildSeverityPrefix(type) + ch_prefix + std::string("> ") + exception.what();
 WriteLog(type, message);
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
 if(!LogDirectory.empty())
  FLAGS_log_dir = LogDirectory;
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

