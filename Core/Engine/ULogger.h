#ifndef ULoggerH
#define ULoggerH

#include "../Utilities/USupport.h"
#include "../System/rdk_system.h"
#include "../System/UGenericMutex.h"
#include "UELockVar.h"
#include "UEPtr.h"

namespace RDK {

class ULogger
{
protected: // ���������
/// ���� �� ����� � ������
RDK::UELockVar<std::string> LogPath;

/// ������� ����� ����� � �����
RDK::UELockVar<std::string> Suffix;

protected: // ������
/// ���� ��� ���������� �����
RDK::UEPtr<std::ofstream> EventsLogFile;

/// ����, ����������� ������ � ���
/// ������������, ���� ���������� ������� ������, ��� ����
bool LogEnabledFlag;


public: // ������������ � �����������
ULogger(void);
virtual ~ULogger(void);

public: // ������ ���������� �����������
/// ���� �� ����� � ������
std::string GetLogPath(void) const;
bool SetLogPath(const std::string &value);

/// ����� ������������
/// 0 - ��������
/// 1 - ��������� ������ ����� � ��������� ���� ����
/// 2 - ��������� ��� ������ � ������ ���� ����
std::string GetSuffix(void) const;
bool SetSuffix(const std::string &value);

public: // ������ ������ �����
/// �������������� ��� � ������� ����� �����
int InitLog(void);

/// ��������� ������ � ���
int LogMessage(const std::string &str);

/// ��������� ����� � ������ � ������� ��������� �������� ����������
int Clear(void);

};

}

#endif
