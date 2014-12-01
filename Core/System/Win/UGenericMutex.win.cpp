#ifndef UGENERIC_MUTEX_WIN_CPP
#define UGENERIC_MUTEX_WIN_CPP
// ---------------------------------------------------------------------------

#include <windows.h>
#include "../UGenericMutex.h"
#include "../../Deploy/Include/initdll_defs.h"

class RDK_LIB_TYPE UGenericMutexWin: public UGenericMutex
{
private:
void* m_UnlockEvent;

DWORD Pid;

public:
UGenericMutexWin();
virtual ~UGenericMutexWin();

virtual bool lock(int lock_id=-1);
virtual bool unlock();
virtual bool wait(int timeout);

private:
UGenericMutexWin(const UGenericMutexWin &copy);
UGenericMutexWin& operator = (const UGenericMutexWin &copy);

};


UGenericMutexWin::UGenericMutexWin()
 : Pid(0)
{
 m_UnlockEvent = CreateEvent(0, TRUE, TRUE, 0);
}

UGenericMutexWin::~UGenericMutexWin()
{
 CloseHandle(m_UnlockEvent);
}

bool UGenericMutexWin::lock(int lock_id)
{
 if(!m_UnlockEvent)
  return false;
 if (WaitForSingleObject(m_UnlockEvent, 0) != WAIT_TIMEOUT)
 {
  if(Pid == GetCurrentProcessId())
   return true;
 }

 if (WaitForSingleObject(m_UnlockEvent, INFINITE) != WAIT_TIMEOUT)
 {
  ResetEvent(m_UnlockEvent);
  LockId=lock_id;
  Pid=GetCurrentProcessId();
  return true;
 }
 return false;

/*
 if(!m_UnlockEvent)
  return false;
 if (WaitForSingleObject(m_UnlockEvent, 0) == WAIT_TIMEOUT)
 {
  if(lock_id>=0 && lock_id == LockId)
   return true;
 }
 else
 {
  if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_TIMEOUT)
   return false;
  ResetEvent(m_UnlockEvent);
  LockId=lock_id;
 }
 return true;
*/
}

bool UGenericMutexWin::unlock()
{
 if(!m_UnlockEvent)
  return true;
 if (WaitForSingleObject(m_UnlockEvent, 0) != WAIT_TIMEOUT)
  return true;
 Pid=0;
 SetEvent(m_UnlockEvent);
 LockId=-1;
 return true;
}

bool UGenericMutexWin::wait(int timeout)
{
 if(!m_UnlockEvent)
  return false;

 if (WaitForSingleObject(m_UnlockEvent, timeout) != WAIT_TIMEOUT)
 {
  return true;
 }
 return false;
}

UGenericMutexWin::UGenericMutexWin(const UGenericMutexWin &copy)
{

}

UGenericMutexWin& UGenericMutexWin::operator = (const UGenericMutexWin &copy)
{
 return *this;
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

UGenericMutexLocker::UGenericMutexLocker(UGenericMutex *m, int lock_id)
{
 if(m)
 {
  m_mutex = m;
  m_mutex->lock(lock_id);
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
