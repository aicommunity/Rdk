#ifndef UGENERIC_MUTEX_WIN_CPP
#define UGENERIC_MUTEX_WIN_CPP
// ---------------------------------------------------------------------------

#pragma hdrstop

#include "../UGenericMutex.h"
#include <windows.h>

class UGenericMutexWin: public UGenericMutex
{
private:
void* m_UnlockEvent;

public:
UGenericMutexWin();
virtual ~UGenericMutexWin();

virtual bool lock();
virtual bool unlock();
virtual bool wait(int timeout);
};


UGenericMutexWin::UGenericMutexWin()
{
 m_UnlockEvent = CreateEvent(0, TRUE, TRUE, 0);
}

UGenericMutexWin::~UGenericMutexWin()
{
 CloseHandle(m_UnlockEvent);
}

bool UGenericMutexWin::lock()
{
 if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 ResetEvent(m_UnlockEvent);
 return true;
}

bool UGenericMutexWin::unlock()
{
 if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
  return false;
 SetEvent(m_UnlockEvent);
 return true;
}

bool UGenericMutexWin::wait(int timeout)
{
 if (WaitForSingleObject(m_UnlockEvent, timeout) != WAIT_TIMEOUT)
 {
  return true;
 }
 return false;
}
// ---------------------------------------------------------------------------
UGenericMutex* UCreateMutex(void)
{
 return new UGenericMutexWin;
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
