#ifndef UENGINE_CONTROL_THREAD_H
#define UENGINE_CONTROL_THREAD_H

#include <string>
#include <vector>
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include "../../Deploy/Include/rdk.h"

#define RDK_THREAD_PRIORITY_REALTIME 0
#define RDK_THREAD_PRIORITY_HIGH 1
#define RDK_THREAD_PRIORITY_ABOVE_NORMAL 2
#define RDK_THREAD_PRIORITY_NORMAL 3
#define RDK_THREAD_PRIORITY_BELOW_NORMAL 4
#define RDK_THREAD_PRIORITY_IDLE 5

#define RDK_DEFAULT_THREAD_PRIORITY RDK_THREAD_PRIORITY_BELOW_NORMAL

namespace RDK {

class UEngineControl;
class UChannelProfiler;


class RDK_LIB_TYPE UEngineControlThread
{
public:
/// Состояние расчета канала
/// 0 - Выключен
/// 1 - Идет расчет
typedef enum { csStopped=0, csRunning=1 } UCalcState;

protected: // Параметры
/// Индекс канала в библиотеке аналитики, управляемый тредом
int EngineIndex;

/// Режим счета
/// 0 - простой расчет
/// 1 - расчет в реальном времени
/// 2 - простой расчет по сигналу
UELockVar<int> CalculateMode;

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
UELockVar<UTime> MinInterstepsInterval;

/// Источник времени для модели
/// 0 - часы
/// 1 - время внешнего видеоисточника
UELockVar<int> CalculationTimeSource;

/// Таймаут ожидания изменения состояния потока
UELockVar<int> ThreadTimeout;

protected: // Данные
/// Временная метка сервера, устанавливаемая перед очередной итерацией расчета
UELockVar<double> ServerTimeStamp;

/// Временная метка расчета, переданная в модель последний раз
UELockVar<double> CalculationTime;

/// Временная метка сервера, когда был произведен последний расчет
UELockVar<double> LastCalculationServerTimeStamp;

/// Метка реального времени окончания последнего расчета
UELockVar<double> RealLastCalculationTime;

/// Класс-владелец потоков
UEngineControl* EngineControl;

/// Экземпляр обработчика данных производительности
UChannelProfiler* Profiler;

protected: // События
// Событие состояния расчета. Выставлено на время активности расчета. Сбрасывается по стопу
UGenericEvent* CalcState;

UGenericEvent* CalcEnable;

UGenericEvent* CalcStarted;

UGenericEvent* CalculationNotInProgress;

UGenericEvent* CalcWaitOff;

protected: // Потоки
/// Поток расчета
boost::thread Thread;

bool Terminated;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineControlThread(UEngineControl* engine_control, int channel_index);
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

/// Источник времени для модели
/// 0 - часы
/// 1 - время внешнего видеоисточника
int GetCalculationTimeSource(void) const;
bool SetCalculationTimeSource(int value);
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Возвращает время расчета, переданное в модель последний раз
/// в соответствии с режимом CalculationTimeSource
double GetCalculationTime(void) const;

/// Временная метка сервера, когда был произведен последний расчет
double GetServerTimeStamp(void) const;
bool SetServerTimeStamp(double value);

/// Метка внешнего источника времени когда был произведен последний расчет
double GetLastCalculationServerTimeStamp(void) const;

double GetRealLastCalculationTime(void) const;

/// Возвращает класс-владелец потока
virtual UEngineControl* GetEngineControl(void);

/// Экземпляр обработчика данных производительности
UChannelProfiler* GetProfiler(void);
// --------------------------

// -------------------------
// Методы управления потоком
// -------------------------
virtual void BeforeCalculate(void);
virtual void ABeforeCalculate(void);

virtual void AfterCalculate(void);
virtual void AAfterCalculate(void);

/// Метод обеспечивает собственно расчет со всеми предварительными и заверщающими
/// действиями
virtual void Calculate(void);

/// Метод, вызываемый потоком
virtual void Execute(void);

/// Проверяет состояние расчета по id канала
virtual UEngineControlThread::UCalcState CheckCalcState(void) const;

/// Возвращает true когда завершится текущая итерация расчета за заданный timeout
/// Если timeout == -1 То ждет с таймаутом по умолчанию
virtual bool WaitForCalculationComplete(int timeout=-1) const;

/// Включает режим ожидания
/// Снимает состояние события CalcWaitOff и немедленно отдает управление
/// НЕ ожидает окончание итерации расчета
virtual void WaitSyncSignal(void);

/// Выключает режим ожидания
/// Взводит состояние события CalcWaitOff и немедленно отдает управление
virtual void WaitSyncSignalOff(void);

/// Возвращает состояния запуска треда
virtual int IsCalcStarted(void) const;

/// Взводит флаг разрешения расчета
virtual bool EnableCalculation(void);

/// Запускает аналитику канала
virtual void Start(void);

/// Останавливает аналитику канала
virtual void Pause(void);

/// Сбрасывает аналитику канала
virtual void Reset(void);

/// Устанавливает приоритет потока
virtual bool SetPriority(int priority_id);
// -------------------------
};

}

#endif
