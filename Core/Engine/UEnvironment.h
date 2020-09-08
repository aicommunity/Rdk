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

/// Описание точки съема данных
struct UDataSource
{
/// Имя компонента
std::string ComponentName;

/// Имя свойства
std::string PropertyName;

/// Последняя точка модельного времени выбора данных
double LastReadTime;

/// Последняя точка модельного времени добавления данных
double LastWriteTime;

/// Указатель на среду
UEnvironment *Env;

UDataSource(UEnvironment *env);

/// Обновляет хранимые данные добавляя новые
virtual void UpdateData(void)=0;

/// Удаляет все хранимые данные
virtual void ClearData(void)=0;

/// Настраивает точку съема данных
virtual bool Configure(const std::string &component_name, const std::string &property_name)=0;

/// Возвращает тип данных
virtual const type_info& GetDataType(void) const=0;
};

template <class T>
struct UDataSourceT: public UDataSource
{
/// Данные
std::vector<MDMatrix<T> > Data;

UDataSourceT(UEnvironment *env);

/// Обновляет хранимые данные добавляя новые
virtual void UpdateData(void);

/// Считывает данные и очищает массив
virtual void ExtractData(std::vector<MDMatrix<T> > &buffer);

/// Удаляет все хранимые данные
virtual void ClearData(void);

/// Настраивает точку съема данных
virtual bool Configure(const std::string &component_name, const std::string &property_name);

/// Возвращает тип данных
virtual const type_info& GetDataType(void) const;
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
std::vector<UDataSource*> DataSources;

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
long long GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(long long value);

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
bool RegisterDataSource(const std::string &component_name, const std::string &property_name);

/// Снимает регистрацию точки съема данных (вида MDMatrix)
void UnRegisterDataSource(const std::string &component_name, const std::string &property_name);

/// Снимает регистрацию всех точек съема данных (вида MDMatrix)
void UnRegisterAllDataSources(void);

/// Возвращает данные точки съема
/// Возвращает false если тип данных не совпал с ожидаемым
template<class T>
bool ReadDataSource(const std::string &component_name, const std::string &property_name, std::vector<MDMatrix<T> > &buffer);
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

template<class T>
UDataSourceT<T>::UDataSourceT(UEnvironment *env)
 : UDataSource(env)
{
}

/// Обновляет хранимые данные добавляя новые
template<class T>
void UDataSourceT<T>::UpdateData(void)
{
 if(!Env)
  return;

 if(!Env->GetModel())
 {
  Env->UnRegisterDataSource(ComponentName,PropertyName);
  return;
 }

 UContainer *cont=Env->GetModel()->GetComponentL(ComponentName);
 if(!cont)
 {
  Env->UnRegisterDataSource(ComponentName,PropertyName);
  return;
 }
 const MDMatrix<T> *data=cont->AccessPropertyData<MDMatrix<T> >(PropertyName);
 if(!data)
 {
  Env->UnRegisterDataSource(ComponentName,PropertyName);
  return;
 }

 if(LastReadTime>=LastWriteTime)
 {
  Data.clear();
  LastReadTime=0.0;
 }
 Data.push_back(*data);
 LastWriteTime=Env->GetTime().GetDoubleTime();
};

/// Считывает данные и очищает массив
template<class T>
void UDataSourceT<T>::ExtractData(std::vector<MDMatrix<T> > &buffer)
{
 buffer=Data;
 LastReadTime=Env->GetTime().GetDoubleTime();
}

/// Удаляет все хранимые данные
template<class T>
void UDataSourceT<T>::ClearData(void)
{
 Data.clear();
 LastReadTime=LastWriteTime=0.0;
};

/// Настраивает точку съема данных
template<class T>
bool UDataSourceT<T>::Configure(const std::string &component_name, const std::string &property_name)
{
 if(ComponentName == component_name && PropertyName == property_name)
  return true;
 ClearData();
 ComponentName.clear();
 PropertyName.clear();
 if(!Env)
  return false;

 if(!Env->GetModel())
  return false;

 UContainer *cont=Env->GetModel()->GetComponentL(ComponentName);
 if(!cont)
  return false;

 UEPtr<UIProperty> prop=cont->FindProperty(PropertyName);
 if(!prop)
  return false;

 if(prop->GetLanguageType() != typeid(MDMatrix<T>))
  return false;

 ComponentName=component_name;
 PropertyName=property_name;
 return true;
};

/// Возвращает тип данных
template<class T>
const type_info& UDataSourceT<T>::GetDataType(void) const
{
 return typeid(MDMatrix<T>);
}

/// Возвращает данные точки съема
/// Возвращает false если тип данных не совпал с ожидаемым
template<class T>
bool UEnvironment::ReadDataSource(const std::string &component_name, const std::string &property_name, std::vector<MDMatrix<T> > &buffer)
{
 for(size_t i=0;i<DataSources.size();i++)
 {
  if(DataSources[i] && DataSources[i]->ComponentName == component_name && DataSources[i]->PropertyName == property_name)
  {
   if(DataSources[i]->GetDataType() != typeid(MDMatrix<T>)
	return false;

   buffer=DataSources[i]->Data;
   return true;
  }
 }
 return false;
}

}
#endif
