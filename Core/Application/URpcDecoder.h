#ifndef URpcDecoder_H
#define URpcDecoder_H

#include "UProject.h"
#include "URpcCommand.h"
#include "URpcDispatcherQueues.h"

namespace RDK {

class URpcDispatcher;
class UApplication;

/// Осуществляет декодирование входных данных вызова RPC и одновременно
/// реализует соответствующе вызов
/// На каждый канал аналитики и канал управления приложением должен использоваться
/// отдельный декодер
class RDK_LIB_TYPE URpcDecoder: public URpcDispatcherQueues
{
protected: // Владелец этого декодера
URpcDispatcher* Dispatcher;

protected: // Потоки
/// Мьютекс дня блокировки данных класса
boost::mutex DispatchMutex;

boost::thread DecoderThread;

bool ThreadTerminated;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoder(URpcDispatcher* dispatcher=0);
virtual ~URpcDecoder(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Устанавливает нового владельца
virtual void SetDispatcher(URpcDispatcher* dispatcher);

/// Создает копию этого декодера
virtual URpcDecoder* New(void)=0;

/// Проверяет, поддерживается ли команда диспетчером
/// ожидает, что команда уже декодирована иначе всегда возвращает false
virtual bool IsCmdSupported(const UEPtr<URpcCommand> &command) const=0;

/// Метод треда
virtual void Process(void);

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
virtual bool ProcessCommand(const UEPtr<URpcCommand> &command);
virtual bool AProcessCommand(const UEPtr<URpcCommand> &command)=0;

/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> GetApplication(void);
// --------------------------
};

}

#endif

