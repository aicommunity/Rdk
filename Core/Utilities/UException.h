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
struct EFatal: public Exception
{
// --------------------------
// ������������ � �����������
// --------------------------
EFatal(void);
EFatal(const EFatal &copy);
virtual ~EFatal(void);
// --------------------------

};

/* ������, �������������� ������������� */
struct EError: public Exception
{
// --------------------------
// ������������ � �����������
// --------------------------
EError(void);
EError(const EError &copy);
virtual ~EError(void);
// --------------------------

};

/* �������������� (�������� �� ������������� ������������� ��������) */
struct EWarning: public Exception
{
// --------------------------
// ������������ � �����������
// --------------------------
EWarning(void);
EWarning(const EWarning &copy);
virtual ~EWarning(void);
// --------------------------

};

/* �������������� ���������, ������ ������� ������������ ������������� */
struct EInfo: public Exception
{
// --------------------------
// ������������ � �����������
// --------------------------
EInfo(void);
EInfo(const EInfo &copy);
virtual ~EInfo(void);
// --------------------------
};

/* ������ �������������� ������ � ����� */
struct EStrToNumber: public EError
{
std::string Str; // ������

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

// ����������, ��������� � ����������������
struct EIdError: public EError
{
// �������������, ��������� ����������
int Id;

// --------------------------
// ������������ � �����������
// --------------------------
EIdError(int id);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// ����������, ��������� � �������
struct ENameError: public EError
{
// �������������, ��������� ����������
std::string Name;

// --------------------------
// ������������ � �����������
// --------------------------
ENameError(const std::string &name);
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


// Id �� ������
struct EIdNotExist: public EIdError
{
EIdNotExist(int id) : EIdError(id) {};
};

// ��� �� �������
struct ENameNotExist: public ENameError
{
ENameNotExist(const std::string &name) : ENameError(name) {};
};

// Id ��� ����������
struct EIdAlreadyExist: public EIdError
{
EIdAlreadyExist(int id) : EIdError(id) {};
};

// ��� ��� ����������
struct ENameAlreadyExist: public ENameError
{
ENameAlreadyExist(const std::string &name) : ENameError(name) {};
};

}

#endif

