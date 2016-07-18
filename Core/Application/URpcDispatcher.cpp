#ifndef URpcDispatcher_CPP
#define URpcDispatcher_CPP

#include "URpcDispatcher.h"
#include "UApplication.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
//#include "../../Deploy/PtzLib/ptzlib_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"

namespace RDK {

// --------------------------
// ������������ � �����������
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
// ������ ����������
// --------------------------
/// ���������, �������������� �� �������� ������� �����������
/// ������� �������������� �������, ����� ������ false
bool URpcDispatcher::IsCmdSupported(const UEPtr<URpcCommand> &command) const
{
 if(!command || !command->IsDecoded)
  return false;

 int channel_index=command->GetChannelIndex();
 if(channel_index<-1 || channel_index>=int(Decoders.size()))
  return false;

 // ��������
 if(channel_index<0)
  channel_index=0;

 return Decoders[channel_index]->IsCmdSupported(command);
}

/// ������������� �������
/// �������� ����� ���� ������� ����������
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

/// ���������� � ������������� ������� ������� �������
UEPtr<URpcDecoder> URpcDispatcher::GetCommonDecoder(void)
{
 return CommonDecoder;
}

bool URpcDispatcher::SetCommonDecoder(const UEPtr<URpcDecoder> &decoder)
{
 if(CommonDecoder == decoder)
  return true;
 if(CommonDecoder)
  CommonDecoder->SetDispatcher(0);

 CommonDecoder=decoder;
 CommonDecoder->SetDispatcher(this);
 return true;
}

/// ��������� ����������
UEPtr<UApplication> URpcDispatcher::GetApplication(void)
{
 return Application;
}

bool URpcDispatcher::SetApplication(UEPtr<UApplication> application)
{
 if(Application == application)
  return true;

 Application=application;
 return true;
}

/// ������������ ��������������� ������� ������� ������
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
	boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	continue;
   }

   boost::mutex::scoped_lock lock(DispatchMutex);

//   UpdateDecoders();
   DispatchCommand(command);
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
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - std::exception - ")+ex_info).c_str());
  break;

  case 2:
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - RDK::UException - ")+ex_info).c_str());
  break;
  }
  ex_flag=0;
 }
}

/// �������� ������� ����������, ���������� ��������� ���������� � ������� �� �������
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
   boost::this_thread::sleep(boost::posix_time::milliseconds(1));
   continue;
  }
  else
   return true;
 }
 MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, (std::string("RPC Dispatcher: SyncDispatchCommand - Processing Wait Timeout")+RDK::sntoa(cmd_id)).c_str());
 return false;
}
// --------------------------


// --------------------------
// ��������������� ������ ����������
// --------------------------
/// ������������ ����� ���������������� ��������
/// ����� ������ ���������� � ����� ������
void URpcDispatcher::DispatchCommand(const UEPtr<URpcCommand> &command)
{
  if(!command->DecodeBasicData())
  {
   // ������ �������������
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, "RPC Dispatcher: DecodeBasicData Fail.");
   PushToProcessedQueue(command);
   return;
  }

  int channel_index=command->GetChannelIndex();
  if(channel_index<-1 || channel_index>=int(Decoders.size()))
  {
   // ������ - ������������ ������ ������
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - Incorrect channel index.")+sntoa(channel_index)).c_str());
   PushToProcessedQueue(command);
   return;
  }

  // TODO: ���� ������������� ��� � ������� ��������� IsCmdSupported
  if(channel_index<0)
  {
   if(!CommonDecoder)
   {
	// ������ - �� ����� ������� ������� �������
	MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, (std::string("RPC Dispatcher: Common decored don't set")).c_str());
	return;
   }

   unsigned common_cmd_id=0;
   if(CommonDecoder->IsCmdSupported(command))
   {
	if(!CommonDecoder->PushCommand(command,common_cmd_id))
	{
	 // ������ ���������� ������� � ������� �� ���������
	 MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - PushCommand to common decoder failed")+RDK::sntoa(common_cmd_id)).c_str());
	}
   return;
   }
   else
    channel_index=0; // TODO: ��������. �� ������� ����� �� ���
  }

  unsigned cmd_id=0;
  if(!Decoders[channel_index]->PushCommand(command,cmd_id))
  {
   // ������ ���������� ������� � ������� �� ���������
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_WARNING, (std::string("RPC Dispatcher: DispatchCommand - PushCommand Failed")+RDK::sntoa(cmd_id)).c_str());
  }
}

/// �������� � ������������ ������ ��������� � ����� �������
void URpcDispatcher::UpdateDecoders(void)
{
 int num_channels=GetNumEngines(); //TODO ���������� ������!
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

