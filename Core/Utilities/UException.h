#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>

namespace RDK {

//class UExceptionDispatcher;

/* ������� ����� ���������� */
class UException
{
protected: // ����� ������
// ��������� ���������� ����� ����������
static long long LastNumber;

// ��������� ����������. ������������ ������ ����� � ������ ������������
//static UExceptionDispatcher* Dispatcher;

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
UException(void);
UException(const UException &copy);
virtual ~UException(void);
// --------------------------

// --------------------------
// ������ ���������� ������ �������
// --------------------------
// ��������� ���������� ����� ����������
static long long GetLastNumber(void);

// ��������� ����������. ������������ ������ ����� � ������ ������������
//static UExceptionDispatcher* GetDispatcher(void);
//static bool SetDispatcher(UExceptionDispatcher* value);
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

};

/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
class UFatalException: public UException
{

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UFatalException(void);
UFatalException(const UFatalException &copy);
virtual ~UFatalException(void);
// --------------------------

};

/* ������, �������������� ������������� */
class UErrException: public UException
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UErrException(void);
UErrException(const UErrException &copy);
virtual ~UErrException(void);
// --------------------------

};

/* �������������� (�������� �� ������������� ������������� ��������) */
class UWarningException: public UException
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UWarningException(void);
UWarningException(const UWarningException &copy);
virtual ~UWarningException(void);
// --------------------------

};

/* �������������� ���������, ������ ������� ������������ ������������� */
class UInfoException: public UException
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UInfoException(void);
UInfoException(const UInfoException &copy);
virtual ~UInfoException(void);
// --------------------------
};

}

#endif

