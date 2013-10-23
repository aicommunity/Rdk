/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UAENVIRONMENT_CPP
#define UAENVIRONMENT_CPP

#include <string.h>
#include "UEnvironment.h"
#include "../System/rdk_system.h"

namespace RDK {

// --------------------------
// Constructors & destructors
// --------------------------
UEnvironment::UEnvironment(void)
{
 // Параметры
 // Индекс предарительно заданной модели обработки
 // 0 - Структура определяется извне
 PredefinedStructure=0;

 // Состояния
 // Признак успешной инициализации
 Initialized=false;

 // Признак наличия сформированной структуры
 Structured=false;

 Model=0;
 Storage=0;
 StoragePresent=false;

 MaxModelDuration=100;

 // Текущий компонент модели
// CurrentComponent=0;

 StartupTime=0;
 CurrentTime=0;
 LastDuration=1;
 ProcEndTime=0;
 MinInterstepsInterval=0;

 CurrentExceptionsLogSize=0;
 ExceptionHandler=0;

 LastReadExceptionLogIndex=-1;
 MaxExceptionsLogSize=1000;
}

UEnvironment::~UEnvironment(void)
{
 DestroyModel();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Индекс предарительно заданной модели обработки
int UEnvironment::GetPredefinedStructure(void) const
{
 return PredefinedStructure;
}

bool UEnvironment::SetPredefinedStructure(int value)
{
 if(PredefinedStructure == value)
  return true;

 PredefinedStructure=value;
 return DestroyStructure();
}


// Идентификатор компонента модели, который будет обсчитываться
const ULongId& UEnvironment::GetModelCalculationComponent(void) const
{
 return ModelCalculationComponent;
}

bool UEnvironment::SetModelCalculationComponent(const ULongId& value)
{
 if(ModelCalculationComponent == value)
  return true;

 ModelCalculationComponent=value;

 return true;
}

// Имя текущего каталога хранения данных
const std::string& UEnvironment::GetCurrentDataDir(void)
{
 return CurrentDataDir;
}

void UEnvironment::SetCurrentDataDir(const std::string& dir)
{
 CurrentDataDir=dir;

 for(std::string::size_type i=0; i<CurrentDataDir.size();i++)
  if(CurrentDataDir[i] == '\\')
   CurrentDataDir[i]='/';

 if(CurrentDataDir.size()>0 && CurrentDataDir[CurrentDataDir.size()-1] != '/')
  CurrentDataDir+='/';
}

/// Минимальный интервал времени между итерациями счета (мс)
long long UEnvironment::GetMinInterstepsInterval(void) const
{
 return MinInterstepsInterval;
}

bool UEnvironment::SetMinInterstepsInterval(long long value)
{
 if(MinInterstepsInterval == value)
  return true;

 MinInterstepsInterval = value;
 return true;
}

/// Флаг включения режима отладки
bool UEnvironment::GetDebugMode(void) const
{
 return DebugMode;
}

bool UEnvironment::SetDebugMode(bool value)
{
 if(DebugMode == value)
  return true;

 DebugMode=value;
 return true;
}
// --------------------------


// --------------------------
// Методы управления состояниями
// --------------------------
// Признак наличия сформированной структуры
bool UEnvironment::IsStructured(void) const
{
 return Structured;
}
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
UStorage* UEnvironment::GetStorage(void)
{
 return Storage;
}

// Устанавливает новое хранилище
// Указатель на старое хранилище более не используется средой
// Ответственность за освобождение памяти лежит на вызывающей стороне
// Текущая модель уничтожается.
bool UEnvironment::SetStorage(UStorage *storage)
{
 if(!storage)
  return false;

 if(Storage == storage)
  return true;

 if(!DestroyModel())
  return false;

 Storage=storage;
 StoragePresent=true;

 return true;
}

// Возвращает указатель на модель
UEPtr<UContainer> UEnvironment::GetModel(void)
{
 return Model;
}

// Создает новую модель из хранилища по имени класса
bool UEnvironment::CreateModel(const NameT& classname)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UContainer>(GetStorage()->TakeObject(classname));
 Model->SetEnvironment(this);
 Ready=false;
 if(Model)
 {
  return true;
 }

 return false;
}

// Создает новую модель из хранилища по id класса
bool UEnvironment::CreateModel(const UId& classid)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UContainer>(Storage->TakeObject(classid));
 Model->SetEnvironment(this);
 Ready=false;
 if(Model)
  return true;

 return false;
}

// Уничтожает текущую модель
bool UEnvironment::DestroyModel(void)
{
 if(!Model)
  return true;

 Model->Free();
 Model=0;
 CurrentComponent=0;

 return true;
}
// --------------------------

// --------------------------
// Методы управления контроллерами
// --------------------------
// --------------------------

// --------------------------
// Операторы доступа к данным среды
// --------------------------
// Возвращает указатель на текущий компонент модели
UEPtr<UContainer> UEnvironment::GetCurrentComponent(void)
{
 return CurrentComponent;
}

// Устанавливает указатель на текущий компонент модели
void UEnvironment::SelectCurrentComponent(const NameT &name)
{
 if(name == ForbiddenName)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(name);
}

void UEnvironment::SelectCurrentComponent(const ULongId &id)
{
 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(id);
}

// Устанавливает указатель на текущий компонент модели на саму модель
void UEnvironment::ResetCurrentComponent(void)
{
 CurrentComponent=Model;
}

// Устанавливает указатель на текущий компонент модели на родительский компонент
// (переход на уровень вверх). Если уже указывает на модель, то не делает ничего
void UEnvironment::UpCurrentComponent(void)
{
 if(CurrentComponent == Model)
  return;

 CurrentComponent=CurrentComponent->GetOwner();
}

// Устанавливает указатель на текущий компонент модели на дочерней компонент на
// любом уровне (переход на уровень вниз).
void UEnvironment::DownCurrentComponent(const NameT &name)
{
 CurrentComponent=GetCurrentComponent()->GetComponentL(name);
}

void UEnvironment::DownCurrentComponent(const ULongId &id)
{
 CurrentComponent=GetCurrentComponent()->GetComponentL(id);
}

/// Время среды
const UTimeControl& UEnvironment::GetTime(void) const
{
 return Time;
}

UTimeControl& UEnvironment::GetTime(void)
{
 return Time;
}

/// Индекс текущего канала в многоканальной библиотеке
int UEnvironment::GetChannelIndex(void) const
{
 return ChannelIndex;
}

bool UEnvironment::SetChannelIndex(int value)
{
 if(ChannelIndex == value)
  return true;

 ChannelIndex=value;
 return true;
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Производит увеличение времени модели на требуемую величину
void UEnvironment::IncreaseModelTimeByStep(void)
{
 Time.IncreaseModelTimeByStep(GetModel()->GetTimeStep());
}
// --------------------------

// --------------------------
// Методы инициализации среды
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к флагам состояний
// --------------------------
// Флаг состояния инициализации
// true - хранилище готово к использованию
// false - хранилище не готово
bool UEnvironment::IsStoragePresent(void) const
{
 return StoragePresent;
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Инициализация среды
void UEnvironment::Init(void)
{
 if(IsInit())
  return;

 AInit();

 ModelCalculationComponent.Resize(0);
 if(Model)
  Model->Init();
 Initialized=true;
 return;
}

// Деинициализация среды
void UEnvironment::UnInit(void)
{
 if(Model)
  Model->UnInit();

 if(!IsInit())
  return;

 AUnInit();

 Initialized=false;
}

// Формирует предварительно заданную модель обработки
bool UEnvironment::CreateStructure(void)
{
 if(Structured)
  return true;

 Init();

 if(!ACreateStructure())
  return false;

 Structured=true;

 if(!Build())
  return false;

 return true;
}

// Уничтожает текущую модель обработки
bool UEnvironment::DestroyStructure(void)
{
 if(!Structured)
  return true;

 if(!IsReady())
  return false;

 if(!ADestroyStructure())
  return false;

 Structured=false;
 return true;
}

// Расчет модели в реальном времени
void UEnvironment::RTCalculate(void)
{
 Build();

// StartProcTime=GetCurrentStartupTime();

 CurrentTime=GetCurrentStartupTime();
 Time.SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);

 long long curtime;
 long long TimerInterval=0;
 //double devicemodeltime=0;

 TimerInterval=GetCurrentStartupTime()-ProcEndTime;
 if(TimerInterval<=0)
  TimerInterval=1;

 int i=0;
 if(LastDuration < TimerInterval)
  LastDuration=TimerInterval;
 double model_duration=(Time.GetRealTime()-Time.GetDoubleTime()*1e6)/1000.0;

 if(model_duration>MaxModelDuration)
  model_duration=MaxModelDuration;

 int elapsed_counter=0;
 if(model_duration>0)
 {
  elapsed_counter=int((model_duration*Model->GetTimeStep())/1000);
 }
 else
 {
  elapsed_counter=0;//(/*LastDuration*/model_duration*Model->GetTimeStep())/1000;
 }

 curtime=GetCurrentStartupTime();
 while(curtime-CurrentTime<TimerInterval && i<elapsed_counter)
 {
  Calculate();

  ++i;
  curtime=GetCurrentStartupTime();
 }

// LastSentTime=GetCurrentStartupTime();
 if(Time.GetRealTime()/1e6<Time.GetDoubleTime())
 {
  Sleep(int(Time.GetDoubleTime()*1000-Time.GetRealTime()/1000));
  Time.SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);
 }

 LastDuration=GetCurrentStartupTime()-CurrentTime;
 ProcEndTime=GetCurrentStartupTime();

 return;
}
// --------------------------

// --------------------------
// Методы управления исключениями
// --------------------------
// Обрабатывает возникшее исключение
void UEnvironment::ProcessException(UException &exception) const
{
// if(!exception)
//  throw exception;

// USharedPtr<UException> ptr=exception;
// ExceptionsLog.push_back(ptr);

 if(LastErrorLevel>exception.GetType())
  LastErrorLevel=exception.GetType();
 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize/*ExceptionsLog.size()*/ > MaxExceptionsLogSize)
 {
//  ExceptionsLog.erase(ExceptionsLog.begin());
  size_t i=TempLogString.find_first_of("\n");
  if(i != string::npos)
  {
   TempLogString.erase(0,i);
  }
 }
// if(!TempLogString.empty())
//  TempLogString+="\r\n";

 TempLogString+=sntoa(ChannelIndex);
 TempLogString+="> ";
 TempLogString+=exception.CreateLogMessage();

 if(ExceptionHandler)
  ExceptionHandler(ChannelIndex);
}


// Возвращает массив зарегистрированных исключений
/*const vector<USharedPtr<UException> > UEnvironment::GetExceptionsLog(void) const
{
 return ExceptionsLog;
}*/


// Возвращает массив строк лога
const char* UEnvironment::GetLog(int &error_level) const
{
 error_level=LastErrorLevel;
 LastErrorLevel=INT_MAX;
 return TempLogString.c_str();
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* UEnvironment::GetUnreadLog(int &error_level)
{
 error_level=LastErrorLevel;
 LastErrorLevel=INT_MAX;
 if(LastReadExceptionLogIndex<=0)
 {
  LastReadExceptionLogIndex=TempLogString.size();
  return TempLogString.c_str();
 }

 if(LastReadExceptionLogIndex<int(TempLogString.size()))
 {
  TempString=TempLogString.substr(LastReadExceptionLogIndex);
  LastReadExceptionLogIndex=TempLogString.size();
  return TempString.c_str();
 }

 TempString="";
 return TempString.c_str();
}

// Управление функцией-обработчиком исключений
UEnvironment::PExceptionHandler UEnvironment::GetExceptionHandler(void) const
{
 return ExceptionHandler;
}

bool UEnvironment::SetExceptionHandler(PExceptionHandler value)
{
 if(ExceptionHandler == value)
  return true;

 ExceptionHandler=value;
 return true;
}

// Максимальное число хранимых исключений
// Если 0, то неограниченно
int UEnvironment::GetMaxExceptionsLogSize(void) const
{
 return MaxExceptionsLogSize;
}

void UEnvironment::SetMaxExceptionsLogSize(int value)
{
 if(MaxExceptionsLogSize == value)
  return;

 MaxExceptionsLogSize=value;
 if(MaxExceptionsLogSize>0 && /*ExceptionsLog.size()*/CurrentExceptionsLogSize>MaxExceptionsLogSize)
 {
  //ExceptionsLog.erase(ExceptionsLog.begin(), ExceptionsLog.begin()+int(ExceptionsLog.size())-MaxExceptionsLogSize);
  CurrentExceptionsLogSize=MaxExceptionsLogSize;
 }
}

/// Очищает лог
void UEnvironment::ClearLog(void)
{
 LastReadExceptionLogIndex=-1;
 CurrentExceptionsLogSize=0;
 LastErrorLevel=INT_MAX;
 TempLogString.clear();
}

// Вызов обработчика исключений среды для простой записи данных в лог
void UEnvironment::LogMessage(int msg_level, const std::string &line)
{
 switch (msg_level)
 {
 case RDK_EX_FATAL:
 {
  EStringFatal exception(line);
  ProcessException(exception);
 }
 break;

 case RDK_EX_ERROR:
 {
  EStringError exception(line);
  ProcessException(exception);
 }
 break;

 case RDK_EX_INFO:
 {
  EStringInfo exception(line);
  ProcessException(exception);
 }
 break;

 case RDK_EX_DEBUG:
 {
  if(DebugMode)
  {
   EStringDebug exception(line);
   ProcessException(exception);
  }
 }
 break;
 }
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Инициализация среды
void UEnvironment::AInit(void)
{
 return;
}

// Деинициализация среды
void UEnvironment::AUnInit(void)
{
 return;
}

// Формирует предварительно заданную модель обработки
bool UEnvironment::ACreateStructure(void)
{
 return true;
}


// Уничтожает текущую модель обработки
bool UEnvironment::ADestroyStructure(void)
{
 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool UEnvironment::ADefault(void)
{
 if(!Model)
  return true;

 MinInterstepsInterval=200;
 DebugMode=false;

// UComponent::SetTime(0);
 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Default())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Default())
   return false;
 }

 ChannelIndex=0;
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UEnvironment::ABuild(void)
{
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Build())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Build())
   return false;
 }

 return true;
}

// Сброс процесса счета.
bool UEnvironment::AReset(void)
{
 StartupTime=GetCurrentStartupTime();
 LastDuration=1;
 ProcEndTime=StartupTime;
 LastStepStartTime=0;
 LastErrorLevel=INT_MAX;

 if(!Model)
  return true;

// RDK::DefaultTimeStep=30;
// Model->SetTimeStep(DefaultTimeStep);

 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Reset())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Reset())
   return false;
 }

 Time.SetTime(0);
 return true;
}

// Выполняет расчет этого объекта
bool UEnvironment::ACalculate(void)
{
 long long cur_time=GetCurrentStartupTime();
 Time.SetRealTime(CalcDiffTime(cur_time,StartupTime)*1000);
 if(!Model)
  return true;

 // Проверяем, достаточно ли велик интервал времени между итерациями счета
 if(cur_time-LastStepStartTime<MinInterstepsInterval)
  return true;

 LastStepStartTime=cur_time;
 if(ModelCalculationComponent.GetSize() == 0)
 {
  if(!Model->Calculate())
   return false;
 }
 else
 {
  UEPtr<UContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Calculate())
   return false;
 }

 // Если мы считаем всю модель, то расчитываем время модели здесь,
 // иначе мы ожидаем, что вызывающий модуль сам расчитает время модели
 if(ModelCalculationComponent.GetSize() == 0)
  IncreaseModelTimeByStep();

 return true;
}
// --------------------------

// --------------------------
// Вспомогательные методы инициализации среды
// --------------------------
// --------------------------

// --------------------------
// Методы управления моделью
// --------------------------
// --------------------------

}
#endif
