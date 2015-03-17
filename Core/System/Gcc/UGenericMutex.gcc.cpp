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

class RDK_LIB_TYPE UGenericEventGcc: public UGenericEvent
{
protected:
//HANDLE Event;

public:
UGenericEventGcc();
virtual ~UGenericEventGcc();

virtual bool set(void);
virtual bool reset(void);
virtual bool wait(unsigned wait_time);


private:
UGenericEventGcc(const UGenericEventGcc &copy);
UGenericEventGcc& operator = (const UGenericEventGcc &copy);
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

UGenericEventGcc::UGenericEventGcc()
{
 //Event=CreateEvent(0,FALSE,TRUE,0);
}

UGenericEventGcc::~UGenericEventGcc()
{
 //if(Event)
 // CloseHandle(Event);
}

bool UGenericEventGcc::set(void)
{
 //SetEvent(Event);
 return true;
}

bool UGenericEventGcc::reset(void)
{
 //ResetEvent(Event);
 return true;
}

bool UGenericEventGcc::wait(unsigned wait_time)
{
 //if(WaitForSingleObject(Event,wait_time) == WAIT_TIMEOUT)
 // return false;
 return true;
}

UGenericEventGcc::UGenericEventGcc(const UGenericEventGcc &copy)
{

}

UGenericEventGcc& UGenericEventGcc::operator = (const UGenericEventGcc &copy)
{
 return *this;
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

UGenericEvent* UCreateEvent(void)
{
 return new UGenericEventGcc;
}

void UDestroyEvent(UGenericEvent* event)
{
 if(event)
  delete event;
}

#endif

