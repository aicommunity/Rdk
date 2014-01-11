#ifndef UGENERIC_MUTEX_GCC_CPP
#define UGENERIC_MUTEX_GCC_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
//#include <windows.h>

class UGenericMutexGcc: public UGenericMutex
{
private:
void* m_UnlockEvent;

public:
UGenericMutexGcc();
virtual ~UGenericMutexGcc();

virtual bool lock();
virtual bool unlock();
virtual bool wait(int timeout);
};


UGenericMutexGcc::UGenericMutexGcc()
{
 //m_UnlockEvent = CreateEvent(0, TRUE, TRUE, 0);
}

UGenericMutexGcc::~UGenericMutexGcc()
{
 //CloseHandle(m_UnlockEvent);
}

bool UGenericMutexGcc::lock()
{
 /*if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 ResetEvent(m_UnlockEvent);*/
 return true;
}

bool UGenericMutexGcc::unlock()
{
 /*if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 SetEvent(m_UnlockEvent);*/
 return true;
}

bool UGenericMutexGcc::wait(int timeout)
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
 return new UGenericMutexGcc;
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

