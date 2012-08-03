#ifndef UEXCEPTION_CPP
#define UEXCEPTION_CPP

#include "UException.h"
//#include "ExceptionDispatcher.h"

namespace RDK {

// Последний порядковый номер исключения
long long Exception::LastNumber=0;

// Диспетчер исключений
//ExceptionDispatcher* Exception::Dispatcher=0;

/* class Exception */
// --------------------------
// Методы управления общими данными
// --------------------------
// Последний порядковый номер исключения
long long Exception::GetLastNumber(void)
{
 return LastNumber;
}
/*
// Диспетчер исключений. Осуществляет запись логов и другую деятельность
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
// Конструкторы и деструкторы
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
// Методы упрвления данными исключения
// --------------------------
// Возвращает порядковый номер исключения
long long Exception::GetNumber(void) const
{
 return Number;
}

// Тип исключения
int Exception::GetType(void) const
{
 return Type;
}

// Время возникновения (обработки) исключения
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
// Методы формирования лога
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





/* Фатальные ошибки (обращение по 0 указателям и т.п.) */
//class EFatal: public Exception

// --------------------------
// Конструкторы и деструкторы
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



/* Ошибки, корректируемые пользователем */
//class EError: public Exception
// --------------------------
// Конструкторы и деструкторы
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



/* Предупреждения (например об неэффективном использовании ресурсов) */
//class EWarning: public Exception
// --------------------------
// Конструкторы и деструкторы
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


/* Информационные сообщения, выдача которых инициируется пользователем */
//class EInfo: public Exception
// --------------------------
// Конструкторы и деструкторы
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

/* Ошибка преобразования строки в число */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStrToNumber::EStrToNumber(const std::string &str)
 : Str(str)
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

ESystemException::ESystemException(const ESystemException &copy)
{

}

ESystemException::~ESystemException(void)
{

}
// --------------------------


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
std::string ESystemException::CreateLogMessage(void) const
{
 return EFatal::CreateLogMessage();
}
// --------------------------

}
#endif

