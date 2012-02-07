#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>
#include <string>

namespace RDK {

/* ������� ����� ���������� */
class Exception
{
protected: // ����� ������
// ��������� ���������� ����� ����������
static long long LastNumber;

// ��������� ����������. ������������ ������ ����� � ������ ������������
//static ExceptionDispatcher* Dispatcher;

protected: // ������ ����������
// ���������� ����� ����������
long long Number;

// ��� ����������
// 0 - ������������
// 1 - ���������
// 2 - ������, ��������� �������������
// 3 - ��������������
// 4 - ����������
int Type;

// ����� ������������� ����������
std::time_t Time;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
Exception(void);
Exception(const Exception &copy);
virtual ~Exception(void);
// --------------------------

// --------------------------
// ������ ���������� ������ �������
// --------------------------
// ��������� ���������� ����� ����������
static long long GetLastNumber(void);

// ��������� ����������. ������������ ������ ����� � ������ ������������
//static ExceptionDispatcher* GetDispatcher(void);
//static bool SetDispatcher(ExceptionDispatcher* value);
// --------------------------

// --------------------------
// ������ ��������� ������� ����������
// --------------------------
// ���������� ���������� ����� ����������
long long GetNumber(void) const;

// ��� ����������
int GetType(void) const;

// ����� ������������� (���������) ����������
std::time_t GetTime(void) const;
void SetTime(std::time_t ex_time);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
class EFatal: public Exception
{

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EFatal(void);
EFatal(const EFatal &copy);
virtual ~EFatal(void);
// --------------------------

};

/* ������, �������������� ������������� */
class EError: public Exception
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EError(void);
EError(const EError &copy);
virtual ~EError(void);
// --------------------------

};

/* �������������� (�������� �� ������������� ������������� ��������) */
class EWarning: public Exception
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EWarning(void);
EWarning(const EWarning &copy);
virtual ~EWarning(void);
// --------------------------

};

/* �������������� ���������, ������ ������� ������������ ������������� */
class EInfo: public Exception
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EInfo(void);
EInfo(const EInfo &copy);
virtual ~EInfo(void);
// --------------------------
};

/* ������ �������������� ������ � ����� */
class EStrToNumber: public EError
{
public: // ������
std::string Str;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
EStrToNumber(const std::string &str);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};



}

#endif

