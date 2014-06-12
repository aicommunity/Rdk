#ifndef URpcDispatcher_CPP
#define URpcDispatcher_CPP

#include "URpcDispatcher.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/PtzLib/ptzlib_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDispatcher::URpcDispatcher(void)
{

}

URpcDispatcher::~URpcDispatcher(void)
{

}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Устанавливает декодер
/// Вызывает смену всех текущих прототипов
void URpcDispatcher::SetDecoderPrototype(const UEPtr<URpcDecoder> &decoder)
{
 DecoderPrototype=decoder;
 for(size_t i=0;i<Decoders.size();i++)
 {
  delete Decoders[i];
  Decoders[i]=0;
 }

 if(DecoderPrototype)
  for(size_t i=0;i<Decoders.size();i++)
  {
   Decoders[i]=DecoderPrototype->New();
  }
}

/// Добавление команды в очередь на обработку
/// Записывает Id команды в cmd_id
/// в случае неудачи возвращает false
bool URpcDispatcher::PushCommand(const UEPtr<URpcCommand> &command, unsigned &cmd_id)
{
 if(!command)
  return false;
 if(!command->DecodeBasicData())
  return false;

 CommandQueue.push_back(command);
 cmd_id=command->CmdId;
 return true;
}

/// Возвращает указатель на команду по ее Id в очереди ответов
/// Возвращаемый указатель равен нулю, если команды нет в очереди
UEPtr<URpcCommand> URpcDispatcher::FindProcessedCommand(unsigned cmd_id)
{
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
bool URpcDispatcher::PopCommand(unsigned cmd_id)
{
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



/// Осуществляет диспетчеризацию текущей очереди команд
void URpcDispatcher::Dispatch(void)
{
 std::list<UEPtr<URpcCommand> >::iterator I=CommandQueue.begin();

 for(;I != CommandQueue.end();++I)
 {
  int num_channels=GetNumEngines();
  if(num_channels != Decoders.size())
   UpdateDecoders(num_channels);

  if(!*I)
  {
   // Предупреждение - была создана пустая команда
   ProcessedCommandQueue.push_back(*I);
   continue;
  }
/*
  if(!(*I)->DecodeBasicData())
  {
   // Ошибка декодирования
   ProcessedCommandQueue.push_back(*I);
   continue;
  }
  */
  int channel_index=(*I)->GetChannelIndex();
  if(channel_index<-1 || channel_index>=int(Decoders.size()))
  {
   // Ошибка - некорректный индекс канала
   ProcessedCommandQueue.push_back(*I);
   continue;
  }

  // Заглушка
  if(channel_index<0)
   channel_index=0;

  // Следующий вызов должен быть произведен в своем потоке
  if(!Decoders[channel_index]->ProcessCommand(*I))
  {
   // Ошибка - команда не была обработана
   ProcessedCommandQueue.push_back(*I);
   continue;
  }
  ProcessedCommandQueue.push_back(*I);
 }
 CommandQueue.clear();
}

/// Очищает очередь
void URpcDispatcher::ClearQueue(void)
{
 CommandQueue.clear();
}

/// Очищает очередь ожидающих отправки ответов
void URpcDispatcher::ClearProcessedQueue(void)
{
 ProcessedCommandQueue.clear();
}

/// Приводит в соответствие список декодеров и число каналов
void URpcDispatcher::UpdateDecoders(int num_channels)
{
 size_t old_size=Decoders.size();
 for(size_t i=num_channels;i<old_size;i++)
 {
  delete Decoders[i];
 }

 Decoders.resize(num_channels);
 for(size_t i=old_size;i<Decoders.size();i++)
  Decoders[i]=DecoderPrototype->New();
}

// --------------------------

}

#endif

