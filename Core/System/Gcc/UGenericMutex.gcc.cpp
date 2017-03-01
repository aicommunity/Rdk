#ifndef UGENERIC_MUTEX_GCC_CPP
#define UGENERIC_MUTEX_GCC_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
#include <pthread.h>
#include <iostream>
#include "pevents.h" // got from https://github.com/NeoSmart/PEvents

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
neosmart::neosmart_event_t Event;

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
 pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
 if(pthread_mutex_init(&mutex, &attr) != 0)
  throw 1;
}

UGenericMutexGcc::~UGenericMutexGcc()
{
 int res = pthread_mutex_destroy(&mutex);
// switch (res)
// {
//  case 0:
//   // Success
//   break;
//
//  case EBUSY:
//   // Attempting to destroy a locked mutex results in undefined behavior.
//   // If we are here, then the behavior has already happened.
//   break;
//
//  case EINVAL:
//   break;
//
//  default:
//   break;
// }
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
 int res = pthread_mutex_lock(&mutex);
 if (res == 0)
 {
  return true;
 }
 else
 {
//  switch (res)
//  {
//   case EINVAL:
//    std::cout << "Mutex locking failed, error: " << res << " (EINVAL)\n";
//    break;
//
//   case EBUSY:
//    std::cout << "Mutex locking failed, error: " << res << " (EBUSY)\n";
//    break;
//
//   case EAGAIN:
//    std::cout << "Mutex locking failed, error: " << res << " (EAGAIN)\n";
//    break;
//
//   case EDEADLK:
//    std::cout << "Mutex locking failed, error: " << res << " (EDEADLK)\n";
//    break;
//
//   default:
//    std::cout << "Mutex locking failed, error: " << res << "\n";
//    break;
//  }

  return false;
 }
 return false;
}

bool UGenericMutexGcc::exclusive_unlock(void)
{
 int res = pthread_mutex_unlock(&mutex);
 if (res != 0)
 {
//  switch (res)
//  {
//  case EPERM:
//   std::cout << "Mutex unlocking failed, error: " << res << " (EPERM)\n";
//   break;
//
//  default:
//   std::cout << "Mutex unlocking failed, error: " << res << " \n";
//   break;
//  }

  return false;
 }

 return true;
}



UGenericEventGcc::UGenericEventGcc()
{
 Event=neosmart::CreateEvent(true,true);
 //Event=CreateEvent(0,FALSE,TRUE,0);

 // Может быть удобно реализовать с помощью
 // condition variables и pthread_cond_timedwait
}

UGenericEventGcc::~UGenericEventGcc()
{
 neosmart::DestroyEvent(Event);
}

bool UGenericEventGcc::set(void)
{
 neosmart::SetEvent(Event);
 return true;
}

bool UGenericEventGcc::reset(void)
{
 neosmart::ResetEvent(Event);
 return true;
}

bool UGenericEventGcc::wait(unsigned wait_time)
{
 if(neosmart::WaitForEvent(Event,wait_time) == WAIT_TIMEOUT)
  return false;
 return true;
// return false;
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

