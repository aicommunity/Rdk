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
#include "UAEnvironment.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UClassLibraryList::UClassLibraryList(void)
{
 Libraries=0;
 Size=RealSize=0;
}

UClassLibraryList::UClassLibraryList(const UClassLibraryList &copy)
{
 Libraries=0;
 Size=RealSize=0;
 *this=copy;
}

UClassLibraryList::~UClassLibraryList(void)
{
 Clear();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Очищает хранилище
void UClassLibraryList::Clear(void)
{
 if(Libraries)
 {
  delete []Libraries;
  Libraries=0;
 }
 Size=RealSize=0;
}

// Возвращает размер массива
int UClassLibraryList::GetSize(void) const
{
 return Size;
}

// Изменяет размер хранилища
// Метод сохраняет старые данные
void UClassLibraryList::Resize(int newsize)
{
 int newrealsize=0;
 if(newsize < 8)
  newrealsize=16;
 else
  newrealsize=newsize<<2;
 if(RealSize<newsize || !Libraries)
 {
  PUALibrary* newbuffer=new PUALibrary[newrealsize];
  memcpy(newbuffer,Libraries,sizeof(PUALibrary)*Size);

  if(Libraries)
   delete []Libraries;
  Libraries=newbuffer;
  RealSize=newrealsize;
  Size=newsize;
 }
 else
 {
  Size=newsize;
 }
}

// Ищет класс по Id и удаляет его из массива
// Возвращает указатель на удаленный класс
PUALibrary UClassLibraryList::Erase(int index)
{
 if(!Libraries || index<0 || index>=Size)
  return 0;

 PUALibrary result=Libraries[index];

 if(index<Size-1)
  memmove(Libraries+index+1,Libraries+index,sizeof(PUALibrary)*(Size-index-1));

 Resize(Size-1);

 return result;
}

// Добавляет новый элемент в конец хранилища
void UClassLibraryList::PushBack(PUALibrary classelement)
{
 Resize(Size+1);
 Libraries[Size-1]=classelement;
}

// Возвращает указатель на массив классов
PUALibrary* UClassLibraryList::GetLibraries(void) const
{
 return Libraries;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор присваивания
UClassLibraryList& UClassLibraryList::operator = (const UClassLibraryList &copy)
{
 Resize(copy.Size);
 for(int i=0;i<Size;i++)
  Libraries[i]=copy.Libraries[i];

 return *this;
}

// Оператор доступа
PUALibrary UClassLibraryList::operator [] (int i)
{
 return Libraries[i];
}
// --------------------------


// --------------------------
// Constructors & destructors
// --------------------------
UAEnvironment::UAEnvironment(void)
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

 MaxModelDuration=0.1;

 // Текущий компонент модели
// CurrentComponent=0;
}

UAEnvironment::~UAEnvironment(void)
{
 DestroyModel();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Индекс предарительно заданной модели обработки
int UAEnvironment::GetPredefinedStructure(void) const
{
 return PredefinedStructure;
}

bool UAEnvironment::SetPredefinedStructure(int value)
{
 if(PredefinedStructure == value)
  return true;

 PredefinedStructure=value;
 return DestroyStructure();
}


// Идентификатор компонента модели, который будет обсчитываться
const ULongId& UAEnvironment::GetModelCalculationComponent(void) const
{
 return ModelCalculationComponent;
}

bool UAEnvironment::SetModelCalculationComponent(const ULongId& value)
{
 if(ModelCalculationComponent == value)
  return true;

 ModelCalculationComponent=value;

 return true;
}
// --------------------------


// --------------------------
// Методы управления состояниями
// --------------------------
// Признак наличия сформированной структуры
bool UAEnvironment::IsStructured(void) const
{
 return Structured;
}
// --------------------------

// --------------------------
// Методы управления данными среды
// --------------------------
// Возвращает указатель на хранилище
UAStorage* UAEnvironment::GetStorage(void)
{
 return Storage;
}

// Устанавливает новое хранилище
// Указатель на старое хранилище более не используется средой
// Ответственность за освобождение памяти лежит на вызывающей стороне
// Текущая модель уничтожается.
bool UAEnvironment::SetStorage(UAStorage *storage)
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
UEPtr<UAContainer> UAEnvironment::GetModel(void)
{
 return Model;
}

// Создает новую модель из хранилища по id класса
bool UAEnvironment::CreateModel(const UId& classid)
{
 if(!IsStoragePresent())
  return false;

 CurrentComponent=Model=dynamic_pointer_cast<UAContainer>(Storage->TakeObject(classid));
 Ready=false;
 if(Model)
  return true;

 return false;
}

// Уничтожает текущую модель
bool UAEnvironment::DestroyModel(void)
{
 if(!Model)
  return true;

 Model->Free();
 Model=0;
 CurrentComponent=0;

 return true;
}

// Возвращает библиотеку по индексу
UALibrary* UAEnvironment::GetClassLibrary(int index)
{
 return ClassLibraryList[index];
}

// Возвращает число библиотек
int UAEnvironment::GetNumClassLibraries(void) const
{
 return ClassLibraryList.GetSize();
}

// Непосредственно добавялет новый образец класса в хранилище
bool UAEnvironment::AddClass(UAContainer *newclass)
{
 if(!Storage)
  return false;

 UId classid=newclass->GetClass();
 if(!Storage->AddClass(newclass,classid) == ForbiddenId)
  return false;

 return true;
}

// Подключает динамическую библиотеку с набором образцов классов.
// Если бибилиотека с таким именем уже существует то возвращает false.
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
bool UAEnvironment::AddClassLibrary(UALibrary *library)
{
 if(!library)
  return false;

 ClassLibraryList.PushBack(library);
 return true;
}

// Удаляет подключенную библиотеку из списка по индексу
// Ответственность за освобождение памяти библиотекой лежит на вызывающей стороне.
bool UAEnvironment::DelClassLibrary(int index)
{
 if(index < 0 || index >= int(ClassLibraryList.GetSize()))
  return false;

 ClassLibraryList.Erase(index);
 return true;
}

// Удаляет из списка все библиотеки
// Ответственность за освобождение памяти лежит на вызывающей стороне.
bool UAEnvironment::DelAllClassLibraries(void)
{
 ClassLibraryList.Clear();
 return true;
}

// Заполняет хранилище данными библиотек
// Операция предварительно уничтожает модель и очищает хранилище
bool UAEnvironment::BuildStorage(void)
{
 if(!IsStoragePresent())
  return false;

 if(!DestroyModel())
  return false;

// if(!Storage->ClearClassesStorage())
//  return false;

 for(int i=0;i<ClassLibraryList.GetSize();i++)
 {
  ClassLibraryList[i]->Upload(Storage);
 }
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
UEPtr<UAComponent> UAEnvironment::GetCurrentComponent(void)
{
 return CurrentComponent;
}
// --------------------------

// --------------------------
// Данные графического интерфеса пользователя
// --------------------------
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
bool UAEnvironment::IsStoragePresent(void) const
{
 return StoragePresent;
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Инициализация среды
void UAEnvironment::Init(void)
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
void UAEnvironment::UnInit(void)
{
 if(Model)
  Model->UnInit();

 if(!IsInit())
  return;

 AUnInit();

 Initialized=false;
}

// Формирует предварительно заданную модель обработки
bool UAEnvironment::CreateStructure(void)
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
bool UAEnvironment::DestroyStructure(void)
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
void UAEnvironment::RTCalculate(void)
{
 Build();

// StartProcTime=GetCurrentStartupTime();

 CurrentTime=GetCurrentStartupTime();
 UAContainer::SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);

 long long curtime;
 long long TimerInterval=0;
 //double devicemodeltime=0;

 TimerInterval=GetCurrentStartupTime()-ProcEndTime;
 if(TimerInterval<=0)
  TimerInterval=1;

 int i=0;
 if(LastDuration < TimerInterval)
  LastDuration=TimerInterval;
 double model_duration=(UAContainer::GetRealTime()-Model->GetDoubleTime()*1e6)/1000.0;

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
 if(UAContainer::GetRealTime()/1e6<Model->GetDoubleTime())
 {
  Sleep(int(Model->GetDoubleTime()*1000-UAContainer::GetRealTime()/1000));
  UAContainer::SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);
 }

 LastDuration=GetCurrentStartupTime()-CurrentTime;
 ProcEndTime=GetCurrentStartupTime();

 return;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Инициализация среды
void UAEnvironment::AInit(void)
{
 return;
}

// Деинициализация среды
void UAEnvironment::AUnInit(void)
{
 return;
}

// Формирует предварительно заданную модель обработки
bool UAEnvironment::ACreateStructure(void)
{
 return true;
}


// Уничтожает текущую модель обработки
bool UAEnvironment::ADestroyStructure(void)
{
 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool UAEnvironment::ADefault(void)
{
 if(!Model)
  return true;

// UAComponent::SetTime(0);
 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Default();
 else
 {
  UEPtr<UAContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Default())
   return false;
 }

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UAEnvironment::ABuild(void)
{
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Build();
 else
 {
  UEPtr<UAContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Build())
   return false;
 }

 return true;
}

// Сброс процесса счета.
bool UAEnvironment::AReset(void)
{
 StartupTime=GetCurrentStartupTime();
 LastDuration=1;
 ProcEndTime=StartupTime;

 if(!Model)
  return true;

// RDK::DefaultTimeStep=30;
// Model->SetTimeStep(DefaultTimeStep);

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Reset();
 else
 {
  UEPtr<UAContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Reset())
   return false;
 }

 return true;
}

// Выполняет расчет этого объекта
bool UAEnvironment::ACalculate(void)
{
 UAContainer::SetRealTime(CalcDiffTime(GetCurrentStartupTime(),StartupTime)*1000);
 if(!Model)
  return true;

 if(ModelCalculationComponent.GetSize() == 0)
  return Model->Calculate();
 else
 {
  UEPtr<UAContainer> destcont;
  destcont=GetModel()->GetComponentL(ModelCalculationComponent);

  if(!destcont)
   return false;

  if(!destcont->Calculate())
   return false;
 }

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
