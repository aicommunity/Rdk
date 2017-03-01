#ifndef ULoggerEnvH
#define ULoggerEnvH

#include "ULogger.h"
#include "../Utilities/UException.h"

namespace RDK {

class UEnvironment;
class UContainer;

class RDK_LIB_TYPE ULoggerEnv: public ULogger
{
public:
/// Прототип функции обратного вызова обработчика исключений
typedef void (*PExceptionHandler)(int channel_index);

/// Прототип функции предобработки исключений
typedef bool (*PExceptionPreprocessor)(UEnvironment * env, UContainer *model, const UException &in_exception, UException &out_exception);

/// Прототип функции постобработки исключений
typedef bool (*PExceptionPostprocessor)(UEnvironment * env, UContainer *model, const UException &exception);

protected: // Параметры
/// Флаг включения режима отладки
bool DebugMode;

/// Маска системных событий для логирования
unsigned int DebugSysEventsMask;

/// Флаг включения вывода лога в отладчик
bool DebuggerMessageFlag;

/// Флаг включения внутренней регистрации событий в лог-файл
/// true - регистрация включена
bool EventsLogMode;

protected: // Данные
// Внешний обработчик исключений
PExceptionHandler ExceptionHandler;

/// Внешняя функция предобработки исключений
PExceptionPreprocessor ExceptionPreprocessor;

/// Внешняя функция постобработки исключений
PExceptionPostprocessor ExceptionPostprocessor;

/// Указатель на среду выполнения
UEnvironment* Environment;

/// Указатель на глобальный логгер объединяющий информацию со всех логгеров
ULoggerEnv* GlobalLogger;

/// Текущее число исключений системы
mutable int CurrentExceptionsLogSize;

/// Максимальное число хранимых исключений
/// Если 0, то неограниченно
int MaxExceptionsLogSize;

/// Индекс последнего считанного символа лога исключений
unsigned LastReadExceptionLogIndex;

/// Максимальный уровень непрочитанных сообщений в логе
mutable int LastErrorLevel;

/// Индекс текущего канала в многоканальной библиотеке
int ChannelIndex;

/// Временное хранилище строк
mutable string TempString;

mutable std::map<unsigned, UException > LogList;

/// Индекс последней строки лога
mutable unsigned LogIndex;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
ULoggerEnv(void);
virtual ~ULoggerEnv(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Флаг включения режима отладки
bool GetDebugMode(void) const;
bool SetDebugMode(bool value);

/// Маска системных событий для логирования
unsigned int GetDebugSysEventsMask(void) const;
bool SetDebugSysEventsMask(unsigned int value);

/// Возвращает флаг включения вывода лога в отладчик
bool GetDebuggerMessageFlag(void) const;

/// Устанавливает флаг включения вывода лога в отладчик
bool SetDebuggerMessageFlag(bool value);

/// Флаг включения внутренней регистрации событий в лог-файл
/// true - регистрация включена
bool GetEventsLogMode(void) const;
bool SetEventsLogMode(bool value);

/// Индекс текущего канала в многоканальной библиотеке
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// Методы управления исключениями
// --------------------------
/// Регистрация среды выполнения
bool RegisterEnvironment(UEnvironment* env);
void UnRegisterEnvironment(void);

/// Регистрация глобального логгера
bool RegisterGlobalLogger(ULoggerEnv* global_logger);
void UnRegisterGlobalLogger(void);

/// Обрабатывает возникшее исключение
virtual void ProcessException(const UException &exception) const;

/// Обрабатывает возникшее исключение (Внутренний метод)
virtual void ProcessExceptionRaw(int type, const UException &exception) const;

/// Обрабатывает возникшее исключение в режиме гобального логгера
virtual void ProcessExceptionGlobal(int type, const UException &exception) const;

/// Максимальное число хранимых исключений
/// Если 0, то неограниченно
int GetMaxExceptionsLogSize(void) const;
void SetMaxExceptionsLogSize(int value);

/// Возвращает массив строк лога
/// Методы GetLog, GetLogLine, GetUnreadLog используют разделяемый ресурс TempString
/// Недопустим одновременный вызов этих методов разных потоков
const char* GetLog(int &error_level) const;

/// Возвращает число строк лога
int GetNumLogLines(void) const;

/// Возвращает строку лога с индексом i
/// Методы GetLog, GetLogLine, GetUnreadLog используют разделяемый ресурс TempString
/// Недопустим одновременный вызов этих методов разных потоков
const char* GetLogLine(int i, int &error_level, int &number, time_t &time) const;

/// Возвращает число непрочитанных строк лога
int GetNumUnreadLogLines(void) const;

/// Возвращает частичный массив строк лога с момента последнего считывания лога
/// этой функцией
/// Методы GetLog, GetLogLine, GetUnreadLog используют разделяемый ресурс TempString
/// Недопустим одновременный вызов этих методов разных потоков
const char* GetUnreadLog(int &error_level, int &number, time_t &time);
bool GetUnreadLog(UException &ex);

// Управление функцией-обработчиком исключений
PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(PExceptionHandler value);

/// Управление функцией-предобработчиком исключений
PExceptionPreprocessor GetExceptionPreprocessor(void) const;
bool SetExceptionPreprocessor(PExceptionPreprocessor value);

/// Управление функцией-постобработки исключений
PExceptionPostprocessor GetExceptionPostprocessor(void) const;
bool SetExceptionPostprocessor(PExceptionPostprocessor value);

/// Очищает лог
void ClearLog(void);

/// Очищает лог прочитанных сообщений
void ClearReadLog(void);

// Вызов обработчика исключений среды для простой записи данных в лог
//void LogMessage(const std::string &str);
void LogMessage(int msg_level, const std::string &line, int error_event_number=0);
void LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number=0);
void LogMessageEx(int msg_level, const std::string &object_name, const std::string &line, int error_event_number=0);
void LogMessageEx(int msg_level, const std::string &object_name, const std::string &method_name, const std::string &line, int error_event_number=0);

/// Сброс логирования
virtual void Reset(void);

void RecreateEventsLogFile(void);
// --------------------------


};

}

#endif
