#ifndef URpcDispatcher_CPP
#define URpcDispatcher_CPP

#include "URpcDispatcher.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/PtzLib/ptzlib_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
URpcDispatcher::URpcDispatcher(void)
{

}

URpcDispatcher::~URpcDispatcher(void)
{

}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
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

/// ������������ ��������������� ������� ������� ������
void URpcDispatcher::Dispatch(void)
{
//    boost::mutex::scoped_lock lock(SendMutex);
 UEPtr<URpcCommand> command;
 while(command=PopFromCommandQueue())
 {
  if(!command->DecodeBasicData())
  {
   // ������ �������������
   PushToProcessedQueue(command);
   continue;
  }

  UpdateDecoders();
  // ��������� ����� ������ ���� ���������� � ����� ������
  DispatchCommand(command);

//  PushToProcessedQueue(command);
 }
}

/// �������� ������� ����������, ���������� ��������� ���������� � ������� �� �������
bool URpcDispatcher::SyncDispatchCommand(const UEPtr<URpcCommand> &command)
{
 unsigned cmd_id=0;
 if(!PushCommand(command,cmd_id))
  return false;
 Dispatch();

 if(!PopProcessedCommand(cmd_id))
  return false;
 return true;
}
// --------------------------


// --------------------------
// ��������������� ������ ����������
// --------------------------
/// ������������ ����� ���������������� ��������
/// ����� ������ ���������� � ����� ������
void URpcDispatcher::DispatchCommand(const UEPtr<URpcCommand> &command)
{
  int channel_index=command->GetChannelIndex();
  if(channel_index<-1 || channel_index>=int(Decoders.size()))
  {
   // ������ - ������������ ������ ������
  }

  // ��������
  if(channel_index<0)
   channel_index=0;

  unsigned cmd_id=0;
  if(!Decoders[channel_index]->PushCommand(command,cmd_id))
  {
   // ������ - ������� �� ���� ����������
  }
}

/// �������� � ������������ ������ ��������� � ����� �������
void URpcDispatcher::UpdateDecoders(void)
{
 int num_channels=GetNumEngines();
 if(num_channels == Decoders.size())
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

