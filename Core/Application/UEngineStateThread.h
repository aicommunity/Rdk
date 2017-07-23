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
class UEngineControl;

class RDK_LIB_TYPE UEngineStateThread
{
public:
/// Состояние расчета канала
/// 0 - Выключен
/// 1 - Идет расчет
/// 2 - Завис
/// 4 - Состояние не определено
enum UCalcState { csRunning=0, csStopped=1, csHanging=2, csUnknown=4 };

protected: // Данные состояния модулей
/// Состояние тредов расчета
/// 0 - запущен
/// 1 - расчет остановлен
/// 2 - расчет запущен, но не выполняется
RDK::UELockVar<std::vector<UCalcState> > CalcThreadStates;

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
std::vector<std::list<double> > AvgIterations;

/// Владелец потока
UEngineControl* EngineControl;

protected: // Данные логгирования
/// Экземпляр класса логирования
ULogger Logger;
/// Файл для сохранения логов
//RDK::UEPtr<std::ofstream> EventsLogFile;

/// Путь до файла логов
//RDK::UELockVar<std::string> EventsLogFilePath;

/// Флаг сохранения в лог данных
//bool EventsLogFlag;

//std::list<std::string> UnsentLog;

/// Временная переменная в которой хранится весь еще не отображенный в интерфейсе лог
/// Очищается каждый раз при запросе этой переменной
std::list<std::string> GuiUnsentLog;

public:
// Событие состояния расчета. Выставлено на время активности расчета. Сбрасывается по стопу
UGenericEvent* CalcState;

UGenericEvent* CalcEnable;

UGenericEvent* CalcStarted;

UGenericEvent* CalculationNotInProgress;

//UGenericMutex* ProcessLogMutex;

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
UEngineStateThread(UEngineControl* engine_control);
virtual ~UEngineStateThread(void);
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Путь до папки с логами
//std::string GetLogDir(void) const;
//bool SetLogDir(const std::string& value);

/// Флаг разрешения логгирования
//bool GetLogFlag(void) const;
//bool SetLogFlag(bool value);
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний тредов
std::vector<UEngineStateThread::UCalcState> ReadCalcThreadStates(void) const;

/// Возвращает состояние одного потока
UEngineStateThread::UCalcState ReadCalcThreadState(int channel_index);
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
/// Возвращает класс-владелец потока
virtual UEngineControl* GetEngineControl(void);

/// Регистрация потока расчета
void RegisterCalcThread(int index, UEngineControlThread *calc_thread);
void UnRegisterCalcThread(int index);

virtual void Execute(void);
virtual void AdditionExecute(void);

/// Функция обеспечивает закрытие текущего файла логов и создание нового
//void RecreateEventsLogFile(void);

/// Закрывает текущий лог
//void CloseEventsLogFile(void);

/// Временная переменная в которой хранится весь еще не отображенный в интерфейсе лог
/// Очищается каждый раз при запросе этой переменной
std::list<std::string> ReadGuiUnsentLog(void);

/// Прерывает исполнение потока
virtual void Terminate(void);

// Общедоступные данные логгирования
static UGenericMutex*& GetRdkExceptionHandlerMutex(void);
static std::list<int>& GetUnsentLogChannelIndexes(void);
// --------------------------

// --------------------------
// Вспомогательные методы
// --------------------------
void ProcessLog(void);
// --------------------------

};

}

#endif
