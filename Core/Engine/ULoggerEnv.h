#ifndef ULoggerEnvH
#define ULoggerEnvH

#include "ULogger.h"
#include "../Utilities/UException.h"

namespace RDK {

class UEnvironment;
class UContainer;

class RDK_LIB_TYPE ULoggerEnv: public ULogger
{
public:
/// �������� ������� ��������� ������ ����������� ����������
typedef void (*PExceptionHandler)(int channel_index);

/// �������� ������� ������������� ����������
typedef bool (*PExceptionPreprocessor)(UEnvironment * env, UContainer *model, const UException &in_exception, UException &out_exception);

/// �������� ������� ������������� ����������
typedef bool (*PExceptionPostprocessor)(UEnvironment * env, UContainer *model, const UException &exception);

protected: // ���������
/// ���� ��������� ������ �������
bool DebugMode;

/// ����� ��������� ������� ��� �����������
unsigned int DebugSysEventsMask;

/// ���� ��������� ������ ���� � ��������
bool DebuggerMessageFlag;

/// ���� ��������� ���������� ����������� ������� � ���-����
/// true - ����������� ��������
bool EventsLogMode;

protected: // ������
// ������� ���������� ����������
PExceptionHandler ExceptionHandler;

/// ������� ������� ������������� ����������
PExceptionPreprocessor ExceptionPreprocessor;

/// ������� ������� ������������� ����������
PExceptionPostprocessor ExceptionPostprocessor;

/// ��������� �� ����� ����������
UEnvironment* Environment;

/// ��������� �� ���������� ������ ������������ ���������� �� ���� ��������
ULoggerEnv* GlobalLogger;

/// ������� ����� ���������� �������
mutable int CurrentExceptionsLogSize;

/// ������������ ����� �������� ����������
/// ���� 0, �� �������������
int MaxExceptionsLogSize;

/// ������ ���������� ���������� ������� ���� ����������
unsigned LastReadExceptionLogIndex;

/// ������������ ������� ������������� ��������� � ����
mutable int LastErrorLevel;

/// ������ �������� ������ � �������������� ����������
int ChannelIndex;

/// ��������� ��������� �����
mutable string TempString;

mutable std::map<unsigned, UException > LogList;

/// ������ ��������� ������ ����
mutable unsigned LogIndex;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
ULoggerEnv(void);
virtual ~ULoggerEnv(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
/// ���� ��������� ������ �������
bool GetDebugMode(void) const;
bool SetDebugMode(bool value);

/// ����� ��������� ������� ��� �����������
unsigned int GetDebugSysEventsMask(void) const;
bool SetDebugSysEventsMask(unsigned int value);

/// ���������� ���� ��������� ������ ���� � ��������
bool GetDebuggerMessageFlag(void) const;

/// ������������� ���� ��������� ������ ���� � ��������
bool SetDebuggerMessageFlag(bool value);

/// ���� ��������� ���������� ����������� ������� � ���-����
/// true - ����������� ��������
bool GetEventsLogMode(void) const;
bool SetEventsLogMode(bool value);

/// ������ �������� ������ � �������������� ����������
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// ������ ���������� ������������
// --------------------------
/// ����������� ����� ����������
bool RegisterEnvironment(UEnvironment* env);
void UnRegisterEnvironment(void);

/// ����������� ����������� �������
bool RegisterGlobalLogger(ULoggerEnv* global_logger);
void UnRegisterGlobalLogger(void);

/// ������������ ��������� ����������
virtual void ProcessException(const UException &exception) const;

/// ������������ ��������� ���������� (���������� �����)
virtual void ProcessExceptionRaw(int type, const UException &exception) const;

/// ������������ ��������� ���������� � ������ ���������� �������
virtual void ProcessExceptionGlobal(int type, const UException &exception) const;

/// ������������ ����� �������� ����������
/// ���� 0, �� �������������
int GetMaxExceptionsLogSize(void) const;
void SetMaxExceptionsLogSize(int value);

/// ���������� ������ ����� ����
/// ������ GetLog, GetLogLine, GetUnreadLog ���������� ����������� ������ TempString
/// ���������� ������������� ����� ���� ������� ������ �������
const char* GetLog(int &error_level) const;

/// ���������� ����� ����� ����
int GetNumLogLines(void) const;

/// ���������� ������ ���� � �������� i
/// ������ GetLog, GetLogLine, GetUnreadLog ���������� ����������� ������ TempString
/// ���������� ������������� ����� ���� ������� ������ �������
const char* GetLogLine(int i, int &error_level, int &number, time_t &time) const;

/// ���������� ����� ������������� ����� ����
int GetNumUnreadLogLines(void) const;

/// ���������� ��������� ������ ����� ���� � ������� ���������� ���������� ����
/// ���� ��������
/// ������ GetLog, GetLogLine, GetUnreadLog ���������� ����������� ������ TempString
/// ���������� ������������� ����� ���� ������� ������ �������
const char* GetUnreadLog(int &error_level, int &number, time_t &time);
bool GetUnreadLog(UException &ex);

// ���������� ��������-������������ ����������
PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(PExceptionHandler value);

/// ���������� ��������-���������������� ����������
PExceptionPreprocessor GetExceptionPreprocessor(void) const;
bool SetExceptionPreprocessor(PExceptionPreprocessor value);

/// ���������� ��������-������������� ����������
PExceptionPostprocessor GetExceptionPostprocessor(void) const;
bool SetExceptionPostprocessor(PExceptionPostprocessor value);

/// ������� ���
void ClearLog(void);

/// ������� ��� ����������� ���������
void ClearReadLog(void);

// ����� ����������� ���������� ����� ��� ������� ������ ������ � ���
//void LogMessage(const std::string &str);
void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
void LogMessageEx(int msg_level, const std::string &object_name, const std::string &line, int error_event_number=0);
void LogMessageEx(int msg_level, const std::string &object_name, const std::string &method_name, const std::string &line, int error_event_number=0);

/// ����� �����������
virtual void Reset(void);

void RecreateEventsLogFile(void);
// --------------------------


};

}

#endif
