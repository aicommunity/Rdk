#ifndef UGENERIC_MUTEX_WIN_CPP
#define UGENERIC_MUTEX_WIN_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
//#include <windows.h>

class UGenericMutexQt: public UGenericMutex
{
private:
void* m_UnlockEvent;

public:
UGenericMutexQt();
virtual ~UGenericMutexQt();

virtual bool lock();
virtual bool unlock();
virtual bool wait(int timeout);
};


UGenericMutexQt::UGenericMutexQt()
{
 //m_UnlockEvent = CreateEvent(0, TRUE, TRUE, 0);
}

UGenericMutexQt::~UGenericMutexQt()
{
 //CloseHandle(m_UnlockEvent);
}

bool UGenericMutexQt::lock()
{
 /*if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 ResetEvent(m_UnlockEvent);*/
 return true;
}

bool UGenericMutexQt::unlock()
{
 /*if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 SetEvent(m_UnlockEvent);*/
 return true;
}

bool UGenericMutexQt::wait(int timeout)
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
 return new UGenericMutexQt;
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
