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
 DispatcherThread=boost::thread(boost::bind(&URpcDispatcher::Dispatch, boost::ref(*this)));
}

URpcDispatcher::~URpcDispatcher(void)
{
 ThreadTerminated=true;
 DispatcherThread.join();
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

/// Осуществляет диспетчеризацию текущей очереди команд
void URpcDispatcher::Dispatch(void)
{
 int ex_flag=0;
 std::string ex_info;
 while (!ThreadTerminated)
 {
  try
  {
   UEPtr<URpcCommand> command=PopFromCommandQueue();

   if(!command)
   {
	boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	continue;
   }

   boost::mutex::scoped_lock lock(DispatchMutex);

//   UpdateDecoders();
   DispatchCommand(command);
  }
  catch(std::exception &std_ex)
  {
   ex_info=std_ex.what();
   ex_flag=1;
  }
  catch(UException &rdk_ex)
  {
   ex_info=rdk_ex.CreateLogMessage();
   ex_flag=1;
  }

  switch(ex_flag)
  {
  case 1:
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - std::exception - ")+ex_info).c_str());
  break;

  case 2:
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - RDK::UException - ")+ex_info).c_str());
  break;
  }
  ex_flag=0;
 }
}

/// Передает команду диспетчеру, дожидается окончания выполнения и удаляет из очереди
bool URpcDispatcher::SyncDispatchCommand(const UEPtr<URpcCommand> &command, unsigned timeout)
{
 unsigned cmd_id=0;
 if(!PushCommand(command,cmd_id))
  return false;

 unsigned long long start_waiting_time=GetCurrentStartupTime();
 while(CalcDiffTime(GetCurrentStartupTime(),start_waiting_time)<timeout)
 {
  if(!PopProcessedCommand(cmd_id))
  {
   boost::this_thread::sleep(boost::posix_time::milliseconds(10));
   continue;
  }
  else
   return true;
 }
 MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Dispatcher: SyncDispatchCommand - Processing Wait Timeout")+RDK::sntoa(cmd_id)).c_str());
 return false;
}
// --------------------------


// --------------------------
// Вспомогательные методы управления
// --------------------------
/// Осуществляет вызов соответствующего декодера
/// Метод должен вызываться в своем потоке
void URpcDispatcher::DispatchCommand(const UEPtr<URpcCommand> &command)
{
  if(!command->DecodeBasicData())
  {
   // Ошибка декодирования
   MEngine_LogMessage(0, RDK_EX_WARNING, "RPC Dispatcher: DecodeBasicData Fail.");
   PushToProcessedQueue(command);
   return;
  }

  int channel_index=command->GetChannelIndex();
  if(channel_index<-1 || channel_index>=int(Decoders.size()))
  {
   // Ошибка - некорректный индекс канала
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - Incorrect channel index.")+sntoa(channel_index)).c_str());
   PushToProcessedQueue(command);
   return;
  }

  // Заглушка
  if(channel_index<0)
   channel_index=0;

  unsigned cmd_id=0;
  if(!Decoders[channel_index]->PushCommand(command,cmd_id))
  {
   // Ошибка постановки команды в очередь на обработку
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - PushCommand Failed")+RDK::sntoa(cmd_id)).c_str());
  }
}

/// Приводит в соответствие список декодеров и число каналов
void URpcDispatcher::UpdateDecoders(void)
{
 int num_channels=GetNumEngines(); //TODO необходимо убрать!
 if(num_channels == int(Decoders.size()))
  return;

 size_t old_size=Decoders.size();
 for(size_t i=num_channels;i<old_size;i++)
 {
  delete Decoders[i];
 }

 Decoders.resize(num_channels);
 for(size_t i=old_size;i<Decoders.size();i++)
 {
  Decoders[i]=DecoderPrototype->New();
  Decoders[i]->SetDispatcher(this);
 }
}
// --------------------------

}

#endif

