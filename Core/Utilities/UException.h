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
struct EFatal: public Exception
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFatal(void);
EFatal(const EFatal &copy);
virtual ~EFatal(void);
// --------------------------

};

/* Ошибки, корректируемые пользователем */
struct EError: public Exception
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EError(void);
EError(const EError &copy);
virtual ~EError(void);
// --------------------------

};

/* Предупреждения (например об неэффективном использовании ресурсов) */
struct EWarning: public Exception
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EWarning(void);
EWarning(const EWarning &copy);
virtual ~EWarning(void);
// --------------------------

};

/* Информационные сообщения, выдача которых инициируется пользователем */
struct EInfo: public Exception
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EInfo(void);
EInfo(const EInfo &copy);
virtual ~EInfo(void);
// --------------------------
};

/* Ошибка преобразования строки в число */
struct EStrToNumber: public EError
{
std::string Str; // Строка

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

// Исключения, связанные с идентификаторами
struct EIdError: public EError
{
// Идентификатор, вызвавший исключение
int Id;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EIdError(int id);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключения, связанные с именами
struct ENameError: public EError
{
// Идентификатор, вызвавший исключение
std::string Name;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ENameError(const std::string &name);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


// Id не найден
struct EIdNotExist: public EIdError
{
EIdNotExist(int id) : EIdError(id) {};
};

// Имя не найдено
struct ENameNotExist: public ENameError
{
ENameNotExist(const std::string &name) : ENameError(name) {};
};

// Id уже существует
struct EIdAlreadyExist: public EIdError
{
EIdAlreadyExist(int id) : EIdError(id) {};
};

// Имя уже существует
struct ENameAlreadyExist: public ENameError
{
ENameAlreadyExist(const std::string &name) : ENameError(name) {};
};

}

#endif

