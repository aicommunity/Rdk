#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>
#include <string>

namespace RDK {

/* Базовый класс исключений */
class Exception
{
protected: // Общие данные
// Последний порядковый номер исключения
static long long LastNumber;

// Диспетчер исключений. Осуществляет запись логов и другую деятельность
//static ExceptionDispatcher* Dispatcher;

protected: // Данные исключения
// Порядковый номер исключения
long long Number;

// Тип исключения
// 0 - неопределено
// 1 - фатальное
// 2 - ошибка, требующая вмешательства
// 3 - предупреждение
// 4 - информация
int Type;

// Время возникновения исключения
std::time_t Time;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
Exception(void);
Exception(const Exception &copy);
virtual ~Exception(void);
// --------------------------

// --------------------------
// Методы управления общими данными
// --------------------------
// Последний порядковый номер исключения
static long long GetLastNumber(void);

// Диспетчер исключений. Осуществляет запись логов и другую деятельность
//static ExceptionDispatcher* GetDispatcher(void);
//static bool SetDispatcher(ExceptionDispatcher* value);
// --------------------------

// --------------------------
// Методы упрвления данными исключения
// --------------------------
// Возвращает порядковый номер исключения
long long GetNumber(void) const;

// Тип исключения
int GetType(void) const;

// Время возникновения (обработки) исключения
std::time_t GetTime(void) const;
void SetTime(std::time_t ex_time);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


/* Фатальные ошибки (обращение по 0 указателям и т.п.) */
class EFatal: public Exception
{

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFatal(void);
EFatal(const EFatal &copy);
virtual ~EFatal(void);
// --------------------------

};

/* Ошибки, корректируемые пользователем */
class EError: public Exception
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EError(void);
EError(const EError &copy);
virtual ~EError(void);
// --------------------------

};

/* Предупреждения (например об неэффективном использовании ресурсов) */
class EWarning: public Exception
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EWarning(void);
EWarning(const EWarning &copy);
virtual ~EWarning(void);
// --------------------------

};

/* Информационные сообщения, выдача которых инициируется пользователем */
class EInfo: public Exception
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EInfo(void);
EInfo(const EInfo &copy);
virtual ~EInfo(void);
// --------------------------
};

/* Ошибка преобразования строки в число */
class EStrToNumber: public EError
{
public: // Данные
std::string Str;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStrToNumber(const std::string &str);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};



}

#endif

