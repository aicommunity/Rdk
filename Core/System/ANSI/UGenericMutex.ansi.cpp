#ifndef UGENERIC_MUTEX_ANSI_CPP
#define UGENERIC_MUTEX_ANSI_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
//#include <windows.h>

class UGenericMutexAnsi: public UGenericMutex
{
private:
void* m_UnlockEvent;

public:
UGenericMutexAnsi();
virtual ~UGenericMutexAnsi();

virtual bool lock();
virtual bool unlock();
virtual bool wait(int timeout);
};


UGenericMutexAnsi::UGenericMutexAnsi()
{
 //m_UnlockEvent = CreateEvent(0, TRUE, TRUE, 0);
}

UGenericMutexAnsi::~UGenericMutexAnsi()
{
 //CloseHandle(m_UnlockEvent);
}

bool UGenericMutexAnsi::lock()
{
 /*if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 ResetEvent(m_UnlockEvent);*/
 return true;
}

bool UGenericMutexAnsi::unlock()
{
 /*if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 SetEvent(m_UnlockEvent);*/
 return true;
}

bool UGenericMutexAnsi::wait(int timeout)
{
 /*if (WaitForSingleObject(m_UnlockEvent, timeout) != WAIT_TIMEOUT)
 {
  return true;
 } */
 return false;
}
// ---------------------------------------------------------------------------
UGenericMutex* UCreateMutex(void)
{
 return new UGenericMutexAnsi;
}

void UDestroyMutex(UGenericMutex* mutex)
{
 if(mutex)
  delete mutex;
}

UGenericMutexLocker::UGenericMutexLocker(UGenericMutex *m)
{
 if(m)
 {
  m_mutex = m;
  m_mutex->lock();
 }
 else
  m_mutex = 0;
}

UGenericMutexLocker::~UGenericMutexLocker()
{
 if(m_mutex)
  m_mutex->unlock();
}
#endif
