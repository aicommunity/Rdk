#ifndef URpcDispatcher_H
#define URpcDispatcher_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDecoder.h"
#include "URpcDispatcherQueues.h"

#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>


namespace RDK {

/// Реализует многопоточный вызов каналов для выполения команд
class RDK_LIB_TYPE URpcDispatcher: public URpcDispatcherQueues
{
protected:
/// Массив декодеров, соответствующих каналам
std::vector<UEPtr<URpcDecoder> > Decoders;

protected:
/// Прототип декодера
UEPtr<URpcDecoder> DecoderPrototype;

protected: // Потоки
/// Мьютекс дня блокировки данных класса
boost::mutex DispatchMutex;

boost::thread DispatcherThread;

bool ThreadTerminated;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDispatcher(void);
virtual ~URpcDispatcher(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Проверяет, поддерживается ли заданная команда диспетчером
/// ожидает декодированную команду, иначе вернет false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const;

/// Устанавливает прототип декодера
/// Вызывает смену всех текущих прототипов
virtual void SetDecoderPrototype(const UEPtr<URpcDecoder> &decoder);

/// Осуществляет диспетчеризацию текущей очереди команд
virtual void Dispatch(void);

/// Передает команду диспетчеру, дожидается окончания выполнения и удаляет из очереди
virtual bool SyncDispatchCommand(const UEPtr<URpcCommand> &command, unsigned timeout);
// --------------------------

// --------------------------
// Вспомогательные методы управления
// --------------------------
protected:
/// Осуществляет вызов соответствующего декодера
/// Метод должен вызываться в своем потоке
virtual void DispatchCommand(const UEPtr<URpcCommand> &command);

public:
/// Приводит в соответствие список декодеров и число каналов
virtual void UpdateDecoders(void);
// --------------------------

};

}

#endif

