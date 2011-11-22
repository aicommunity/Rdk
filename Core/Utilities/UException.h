#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>

namespace RDK {

//class UExceptionDispatcher;

/* Базовый класс исключений */
class UException
{
protected: // Общие данные
// Последний порядковый номер исключения
static long long LastNumber;

// Диспетчер исключений. Осуществляет запись логов и другую деятельность
//static UExceptionDispatcher* Dispatcher;

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
UException(void);
UException(const UException &copy);
virtual ~UException(void);
// --------------------------

// --------------------------
// Методы управления общими данными
// --------------------------
// Последний порядковый номер исключения
static long long GetLastNumber(void);

// Диспетчер исключений. Осуществляет запись логов и другую деятельность
//static UExceptionDispatcher* GetDispatcher(void);
//static bool SetDispatcher(UExceptionDispatcher* value);
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

};

/* Фатальные ошибки (обращение по 0 указателям и т.п.) */
class UFatalException: public UException
{

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UFatalException(void);
UFatalException(const UFatalException &copy);
virtual ~UFatalException(void);
// --------------------------

};

/* Ошибки, корректируемые пользователем */
class UErrException: public UException
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UErrException(void);
UErrException(const UErrException &copy);
virtual ~UErrException(void);
// --------------------------

};

/* Предупреждения (например об неэффективном использовании ресурсов) */
class UWarningException: public UException
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UWarningException(void);
UWarningException(const UWarningException &copy);
virtual ~UWarningException(void);
// --------------------------

};

/* Информационные сообщения, выдача которых инициируется пользователем */
class UInfoException: public UException
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UInfoException(void);
UInfoException(const UInfoException &copy);
virtual ~UInfoException(void);
// --------------------------
};

}

#endif

