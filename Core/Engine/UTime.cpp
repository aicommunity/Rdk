#ifndef UTIME_CPP
#define UTIME_CPP

#include "UTime.h"

namespace RDK {

UTime DefaultTimeStep=(UTime)2000;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UTimeControl::UTimeControl(void)
{
 // Текущее время модели
 Time=0;
 DoubleTime=0;

 // Реальное время
 RealTime=0;
 DoubleRealTime=0;

 // Мгновенный шаг в реальном времени
 RealTimeStep=1;
 DoubleRealTimeStep=1.0e-6;

 // Текущее время внешних источников данных в микросекундах
// SourceTime=0;

 // Текущее время внешних источников данных в секундах
 DoubleSourceTime=0;

 // Мгновенный шаг во времени внешних источников данных в микросекундах
// SourceTimeStep=1;

 // Мгновенный шаг во времени внешних источников данных в секундах
 DoubleSourceTimeStep=1.0e-6;
}

UTimeControl::~UTimeControl(void)
{

}
// --------------------------

// --------------------------
// Методы управления глобальными свойствами
// --------------------------
/// Время начала расчета в днях
double UTimeControl::GetSourceStartGlobalTime(void) const
{
 return SourceStartGlobalTime;
}

bool UTimeControl::SetSourceStartGlobalTime(double value)
{
 SourceStartGlobalTime=value;
 return true;
}

/// Время начала расчета в днях
double UTimeControl::GetSourceStepGlobalTime(void) const
{
 return SourceStepGlobalTime;
}

bool UTimeControl::SetSourceStepGlobalTime(double value)
{
 SourceStepGlobalTime=value;
 return true;
}

/// Текущее время в днях
double UTimeControl::GetSourceStartLocalTime(void) const
{
 return SourceStartLocalTime;
}

double UTimeControl::SetSourceStartLocalTime(double value)
{
 SourceStartLocalTime=value;
 return true;
}

/// Текущее время в днях
double UTimeControl::GetSourceStepLocalTime(void) const
{
 return SourceStepLocalTime;
}

double UTimeControl::SetSourceStepLocalTime(double value)
{
 SourceStepLocalTime=value;
 return SetRealTime((SourceStepLocalTime-SourceStartLocalTime)*1000.0);
}
// --------------------------

// --------------------------
// Временные метки в процессе работы в секундах от начала дня или др.
// системного события
// --------------------------
/*
/// Время начала расчета в секундах
double UTimeControl::GetModelStartSecondTime(void) const
{
 return ModelStartSecondTime;
}

bool UTimeControl::SetModelStartSecondTime(double value)
{
 ModelStartSecondTime=value;
 return true;
}

/// Время начала расчета текущего шага в секундах
double UTimeControl::GetStepStartSecondTime(void) const
{
 return StepStartSecondTime;
}

bool UTimeControl::SetStepStartSecondTime(double value)
{
 StepStartSecondTime=value;
 return SetRealTime((StepStartSecondTime-ModelStartSecondTime)*1000);
}                            */
// --------------------------

// --------------------------
// Методы управления глобальными свойствами
// --------------------------
// Возвращает текущее время модели
const ULongTime& UTimeControl::GetTime(void) const
{
 return Time;
}

const double& UTimeControl::GetDoubleTime(void) const
{
 return DoubleTime;
}

// Устанавливает текущее время модели
bool UTimeControl::SetTime(ULongTime value)
{
 if(Time == value)
  return true;

 Time=value;
 DoubleTime=Time/1000000.0;
 return true;
}

// Увеличивает время модели на заданную величину
bool UTimeControl::IncreaseModelTime(ULongTime value)
{
 return SetTime(UTimeControl::GetTime()+value);
}

// Увеличивает время модели на заданную величину при заданном шаге вычислений
// в тысячных долях секунды
bool UTimeControl::IncreaseModelTimeByStep(ULongTime step)
{
 return IncreaseModelTime(1000000/step);
}

// Возвращает реальное время
const ULongTime& UTimeControl::GetRealTime(void) const
{
 return RealTime;
}

const double& UTimeControl::GetDoubleRealTime(void) const
{
 return DoubleRealTime;
}

// Устанавливает реальное время
bool UTimeControl::SetRealTime(ULongTime value)
{
 RealTimeStep=value-RealTime;
 if(RealTimeStep == 0)
  RealTimeStep=1;

 DoubleRealTimeStep=RealTimeStep/1.0e6;

 RealTime=value;
 DoubleRealTime=RealTime/1000000.0;
 return true;
}

// Увеличивает реальное время на заданную величину
bool UTimeControl::IncreaseRealTime(ULongTime value)
{
 return SetRealTime(RealTime+value);
}

// Возвращает мгновенный шаг в реальном времени
const ULongTime& UTimeControl::GetRealTimeStep(void) const
{
 return RealTimeStep;
}

const double& UTimeControl::GetDoubleRealTimeStep(void) const
{
 return DoubleRealTimeStep;
}
// --------------------------

// --------------------------
/// Методы доступа к текущему времени расчета от начала времен в днях
// --------------------------
/// Возвращает текущее время от начала времен в днях
/// Принимает аргументом текущее локальное время в секундах или миллисекундах
double UTimeControl::CalcCurrentGlobalTime(double current_local_time)
{
 return SourceStepGlobalTime+(current_local_time-SourceStepLocalTime)/86400.0;
}

double UTimeControl::CalcCurrentGlobalTime(ULongTime current_local_time)
{
 return SourceStepGlobalTime+(double(current_local_time)/1000.0-SourceStepLocalTime)/86400.0;
}
// --------------------------


}

#endif

