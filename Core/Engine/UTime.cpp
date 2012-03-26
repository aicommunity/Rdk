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
// --------------------------


}

#endif

