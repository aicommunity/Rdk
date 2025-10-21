#ifndef URpcDecoder_CPP
#define URpcDecoder_CPP

#include "URpcDecoder.h"
#include "URpcDispatcher.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include <chrono>
#include <thread>


namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
URpcDecoder::URpcDecoder(URpcDispatcher* dispatcher)
 : Dispatcher(dispatcher), ThreadTerminated(false)
{
 DecoderThread=std::thread([this]() { this->Process(); });
}

URpcDecoder::~URpcDecoder(void)
{
 ThreadTerminated=true;
 DecoderThread.join();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// ������������� ������ ���������
void URpcDecoder::SetDispatcher(URpcDispatcher* dispatcher)
{
 if(Dispatcher == dispatcher)
  return;
 std::lock_guard<std::mutex> lock(DispatchMutex);
 Dispatcher=dispatcher;
}

/// ����� �����
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
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	continue;
   }

   std::lock_guard<std::mutex> lock(DispatchMutex);
   if(!ProcessCommand(command))
   {
	// ������ ��������� �������
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

/// ��������� �����
void URpcDecoder::StopProcessThread(void)
{
    ThreadTerminated=true;
    DecoderThread.join();
}

/// ������������ ������������� � ����� ������� �� ������� ������
/// ���������� false ���� ������� �� ��������������
bool URpcDecoder::ProcessCommand(const UEPtr<URpcCommand> &command)
{
 return AProcessCommand(command);
}


/// ���������� ��������� �� ��������� ����������
UEPtr<UApplication> URpcDecoder::GetApplication(void)
{
 if(!Dispatcher)
  return 0;

 return Dispatcher->GetApplication();
}

// --------------------------


}
#endif

