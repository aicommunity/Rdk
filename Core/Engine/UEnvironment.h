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
#include "ULoggerEnv.h"
#include "UController.h"

namespace RDK {

class UEnvironment;

/// Описание Predefined structure
struct UEnvPredefinedStructDescription
{
/// Id структуры
int Id;

/// Краткое описание
std::string ShortDescription;

/// Подробное описание
std::string DetailDescription;
};


/// Функция должна быть реализована в конечном проекте
extern RDK_LIB_TYPE bool RDK_CALL RdkCreatePredefinedStructure(RDK::UEnvironment* env, int predefined_structure);

class RDK_LIB_TYPE UEnvironment: virtual public UModule
{
protected: // Параметры
// Индекс предварительно заданной модели обработки
// 0 - Структура определяется извне
int PredefinedStructure;

// Имя текущего каталога хранения данных
std::string CurrentDataDir;

// Имя каталога бинарных файлов
std::string SystemDir;

/// Минимальный интервал времени между итерациями счета (мс)
ULongTime MinInterstepsInterval;

/// Максимальное время расчета модели (сек)
/// если 0 - то не ограничено
double MaxCalcTime;

/// Флаг включения использования индидвидуальных параметров TimeStep для
/// каждого компонента
bool UseIndTimeStepFlag;

protected: // Состояния
// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool StoragePresent;

// Признак успешной инициализации
//bool Initialized;

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

/// Шрифты
RDK::UBitmapFontCollection Fonts;

// Максимальный интеревал времени, который систем пытается "догнать" в режиме
// расчета реального времени (мс)
double MaxModelDuration;

/// Индекс текущего канала в многоканальной библиотеке
int ChannelIndex;

/// Имя компонента модели которому может передаваться сигнал о сбое в работе
/// источника данных
std::string SourceControllerName;

/// Имя переменной состояния компонента модели которому может передаваться
/// сигнал о сбое в работе источника данных
std::string SourceControllerProperty;

/// Набор вариантов predefined structure
std::map<int, UEnvPredefinedStructDescription> PredefinedStructures;

/// Набор источников данных
std::vector<UControllerDataReader*> DataReaders;

protected: // Переменные быстрого доступа
// Текущий компонент модели
UEPtr<UComponent> CurrentComponent;

protected: // Временные переменные
unsigned long long StartupTime;

unsigned long long CurrentTime,LastDuration, ProcEndTime, LastStepStartTime;

/// Время, расчитанное в модели за один вызов RTCalculate;
double RTModelCalcTime;

/// Экземпляр класса для логирования
mutable UEPtr<ULoggerEnv> Logger;

/// Флаг, выставляемый если достигнут конец расчета (по MaxCalcTime)
bool CalcFinishedFlag;

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
unsigned long long GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(unsigned long long value);

/// Максимальное время расчета модели (сек)
/// если 0 - то не ограничено
double GetMaxCalcTime(void) const;
bool SetMaxCalcTime(double value);

/// Флаг включения использования индидвидуальных параметров TimeStep для
/// каждого компонента
bool GetUseIndTimeStepFlag(void) const;
bool SetUseIndTimeStepFlag(bool value);
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

/// Возвращает время, потраченное на последний RT расчет
double GetRTLastDuration(void) const;

/// Время, расчитанное в модели за один вызов RTCalculate;
double GetRTModelCalcTime(void) const;

/// Производительность RT расчета (отношение RTModelCalcTime/RTLastDuration)
double CalcRTPerformance(void) const;
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Указатель на логгер
UEPtr<ULoggerEnv> const GetLogger(void) const;
virtual bool SetLogger(UEPtr<ULoggerEnv> logger);

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

/// Инициализирует модель
virtual void ModelInit(void);

/// Деинициализирует модель
virtual void ModelUnInit(void);

/// Время среды
const UTimeControl& GetTime(void) const;
UTimeControl& GetTime(void);

/// Индекс текущего канала в многоканальной библиотеке
int GetChannelIndex(void) const;
bool SetChannelIndex(int value);

/// Шрифты
RDK::UBitmapFontCollection& GetFonts(void);
bool SetFonts(const RDK::UBitmapFontCollection& value);

/// Возвращает набор вариантов predefined structures
const std::map<int, UEnvPredefinedStructDescription>& GetPredefinedStructures(void) const;

/// Возвращает вариантов predefined structure по id
UEnvPredefinedStructDescription GetPredefinedStructureDescription(int id) const;

/// Добавляет вариант predefined structure
bool AddPredefinedStructure(const UEnvPredefinedStructDescription &descr);

/// Добавляет удаляет predefined structure по id
void DelPredefinedStructure(int id);

/// Удаляет все predefined structures
void ClearPredefinedStructures(void);
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
// Методы управления регистрацией данных
// --------------------------
/// Регистрирует новую точку съема данных (вида MDMatrix)
UControllerDataReader* RegisterDataReader(const std::string &component_name, const std::string &property_name, int row=0, int col=0);

/// Снимает регистрацию точки съема данных (вида MDMatrix)
void UnRegisterDataReader(const std::string &component_name, const std::string &property_name, int row, int col);

/// Снимает регистрацию всех точек съема данных (вида MDMatrix)
void UnRegisterAllDataReaders(void);

/// Возвращает данные точки съема
UControllerDataReader* GetDataReader(const std::string &component_name, const std::string &property_name, int row, int col);
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
// Формирует предварительно заданную модель обработки
virtual bool CreateStructure(void);

// Уничтожает текущую модель обработки
virtual bool DestroyStructure(void);

/// Флаг, выставляемый если достигнут конец расчета (по MaxCalcTime)
bool IsCalcFinished(void) const;

// Расчет модели в реальном времени
virtual void RTCalculate(void);

/// Расчет модели порциями длительностью calc_intervsal секунд с максимально возможной скоростью
virtual void FastCalculate(double calc_interval);

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
