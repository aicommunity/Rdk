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

 MaxModelDuration=1000;

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
 MaxCalcTime=0.0;
 CalcFinishedFlag=false;

 UseIndTimeStepFlag=false;
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
unsigned long long UEnvironment::GetMinInterstepsInterval(void) const
{
 return MinInterstepsInterval;
}

bool UEnvironment::SetMinInterstepsInterval(unsigned long long value)
{
 if(MinInterstepsInterval == value)
  return true;

 MinInterstepsInterval = value;
 return true;
}

/// Максимальное время расчета модели (сек)
/// если 0 - то не ограничено
double UEnvironment::GetMaxCalcTime(void) const
{
 return MaxCalcTime;
}

bool UEnvironment::SetMaxCalcTime(double value)
{
 if(value<0)
  return false;
 MaxCalcTime=value;
 return true;
}

/// Флаг включения использования индидвидуальных параметров TimeStep для
/// каждого компонента
bool UEnvironment::GetUseIndTimeStepFlag(void) const
{
 return UseIndTimeStepFlag;
}

bool UEnvironment::SetUseIndTimeStepFlag(bool value)
{
 UseIndTimeStepFlag=value;
 if(Model)
  Model->ChangeUseIndTimeStepMode(value);
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

 UnRegisterAllDataReaders();

 Model->Free();
 Model=0;
 CurrentComponent=0;

 return true;
}

/// Инициализирует модель
void UEnvironment::ModelInit(void)
{
    if(!Model)
     return;

    Model->Init();
}

/// Деинициализирует модель
void UEnvironment::ModelUnInit(void)
{
    if(!Model)
     return;

    Model->UnInit();
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

/// Возвращает набор вариантов predefined structures
const std::map<int, UEnvPredefinedStructDescription>& UEnvironment::GetPredefinedStructures(void) const
{
 return PredefinedStructures;
}

/// Возвращает вариантов predefined structure по id
UEnvPredefinedStructDescription UEnvironment::GetPredefinedStructureDescription(int id) const
{
 std::map<int, UEnvPredefinedStructDescription>::const_iterator I=PredefinedStructures.find(id);
 if(I != PredefinedStructures.end())
  return I->second;

 return UEnvPredefinedStructDescription();
}

/// Добавляет вариант predefined structure
bool UEnvironment::AddPredefinedStructure(const UEnvPredefinedStructDescription &descr)
{
 std::map<int, UEnvPredefinedStructDescription>::const_iterator I=PredefinedStructures.find(descr.Id);
 if(I != PredefinedStructures.end())
  return false;
 PredefinedStructures[descr.Id]=descr;
 return true;
}

/// Добавляет удаляет predefined structure по id
void UEnvironment::DelPredefinedStructure(int id)
{
 std::map<int, UEnvPredefinedStructDescription>::iterator I=PredefinedStructures.find(id);
 if(I == PredefinedStructures.end())
  return;

 PredefinedStructures.erase(I);
}

/// Удаляет все predefined structures
void UEnvironment::ClearPredefinedStructures(void)
{
 PredefinedStructures.clear();
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
 catch(UContainer::EComponentNameNotExist &)
 {
  return false;
 }
 return true;
}
// --------------------------


// --------------------------
// Методы управления регистрацией данных
// --------------------------
/// Регистрирует новую точку съема данных (вида MDMatrix)
UControllerDataReader* UEnvironment::RegisterDataReader(const std::string &component_name, const std::string &property_name, int row, int col)
{
 for(size_t i=0;i<DataReaders.size();i++)
  if(DataReaders[i].first && DataReaders[i].first ->GetComponentName() == component_name && DataReaders[i].first ->GetPropertyName() == property_name &&
     DataReaders[i].first ->MRow == row && DataReaders[i].first ->MCol == col)
  {
   DataReaders[i].second++;
   return DataReaders[i].first;
  }

 if(!Model)
  return 0;

 UContainer *cont=Model->GetComponentL(component_name);
 if(!cont)
  return 0;

 UEPtr<UIProperty> prop=cont->FindProperty(property_name);
 if(!prop)
  return 0;

 UControllerDataReader *data=new UControllerDataReader();
 if(!data->Configure(cont,prop))
  {
   delete data;
   return 0;
  }

 data->SetMatrixCoord(row,col);

 DataReaders.push_back(std::make_pair(data,1));
 return data;
}

/// Снимает регистрацию точки съема данных (вида MDMatrix)
void UEnvironment::UnRegisterDataReader(const std::string &component_name, const std::string &property_name, int row, int col)
{
 for(size_t i=0;i<DataReaders.size();i++)
 {
  if(DataReaders[i].first && DataReaders[i].first->GetComponentName() == component_name && DataReaders[i].first->GetPropertyName() == property_name &&
     DataReaders[i].first->MRow == row && DataReaders[i].first->MCol == col)
  {
   DataReaders[i].second--;
   if(DataReaders[i].second==0)
   {
    delete DataReaders[i].first;
    DataReaders.erase(DataReaders.begin()+i);
   }
   return;
  }
 }
}

/// Снимает регистрацию всех точек съема данных (вида MDMatrix)
void UEnvironment::UnRegisterAllDataReaders(void)
{
 for(size_t i=0;i<DataReaders.size();i++)
  delete DataReaders[i].first;
 DataReaders.clear();
}

/// Возвращает данные точки съема
UControllerDataReader* UEnvironment::GetDataReader(const std::string &component_name, const std::string &property_name, int row, int col)
{
 for(size_t i=0;i<DataReaders.size();i++)
 {
  if(DataReaders[i].first && DataReaders[i].first->GetComponentName() == component_name && DataReaders[i].first->GetPropertyName() == property_name &&
     DataReaders[i].first->MRow == row && DataReaders[i].first->MCol == col)
  {
   return DataReaders[i].first;
  }
 }
 return 0;
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
 if(MaxCalcTime>0.0 && Time.GetDoubleTime()>=MaxCalcTime)
  CalcFinishedFlag=true;
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

/// Флаг, выставляемый если достигнут конец расчета (по MaxCalcTime)
bool UEnvironment::IsCalcFinished(void) const
{
 return CalcFinishedFlag;
}

// Расчет модели в реальном времени
void UEnvironment::RTCalculate(void)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't ialized.");
  return;
 }

 if(IsCalcFinished())
  return;
 Build();

 CurrentTime=GetCurrentStartupTime();
// Time.SetSourceCurrentLocalTime(double(GetCurrentStartupTime())/1000.0);

 // Если первый шаг расчета после Reset
 if(Time.GetTime() == 0)
 {
  Time.SetSourceStartLocalTime(Time.GetSourceCurrentLocalTime());
  Time.SetSourceStartGlobalTime(Time.GetSourceCurrentGlobalTime());
  StartupTime=CurrentTime;
  ProcEndTime=StartupTime;
 }

 double cur_time=(Time.GetSourceCurrentGlobalTime()-Time.GetSourceStartGlobalTime())*86400.0;
 Time.SetSourceCurrentLocalTime(cur_time);

 unsigned long long curtime;
 unsigned long long timer_interval=0;
 //double devicemodeltime=0;

 timer_interval=CalcDiffTime(GetCurrentStartupTime(),ProcEndTime);
 if(timer_interval<=0)
  timer_interval=1;

 int i=0;
 if(LastDuration < timer_interval)
  LastDuration=timer_interval;
 //double realtime=Time.GetSourceCurrentLocalTime();
 //double doubletime=Time.GetDoubleTime();
 double model_duration=Time.GetSourceCurrentLocalTime()-Time.GetDoubleTime();
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
  if(Time.GetTime() == 0)
   elapsed_counter=1;
  else
   elapsed_counter=0;
 }

 curtime=GetCurrentStartupTime();
 while(curtime-CurrentTime<timer_interval && i<elapsed_counter)
 {
  Calculate();
  //for(size_t i=0;i<DataReaders.size();i++)
  // DataReaders[i]->Update();

  ++i;
  curtime=GetCurrentStartupTime();
  if(MaxCalcTime>0.0 && Time.GetDoubleTime()>=MaxCalcTime)
  {
   CalcFinishedFlag=true;
   break;
  }
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
}

/// Расчет модели порциями длительностью calc_intervsal секунд с максимально возможной скоростью
void UEnvironment::FastCalculate(double calc_interval)
{
 if(!IsInit())
 {
  Logger->LogMessage(RDK_EX_ERROR, __FUNCTION__, "Environment does't initialized.");
  return;
 }

 if(IsCalcFinished())
  return;
 Build();

 CurrentTime=GetCurrentStartupTime();
// Time.SetSourceCurrentLocalTime(double(GetCurrentStartupTime())/1000.0);

 // Если первый шаг расчета после Reset
 if(Time.GetTime() == 0)
 {
  Time.SetSourceStartLocalTime(Time.GetSourceCurrentLocalTime());
  Time.SetSourceStartGlobalTime(Time.GetSourceCurrentGlobalTime());
  StartupTime=CurrentTime;
  ProcEndTime=StartupTime;
 }

 ULongTime cur_time=(ULongTime)((Time.GetSourceCurrentGlobalTime()-Time.GetSourceStartGlobalTime())*(86400.0*1000.0));
 Time.SetSourceCurrentLocalTime(cur_time/1000.0);

 //unsigned long long curtime;
 unsigned long long timer_interval=0;
 //double devicemodeltime=0;

 timer_interval=CalcDiffTime(GetCurrentStartupTime(),ProcEndTime);
 if(timer_interval<=0)
  timer_interval=1;

 int i=0;
 if(LastDuration < timer_interval)
  LastDuration=timer_interval;
 double model_duration=calc_interval*1000.0;
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

 while(i<elapsed_counter)
 {
  Calculate();
  //for(size_t i=0;i<DataReaders.size();i++)
  // DataReaders[i]->Update();

  ++i;
  if(MaxCalcTime>0.0 && Time.GetDoubleTime()>=MaxCalcTime)
  {
   CalcFinishedFlag=true;
   break;
  }
 }
// curtime=GetCurrentStartupTime();

 if(Time.GetRealTime()/1e6<Time.GetDoubleTime())
 {
  Sleep(int(Time.GetDoubleTime()*1000-Time.GetRealTime()/1000));
  Time.SetSourceCurrentLocalTime(double(GetCurrentStartupTime())/1000.0);
 }

 LastDuration=CalcDiffTime(GetCurrentStartupTime(),CurrentTime);
 ProcEndTime=GetCurrentStartupTime();
 double model_stop_calc_time=Time.GetDoubleTime();
 RTModelCalcTime=model_stop_calc_time-model_start_calc_time;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Инициализация среды
void UEnvironment::AInit(void)
{
 ModelCalculationComponent.Resize(0);
 ModelInit();
 return;
}

// Деинициализация среды
void UEnvironment::AUnInit(void)
{
 ModelUnInit();

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
 CalcFinishedFlag=false;
 StartupTime=0;
 ProcEndTime=StartupTime;
 LastDuration=1;
 LastStepStartTime=0;
 Time.SetTime(0);

 if(Logger)
  Logger->Reset();
 RTModelCalcTime=0;

 for(size_t i=0;i<DataReaders.size();i++)
  DataReaders[i].first->Clear();

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

 if(IsCalcFinished())
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

 //for(size_t i=0;i<DataReaders.size();i++)
 // DataReaders[i]->Update();

 // Если мы считаем всю модель, то расчитываем время модели здесь,
 // иначе мы ожидаем, что вызывающий модуль сам расчитает время модели
 if(ModelCalculationComponent.GetSize() == 0)
  IncreaseModelTimeByStep();

 return true;
}
// --------------------------

}
#endif
