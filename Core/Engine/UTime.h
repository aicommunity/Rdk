#ifndef UTIME_H
#define UTIME_H

namespace RDK {

typedef int UTime;
typedef long long ULongTime;

extern const UTime DefaultTimeStep;

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

public: // Открытые методы
// --------------------------
// Методы управления глобальными свойствами
// --------------------------
// Возвращает текущее время модели
static const ULongTime& GetTime(void);
static const double& GetDoubleTime(void);

// Устанавливает текущее время модели
static bool SetTime(ULongTime value);

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
// --------------------------


};

}

#endif

