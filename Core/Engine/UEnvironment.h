/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UAENVIRONMENT_H
#define UAENVIRONMENT_H

#include "UStorage.h"
#include "ULibrary.h"
#include "../System/rdk_system.h"

namespace RDK {

class RDK_LIB_TYPE UEnvironment: virtual public UModule
{
public:
// Прототип функции обратного вызова обработчика исключений
typedef void (*PExceptionHandler)(int channel_index);

protected: // Параметры
// Индекс предарительно заданной модели обработки
// 0 - Структура определяется извне
int PredefinedStructure;

// Имя текущего каталога хранения данных
std::string CurrentDataDir;

// Имя каталога бинарных файлов
std::string SystemDir;

/// Минимальный интервал времени между итерациями счета (мс)
ULongTime MinInterstepsInterval;

/// Флаг включения режима отладки
bool DebugMode;

protected: // Состояния
// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool StoragePresent;

// Признак успешной инициализации
bool Initialized;

// Признак наличия сформированной структуры
bool Structured;

// Идентификатор компонента модели, который будет обсчитываться
ULongId ModelCalculationComponent;

protected: // Данные
// Хранилище
UStorage* Storage;

// Исследуемая модель
UEPtr<UContainer> Model;

/// Время среды
UTimeControl Time;

// Максимальный интеревал времени, который систем пытается "догнать" в режиме
// расчета реального времени (мс)
double MaxModelDuration;

// Внешний обработчик исключений
PExceptionHandler ExceptionHandler;

// Текущее число исключений системы
mutable int CurrentExceptionsLogSize;

// Максимальное число хранимых исключений
// Если 0, то неограниченно
int MaxExceptionsLogSize;

// Индекс последнего считанного символа лога исключений
unsigned LastReadExceptionLogIndex;

// Максимальный уровень непрочитанных сообщений в логе
mutable int LastErrorLevel;

/// Индекс текущего канала в многоканальной библиотеке
int ChannelIndex;

/// Имя компонента модели которому может передаваться сигнал о сбое в работе
/// источника данных
std::string SourceControllerName;

/// Имя переменной состояния компонента модели которому может передаваться
/// сигнал о сбое в работе источника данных
std::string SourceControllerProperty;

protected: // Контроллеры

protected: // Данные графического интерфеса пользователя


protected: // Переменные быстрого доступа
// Текущий компонент модели
UEPtr<UComponent> CurrentComponent;

protected: // Временные переменные
unsigned long long StartupTime;

unsigned long long /*StartProcTime,*/CurrentTime,LastDuration/*,LastSentTime*/, ProcEndTime, LastStepStartTime;

// Временное хранилище буфера для лога
//mutable string TempLogString;

// Временное хранилище строк
mutable string TempString;

mutable std::map<unsigned, pair<std::string, int> > LogList;

/// Индекс последней строки лога
mutable unsigned LogIndex;

UGenericMutex* LogMutex;


public: // Public methods
// --------------------------
// Constructors & destructors
// --------------------------
UEnvironment(void);
virtual ~UEnvironment(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
public:
// Индекс предарительно заданной модели обработки
int GetPredefinedStructure(void) const;
bool SetPredefinedStructure(int value);

// Идентификатор компонента модели, который будет обсчитываться
const ULongId& GetModelCalculationComponent(void) const;
bool SetModelCalculationComponent(const ULongId& value);

// Имя текущего каталога хранения данных
const std::string& GetCurrentDataDir(void);
void SetCurrentDataDir(const std::string& dir);

// Имя каталога бинарных файлов
const std::string& GetSystemDir(void) const;
void SetSystemDir(const std::string& dir);

/// Минимальный интервал времени между итерациями счета (мс)
long long GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(long long value);

/// Флаг включения режима отладки
bool GetDebugMode(void) const;
bool SetDebugMode(bool value);
// --------------------------

// --------------------------
// Методы управления состояниями
// --------------------------
public:
// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool IsStoragePresent(void) const;

// Признак наличия сформированной структуры
virtual bool IsStructured(void) const;
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
virtual UStorage* GetStorage(void);

// Устанавливает новое хранилище
// Указатель на старое хранилище более не используется средой
// Ответственность за освобождение памяти лежит на вызывающей стороне
// Текущая модель уничтожается.
virtual bool SetStorage(UStorage *storage);

// Возвращает указатель на модель
UEPtr<UContainer> GetModel(void);

// Создает новую модель из хранилища по имени класса
virtual bool CreateModel(const NameT& classname);

// Создает новую модель из хранилища по id класса
virtual bool CreateModel(const UId& classid);

// Уничтожает текущую модель
virtual bool DestroyModel(void);

/// Время среды
const UTimeControl& GetTime(void) const;
UTimeControl& GetTime(void);

/// Индекс текущего канала в многоканальной библиотеке
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);
// --------------------------

// --------------------------
// Методы управления моделью
// --------------------------
// --------------------------

// --------------------------
// Методы управления контроллерами
// --------------------------
/// Инициализация компонента и переменной состояния модели которому может
/// передаваться сигнал о сбое в работе
/// источника данных
bool RegisterSourceController(const std::string &component_name, const std::string &property_name);

/// Активация извещения о сбое в работе источника данных
bool CallSourceController(void);
// --------------------------

// --------------------------
// Операторы доступа к данным среды
// --------------------------
// Возвращает указатель на текущий компонент модели
UEPtr<UContainer> GetCurrentComponent(void);

// Устанавливает указатель на текущий компонент модели
// Если имя или id не задано, или Forbidden, то устанавливает
// указатель на модель
virtual void SelectCurrentComponent(const NameT &name);
virtual void SelectCurrentComponent(const ULongId &id);

// Устанавливает указатель на текущий компонент модели на саму модель
virtual void ResetCurrentComponent(void);

// Устанавливает указатель на текущий компонент модели на родительский компонент
// (переход на уровень вверх). Если уже указывает на модель, то не делает ничего
virtual void UpCurrentComponent(void);

// Устанавливает указатель на текущий компонент модели на дочерней компонент на
// любом уровне (переход на уровень вниз).
virtual void DownCurrentComponent(const NameT &name);
virtual void DownCurrentComponent(const ULongId &id);
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
public:
// Производит увеличение времени модели на требуемую величину
void IncreaseModelTimeByStep(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
public:
// Инициализация среды
virtual void Init(void);

// Деинициализация среды
virtual void UnInit(void);

// Формирует предварительно заданную модель обработки
virtual bool CreateStructure(void);

// Уничтожает текущую модель обработки
virtual bool DestroyStructure(void);

// Расчет модели в реальном времени
virtual void RTCalculate(void);
// --------------------------

// --------------------------
// Методы управления исключениями
// --------------------------
public:
// Обрабатывает возникшее исключение
virtual void ProcessException(UException &exception) const;

// Максимальное число хранимых исключений
// Если 0, то неограниченно
int GetMaxExceptionsLogSize(void) const;
void SetMaxExceptionsLogSize(int value);

// Возвращает массив строк лога
const char* GetLog(int &error_level) const;

/// Возвращает число строк лога
int GetNumLogLines(void) const;

/// Возвращает строку лога с индексом i
const char* GetLogLine(int i) const;

/// Возвращает число непрочитанных строк лога
int GetNumUnreadLogLines(void) const;

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* GetUnreadLog(int &error_level);

// Управление функцией-обработчиком исключений
PExceptionHandler GetExceptionHandler(void) const;
bool SetExceptionHandler(PExceptionHandler value);

/// Очищает лог
void ClearLog(void);

/// Очищает лог прочитанных сообщений
void ClearReadLog(void);

// Вызов обработчика исключений среды для простой записи данных в лог
void LogMessage(int msg_level, const std::string &line);
void LogMessage(int msg_level, const std::string &method_name, const std::string &line);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Инициализация среды
virtual void AInit(void);

// Деинициализация среды
virtual void AUnInit(void);

// Формирует предварительно заданную модель обработки
virtual bool ACreateStructure(void);

// Уничтожает текущую модель обработки
virtual bool ADestroyStructure(void);

protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

}
#endif
