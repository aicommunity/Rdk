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

virtual bool shared_lock(void);
virtual bool shared_unlock(void);

virtual bool exclusive_lock(void);
virtual bool exclusive_unlock(void);

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

bool UGenericMutexGcc::shared_lock(void)
{
 return true;
}

bool UGenericMutexGcc::shared_unlock(void)
{
 return true;
}

bool UGenericMutexGcc::exclusive_lock(void)
{
 return true;
}

bool UGenericMutexGcc::exclusive_unlock(void)
{
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

