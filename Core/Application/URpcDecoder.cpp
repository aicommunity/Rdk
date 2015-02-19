#ifndef URpcDecoder_CPP
#define URpcDecoder_CPP

#include "URpcDecoder.h"
#include "URpcDispatcher.h"


namespace RDK {

// --------------------------
// ������������ � �����������
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
// ������ ����������
// --------------------------
/// ������������� ������ ���������
void URpcDecoder::SetDispatcher(URpcDispatcher* dispatcher)
{
 if(Dispatcher == dispatcher)
  return;
 boost::mutex::scoped_lock lock(DispatchMutex);
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
	boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	continue;
   }

   boost::mutex::scoped_lock lock(DispatchMutex);
   if(!ProcessCommand(command))
   {
	// ������ ��������� �������
	MEngine_LogMessage(command->ChannelIndex, RDK_EX_WARNING, (std::string("RPC Decoder: Process - ProcessCommand Fail. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command= ")+command->FunctionName).c_str());
   }
   else
	MEngine_LogMessage(command->ChannelIndex, RDK_EX_DEBUG, (std::string("RPC Decoder: Process - Processed Command. CmdId=")+sntoa(command->GetCmdId())+std::string(" Command= ")+command->FunctionName).c_str());

   if(Dispatcher)
	Dispatcher->PushToProcessedQueue(command);
//   boost::this_thread::sleep(boost::posix_time::milliseconds(3));
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
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Decoder: Process - std::exception - ")+ex_info).c_str());
  break;

  case 2:
   MEngine_LogMessage(0, RDK_EX_WARNING, (std::string("RPC Decoder: Process - RDK::UException - ")+ex_info).c_str());
  break;
  }
  ex_flag=0;

 }
}

/// ������������ ������������� � ����� ������� �� ������� ������
/// ���������� false ���� ������� �� ��������������
bool URpcDecoder::ProcessCommand(const UEPtr<URpcCommand> &command)
{
 return AProcessCommand(command);
}
// --------------------------


}
#endif

