#ifndef UTIME_H
#define UTIME_H

namespace RDK {

typedef int UTime;
typedef long long ULongTime;

extern UTime DefaultTimeStep;

class UTimeControl
{
private: // Глобальные свойства
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
ULongTime SourceTime;

// Текущее время внешних источников данных в секундах
double DoubleSourceTime;

// Мгновенный шаг во времени внешних источников данных в микросекундах
ULongTime SourceTimeStep;

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


// Текущее время внешних источников данных в микросекундах
const ULongTime& GetSourceTime(void) const;
const double& GetDoubleSourceTime(void) const;

// Устанавливает время внешних источников данных
bool SetSourceTime(ULongTime value);

// Увеличивает время внешних источников данных на заданную величину
bool IncreaseSourceTime(ULongTime value);

// Мгновенный шаг во времени внешних источников данных в микросекундах
const ULongTime& GetSourceTimeStep(void) const;
const double& GetDoubleSourceTimeStep(void) const;
// --------------------------


};

}

#endif

