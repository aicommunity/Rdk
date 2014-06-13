#ifndef URpcDispatcherQueues_H
#define URpcDispatcherQueues_H

#include "UProject.h"
#include "URpcCommand.h"

#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>


namespace RDK {

/// Реализует управление очередями команд
class URpcDispatcherQueues
{
protected:
/// Очередь команд, ожидающих обработки
std::list<UEPtr<URpcCommand> > CommandQueue;

/// Очередь ответов на команды, ожидающих отправки
std::list<UEPtr<URpcCommand> > ProcessedCommandQueue;

protected: // Потоки
/// Мьютекс дня блокировки доступа к очереди команд
boost::mutex CommandQueueMutex;

/// Мьютекс дня блокировки доступа к очереди ответов
boost::mutex ProcessedCommandQueueMutex;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDispatcherQueues(void);
virtual ~URpcDispatcherQueues(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Добавление команды в очередь на обработку
/// Записывает Id команды в cmd_id
/// в случае неудачи возвращает false
virtual bool PushCommand(const UEPtr<URpcCommand> &command, unsigned &cmd_id);

/// Добавление команды в очередь на обработку
/// в случае неудачи возвращает false
virtual bool PushCommand(const UEPtr<URpcCommand> &command);

/// Возвращает указатель на команду по ее Id в очереди ответов
/// Возвращаемый указатель равен нулю, если команды нет в очереди
virtual UEPtr<URpcCommand> FindProcessedCommand(unsigned cmd_id);

/// Удаляет команду из очереди ответов по ее идентификатору
/// Возвращает true если команда была в очереди
virtual bool PopProcessedCommand(unsigned cmd_id);

/// Возвращает указатель на самый старый ответ
/// и удаляет его из очереди
virtual UEPtr<URpcCommand> PopProcessedCommand(void);

/// Возвращает true если очередь ответов не пуста
virtual bool CheckProcessedCommand(void);

/// Очищает очередь
virtual void ClearQueue(void);

/// Очищает очередь ожидающих отправки ответов
virtual void ClearProcessedQueue(void);
// --------------------------

// --------------------------
// Вспомогательные методы управления
// --------------------------
public:
/// Возвращает следующую на очереди команду из CommandQueue и удаляет ее из очереди
UEPtr<URpcCommand> PopFromCommandQueue(void);

/// Кладет команду в очередь ответов
void PushToProcessedQueue(const UEPtr<URpcCommand> &command);
// --------------------------

};

}

#endif

