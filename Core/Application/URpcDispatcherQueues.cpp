#ifndef URpcDispatcherQueues_CPP
#define URpcDispatcherQueues_CPP

#include "URpcDispatcherQueues.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/PtzLib/ptzlib_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDispatcherQueues::URpcDispatcherQueues(void)
{

}

URpcDispatcherQueues::~URpcDispatcherQueues(void)
{

}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Добавление команды в очередь на обработку
/// Записывает Id команды в cmd_id
/// в случае неудачи возвращает false
bool URpcDispatcherQueues::PushCommand(const UEPtr<URpcCommand> &command, unsigned &cmd_id)
{
 if(!command)
  return false;
 if(!command->DecodeBasicData())
 {
  // Ошибка декодирования
  return false;
 }

 boost::mutex::scoped_lock lock(CommandQueueMutex);
 CommandQueue.push_back(command);
 cmd_id=command->CmdId;
 return true;
}

/// Добавление команды в очередь на обработку
/// в случае неудачи возвращает false
bool URpcDispatcherQueues::PushCommand(const UEPtr<URpcCommand> &command)
{
 unsigned cmd_id(0);
 return PushCommand(command,cmd_id);
}



/// Возвращает указатель на команду по ее Id в очереди ответов
/// Возвращаемый указатель равен нулю, если команды нет в очереди
UEPtr<URpcCommand> URpcDispatcherQueues::FindProcessedCommand(unsigned cmd_id)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);
 std::list<UEPtr<URpcCommand> >::iterator I=ProcessedCommandQueue.begin();

 for(;I != ProcessedCommandQueue.end();++I)
 {
  if((*I)->CmdId == cmd_id)
   return *I;
 }
 return 0;
}

/// Удаляет команду из очереди ответов по ее идентификатору
/// Возвращает true если команда была в очереди
bool URpcDispatcherQueues::PopProcessedCommand(unsigned cmd_id)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);
 std::list<UEPtr<URpcCommand> >::iterator I=ProcessedCommandQueue.begin();

 for(;I != ProcessedCommandQueue.end();++I)
 {
  if((*I)->CmdId == cmd_id)
  {
   ProcessedCommandQueue.erase(I);
   return true;
  }
 }
 return false;
}

/// Возвращает указатель на самый старый ответ
/// и удаляет его из очереди
UEPtr<URpcCommand> URpcDispatcherQueues::PopProcessedCommand(void)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);
 UEPtr<URpcCommand> result;
 std::list<UEPtr<URpcCommand> >::iterator I=ProcessedCommandQueue.begin();

 if(ProcessedCommandQueue.begin() != ProcessedCommandQueue.end())
 {
  result=*(ProcessedCommandQueue.begin());
  ProcessedCommandQueue.erase(ProcessedCommandQueue.begin());
 }
 return result;
}

/// Возвращает true если очередь ответов не пуста
bool URpcDispatcherQueues::CheckProcessedCommand(void)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);

 if(ProcessedCommandQueue.begin() != ProcessedCommandQueue.end())
  return true;

 return false;
}


/// Очищает очередь
void URpcDispatcherQueues::ClearQueue(void)
{
 boost::mutex::scoped_lock lock(CommandQueueMutex);
 CommandQueue.clear();
}

/// Очищает очередь ожидающих отправки ответов
void URpcDispatcherQueues::ClearProcessedQueue(void)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);
 ProcessedCommandQueue.clear();
}
// --------------------------


// --------------------------
// Вспомогательные методы управления
// --------------------------
/// Возвращает следующую на очереди команду из CommandQueue и удаляет ее из очереди
UEPtr<URpcCommand> URpcDispatcherQueues::PopFromCommandQueue(void)
{
 UEPtr<URpcCommand> result;
 boost::mutex::scoped_lock lock(CommandQueueMutex);
 if(CommandQueue.begin() != CommandQueue.end())
 {
  result=*(CommandQueue.begin());
  CommandQueue.erase(CommandQueue.begin());
 }
 return result;
}

/// Кладет команду в очередь ответов
void URpcDispatcherQueues::PushToProcessedQueue(const UEPtr<URpcCommand> &command)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);
 ProcessedCommandQueue.push_back(command);
}
// --------------------------

}

#endif

