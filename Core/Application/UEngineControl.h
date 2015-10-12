#ifndef UENGINE_CONTROL_H
#define UENGINE_CONTROL_H

#include "UEngineControlThread.h"
//#include <boost/asio.hpp>
//#include <boost/chrono.hpp>

namespace RDK {

class UEngineStateThread;
class UEngineControlThread;

class RDK_LIB_TYPE UEngineControl
{
protected: // Параметры
/// Режим работы
/// 0 - однопоточный (одноканальный) режим
/// 1 - многопоточный режим
RDK::UELockVar<int> ThreadMode;

protected: // Данные
/// Потоки запуска многоканальной аналитики
std::vector<UEngineControlThread*> EngineControlThreads;

/// Поток мониторинга состояния расчета
UEngineStateThread *EngineStateThread;

//boost::asio::io_service io;
//boost::asio::deadline_timer Timer(io);

/// Флаг состояния инцициализации
bool InitFlag;

protected:
UGenericEvent* ThreadCalcCompleteEvent;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineControl(void);
virtual ~UEngineControl(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Управление режимом работы
/// 0 - однопоточный режим
/// 1 - многопоточный режим
int GetThreadMode(void) const;
void SetThreadMode(int mode);

// Управление режимом расчетов
int GetCalculateMode(int engine_index) const;
void SetCalculateMode(int engine_index, int value);

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
int GetCalculationTimeSource(int engine_index) const;
bool SetCalculationTimeSource(int engine_index, int value);

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
RDK::UTime GetMinInterstepsInterval(int engine_index) const;
bool SetMinInterstepsInterval(int engine_index, RDK::UTime value);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Доступ к треду мониторинга состояния модулей сервера
UEngineStateThread* GetEngineStateThread(void);

/// Возвращает заданный поток расчета
UEngineControlThread* GetEngineThread(int i);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Инициализация (выполняется первой)
virtual void Init(void);

/// Деинициализация (выполняется последней)
virtual void UnInit(void);

/// Проверка состояния инициализации
bool IsInit(void) const;

/// Создание нового треда расчета
virtual UEngineControlThread* CreateEngineThread(UEngineControl* engine_control, int engine_index);

/// Создание нового треда расчета
virtual UEngineStateThread* CreateEngineStateThread(UEngineControl* engine_control);

/// Управление числом каналов
int GetNumEngines(void) const;
bool SetNumEngines(int num);
bool InsertEngine(int index);
bool DeleteEngine(int index);

/// Запускает аналитику выбранного канала, или всех, если engine_index == -1
virtual void StartEngine(int engine_index);

/// Останавливает аналитику выбранного канала, или всех, если engine_index == -1
virtual void PauseEngine(int engine_index);

/// Сбрасывает аналитику выбранного канала, или всех, если engine_index == -1
virtual void ResetEngine(int engine_index);

/// Делает шаг расчета выбранного канала, или всех, если engine_index == -1
virtual void StepEngine(int engine_index);

/// Проверяет состояние расчета по id канала
/// 0 - Не считает
/// 1 - Идет расчет
virtual int CheckCalcState(int channel_id) const;

/// Вклчает мониторинг сервера
void StartEngineStateThread(void);

/// Останавливает мониторинг сервера
void StopEngineStateThread(void);

void TimerExecture(void);
// --------------------------
};

}

#endif
