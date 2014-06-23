#ifndef URpcDecoder_CPP
#define URpcDecoder_CPP

#include "URpcDecoder.h"
#include "URpcDispatcher.h"


namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
URpcDecoder::URpcDecoder(URpcDispatcher* dispatcher)
 : Dispatcher(dispatcher), ThreadTerminated(false)
{
 DecoderThread=boost::thread(boost::bind(&URpcDecoder::Process, boost::ref(*this)));
}

URpcDecoder::~URpcDecoder(void)
{
 ThreadTerminated=true;
 DecoderThread.join();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Устанавливает нового владельца
void URpcDecoder::SetDispatcher(URpcDispatcher* dispatcher)
{
 if(Dispatcher == dispatcher)
  return;
 boost::mutex::scoped_lock lock(DispatchMutex);
 Dispatcher=dispatcher;
}

/// Метод треда
void URpcDecoder::Process(void)
{
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
   if(!ProcessCommand(command))
   {
	// ошибка выполения команды
	MEngine_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Decoder: Process - ProcessCommand Fail. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command= ")+command->FunctionName).c_str());
   }

   if(Dispatcher)
	Dispatcher->PushToProcessedQueue(command);
   boost::this_thread::sleep(boost::posix_time::milliseconds(3));
  }
  catch(std::exception &std_ex)
  {
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Decoder: Process - std::exception - ")+std_ex.what()).c_str());
  }
  catch(UException &rdk_ex)
  {
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Decoder: Process - RDK::UException - ")+rdk_ex.CreateLogMessage()).c_str());
  }
 }
}

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
bool URpcDecoder::ProcessCommand(const UEPtr<URpcCommand> &command)
{
 return AProcessCommand(command);
}
// --------------------------


}
#endif

