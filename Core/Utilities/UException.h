#ifndef UEXCEPTION_H
#define UEXCEPTION_H

#include <ctime>
#include <string>
#include "../../Deploy/Include/initdll_defs.h"

#define RDK_EX_UNKNOWN 0
#define RDK_EX_FATAL 1
#define RDK_EX_ERROR 2
#define RDK_EX_WARNING 3
#define RDK_EX_INFO 4
#define RDK_EX_DEBUG 5

#define RDK_SYS_DEBUG_CALC 1
#define RDK_SYS_DEBUG_RESET 2
#define RDK_SYS_DEBUG_PROPERTIES 4
#define RDK_SYS_DEBUG_PARAMETERS 8
#define RDK_SYS_DEBUG_STATES 16
#define RDK_SYS_DEBUG_INPUTS 32
#define RDK_SYS_DEBUG_OUTPUTS 64

#define RDK_SYS_MESSAGE_ENTER 1
#define RDK_SYS_MESSAGE_EXIT_OK 2
#define RDK_SYS_MESSAGE_EXIT_ININIT_FAIL 4
#define RDK_SYS_MESSAGE_NEW_CALC_ITERATION 8


namespace RDK {

/* Базовый класс исключений */
class RDK_LIB_TYPE UException: public std::exception
{
protected: // Данные исключения
// Порядковый номер исключения
//long long Number;

/// Тип исключения
/// 0 - неопределено
/// 1 - фатальное
/// 2 - ошибка, требующая вмешательства
/// 3 - предупреждение
/// 4 - информация
/// 5 - отладка
int Type;

// Время возникновения исключения
std::time_t Time;

/// Имя файла в котором произошло исключение
mutable std::string ExFileName;

/// Строка на которой произошло исключение
mutable int ExLineNumber;

/// Имя объекта сгенерировавшего искючение
mutable std::string ObjectName;

/// Сообщение исключения
mutable std::string Message;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UException(void);
UException(const UException &copy);
virtual ~UException(void) throw();
// --------------------------

// --------------------------
// Методы управления общими данными
// --------------------------
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

/// Имя файла в котором произошло исключение
std::string GetExFileName(void) const;
void SetExFileName(const std::string &value);

/// Строка на которой произошло исключение
int GetExLineNumber(void) const;
void SetExLineNumber(int value);

/// Имя объекта сгенерировавшего искючение
std::string GetObjectName(void) const;
void SetObjectName(const std::string &value);
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
/// Возвращает строку лога об исключении
virtual char const * what() const throw();

protected:
/// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


/* Фатальные ошибки (обращение по 0 указателям и т.п.) */
struct RDK_LIB_TYPE EFatal: public UException
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EFatal(void);
EFatal(const EFatal &copy);
virtual ~EFatal(void) throw();
// --------------------------

};

/* Ошибки, корректируемые пользователем */
struct RDK_LIB_TYPE EError: public UException
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EError(void);
EError(const EError &copy);
virtual ~EError(void) throw();
// --------------------------

};

/* Предупреждения (например об неэффективном использовании ресурсов) */
struct RDK_LIB_TYPE EWarning: public UException
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EWarning(void);
EWarning(const EWarning &copy);
virtual ~EWarning(void) throw();
// --------------------------

};

/* Информационные сообщения, выдача которых инициируется пользователем */
struct RDK_LIB_TYPE EInfo: public UException
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EInfo(void);
EInfo(const EInfo &copy);
virtual ~EInfo(void) throw();
// --------------------------
};

/* Отладочные сообщения, выдача которых инициируется пользователем */
struct RDK_LIB_TYPE EDebug: public UException
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
EDebug(void);
EDebug(const EDebug &copy);
virtual ~EDebug(void) throw();
// --------------------------
};

/* Ошибка преобразования строки в число */
struct RDK_LIB_TYPE EStrToNumber: public EError
{
std::string Str; // Строка

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStrToNumber(const std::string &str);
virtual ~EStrToNumber(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключения, связанные с идентификаторами
struct RDK_LIB_TYPE EIdError: public EError
{
// Идентификатор, вызвавший исключение
int Id;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EIdError(int id);
virtual ~EIdError(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключения, связанные с именами
struct RDK_LIB_TYPE ENameError: public EError
{
// Идентификатор, вызвавший исключение
std::string Name;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ENameError(const std::string &name);
virtual ~ENameError(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};


// Id не найден
struct RDK_LIB_TYPE EIdNotExist: public EIdError
{
EIdNotExist(int id) : EIdError(id) {};
};

// Имя не найдено
struct RDK_LIB_TYPE ENameNotExist: public ENameError
{
ENameNotExist(const std::string &name) : ENameError(name) {};
};

// Id уже существует
struct RDK_LIB_TYPE EIdAlreadyExist: public EIdError
{
EIdAlreadyExist(int id) : EIdError(id) {};
};

// Имя уже существует
struct RDK_LIB_TYPE ENameAlreadyExist: public ENameError
{
ENameAlreadyExist(const std::string &name) : ENameError(name) {};
};

// Id не определен (forbidden id)
struct RDK_LIB_TYPE EForbiddenId: public EIdError
{
EForbiddenId(int id) : EIdError(id) {};
};

// Id не корректен
struct RDK_LIB_TYPE EInvalidId: public EIdError
{
EInvalidId(int id) : EIdError(id) {};
};

// Исключения, связанные с индексами
struct RDK_LIB_TYPE EIndexError: public EError
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
struct RDK_LIB_TYPE EInvalidIndex: public EIndexError
{
EInvalidIndex(int index) : EIndexError(index) {};
};


struct RDK_LIB_TYPE ESystemException: public EFatal
{
// Дополнительная информация об исключении
std::string Info;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
ESystemException(void);
ESystemException(const std::string &info);
ESystemException(const ESystemException &copy);
virtual ~ESystemException(void) throw();
// --------------------------


// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------

};

// Исключения, связанные с обработкой возвращаемых значений методов
struct RDK_LIB_TYPE EFunctionReturnFalse: public EError
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
virtual ~EFunctionReturnFalse(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключения, связанные с обработкой возвращаемых значений методов
struct RDK_LIB_TYPE EFunctionReturnError: public EError
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
virtual ~EFunctionReturnError(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключение с простой строкой текста как фатальная ошибка
struct RDK_LIB_TYPE EStringFatal: public EFatal
{
std::string Str;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringFatal(const std::string &str);
virtual ~EStringFatal(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключение с простой строкой текста как ошибка
struct RDK_LIB_TYPE EStringError: public EError
{
std::string Str;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringError(const std::string &str);
virtual ~EStringError(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключение с простой строкой текста как предупреждение
struct RDK_LIB_TYPE EStringWarning: public EWarning
{
std::string Str;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringWarning(const std::string &str);
virtual ~EStringWarning(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключение с простой строкой текста как информационное сообщение
struct RDK_LIB_TYPE EStringInfo: public EInfo
{
std::string Str;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringInfo(const std::string &str);
virtual ~EStringInfo(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключение с простой строкой текста как информационное сообщение
struct RDK_LIB_TYPE EStringDebug: public EDebug
{
std::string Str;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
EStringDebug(const std::string &str);
virtual ~EStringDebug(void) throw();
// --------------------------

// --------------------------
// Методы формирования лога
// --------------------------
// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const;
// --------------------------
};

// Исключение, которое невозможно обработать
struct RDK_LIB_TYPE UExceptionUnhandled: public EFatal
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
UExceptionUnhandled(const std::string &file_name, int line, const std::string &function_name);
virtual ~UExceptionUnhandled(void) throw();
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

