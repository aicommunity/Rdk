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

/// ���������� ������� � ������� �� ���������
/// ���������� Id ������� � cmd_id
/// � ������ ������� ���������� false
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

/// ���������� ��������� �� ������� �� �� Id � ������� �������
/// ������������ ��������� ����� ����, ���� ������� ��� � �������
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

/// ������� ������� �� ������� ������� �� �� ��������������
/// ���������� true ���� ������� ���� � �������
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



/// ������������ ��������������� ������� ������� ������
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
   // �������������� - ���� ������� ������ �������
   ProcessedCommandQueue.push_back(*I);
   continue;
  }
/*
  if(!(*I)->DecodeBasicData())
  {
   // ������ �������������
   ProcessedCommandQueue.push_back(*I);
   continue;
  }
  */
  int channel_index=(*I)->GetChannelIndex();
  if(channel_index<-1 || channel_index>=int(Decoders.size()))
  {
   // ������ - ������������ ������ ������
   ProcessedCommandQueue.push_back(*I);
   continue;
  }

  // ��������
  if(channel_index<0)
   channel_index=0;

  // ��������� ����� ������ ���� ���������� � ����� ������
  if(!Decoders[channel_index]->ProcessCommand(*I))
  {
   // ������ - ������� �� ���� ����������
   ProcessedCommandQueue.push_back(*I);
   continue;
  }
  ProcessedCommandQueue.push_back(*I);
 }
 CommandQueue.clear();
}

/// ������� �������
void URpcDispatcher::ClearQueue(void)
{
 CommandQueue.clear();
}

/// ������� ������� ��������� �������� �������
void URpcDispatcher::ClearProcessedQueue(void)
{
 ProcessedCommandQueue.clear();
}

/// �������� � ������������ ������ ��������� � ����� �������
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

