#ifndef ULoggerH
#define ULoggerH

#include "../Utilities/USupport.h"
#include "../System/rdk_system.h"
#include "../System/UGenericMutex.h"
#include "UELockVar.h"
#include "UEPtr.h"

namespace RDK {

class RDK_LIB_TYPE ULogger
{
protected: // ���������
/// ���� �� ����� � ������
RDK::UELockVar<std::string> LogDir;

/// ������� ����� ����� � �����
RDK::UELockVar<std::string> Suffix;

protected: // ������
/// ���� ��� ���������� �����
RDK::UEPtr<std::ofstream> EventsLogFile;

/// ����, ����������� ������ � ���
/// ������������, ���� ���������� ������� ������, ��� ����
bool LogEnabledFlag;

UGenericMutex* LogMutex;

public: // ������������ � �����������
ULogger(void);
virtual ~ULogger(void);

public: // ������ ���������� �����������
/// ���� �� ����� � ������
std::string GetLogDir(void) const;
bool SetLogDir(const std::string &value);

/// ������� ����� �����
std::string GetSuffix(void) const;
bool SetSuffix(const std::string &value);

public: // ������ ������ �����
/// �������������� ��� � ������� ����� �����
int InitLog(void);

/// ��������� ������ � ���
int WriteMessageToFile(const std::string &str);

/// ��������� ����� � ������ � ������� ��������� �������� ����������
int Clear(void);

/// ���������� true ���� ���� ������ ����� ������
bool IsLogFileCreated(void) const;

};

}

#endif
