#ifndef UTIME_H
#define UTIME_H

namespace RDK {

typedef int UTime;
typedef long long ULongTime;

extern UTime DefaultTimeStep;

class UTimeControl
{
private: // Временные метки в процссе работы в днях от начала времен
/// Время начала расчета в днях
double SourceStartGlobalTime;

/// Время начала расчета текущего шага в днях
double SourceCurrentGlobalTime;

/// Время начала расчета в секундах от начала дня или другого события системы
double SourceStartLocalTime;

/// Время начала расчета текущего шага в секундах от начала дня или другого события системы
double SourceCurrentLocalTime;

private: // Временные метки в процессе работы в секундах от начала дня или др.
		 // системного события
/// Время начала расчета в секундах
double ModelStartSecondTime;

/// Время начала расчета текущего шага в секундах
double StepStartSecondTime;

private: // Свойства времени в процессе работы в микросекундах и секундах от начала расчета
// Текущее время модели в микросекундах
ULongTime Time;

// Текущее время модели в секундах
double DoubleTime;

// Реальное время в микросекундах
ULongTime RealTime;

// Реальное время в секундах
double DoubleRealTime;

// Мгновенный шаг в реальном времени в микросекундах
ULongTime RealTimeStep;

// Мгновенный шаг в реальном времени в секундах
double DoubleRealTimeStep;

// Текущее время внешних источников данных в микросекундах
//ULongTime SourceTime;

// Текущее время внешних источников данных в секундах
double DoubleSourceTime;

// Мгновенный шаг во времени внешних источников данных в микросекундах
//ULongTime SourceTimeStep;

// Мгновенный шаг во времени внешних источников данных в секундах
double DoubleSourceTimeStep;


public: // Открытые методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UTimeControl(void);
virtual ~UTimeControl(void);
// --------------------------

// --------------------------
// Методы управления глобальными свойствами
// --------------------------
/// Время начала расчета в днях
double GetSourceStartGlobalTime(void) const;
bool SetSourceStartGlobalTime(double value);

/// Время начала расчета в днях
double GetSourceCurrentGlobalTime(void) const;
bool SetSourceCurrentGlobalTime(double value);

/// Текущее время в днях
double GetSourceStartLocalTime(void) const;
double SetSourceStartLocalTime(double value);

/// Текущее время в секундах
double GetSourceCurrentLocalTime(void) const;
double SetSourceCurrentLocalTime(double value);

/// Текущее время в миллисекундах
long long GetSourceCurrentLocalTimeMs(void) const;
// --------------------------

// --------------------------
// Методы управления глобальными свойствами
// --------------------------
// Возвращает текущее время модели
const ULongTime& GetTime(void) const;
const double& GetDoubleTime(void) const;

// Устанавливает текущее время модели
bool SetTime(ULongTime value);

// Увеличивает время модели на заданную величину
bool IncreaseModelTime(ULongTime value);

// Увеличивает время модели на заданную величину при заданном шаге вычислений
// в тысячных долях секунды
bool IncreaseModelTimeByStep(ULongTime step);

// Возвращает реальное время
const ULongTime& GetRealTime(void) const;
const double& GetDoubleRealTime(void) const;

// Устанавливает реальное время
bool SetRealTime(ULongTime value);

// Увеличивает реальное время на заданную величину
bool IncreaseRealTime(ULongTime value);

// Возвращает мгновенный шаг в реальном времени
const ULongTime& GetRealTimeStep(void) const;
const double& GetDoubleRealTimeStep(void) const;
// --------------------------

// --------------------------
/// Методы доступа к текущему времени расчета от начала времен в днях
// --------------------------
/// Возвращает текущее время от начала времен в днях
/// Принимает аргументом текущее локальное время в секундах или миллисекундах
double CalcCurrentGlobalTime(double current_local_time) const;
double CalcCurrentGlobalTime(ULongTime current_local_time) const;
// --------------------------

};

}

#endif

