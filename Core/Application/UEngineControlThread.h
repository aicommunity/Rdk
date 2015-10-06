#ifndef UENGINE_CONTROL_THREAD_H
#define UENGINE_CONTROL_THREAD_H

#include <string>
#include <vector>
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include "../../Deploy/Include/rdk.h"

namespace RDK {

class RDK_LIB_TYPE UEngineControlThread
{
protected: // Параметры
/// Индекс канала в библиотеке аналитики, управляемый тредом
int EngineIndex;

/// Режим счета
/// 0 - простой расчет
/// 1 - расчет в реальном времени
UELockVar<int> CalculateMode;

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
UELockVar<UTime> MinInterstepsInterval;

protected: // Данные
/// Метка реального времени окончания последнего расчета
UELockVar<double> RealLastCalculationTime;

protected: // События
// Событие состояния расчета. Выставлено на время активности расчета. Сбрасывается по стопу
UGenericEvent* CalcState;

UGenericEvent* CalcEnable;

UGenericEvent* CalcStarted;

UGenericEvent* CalculationNotInProgress;



protected: // Потоки
/// Поток расчета
boost::thread Thread;

bool Terminated;

/// Мьютекс дня блокировки данных класса
//UGenericMutex* SendMutex;

/// Мьютекс дня блокировки доступа к очереди команд
//mutable boost::mutex CommandMutex;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineControlThread(int engine_index);
virtual ~UEngineControlThread(void);
// --------------------------


// --------------------------
// Управление параметрами
// --------------------------
/// Режим счета
int GetCalculateMode(void) const;
bool SetCalculateMode(int value);

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
int GetMinInterstepsInterval(void) const;
bool SetMinInterstepsInterval(RDK::UTime value);
// --------------------------

// -------------------------
// Методы управления потоком
// -------------------------
virtual void BeforeCalculate(void);

virtual void AfterCalculate(void);

/// Метод, вызываемый потоком
virtual void Execute(void);

double GetRealLastCalculationTime(void) const;
// -------------------------
};

}

#endif
