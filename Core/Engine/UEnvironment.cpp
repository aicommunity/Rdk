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
#include "limits.h"
#include "UEnvironment.h"
#include "../System/rdk_system.h"
#include "../Application/UIVisualController.h"


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

 ChannelIndex=0;
 LastStepStartTime=0;

 RTModelCalcTime=0.0;
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
/*
 if(EventsLogMode && IsInit())
 {
  Logger.Clear();
  Logger.SetLogPath(CurrentDataDir+"EventsLog/");
  Logger.InitLog();
 }*/
}

// Имя каталога бинарных файлов
const std::string& UEnvironment::GetSystemDir(void) const
{
 return SystemDir;
}

void UEnvironment::SetSystemDir(const std::string& dir)
{
 SystemDir=dir;
 for(std::string::size_type i=0; i<SystemDir.size();i++)
  if(SystemDir[i] == '\\')
   SystemDir[i]='/';

 if(SystemDir.size()>0 && SystemDir[SystemDir.size()-1] != '/')
  SystemDir+='/';
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
/*
/// Флаг включения режима отладки
bool UEnvironment::GetDebugMode(void) const
{
 return Logger.GetDebugMode();
}

bool UEnvironment::SetDebugMode(bool value)
{
 return Logger.SetDebugMode();
}

/// Маска системных событий для логирования
unsigned int UEnvironment::GetDebugSysEventsMask(void) const
{
 return DebugSysEventsMask;
}

bool UEnvironment::SetDebugSysEventsMask(unsigned int value)
{
 if(DebugSysEventsMask == value)
  return true;

 DebugSysEventsMask=value;
 return true;
}

/// Возвращает флаг включения вывода лога в отладчик
bool UEnvironment::GetDebuggerMessageFlag(void) const
{
 return DebuggerMessageFlag;
}

/// Устанавливает флаг включения вывода лога в отладчик
bool UEnvironment::SetDebuggerMessageFlag(bool value)
{
 if(DebuggerMessageFlag == value)
  return true;

 DebuggerMessageFlag=value;
 return true;
}

/// Флаг включения внутренней регистрации событий в лог-файл
/// true - регистрация включена
bool UEnvironment::GetEventsLogMode(void) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 return EventsLogMode;
}

bool UEnvironment::SetEventsLogMode(bool value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 EventsLogMode=value;
 return true;
}              */
// --------------------------


// --------------------------
// Методы управления состояниями
// --------------------------
// Признак наличия сформированной структуры
bool UEnvironment::IsStructured(void) const
{
 return Structured;
}

/// Возвращает время, потраченное на последний RT расчет
double UEnvironment::GetRTLastDuration(void) const
{
 return double(LastDuration)/1000.0;
}

/// Время, расчитанное в модели за один вызов RTCalculate;
double UEnvironment::GetRTModelCalcTime(void) const
{
 return RTModelCalcTime;
}

/// Производительность RT расчета (отношение RTModelCalcTime/RTLastDuration)
double UEnvironment::CalcRTPerformance(void) const
{
 if(LastDuration>0)
  return RTModelCalcTime/(double(LastDuration)/1000.0);

 return 1000000; // TODO:
}
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Указатель на логгер
UEPtr<ULoggerEnv> const UEnvironment::GetLogger(void) const
{
 return Logger;
}

bool UEnvironment::SetLogger(UEPtr<ULoggerEnv> logger)
{
 if(Logger == logger)
  return true;

 Logger=logger;
 return true;
}

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
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return false;
 }

 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UContainer>(GetStorage()->TakeObject(classname));
 Model->SetLogger(Logger);
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
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return false;
 }

 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UContainer>(Storage->TakeObject(classid));
 Model->SetLogger(Logger);
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
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

 if(name == ForbiddenName)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(name);
}

void UEnvironment::SelectCurrentComponent(const ULongId &id)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

 if(id.GetSize() == 0 || id[0] == ForbiddenId)
  CurrentComponent=Model;
 else
  CurrentComponent=Model->GetComponentL(id);
}

// Устанавливает указатель на текущий компонент модели на саму модель
void UEnvironment::ResetCurrentComponent(void)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

 CurrentComponent=Model;
}

// Устанавливает указатель на текущий компонент модели на родительский компонент
// (переход на уровень вверх). Если уже указывает на модель, то не делает ничего
void UEnvironment::UpCurrentComponent(void)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

 if(CurrentComponent == Model)
  return;

 CurrentComponent=CurrentComponent->GetOwner();
}

// Устанавливает указатель на текущий компонент модели на дочерней компонент на
// любом уровне (переход на уровень вниз).
void UEnvironment::DownCurrentComponent(const NameT &name)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

 CurrentComponent=GetCurrentComponent()->GetComponentL(name);
}

void UEnvironment::DownCurrentComponent(const ULongId &id)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

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
/*
 Logger.SetSuffix(std::string(" Ch")+sntoa(ChannelIndex,2));
 if(EventsLogMode && IsInit())
 {
  Logger.Clear();
  Logger.InitLog();
 }
  */
 return true;
}

/// Шрифты
RDK::UBitmapFontCollection& UEnvironment::GetFonts(void)
{
 return Fonts;
}

bool UEnvironment::SetFonts(const RDK::UBitmapFontCollection& value)
{
 Fonts=value;
 return true;
}
// --------------------------


// --------------------------
// Методы управления контроллерами
// --------------------------
/// Инициализация компонента и переменной состояния модели которому может
/// передаваться сигнал о сбое в работе
/// источника данных
bool UEnvironment::RegisterSourceController(const std::string &component_name, const std::string &property_name)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return false;
 }

 SourceControllerName=component_name;
 SourceControllerProperty=property_name;
 return true;
}


/// Активация извещения о сбое в работе источника данных
bool UEnvironment::CallSourceController(void)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return false;
 }

 if(SourceControllerName.empty() || SourceControllerProperty.empty())
  return false;
 try
 {
  if(!GetModel())
   return false;
  UEPtr<UContainer> cont=GetModel()->GetComponentL(SourceControllerName);

  UEPtr<UIProperty> iproperty=cont->FindProperty(SourceControllerProperty);
  bool value=true;
  if(!iproperty->ReadFromMemory(&value))
   return false;
 }
 catch(UContainer::EComponentNameNotExist &exception)
 {
  return false;
 }
 return true;
}
// --------------------------


// --------------------------
// Методы управления счетом
// --------------------------
// Производит увеличение времени модели на требуемую величину
void UEnvironment::IncreaseModelTimeByStep(void)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

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
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

 Build();

 CurrentTime=GetCurrentStartupTime();
 Time.SetSourceCurrentLocalTime(double(GetCurrentStartupTime())/1000.0);

 // Если первый шаг расчета после Reset
 if(Time.GetTime() == 0)
 {
  Time.SetSourceStartLocalTime(Time.GetSourceCurrentLocalTime());
  Time.SetSourceStartGlobalTime(Time.GetSourceCurrentGlobalTime());
  StartupTime=CurrentTime;
  ProcEndTime=StartupTime;
 }


 unsigned long long curtime;
 unsigned long long timer_interval=0;
 //double devicemodeltime=0;

 timer_interval=CalcDiffTime(GetCurrentStartupTime(),ProcEndTime);
 if(timer_interval<=0)
  timer_interval=1;

 int i=0;
 if(LastDuration < timer_interval)
  LastDuration=timer_interval;
 double model_duration=(Time.GetRealTime()-Time.GetDoubleTime()*1e6)/1000.0;
 double model_start_calc_time=Time.GetDoubleTime();

 if(model_duration>MaxModelDuration)
  model_duration=MaxModelDuration;

 int elapsed_counter=0;
 if(model_duration>0)
 {
  elapsed_counter=int((model_duration*Model->GetTimeStep())/1000);
 }
 else
 {
  elapsed_counter=0;
 }

 curtime=GetCurrentStartupTime();
 while(curtime-CurrentTime<timer_interval && i<elapsed_counter)
 {
  Calculate();

  ++i;
  curtime=GetCurrentStartupTime();
 }

 if(Time.GetRealTime()/1e6<Time.GetDoubleTime())
 {
  Sleep(int(Time.GetDoubleTime()*1000-Time.GetRealTime()/1000));
  Time.SetSourceCurrentLocalTime(double(GetCurrentStartupTime())/1000.0);
 }

 LastDuration=CalcDiffTime(GetCurrentStartupTime(),CurrentTime);
 ProcEndTime=GetCurrentStartupTime();
 double model_stop_calc_time=Time.GetDoubleTime();
 RTModelCalcTime=model_stop_calc_time-model_start_calc_time;

 return;
}
// --------------------------

// --------------------------
// Методы управления исключениями
// --------------------------
/*
// Обрабатывает возникшее исключение
void UEnvironment::ProcessException(UException &exception) const
{
 UGenericMutexExclusiveLocker lock(LogMutex);

 UException* processed_exception=&exception;
 UException temp_ex;
 if(ExceptionPreprocessor)
 {
  if(ExceptionPreprocessor(const_cast<UEnvironment*>(this),Model, exception,temp_ex))
   processed_exception=&temp_ex;
 }

 if(LastErrorLevel>processed_exception->GetType())
  LastErrorLevel=processed_exception->GetType();
 ++CurrentExceptionsLogSize;
 if(CurrentExceptionsLogSize > MaxExceptionsLogSize)
 {
  int erase_size=CurrentExceptionsLogSize - MaxExceptionsLogSize-1;
  if(int(LogList.size())>erase_size)
  {
   for(int i=0;i<erase_size;i++)
    LogList.erase(LogList.begin());
  }
  else
   LogList.clear();
 }

 UException log(*processed_exception);
 log.SetMessage(sntoa(ChannelIndex)+std::string("> ")+processed_exception->what());
 LogList[LogIndex++]=log;

 if(EventsLogMode) // Если включено, то сохраняем события в файл
 {
  Logger.LogMessage(log.GetMessage());  // TODO: Проверить на RDK_SUCCESS
 }

 if(DebuggerMessageFlag)
  RdkDebuggerMessage(log.GetMessage());

 if(ExceptionPostprocessor)
  ExceptionPostprocessor(const_cast<UEnvironment*>(this),Model, *processed_exception); // TODO: Нет проверки возвращаемого значения

 if(ExceptionHandler)
  ExceptionHandler(ChannelIndex);
}

// Возвращает массив строк лога
const char* UEnvironment::GetLog(int &error_level) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 TempString.clear();
 std::map<unsigned, UException >::const_iterator I,J;
 I=LogList.begin(); J=LogList.end();
 for(;I != J;++I)
 {
  TempString+=I->second.GetMessage();
  TempString+="/r/n";
 }
 error_level=LastErrorLevel;
 LastErrorLevel=INT_MAX;
 return TempString.c_str();
}

/// Возвращает число строк лога
int UEnvironment::GetNumLogLines(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 return int(LogList.size());
}

/// Возвращает строку лога с индексом i
const char* UEnvironment::GetLogLine(int i, int &error_level, int &number, time_t &time) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 std::map<unsigned, UException >::const_iterator I=LogList.find(i);

 if(I == LogList.end())
 {
  TempString.clear();
  return TempString.c_str();
 }

 TempString=I->second.GetMessage();
 error_level=I->second.GetType();
 number=I->second.GetNumber();
 time=I->second.GetTime();
 return TempString.c_str();
}

/// Возвращает число непрочитанных строк лога
int UEnvironment::GetNumUnreadLogLines(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);

 std::map<unsigned, UException >::const_iterator I=LogList.find(LastReadExceptionLogIndex);
 if(I == LogList.end())
  return int(LogList.size());

 int size=0;
 for(;I!=LogList.end();++I)
  ++size;
 return size;
}

// Возвращает частичный массив строк лога с момента последнего считывания лога
// этой функцией
const char* UEnvironment::GetUnreadLog(int &error_level, int &number, time_t &time)
{
 UGenericMutexSharedLocker lock(LogMutex);
 TempString.clear();
 error_level=INT_MAX;
 number=0;
 time=time_t();

 if(LogList.empty())
  return TempString.c_str();

 if(LastReadExceptionLogIndex == 0)
 {
  LastReadExceptionLogIndex=LogList.begin()->first;
  TempString=LogList.begin()->second.GetMessage();
  error_level=LogList.begin()->second.GetType();
  number=LogList.begin()->second.GetNumber();
  time=LogList.begin()->second.GetTime();
 }
 else
 {
  std::map<unsigned, UException >::const_iterator I=LogList.find(LastReadExceptionLogIndex);
  if(I != LogList.end())
  {
   ++I;
   if(I != LogList.end())
   {
	LastReadExceptionLogIndex=I->first;
	TempString=I->second.GetMessage();
	error_level=I->second.GetType();
	number=I->second.GetNumber();
	time=I->second.GetTime();
	return TempString.c_str();
   }
  }
  else
   LastReadExceptionLogIndex=LogList.begin()->first;
 }

 error_level=RDK_EX_UNKNOWN;
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

// Управление функцией-предобработчиком исключений
UEnvironment::PExceptionPreprocessor UEnvironment::GetExceptionPreprocessor(void) const
{
 return ExceptionPreprocessor;
}

bool UEnvironment::SetExceptionPreprocessor(PExceptionPreprocessor value)
{
 if(ExceptionPreprocessor == value)
  return true;

 ExceptionPreprocessor=value;
 return true;
}

/// Управление функцией-постобработки исключений
UEnvironment::PExceptionPostprocessor UEnvironment::GetExceptionPostprocessor(void) const
{
 return ExceptionPostprocessor;
}

bool UEnvironment::SetExceptionPostprocessor(PExceptionPostprocessor value)
{
 if(ExceptionPostprocessor == value)
  return true;

 ExceptionPostprocessor=value;
 return true;
}



// Максимальное число хранимых исключений
// Если 0, то неограниченно
int UEnvironment::GetMaxExceptionsLogSize(void) const
{
 UGenericMutexSharedLocker lock(LogMutex);
 return MaxExceptionsLogSize;
}

void UEnvironment::SetMaxExceptionsLogSize(int value)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 if(MaxExceptionsLogSize == value)
  return;

 MaxExceptionsLogSize=value;
 if(MaxExceptionsLogSize>0 && CurrentExceptionsLogSize>MaxExceptionsLogSize)
 {
  //ExceptionsLog.erase(ExceptionsLog.begin(), ExceptionsLog.begin()+int(ExceptionsLog.size())-MaxExceptionsLogSize);
  CurrentExceptionsLogSize=MaxExceptionsLogSize;
 }
}

/// Очищает лог
void UEnvironment::ClearLog(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 LastReadExceptionLogIndex=0;
 CurrentExceptionsLogSize=0;
 LastErrorLevel=INT_MAX;
 LogList.clear();
 LogIndex=1;
}

/// Очищает лог прочитанных сообщений
void UEnvironment::ClearReadLog(void)
{
 UGenericMutexExclusiveLocker lock(LogMutex);
 std::map<unsigned, UException >::iterator I=LogList.find(LastReadExceptionLogIndex);
 if(I != LogList.end())
 {
  ++I;
  std::map<unsigned, UException >::iterator J=LogList.begin(),K;
  while(J != I)
  {
   K=J; ++K;
   LogList.erase(J);
   J=K;
  }
 }
 if(!LogList.empty())
  LastReadExceptionLogIndex=LogList.begin()->first;
 else
  LastReadExceptionLogIndex=0;
 CurrentExceptionsLogSize=LogList.size();
 LastErrorLevel=INT_MAX;
// TempLogString.clear();
}

// Вызов обработчика исключений среды для простой записи данных в лог
void UEnvironment::LogMessage(int msg_level, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, "", line, error_event_number);
}

void UEnvironment::LogMessage(int msg_level, const std::string &method_name, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, "", method_name, line, error_event_number);
}

void UEnvironment::LogMessageEx(int msg_level, const std::string &object_name, const std::string &line, int error_event_number)
{
 switch (msg_level)
 {
 case RDK_EX_FATAL:
 {
  EStringFatal exception(line,error_event_number);
 }
 break;

 case RDK_EX_ERROR:
 {
  EStringError exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;

 case RDK_EX_WARNING:
 {
  EStringWarning exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;

 case RDK_EX_INFO:
 {
  EStringInfo exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;

 case RDK_EX_DEBUG:
 {
  if(DebugMode)
  {
   EStringDebug exception(line,error_event_number);
   exception.SetObjectName(object_name);
   ProcessException(exception);
  }
 }
 break;

 case RDK_EX_APP:
 {
  EStringApp exception(line,error_event_number);
  exception.SetObjectName(object_name);
  ProcessException(exception);
 }
 break;
 }
}

void UEnvironment::LogMessageEx(int msg_level, const std::string &object_name, const std::string &method_name, const std::string &line, int error_event_number)
{
 LogMessageEx(msg_level, object_name, method_name+std::string(" - ")+line, error_event_number);
}           */
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Инициализация среды
void UEnvironment::AInit(void)
{
/* if(EventsLogMode)
 {
  Logger.SetLogPath(CurrentDataDir+"EventsLog/");
  Logger.SetSuffix(std::string(" Ch")+sntoa(ChannelIndex,2));
  Logger.InitLog();
 }
  */
 SetFonts(RDK::GlobalFonts);

 ModelCalculationComponent.Resize(0);
 if(Model)
  Model->Init();
 return;
}

// Деинициализация среды
void UEnvironment::AUnInit(void)
{
 if(Model)
  Model->UnInit();

 return;
}

// Формирует предварительно заданную модель обработки
bool UEnvironment::ACreateStructure(void)
{
 RdkCreatePredefinedStructure(this, PredefinedStructure);
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

 MinInterstepsInterval=0;
// DebugMode=false;
// EventsLogMode=false;

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
 StartupTime=0;
 ProcEndTime=StartupTime;
 LastDuration=1;
 LastStepStartTime=0;
// LastErrorLevel=INT_MAX;
 if(Logger)
  Logger->Reset();
 RTModelCalcTime=0;

// if(EventsLogMode)
// {
//  Logger.Clear();
//  Logger.InitLog();
// }

 if(!Model)
  return true;

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
 // Если первый шаг расчета после Reset
 if(Time.GetTime() == 0)
 {
  Time.SetSourceStartLocalTime(0);
  Time.SetSourceStartGlobalTime(Time.GetSourceCurrentGlobalTime());
  StartupTime=GetCurrentStartupTime();
  ProcEndTime=StartupTime;
 }

 ULongTime cur_time=(ULongTime)((Time.GetSourceCurrentGlobalTime()-Time.GetSourceStartGlobalTime())*(86400.0*1000.0));
 Time.SetSourceCurrentLocalTime(cur_time/1000.0);

 if(!Model)
  return true;

 // Проверяем, достаточно ли велик интервал времени между итерациями счета
 if(MinInterstepsInterval>0 && CalcDiffTime(cur_time,LastStepStartTime)<MinInterstepsInterval)
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

}
#endif
