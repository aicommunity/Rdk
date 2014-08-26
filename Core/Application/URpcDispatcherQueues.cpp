#ifndef URpcDispatcherQueues_CPP
#define URpcDispatcherQueues_CPP

#include "URpcDispatcherQueues.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "../../Deploy/PtzLib/ptzlib_initdll.h"
#include "../../Deploy/Include/rdk_rpc.h"

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
URpcDispatcherQueues::URpcDispatcherQueues(void)
{

}

URpcDispatcherQueues::~URpcDispatcherQueues(void)
{

}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// ���������� ������� � ������� �� ���������
/// ���������� Id ������� � cmd_id
/// � ������ ������� ���������� false
bool URpcDispatcherQueues::PushCommand(const UEPtr<URpcCommand> &command, unsigned &cmd_id)
{
 if(!command)
  return false;
 if(!command->DecodeBasicData())
 {
  // ������ �������������
  return false;
 }

 boost::mutex::scoped_lock lock(CommandQueueMutex);
 CommandQueue.push_back(command);
 cmd_id=command->CmdId;
 return true;
}

/// ���������� ������� � ������� �� ���������
/// � ������ ������� ���������� false
bool URpcDispatcherQueues::PushCommand(const UEPtr<URpcCommand> &command)
{
 unsigned cmd_id(0);
 return PushCommand(command,cmd_id);
}



/// ���������� ��������� �� ������� �� �� Id � ������� �������
/// ������������ ��������� ����� ����, ���� ������� ��� � �������
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

/// ������� ������� �� ������� ������� �� �� ��������������
/// ���������� true ���� ������� ���� � �������
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

/// ���������� ��������� �� ����� ������ �����
/// � ������� ��� �� �������
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

/// ���������� true ���� ������� ������� �� �����
bool URpcDispatcherQueues::CheckProcessedCommand(void)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);

 if(ProcessedCommandQueue.begin() != ProcessedCommandQueue.end())
  return true;

 return false;
}


/// ������� �������
void URpcDispatcherQueues::ClearQueue(void)
{
 boost::mutex::scoped_lock lock(CommandQueueMutex);
 CommandQueue.clear();
}

/// ������� ������� ��������� �������� �������
void URpcDispatcherQueues::ClearProcessedQueue(void)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);
 ProcessedCommandQueue.clear();
}
// --------------------------


// --------------------------
// ��������������� ������ ����������
// --------------------------
/// ���������� ��������� �� ������� ������� �� CommandQueue � ������� �� �� �������
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

/// ������ ������� � ������� �������
void URpcDispatcherQueues::PushToProcessedQueue(const UEPtr<URpcCommand> &command)
{
 boost::mutex::scoped_lock lock(ProcessedCommandQueueMutex);
 ProcessedCommandQueue.push_back(command);
}
// --------------------------

}

#endif

