#ifndef UENGINE_STATE_THREAD_H
#define UENGINE_STATE_THREAD_H

#include <string>
#include <vector>
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include "../../Deploy/Include/rdk.h"


namespace RDK {

class UEngineControlThread;

class RDK_LIB_TYPE UEngineStateThread
{
protected: // Параметры

protected: // Данные состояния модулей
/// Состояние тредов расчета
/// 0 - запущен
/// 1 - расчет остановлен
/// 2 - расчет запущен, но не выполняется
RDK::UELockVar<std::vector<int> > CalcThreadStates;

protected: // Внутренние данные
/// Последние моменты времени опроса состояния тредов расчета
std::vector<double> CalcThreadStateTime;

/// Последние моменты времени успешного расчета
std::vector<RDK::ULongTime> CalcThreadSuccessTime;

/// Длина очереди для расчета среднего времени обработки канала
/// (число итераций для усреднения)
int NumAvgIterations;

/// Число раз, в которое время расчета должно превысить среднее время расчета
/// чтобы считать, что произошел останов
double AvgThreshold;

/// История последних моментов времени успешного расчета
std::vector<std::vector<double> > AvgIterations;

public:
// Событие состояния расчета. Выставлено на время активности расчета. Сбрасывается по стопу
UGenericEvent* CalcState;

UGenericEvent* CalcEnable;

UGenericEvent* CalcStarted;

UGenericEvent* CalculationNotInProgress;

protected: // Потоки
/// Потоки расчета
std::vector<UEngineControlThread*> CalcThreads;

/// Поток расчета
boost::thread Thread;

bool Terminated;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineStateThread(void);
virtual ~UEngineStateThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний тредов
std::vector<int> ReadCalcThreadStates(void) const;
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
/// Регистрация потока расчета
void RegisterCalcThread(int index, UEngineControlThread *calc_thread);
void UnRegisterCalcThread(int index);

virtual void Execute(void);
virtual void AdditionExecute(void);
// --------------------------
};

}

#endif
