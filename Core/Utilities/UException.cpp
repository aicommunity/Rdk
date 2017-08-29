#ifndef UEXCEPTION_CPP
#define UEXCEPTION_CPP

#include <typeinfo>
#include "UException.h"
#include "USupport.h"
#include "../System/rdk_system.h"
//#include "ExceptionDispatcher.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UException::UException(void)
: Number(0), Type(0), ExLineNumber(-1)
{
 unsigned long long seconds(0);
 GetTimeOfDayInMicroseconds(seconds, TimeMsecs);
 Time=seconds;
// std::time(&Time);
 Time=seconds;
}

UException::UException(const UException &copy)
: Number(copy.Number), Type(copy.Type), ExFileName(copy.ExFileName), ExLineNumber(copy.ExLineNumber),
  ObjectName(copy.ObjectName), InfoMessageString(copy.InfoMessageString)
{
// Number=copy.Number;
 Time=copy.Time;
}


UException::~UException(void) throw()
{

}
// --------------------------


// --------------------------
// Методы упрвления данными исключения
// --------------------------
// Возвращает номер исключения
int UException::GetNumber(void) const
{
 return Number;
}

// Тип исключения
int UException::GetType(void) const
{
 return Type;
}

// Время возникновения (обработки) исключения
std::time_t UException::GetTime(void) const
{
 return Time;
}

void UException::SetTime(std::time_t ex_time)
{
 Time=ex_time;
}

/// Имя файла в котором произошло исключение
std::string UException::GetExFileName(void) const
{
 return ExFileName;
}

void UException::SetExFileName(const std::string &value)
{
 ExFileName=value;
}

/// Строка на которой произошло исключение
int UException::GetExLineNumber(void) const
{
 return ExLineNumber;
}

void UException::SetExLineNumber(int value)
{
 ExLineNumber=value;
}

/// Имя объекта сгенерировавшего искючение
std::string UException::GetObjectName(void) const
{
 return ObjectName;
}

void UException::SetObjectName(const std::string &value)
{
 ObjectName=value;
}

/// Сообщение исключения сохраненное во внутреннем буфере
const std::string& UException::GetMessage(void) const
{
 return Message;
}

void UException::SetMessage(const std::string& value)
{
 Message=value;
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
/// Возвращает строку лога об исключении
char const * UException::what() const throw()
{
 Message=GenerateLogPrefix();
 Message+=CreateLogMessage();
 Message+=InfoMessageString;
 return Message.c_str();
}

/// Оборачивает этим исключнением уже существующее, с возможностью подмены требуемых данных
void UException::Wrap(const UException &ex, const std::string &new_message)
{
 Number=ex.Number;
 Time=ex.Time;
 ExFileName=ex.ExFileName;
 ExLineNumber=ex.ExLineNumber;
 ObjectName=ex.ObjectName;
 InfoMessageString=ex.CreateLogMessage()+ex.InfoMessageString+new_message;
}

void UException::Wrap(const UException &ex, const std::string &new_message, int number)
{
 Wrap(ex, new_message);
 Number=number;
}

void UException::Wrap(const UException &ex, const std::string &new_message, int number, int new_type)
{
 Wrap(ex, new_message, number);
 Type=new_type;
}

std::string UException::CreateLogMessage(void) const
{
 std::string result;
 return result;
};

std::string UException::GenerateLogPrefix(void) const
{
 std::string result;

 std::time_t ex_time=GetTime();
 tm time_result;
 tm* time_struct(0);

#ifdef _MSC_VER
 localtime_s(&time_result,&ex_time);
 time_struct=&time_result;
#else
 time_struct=localtime_s(&ex_time,&time_result);
#endif

 if(!time_struct)
  return result;

 result+=sntoa(time_struct->tm_mday,2);
 result+="/";
 result+=sntoa(time_struct->tm_mon+1,2);
 result+="/";
 result+=sntoa(time_struct->tm_year+1900,4);
 result+=" ";

 result+=sntoa(time_struct->tm_hour,2);
 result+=":";
 result+=sntoa(time_struct->tm_min,2);
 result+=":";
 result+=sntoa(time_struct->tm_sec,2);
 result+=".";
 result+=sntoa(TimeMsecs/1000,3);
 result+=" ";

 result+=sntoa(GetType());
 result+="> [";
 if(Number != 0)
 {
  result+=sntoa(Number);
  result+=": ";
 }
 result+=typeid(*this).name();
 result+="]";

 if(!ExFileName.empty())
 {
  result+="> ";
  result+=ExFileName;
  result+=":";
  result+=sntoa(ExLineNumber);
  result+="> ";
 }

 if(!ObjectName.empty())
 {
  result+=" ";
  result+=ObjectName;
  result+="> ";
 }

 return result;
}
// --------------------------





/* Фатальные ошибки (обращение по 0 указателям и т.п.) */
//class EFatal: public UException

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFatal::EFatal(void)
{
 Type=RDK_EX_FATAL;
}

EFatal::EFatal(const EFatal &copy)
 : UException(copy)
{

}

EFatal::~EFatal(void) throw()
{

}
// --------------------------



/* Ошибки, корректируемые пользователем */
//class EError: public UException
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EError::EError(void)
{
 Type=RDK_EX_ERROR;
}

EError::EError(const EError &copy)
 : UException(copy)
{

}

EError::~EError(void) throw()
{

}
// --------------------------



/* Предупреждения (например об неэффективном использовании ресурсов) */
//class EWarning: public UException
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EWarning::EWarning(void)
{
 Type=RDK_EX_WARNING;
}

EWarning::EWarning(const EWarning &copy)
 : UException(copy)
{

}

EWarning::~EWarning(void) throw()
{

}
// --------------------------


/* Информационные сообщения, выдача которых инициируется пользователем */
//class EInfo: public UException
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EInfo::EInfo(void)
{
 Type=RDK_EX_INFO;
}

EInfo::EInfo(const EInfo &copy)
 : UException(copy)
{


}

EInfo::~EInfo(void) throw()
{

}
// --------------------------

/* Отладочные сообщения, выдача которых инициируется пользователем */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EDebug::EDebug(void)
{
 Type=RDK_EX_DEBUG;
}

EDebug::EDebug(const EDebug &copy)
 : UException(copy)
{

}

EDebug::~EDebug(void) throw()
{

}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EApp::EApp(void)
{
 Type=RDK_EX_APP;
}

EApp::EApp(const EApp &copy)
 : UException(copy)
{
}

EApp::~EApp(void) throw()
{
}
// --------------------------

/* Ошибка преобразования строки в число */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStrToNumber::EStrToNumber(const std::string &str)
 : Str(str)
{

}

EStrToNumber::~EStrToNumber(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EStrToNumber::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Str=")+Str;
}
// --------------------------

// Исключения, связанные с идентификаторами
//struct EIdError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EIdError::EIdError(int id) : Id(id)
{
}

EIdError::~EIdError(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EIdError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Id=")+sntoa(Id);
}
// --------------------------

// Исключения, связанные с именами
//struct ENameError
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ENameError::ENameError(const std::string &name) : Name(name)
{
}

ENameError::~ENameError(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string ENameError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Name=")+Name;
}
// --------------------------

// Исключения, связанные с индексами
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EIndexError::EIndexError(int index) : Index(index)
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EIndexError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+string(" Index=")+sntoa(Index);
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
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

ESystemException::~ESystemException(void) throw()
{

}
// --------------------------


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string ESystemException::CreateLogMessage(void) const
{
 return EFatal::CreateLogMessage()+Info;
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFunctionReturnFalse::EFunctionReturnFalse(const std::string &file_name, const std::string &function_name, int line)
 : FileName(file_name), FunctionName(function_name), Line(line)
{

}

EFunctionReturnFalse::~EFunctionReturnFalse(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EFunctionReturnFalse::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" File=")+FileName+
			std::string(" Function=")+FunctionName+
			std::string(" Line=")+sntoa(Line);
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFunctionReturnError::EFunctionReturnError(const std::string &file_name, const std::string &function_name, int line, int code)
 : FileName(file_name), FunctionName(function_name), Line(line), Code(code)
{

}

EFunctionReturnError::~EFunctionReturnError(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EFunctionReturnError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" File=")+FileName+
			std::string(" Function=")+FunctionName+
			std::string(" Line=")+sntoa(Line)+
			std::string(" Code=")+sntoa(Code);
}
// --------------------------

// Исключение с простой строкой текста как фатальная ошибка
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringFatal::EStringFatal(const std::string &str, int number)
 : Str(str)
{
 Number=number;
}

EStringFatal::~EStringFatal(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EStringFatal::CreateLogMessage(void) const
{
 return EFatal::CreateLogMessage()+std::string(" ")+Str;
}


// Исключение с простой строкой текста как фатальная ошибка
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringError::EStringError(const std::string &str, int number)
 : Str(str)
{
 Number=number;
}

EStringError::~EStringError(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EStringError::CreateLogMessage(void) const
{
 return EError::CreateLogMessage()+std::string(" ")+Str;
}

// --------------------------
// Исключение с простой строкой текста как предупреждение
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringWarning::EStringWarning(const std::string &str, int number)
 : Str(str)
{
 Number=number;
}

EStringWarning::~EStringWarning(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EStringWarning::CreateLogMessage(void) const
{
 return EWarning::CreateLogMessage()+std::string(" ")+Str;
}
// --------------------------


// Исключение с простой строкой текста как информационное сообщение
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringInfo::EStringInfo(const std::string &str, int number)
 : Str(str)
{
 Number=number;
}

EStringInfo::~EStringInfo(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EStringInfo::CreateLogMessage(void) const
{
 return EInfo::CreateLogMessage()+std::string(" ")+Str;
}
// --------------------------

// Исключение с простой строкой текста как отладочное сообщение
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringDebug::EStringDebug(const std::string &str, int number)
 : Str(str)
{
 Number=number;
}

EStringDebug::~EStringDebug(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EStringDebug::CreateLogMessage(void) const
{
 return EDebug::CreateLogMessage()+std::string(" ")+Str;
}
// --------------------------

/// Исключение для информирования о высокоуровневом событии приложения
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringApp::EStringApp(const std::string &str, int number)
 : Str(str)
{
 Number=number;
}

EStringApp::~EStringApp(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string EStringApp::CreateLogMessage(void) const
{
 return EApp::CreateLogMessage()+std::string(" ")+Str;
}
// --------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UExceptionUnhandled::UExceptionUnhandled(const std::string &file_name, int line, const std::string &function_name)
 : FileName(file_name), FunctionName(function_name), Line(line)
{

}

UExceptionUnhandled::~UExceptionUnhandled(void) throw()
{
}
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string UExceptionUnhandled::CreateLogMessage(void) const
{
 return EFatal::CreateLogMessage()+std::string(" File=")+FileName+
			std::string(":")+sntoa(Line)+
			std::string(" Func=")+FunctionName;
}
// --------------------------


}
#endif

