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
static ULongTime Time;

// Текущее время модели в секундах
static double DoubleTime;

// Реальное время в микросекундах
static ULongTime RealTime;

// Реальное время в секундах
static double DoubleRealTime;

// Мгновенный шаг в реальном времени в микросекундах
static ULongTime RealTimeStep;

// Мгновенный шаг в реальном времени в секундах
static double DoubleRealTimeStep;

// Текущее время внешних источников данных в микросекундах
static ULongTime SourceTime;

// Текущее время внешних источников данных в секундах
static double DoubleSourceTime;

// Мгновенный шаг во времени внешних источников данных в микросекундах
static ULongTime SourceTimeStep;

// Мгновенный шаг во времени внешних источников данных в секундах
static double DoubleSourceTimeStep;


public: // Открытые методы
// --------------------------
// Методы управления глобальными свойствами
// --------------------------
// Возвращает текущее время модели
static const ULongTime& GetTime(void);
static const double& GetDoubleTime(void);

// Устанавливает текущее время модели
static bool SetTime(ULongTime value);

// Увеличивает время модели на заданную величину
static bool IncreaseModelTime(ULongTime value);

// Увеличивает время модели на заданную величину при заданном шаге вычислений
// в тысячных долях секунды
static bool IncreaseModelTimeByStep(ULongTime step);

// Возвращает реальное время
static const ULongTime& GetRealTime(void);
static const double& GetDoubleRealTime(void);

// Устанавливает реальное время
static bool SetRealTime(ULongTime value);

// Увеличивает реальное время на заданную величину
static bool IncreaseRealTime(ULongTime value);

// Возвращает мгновенный шаг в реальном времени
static const ULongTime& GetRealTimeStep(void);
static const double& GetDoubleRealTimeStep(void);


// Текущее время внешних источников данных в микросекундах
static const ULongTime& GetSourceTime(void);
static const double& GetDoubleSourceTime(void);

// Устанавливает время внешних источников данных
static bool SetSourceTime(ULongTime value);

// Увеличивает время внешних источников данных на заданную величину
static bool IncreaseSourceTime(ULongTime value);

// Мгновенный шаг во времени внешних источников данных в микросекундах
static const ULongTime& GetSourceTimeStep(void);
static const double& GetDoubleSourceTimeStep(void);
// --------------------------


};

}

#endif

