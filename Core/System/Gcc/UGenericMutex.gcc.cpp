#ifndef UGENERIC_MUTEX_GCC_CPP
#define UGENERIC_MUTEX_GCC_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
#include <pthread.h>

class RDK_LIB_TYPE UGenericMutexGcc: public UGenericMutex
{
private:
 pthread_mutex_t mutex;

public:
 UGenericMutexGcc();
 virtual ~UGenericMutexGcc();

 virtual bool shared_lock(void);
 virtual bool shared_unlock(void);

 virtual bool exclusive_lock(void);
 virtual bool exclusive_unlock(void);
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
 pthread_mutexattr_t attr;
 pthread_mutexattr_init(&attr);
 pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
 if(pthread_mutex_init(&mutex, &attr) != 0)
  throw 1;
}

UGenericMutexGcc::~UGenericMutexGcc()
{
 int res = pthread_mutex_destroy(&mutex);
 switch (res)
 {
  case 0:
   // Success
   break;

  case EBUSY:
   // Attempting to destroy a locked mutex results in undefined behavior.
   // If we are here, then the behavior has already happened.
   break;

  case EINVAL:
   break;

  default:
   break;
 }
}

bool UGenericMutexGcc::shared_lock(void)
{
 return exclusive_lock();
}

bool UGenericMutexGcc::shared_unlock(void)
{
 return exclusive_unlock();
}

bool UGenericMutexGcc::exclusive_lock(void)
{
 if (pthread_mutex_lock(&mutex) == 0)
 {
  return true;
 }
 else
 {
  return false;
 }
 return false;
}

bool UGenericMutexGcc::exclusive_unlock(void)
{
 if (pthread_mutex_unlock(&mutex) != 0)
 {
  return false;
 }

 return true;
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

