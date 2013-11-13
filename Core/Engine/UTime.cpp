#ifndef UTIME_CPP
#define UTIME_CPP

#include "UTime.h"

namespace RDK {

UTime DefaultTimeStep=(UTime)2000;

// --------------------------
//  онструкторы и деструкторы
// --------------------------
UTimeControl::UTimeControl(void)
{
 // “екущее врем€ модели
 Time=0;
 DoubleTime=0;

 // –еальное врем€
 RealTime=0;
 DoubleRealTime=0;

 // ћгновенный шаг в реальном времени
 RealTimeStep=1;
 DoubleRealTimeStep=1.0e-6;

 // “екущее врем€ внешних источников данных в микросекундах
 SourceTime=0;

 // “екущее врем€ внешних источников данных в секундах
 DoubleSourceTime=0;

 // ћгновенный шаг во времени внешних источников данных в микросекундах
 SourceTimeStep=1;

 // ћгновенный шаг во времени внешних источников данных в секундах
 DoubleSourceTimeStep=1.0e-6;
}

UTimeControl::~UTimeControl(void)
{

}
// --------------------------

// --------------------------
// ћетоды управлени€ глобальными свойствами
// --------------------------
/// ¬рем€ начала расчета в дн€х
double UTimeControl::GetStartDayTime(void) const
{
 return StartDayTime;
}

bool UTimeControl::SetStartDayTime(double value)
{
 StartDayTime=value;
 return true;
}

/// “екущее врем€ в дн€х
double UTimeControl::GetCurrentDayTime(void) const
{
 return CurrentDayTime;
}

double UTimeControl::SetCurrentDayTime(double value)
{
 CurrentDayTime=value;
 return true;
}
// --------------------------


// --------------------------
// ћетоды управлени€ глобальными свойствами
// --------------------------
// ¬озвращает текущее врем€ модели
const ULongTime& UTimeControl::GetTime(void) const
{
 return Time;
}

const double& UTimeControl::GetDoubleTime(void) const
{
 return DoubleTime;
}

// ”станавливает текущее врем€ модели
bool UTimeControl::SetTime(ULongTime value)
{
 if(Time == value)
  return true;

 Time=value;
 DoubleTime=Time/1000000.0;
 return true;
}

// ”величивает врем€ модели на заданную величину
bool UTimeControl::IncreaseModelTime(ULongTime value)
{
 return SetTime(UTimeControl::GetTime()+value);
}

// ”величивает врем€ модели на заданную величину при заданном шаге вычислений
// в тыс€чных дол€х секунды
bool UTimeControl::IncreaseModelTimeByStep(ULongTime step)
{
 return IncreaseModelTime(1000000/step);
}

// ¬озвращает реальное врем€
const ULongTime& UTimeControl::GetRealTime(void) const
{
 return RealTime;
}

const double& UTimeControl::GetDoubleRealTime(void) const
{
 return DoubleRealTime;
}

// ”станавливает реальное врем€
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

// ”величивает реальное врем€ на заданную величину
bool UTimeControl::IncreaseRealTime(ULongTime value)
{
 return SetRealTime(RealTime+value);
}

// ¬озвращает мгновенный шаг в реальном времени
const ULongTime& UTimeControl::GetRealTimeStep(void) const
{
 return RealTimeStep;
}

const double& UTimeControl::GetDoubleRealTimeStep(void) const
{
 return DoubleRealTimeStep;
}

// “екущее врем€ внешних источников данных в микросекундах
const ULongTime& UTimeControl::GetSourceTime(void) const
{
 return SourceTime;
}

const double& UTimeControl::GetDoubleSourceTime(void) const
{
 return DoubleSourceTime;
}

// ”станавливает врем€ внешних источников данных
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

// ”станавливает врем€ внешних источников данных в дн€х
bool UTimeControl::SetDoubleSourceTime(double value)
{
 DoubleSourceTime=value*86400;

 long long new_source_time=DoubleSourceTime/1.0e6;

 SourceTimeStep=new_source_time-SourceTime;
 if(SourceTimeStep == 0)
  SourceTimeStep=1;

 SourceTime=new_source_time;
 return true;
}

// ”величивает врем€ внешних источников данных на заданную величину
bool UTimeControl::IncreaseSourceTime(ULongTime value)
{
 return SetSourceTime(SourceTime+value);
}

// ћгновенный шаг во времени внешних источников данных в микросекундах
const ULongTime& UTimeControl::GetSourceTimeStep(void) const
{
 return SourceTimeStep;
}

const double& UTimeControl::GetDoubleSourceTimeStep(void) const
{
 return DoubleSourceTimeStep;
}
// --------------------------


}

#endif

