#ifndef UEXCEPTIONLOGGER_H
#define UEXCEPTIONLOGGER_H

#include <mutex>
#include <string>
#include "../Utilities/UException.h"
#include "../Utilities/USupport.h"
#include "../../Deploy/Include/rdk_logging.h"

namespace RDK
{

class UEnvironment;
class UContainer;

class UExceptionLogger
{
public:
 typedef void (*PExceptionHandler)(int channel_index);
 typedef bool (*PExceptionPreprocessor)(UEnvironment * env, UContainer *model, const UException &in_exception, UException &out_exception);
 typedef bool (*PExceptionPostprocessor)(UEnvironment * env, UContainer *model, const UException &exception);

 UExceptionLogger();
 ~UExceptionLogger();

 bool GetDebugMode(void) const;
 bool SetDebugMode(bool value);

 unsigned int GetDebugSysEventsMask(void) const;
 bool SetDebugSysEventsMask(unsigned int value);

 bool GetDebuggerMessageFlag(void) const;
 bool SetDebuggerMessageFlag(bool value);

 int GetChannelIndex(void) const;
 bool SetChannelIndex(int value);

 bool RegisterEnvironment(UEnvironment* env);
 void UnRegisterEnvironment(void);

 bool RegisterGlobalLogger(UExceptionLogger* global_logger);
 void UnRegisterGlobalLogger(void);

 PExceptionHandler GetExceptionHandler(void) const;
 bool SetExceptionHandler(PExceptionHandler value);

 PExceptionPreprocessor GetExceptionPreprocessor(void) const;
 bool SetExceptionPreprocessor(PExceptionPreprocessor value);

 PExceptionPostprocessor GetExceptionPostprocessor(void) const;
 bool SetExceptionPostprocessor(PExceptionPostprocessor value);

 void ProcessException(const UException &exception) const;
 void ProcessExceptionGlobal(int type, const UException &exception) const;

 void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
 void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
 void LogMessageEx(int msg_level, const std::string &object_name, const std::string &line, int error_event_number=0);
 void LogMessageEx(int msg_level, const std::string &object_name, const std::string &method_name, const std::string &line, int error_event_number=0);

 // Управление режимом инициализации (для предотвращения фатальных крашей)
 static bool IsInitializationMode(void);
 static void SetInitializationMode(bool mode);

 std::string GetLogDir(void) const;
 bool SetLogDir(const std::string &value);

private:
 void WriteLog(int severity, const std::string &message) const;

 mutable std::mutex LoggerMutex;
 bool DebugMode;
 unsigned int DebugSysEventsMask;
 bool DebuggerMessageFlag;
 int ChannelIndex;
 std::string LogDirectory;

 UEnvironment* Environment;
 UExceptionLogger* GlobalLogger;

 PExceptionHandler ExceptionHandler;
 PExceptionPreprocessor ExceptionPreprocessor;
 PExceptionPostprocessor ExceptionPostprocessor;
};

}

#endif

