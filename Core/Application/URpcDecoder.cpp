#ifndef URpcDecoder_CPP
#define URpcDecoder_CPP

#include "URpcDecoder.h"
#include "URpcDispatcher.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"


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
 int ex_flag=0;
 std::string ex_info;
 while (!ThreadTerminated)
 {
  try
  {
   UEPtr<URpcCommand> command=PopFromCommandQueue();
   if(!command)
   {
	boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	continue;
   }

   boost::mutex::scoped_lock lock(DispatchMutex);
   if(!ProcessCommand(command))
   {
	// ошибка выполения команды
	MLog_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Decoder: Process - ProcessCommand Fail. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command= ")+command->FunctionName).c_str());
   }
   else
	MLog_LogMessage(command->ChannelIndex, RDK_EX_DEBUG, (std::string("RPC Decoder: Process - Processed Command. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command= ")+command->FunctionName).c_str());

   if(Dispatcher)
	Dispatcher->PushToProcessedQueue(command);
//   boost::this_thread::sleep(boost::posix_time::milliseconds(3));
  }
  catch(UException &rdk_ex)
  {
   ex_info=rdk_ex.what();
   ex_flag=1;
  }
  catch(std::exception &std_ex)
  {
   ex_info=std_ex.what();
   ex_flag=1;
  }

  switch(ex_flag)
  {
  case 1:
   MLog_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Decoder: Process - std::exception - ")+ex_info).c_str());
  break;

  case 2:
   MLog_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Decoder: Process - RDK::UException - ")+ex_info).c_str());
  break;
  }
  ex_flag=0;

 }
}

/// Остановка треда
void URpcDecoder::StopProcessThread(void)
{
    ThreadTerminated=true;
    DecoderThread.join();
}

/// Осуществляет декодирование и вызов команды по текущим данным
/// Возвращает false если команда не поддерживается
bool URpcDecoder::ProcessCommand(const UEPtr<URpcCommand> &command)
{
 return AProcessCommand(command);
}


/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> URpcDecoder::GetApplication(void)
{
 if(!Dispatcher)
  return 0;

 return Dispatcher->GetApplication();
}

// --------------------------


}
#endif

