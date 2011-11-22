#ifndef UEXCEPTION_CPP
#define UEXCEPTION_CPP

#include "UException.h"
//#include "UExceptionDispatcher.h"

namespace RDK {

// ��������� ���������� ����� ����������
long long UException::LastNumber=0;

// ��������� ����������
//UExceptionDispatcher* UException::Dispatcher=0;

/* class UException */
// --------------------------
// ������ ���������� ������ �������
// --------------------------
// ��������� ���������� ����� ����������
long long UException::GetLastNumber(void)
{
 return LastNumber;
}
/*
// ��������� ����������. ������������ ������ ����� � ������ ������������
UExceptionDispatcher* UException::GetDispatcher(void)
{
 return Dispatcher;
}

bool UException::SetDispatcher(UExceptionDispatcher* value)
{
 if(Dispatcher == value)
  return true;

 Dispatcher = value;
 return true;
} */
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
UException::UException(void)
: Type(0)
{
 Number=++LastNumber;
 std::time(&Time);
}

UException::UException(const UException &copy)
{
 Number=copy.Number;
 Time=copy.Time;
}


UException::~UException(void)
{

}
// --------------------------


// --------------------------
// ������ ��������� ������� ����������
// --------------------------
// ���������� ���������� ����� ����������
long long UException::GetNumber(void) const
{
 return Number;
}

// ��� ����������
int UException::GetType(void) const
{
 return Type;
}

// ����� ������������� (���������) ����������
std::time_t UException::GetTime(void) const
{
 return Time;
}

void UException::SetTime(std::time_t ex_time)
{
 Time=ex_time;
}
// --------------------------




/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
//class UFatalException: public UException

// --------------------------
// ������������ � �����������
// --------------------------
UFatalException::UFatalException(void)
{
 Type=1;
}

UFatalException::UFatalException(const UFatalException &copy)
{

}

UFatalException::~UFatalException(void)
{

}
// --------------------------



/* ������, �������������� ������������� */
//class UErrException: public UException
// --------------------------
// ������������ � �����������
// --------------------------
UErrException::UErrException(void)
{
 Type=2;
}

UErrException::UErrException(const UErrException &copy)
{

}

UErrException::~UErrException(void)
{

}
// --------------------------



/* �������������� (�������� �� ������������� ������������� ��������) */
//class UWarningException: public UException
// --------------------------
// ������������ � �����������
// --------------------------
UWarningException::UWarningException(void)
{
 Type=3;
}

UWarningException::UWarningException(const UWarningException &copy)
{

}

UWarningException::~UWarningException(void)
{

}
// --------------------------


/* �������������� ���������, ������ ������� ������������ ������������� */
//class UInfoException: public UException
// --------------------------
// ������������ � �����������
// --------------------------
UInfoException::UInfoException(void)
{
 Type=4;
}

UInfoException::UInfoException(const UInfoException &copy)
{


}

UInfoException::~UInfoException(void)
{

}
// --------------------------

}
#endif

