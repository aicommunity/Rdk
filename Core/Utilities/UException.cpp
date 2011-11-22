#ifndef UEXCEPTION_CPP
#define UEXCEPTION_CPP

#include "UException.h"
//#include "UExceptionDispatcher.h"

namespace RDK {

// Последний порядковый номер исключения
long long UException::LastNumber=0;

// Диспетчер исключений
//UExceptionDispatcher* UException::Dispatcher=0;

/* class UException */
// --------------------------
// Методы управления общими данными
// --------------------------
// Последний порядковый номер исключения
long long UException::GetLastNumber(void)
{
 return LastNumber;
}
/*
// Диспетчер исключений. Осуществляет запись логов и другую деятельность
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
// Конструкторы и деструкторы
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
// Методы упрвления данными исключения
// --------------------------
// Возвращает порядковый номер исключения
long long UException::GetNumber(void) const
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
// --------------------------




/* Фатальные ошибки (обращение по 0 указателям и т.п.) */
//class UFatalException: public UException

// --------------------------
// Конструкторы и деструкторы
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



/* Ошибки, корректируемые пользователем */
//class UErrException: public UException
// --------------------------
// Конструкторы и деструкторы
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



/* Предупреждения (например об неэффективном использовании ресурсов) */
//class UWarningException: public UException
// --------------------------
// Конструкторы и деструкторы
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


/* Информационные сообщения, выдача которых инициируется пользователем */
//class UInfoException: public UException
// --------------------------
// Конструкторы и деструкторы
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

