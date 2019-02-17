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

 SourceStartGlobalTime=0;
 SourceCurrentGlobalTime=0;
 SourceStartLocalTime=0;
 SourceCurrentLocalTime=0;
 ModelStartSecondTime=0.0;
 StepStartSecondTime=0.0;
}

UTimeControl::~UTimeControl(void)
{

}
// --------------------------

// --------------------------
// Методы управления глобальными свойствами
// --------------------------
/// Время начала расчета в днях (по времени источника данных)
double UTimeControl::GetSourceStartGlobalTime(void) const
{
 return SourceStartGlobalTime;
}

bool UTimeControl::SetSourceStartGlobalTime(double value)
{
 SourceStartGlobalTime=value;
 return true;
}

/// Текущее время источника данных в днях
double UTimeControl::GetSourceCurrentGlobalTime(void) const
{
 return SourceCurrentGlobalTime;
}

bool UTimeControl::SetSourceCurrentGlobalTime(double value)
{
 SourceCurrentGlobalTime=value;
 return true;
}

/// Текущее время в днях
double UTimeControl::GetSourceStartLocalTime(void) const
{
 return SourceStartLocalTime;
}

bool UTimeControl::SetSourceStartLocalTime(double value)
{
 SourceStartLocalTime=value;
 return true;
}

/// Текущее время в днях
double UTimeControl::GetSourceCurrentLocalTime(void) const
{
 return SourceCurrentLocalTime;
}

bool UTimeControl::SetSourceCurrentLocalTime(double value)
{
 SourceCurrentLocalTime=value;
 return SetRealTime(ULongTime((SourceCurrentLocalTime-SourceStartLocalTime)*1000000.0));
}

/// Текущее время в миллисекундах
ULongTime UTimeControl::GetSourceCurrentLocalTimeMs(void) const
{
 return ULongTime(SourceCurrentLocalTime*1000);
}
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
double UTimeControl::CalcCurrentGlobalTime(double current_local_time) const
{
 return SourceCurrentGlobalTime+(current_local_time-SourceCurrentLocalTime)/86400.0;
}

double UTimeControl::CalcCurrentGlobalTime(ULongTime current_local_time) const
{
 return SourceCurrentGlobalTime+(double(current_local_time)/1000.0-SourceCurrentLocalTime)/86400.0;
}
// --------------------------


}

#endif

