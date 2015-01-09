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

virtual bool shared_lock(void);
virtual bool shared_unlock(void);

virtual bool exclusive_lock(void);
virtual bool exclusive_unlock(void);

private:
UGenericMutexWin(const UGenericMutexWin &copy);
UGenericMutexWin& operator = (const UGenericMutexWin &copy);

};


UGenericMutexWin::UGenericMutexWin()
 : Pid(0)
{
 m_UnlockEvent = CreateMutex(0, FALSE, 0);
}

UGenericMutexWin::~UGenericMutexWin()
{
 BOOL res=CloseHandle(m_UnlockEvent);
 if(!res)
  throw 1;
}

bool UGenericMutexWin::shared_lock(void)
{
 if(!m_UnlockEvent)
  return false;
/* DWORD wait_res=WaitForSingleObject(m_UnlockEvent, 0);
 if (wait_res != WAIT_OBJECT_0)
 {
  if(Pid == GetCurrentThreadId())
   return true;
 }
 else
 if(wait_res == WAIT_OBJECT_0)
 {
  LockId=lock_id;
  Pid=GetCurrentThreadId();
  return true;
 }
  */
 if (WaitForSingleObject(m_UnlockEvent, INFINITE) == WAIT_OBJECT_0)
 {
//  ResetEvent(m_UnlockEvent);
//  LockId=lock_id;
//  Pid=GetCurrentThreadId();
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

bool UGenericMutexWin::shared_unlock(void)
{
 if(!m_UnlockEvent)
  return true;
// if (WaitForSingleObject(m_UnlockEvent, 0) != WAIT_TIMEOUT)
//  return true;
// Pid=0;
// SetEvent(m_UnlockEvent);
 BOOL res=ReleaseMutex(m_UnlockEvent);
 if(res != TRUE)
  throw 2;
// LockId=-1;
 return true;
}

/*bool UGenericMutexWin::wait(int timeout)
{
 if(!m_UnlockEvent)
  return false;

 if (WaitForSingleObject(m_UnlockEvent, timeout) != WAIT_TIMEOUT)
 {
  return true;
 }
 return false;
} */

bool UGenericMutexWin::exclusive_lock(void)
{
 return shared_lock();
}

bool UGenericMutexWin::exclusive_unlock(void)
{
 return shared_unlock();
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

#endif
