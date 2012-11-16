#ifndef UTIME_CPP
#define UTIME_CPP

#include "UTime.h"

namespace RDK {

UTime DefaultTimeStep=(UTime)2000;

// --------------------------
// Глобальные свойства
// --------------------------
// Текущее время модели
ULongTime UTimeControl::Time=0;
double UTimeControl::DoubleTime=0;

// Реальное время
ULongTime UTimeControl::RealTime=0;
double UTimeControl::DoubleRealTime=0;

// Мгновенный шаг в реальном времени
ULongTime UTimeControl::RealTimeStep=1;
double UTimeControl::DoubleRealTimeStep=1.0e-6;


// Текущее время внешних источников данных в микросекундах
ULongTime UTimeControl::SourceTime=0;

// Текущее время внешних источников данных в секундах
double UTimeControl::DoubleSourceTime=0;

// Мгновенный шаг во времени внешних источников данных в микросекундах
ULongTime UTimeControl::SourceTimeStep=1;

// Мгновенный шаг во времени внешних источников данных в секундах
double UTimeControl::DoubleSourceTimeStep=1.0e-6;
// --------------------------

// --------------------------
// Методы управления глобальными свойствами
// --------------------------
// Возвращает текущее время модели
const ULongTime& UTimeControl::GetTime(void)
{
 return Time;
}

const double& UTimeControl::GetDoubleTime(void)
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
 return UTimeControl::SetTime(UTimeControl::GetTime()+value);
}

// Увеличивает время модели на заданную величину при заданном шаге вычислений
// в тысячных долях секунды
bool UTimeControl::IncreaseModelTimeByStep(ULongTime step)
{
 return UTimeControl::IncreaseModelTime(1000000/step);
}

// Возвращает реальное время
const ULongTime& UTimeControl::GetRealTime(void)
{
 return RealTime;
}

const double& UTimeControl::GetDoubleRealTime(void)
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
const ULongTime& UTimeControl::GetRealTimeStep(void)
{
 return RealTimeStep;
}

const double& UTimeControl::GetDoubleRealTimeStep(void)
{
 return DoubleRealTimeStep;
}

// Текущее время внешних источников данных в микросекундах
const ULongTime& UTimeControl::GetSourceTime(void)
{
 return SourceTime;
}

const double& UTimeControl::GetDoubleSourceTime(void)
{
 return DoubleSourceTime;
}

// Устанавливает время внешних источников данных
bool UTimeControl::SetSourceTime(ULongTime value)
{
 SourceTimeStep=value-SourceTime;
 if(SourceTimeStep == 0)
  SourceTimeStep=1;

 DoubleSourceTimeStep=SourceTimeStep/1.0e6;

 SourceTime=value;
 DoubleSourceTime=SourceTime/1000000.0;
 return true;
}

// Увеличивает время внешних источников данных на заданную величину
bool UTimeControl::IncreaseSourceTime(ULongTime value)
{
 return SetSourceTime(SourceTime+value);
}

// Мгновенный шаг во времени внешних источников данных в микросекундах
const ULongTime& UTimeControl::GetSourceTimeStep(void)
{
 return SourceTimeStep;
}

const double& UTimeControl::GetDoubleSourceTimeStep(void)
{
 return DoubleSourceTimeStep;
}
// --------------------------


}

#endif

