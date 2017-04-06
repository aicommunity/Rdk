#ifndef UENGINE_CONTROL_H
#define UENGINE_CONTROL_H

#include "UEngineControlThread.h"
#include "UIVisualController.h"
//#include <boost/asio.hpp>
//#include <boost/chrono.hpp>

namespace RDK {

class UEngineStateThread;
class UEngineControlThread;
class UBroadcasterInterface;
class UChannelProfiler;
class UApplication;

class RDK_LIB_TYPE UEngineControl: public UAppController
{
friend class UApplication;

public:
/// Состояние расчета канала
/// 0 - Выключен
/// 1 - Идет расчет
/// 2 - Завис
/// 4 - Состояние не определено
enum UCalcState { csStopped=0, csRunning=1, csHanging=2, csUnknown=4 };

protected: // Параметры
/// Режим работы
/// 0 - однопоточный (одноканальный) режим
/// 1 - многопоточный режим
RDK::UELockVar<int> ThreadMode;

/// Режим вызова контроллеров
/// 0 - при обновлении интерфейса
/// 1 - индивидуально для каждого канала во время каждого расчета канала
RDK::UELockVar<int> UseControllersMode;

/// Время обновления интерфейсов, в мс.
/// По умолчанию равно 100, может меняться методами get/set
RDK::UELockVar<int> UpdateInterval;

protected: // Данные
/// Указатель на экземпляр приложения
UEPtr<UApplication> Application;

/// Потоки запуска многоканальной аналитики
std::vector<UEngineControlThread*> EngineControlThreads;

/// Поток мониторинга состояния расчета
UEngineStateThread *EngineStateThread;

std::vector<RDK::UEPtr<UBroadcasterInterface> > BroadcastersList;

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

/// Режим вызова контроллеров
/// 0 - при обновлении интерфейса
/// 1 - индивидуально для каждого канала во время каждого расчета канала
int GetUseControllersMode(void) const;
void SetUseControllersMode(int value);

// Управление режимом расчетов
int GetCalculateMode(int channel_index) const;
void SetCalculateMode(int channel_index, int value);

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
int GetCalculationTimeSource(int channel_index) const;
bool SetCalculationTimeSource(int channel_index, int value);

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
RDK::UTime GetMinInterstepsInterval(int channel_index) const;
bool SetMinInterstepsInterval(int channel_index, RDK::UTime value);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> GetApplication(void);
bool SetApplication(UEPtr<UApplication> value);

/// Доступ к треду мониторинга состояния модулей сервера
UEngineStateThread* GetEngineStateThread(void);

/// Возвращает заданный поток расчета
UEngineControlThread* GetEngineThread(int i);

/// Возвращает данные о производительности канала
UChannelProfiler* GetChannelProfiler(int i);
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
virtual UEngineControlThread* CreateEngineThread(UEngineControl* engine_control, int channel_index);

/// Создание нового треда расчета
virtual UEngineStateThread* CreateEngineStateThread(UEngineControl* engine_control);

// Управление временной меткой сервера
double GetServerTimeStamp(int channel_index) const;
void SetServerTimeStamp(int channel_index, double stamp);

/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
virtual void StartChannel(int channel_index);

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
virtual void PauseChannel(int channel_index);

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
virtual void ResetChannel(int channel_index);

/// Делает шаг расчета выбранного канала, или всех, если channel_index == -1
virtual void StepChannel(int channel_index);

/// Проверяет состояние расчета по id канала
virtual UEngineControl::UCalcState CheckCalcState(int channel_id) const;

/// Вклчает мониторинг сервера
void StartEngineStateThread(void);

/// Останавливает мониторинг сервера
void StopEngineStateThread(void);

void TimerExecute(void);

/// Регистрирует вещатель метаданных
void RegisterMetadataBroadcaster(UBroadcasterInterface *broadcaster);

/// Снимает регистрацию вещателя метаданных
void UnRegisterMetadataBroadcaster(UBroadcasterInterface *broadcaster);

/// Отправляет метаданные во все зарегистрированные вещатели
virtual bool AddMetadata(int channel_index, double time_stamp);

/// Инициирует процедуру отправки метаданных всеми зарегистрированными вещателями
virtual bool SendMetadata(void);
// --------------------------

// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
virtual void SaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
virtual void LoadParameters(RDK::USerStorageXML &xml);
// --------------------------

///Установка значения инетрвала обновления в мс.
int getUpdateInterval() const;

///Получение значения инетрвала обновления в мс.
void setUpdateInterval(int value);

private: // Вспомогательные методы
// --------------------------
/// Управление числом каналов
// --------------------------
virtual int GetNumChannels(void) const;
virtual bool SetNumChannels(int num);
virtual bool InsertChannel(int index);
virtual bool DeleteChannel(int index);
// --------------------------


};

}

#endif
