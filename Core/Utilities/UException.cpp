#ifndef UEXCEPTION_CPP
#define UEXCEPTION_CPP

#include "UException.h"
//#include "ExceptionDispatcher.h"

namespace RDK {

// ��������� ���������� ����� ����������
long long Exception::LastNumber=0;

// ��������� ����������
//ExceptionDispatcher* Exception::Dispatcher=0;

/* class Exception */
// --------------------------
// ������ ���������� ������ �������
// --------------------------
// ��������� ���������� ����� ����������
long long Exception::GetLastNumber(void)
{
 return LastNumber;
}
/*
// ��������� ����������. ������������ ������ ����� � ������ ������������
ExceptionDispatcher* Exception::GetDispatcher(void)
{
 return Dispatcher;
}

bool Exception::SetDispatcher(ExceptionDispatcher* value)
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
Exception::Exception(void)
: Type(0)
{
 Number=++LastNumber;
 std::time(&Time);
}

Exception::Exception(const Exception &copy)
{
 Number=copy.Number;
 Time=copy.Time;
}


Exception::~Exception(void)
{

}
// --------------------------


// --------------------------
// ������ ��������� ������� ����������
// --------------------------
// ���������� ���������� ����� ����������
long long Exception::GetNumber(void) const
{
 return Number;
}

// ��� ����������
int Exception::GetType(void) const
{
 return Type;
}

// ����� ������������� (���������) ����������
std::time_t Exception::GetTime(void) const
{
 return Time;
}

void Exception::SetTime(std::time_t ex_time)
{
 Time=ex_time;
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
std::string Exception::CreateLogMessage(void) const
{
 std::string result;

 result+=sntoa(GetNumber());
 result+=" ";
 result+=sntoa(GetTime());
 result+=" ";
 result+=sntoa(GetType());
 result+="> ";
 result+=typeid(*this).name();

 return result;
};
// --------------------------





/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
//class EFatal: public Exception

// --------------------------
// ������������ � �����������
// --------------------------
EFatal::EFatal(void)
{
 Type=1;
}

EFatal::EFatal(const EFatal &copy)
{

}

EFatal::~EFatal(void)
{

}
// --------------------------



/* ������, �������������� ������������� */
//class EError: public Exception
// --------------------------
// ������������ � �����������
// --------------------------
EError::EError(void)
{
 Type=2;
}

EError::EError(const EError &copy)
{

}

EError::~EError(void)
{

}
// --------------------------



/* �������������� (�������� �� ������������� ������������� ��������) */
//class EWarning: public Exception
// --------------------------
// ������������ � �����������
// --------------------------
EWarning::EWarning(void)
{
 Type=3;
}

EWarning::EWarning(const EWarning &copy)
{

}

EWarning::~EWarning(void)
{

}
// --------------------------


/* �������������� ���������, ������ ������� ������������ ������������� */
//class EInfo: public Exception
// --------------------------
// ������������ � �����������
// --------------------------
EInfo::EInfo(void)
{
 Type=4;
}

EInfo::EInfo(const EInfo &copy)
{


}

EInfo::~EInfo(void)
{

}
// --------------------------

/* ������ �������������� ������ � ����� */
// --------------------------
// ������������ � �����������
// --------------------------
EStrToNumber::EStrToNumber(const std::string &str)
 : Str(str)
{

}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string EStrToNumber::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Str=")+Str;
}
// --------------------------

// ����������, ��������� � ����������������
//struct EIdError
// --------------------------
// ������������ � �����������
// --------------------------
EIdError::EIdError(int id) : Id(id)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string EIdError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Id=")+sntoa(Id);
}
// --------------------------

// ����������, ��������� � �������
//struct ENameError
// --------------------------
// ������������ � �����������
// --------------------------
ENameError::ENameError(const std::string &name) : Name(name)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string ENameError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Name=")+Name;
}
// --------------------------

// ����������, ��������� � ���������
// --------------------------
// ������������ � �����������
// --------------------------
EIndexError::EIndexError(int index) : Index(index)
{
}
// --------------------------

// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string EIndexError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Index=")+sntoa(Index);
}
// --------------------------

// --------------------------
// ������������ � �����������
// --------------------------
ESystemException::ESystemException(void)
{

}

ESystemException::ESystemException(const ESystemException &copy)
{

}

ESystemException::~ESystemException(void)
{

}
// --------------------------


// --------------------------
// ������ ������������ ����
// --------------------------
// ��������� ������ ���� �� ����������
std::string ESystemException::CreateLogMessage(void) const
{
 return EFatal::CreateLogMessage();
}
// --------------------------

}
#endif

