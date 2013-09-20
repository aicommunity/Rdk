#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>
#include <string>

namespace RDK {

/* Базовый класс исключений */
class UException
{
protected: // Общие данные
// Последний порядковый номер исключения
//static long long LastNumber;

protected: // Данные исключения
// Порядковый номер исключения
//long long Number;

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
//static long long GetLastNumber(void);
// --------------------------

// --------------------------
// Методы упрвления данными исключения
// --------------------------
// Возвращает порядковый номер исключения
//long long GetNumber(void) const;

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
struct EFatal: public UException
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
struct EError: public UException
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
struct EWarning: public UException
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
struct EInfo: public UException
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

// Id не определен (forbidden id)
struct EForbiddenId: public EIdError
{
EForbiddenId(int id) : EIdError(id) {};
};

// Id не корректен
struct EInvalidId: public EIdError
{
EInvalidId(int id) : EIdError(id) {};
};

// Исключения, связанные с индексами
struct EIndexError: public EError
{
// Индекс, вызвавший исключение
int Index;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EIndexError(int index);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Id не корректен
struct EInvalidIndex: public EIndexError
{
EInvalidIndex(int index) : EIndexError(index) {};
};


struct ESystemException: public EFatal
{
// Дополнительная информация об исключении
std::string Info;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ESystemException(void);
ESystemException(const std::string &info);
ESystemException(const ESystemException &copy);
virtual ~ESystemException(void);
// --------------------------


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------

};

// Исключения, связанные с обработкой возвращаемых значений методов
struct EFunctionReturnFalse: public EError
{
// Имя файла
std::string FileName;

// Имя функции в которой произошла ошибка
std::string FunctionName;

// Строка в исходнике
int Line;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFunctionReturnFalse(const std::string &file_name, const std::string &function_name, int line);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключения, связанные с обработкой возвращаемых значений методов
struct EFunctionReturnError: public EError
{
// Имя файла
std::string FileName;

// Имя функции в которой произошла ошибка
std::string FunctionName;

// Строка в исходнике
int Line;

// Код ошибки
int Code;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFunctionReturnError(const std::string &file_name, const std::string &function_name, int line, int code);
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

