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
 while (!ThreadTerminated)
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
   // ������ ��������� �������
  }

  if(Dispatcher)
   Dispatcher->PushToProcessedQueue(command);
  boost::this_thread::sleep(boost::posix_time::milliseconds(3));
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

