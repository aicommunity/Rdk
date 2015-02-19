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

virtual bool lock(int lock_id=-1);
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

bool UGenericMutexGcc::lock(int lock_id)
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

#endif

