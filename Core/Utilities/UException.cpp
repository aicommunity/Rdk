#ifndef UEXCEPTION_CPP
#define UEXCEPTION_CPP

#include "UException.h"
//#include "ExceptionDispatcher.h"

namespace RDK {

// ��������� ���������� ����� ����������
//long long UException::LastNumber=0;

// ��������� ����������
//ExceptionDispatcher* UException::Dispatcher=0;

/* class UException */
// --------------------------
// ������ ���������� ������ �������
// --------------------------
// ��������� ���������� ����� ����������
/*long long UException::GetLastNumber(void)
{
 return LastNumber;
} */
/*
// ��������� ����������. ������������ ������ ����� � ������ ������������
ExceptionDispatcher* UException::GetDispatcher(void)
{
 return Dispatcher;
}

bool UException::SetDispatcher(ExceptionDispatcher* value)
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
//Number=++LastNumber;
 std::time(&Time);
}

UException::UException(const UException &copy)
{
// Number=copy.Number;
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
/*long long UException::GetNumber(void) const
{
 return Number;
} */

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

// --------------------------
// ������ ������������ ����
// --------------------------
std::string UException::CreateLogMessage(void) const
{
 std::string result;
  /*
 result+=sntoa(GetNumber(),4);
 result+=" ";
    */
 std::time_t ex_time=GetTime();
 tm* time_stuct=localtime(&ex_time);

 result+=sntoa(time_stuct->tm_mday,2);
 result+="/";
 result+=sntoa(time_stuct->tm_mon+1,2);
 result+="/";
 result+=sntoa(time_stuct->tm_year+1900,4);
 result+=" ";

 result+=sntoa(time_stuct->tm_hour,2);
 result+=":";
 result+=sntoa(time_stuct->tm_min,2);
 result+=":";
 result+=sntoa(time_stuct->tm_sec,2);
 result+=" ";

 result+=sntoa(GetType());
 result+="> ";
 result+=typeid(*this).name();

 return result;
};
// --------------------------





/* ��������� ������ (��������� �� 0 ���������� � �.�.) */
//class EFatal: public UException

// --------------------------
// ������������ � �����������
// --------------------------
EFatal::EFatal(void)
{
 Type=1;
}

EFatal::EFatal(const EFatal &copy)
 : UException(copy)
{

}

EFatal::~EFatal(void)
{

}
// --------------------------



/* ������, �������������� ������������� */
//class EError: public UException
// --------------------------
// ������������ � �����������
// --------------------------
EError::EError(void)
{
 Type=2;
}

EError::EError(const EError &copy)
 : UException(copy)
{

}

EError::~EError(void)
{

}
// --------------------------



/* �������������� (�������� �� ������������� ������������� ��������) */
//class EWarning: public UException
// --------------------------
// ������������ � �����������
// --------------------------
EWarning::EWarning(void)
{
 Type=3;
}

EWarning::EWarning(const EWarning &copy)
 : UException(copy)
{

}

EWarning::~EWarning(void)
{

}
// --------------------------


/* �������������� ���������, ������ ������� ������������ ������������� */
//class EInfo: public UException
// --------------------------
// ������������ � �����������
// --------------------------
EInfo::EInfo(void)
{
 Type=4;
}

EInfo::EInfo(const EInfo &copy)
 : UException(copy)
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

ESystemException::ESystemException(const std::string &info)
: Info(info)
{

}

ESystemException::ESystemException(const ESystemException &copy)
: EFatal(copy), Info(copy.Info)
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
 return EFatal::CreateLogMessage()+Info;
}
// --------------------------

}
#endif

