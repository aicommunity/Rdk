#ifndef URpcDispatcher_H
#define URpcDispatcher_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDecoder.h"

namespace RDK {

/// Реализует многопоточный вызов каналов для выполения команд
class URpcDispatcher
{
protected:
/// Очередь команд, ожидающих обработки
std::list<UEPtr<URpcCommand> > CommandQueue;

/// Очередь ответов на команды, ожидающих отправки
std::list<UEPtr<URpcCommand> > ProcessedCommandQueue;

/// Массив декодеров, соответствующих каналам
std::vector<UEPtr<URpcDecoder> > Decoders;

protected:
/// Прототип декодера
UEPtr<URpcDecoder> DecoderPrototype;


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
/// Устанавливает прототип декодера
/// Вызывает смену всех текущих прототипов
virtual void SetDecoderPrototype(const UEPtr<URpcDecoder> &decoder);

/// Добавление команды в очередь на обработку
/// Записывает Id команды в cmd_id
/// в случае неудачи возвращает false
virtual bool PushCommand(const UEPtr<URpcCommand> &command, unsigned &cmd_id);

/// Возвращает указатель на команду по ее Id в очереди ответов
/// Возвращаемый указатель равен нулю, если команды нет в очереди
virtual UEPtr<URpcCommand> FindProcessedCommand(unsigned cmd_id);

/// Удаляет команду из очереди ответов по ее идентификатору
/// Возвращает true если команда была в очереди
virtual bool PopCommand(unsigned cmd_id);

/// Осуществляет диспетчеризацию текущей очереди команд
virtual void Dispatch(void);

/// Очищает очередь
virtual void ClearQueue(void);

/// Очищает очередь ожидающих отправки ответов
virtual void ClearProcessedQueue(void);

/// Приводит в соответствие список декодеров и число каналов
virtual void UpdateDecoders(int num_channels);
// --------------------------

};

}

#endif

