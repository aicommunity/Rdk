#ifndef UGENERIC_MUTEX_GCC_CPP
#define UGENERIC_MUTEX_GCC_CPP
// ---------------------------------------------------------------------------

#ifndef _WIN32

#include "../UGenericMutex.h"
#include <pthread.h>
#include <iostream>
#include <cerrno>
#include <stdexcept>
#include <cstring>
#include "pevents.h" // got from https://github.com/NeoSmart/PEvents

class RDK_LIB_TYPE UGenericMutexGcc: public UGenericMutex
{
private:
 pthread_rwlock_t m_rwlock;

public:
 UGenericMutexGcc();
 virtual ~UGenericMutexGcc() noexcept;

 virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
 virtual bool shared_unlock() noexcept;

 virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
 virtual bool exclusive_unlock() noexcept;

private:
 UGenericMutexGcc(const UGenericMutexGcc &copy) = delete;
 UGenericMutexGcc& operator = (const UGenericMutexGcc &copy) = delete;
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

bool UGenericMutexGcc::shared_lock(unsigned timeout)
{
 int res;

 if(timeout == RDK_MUTEX_TIMEOUT)
 {
  res = pthread_rwlock_rdlock(&m_rwlock);
 }
 else
 {
  struct timespec abs_time;
  if(clock_gettime(CLOCK_REALTIME, &abs_time) != 0)
  {
   // Ошибка получения времени - используем обычную блокировку
   res = pthread_rwlock_rdlock(&m_rwlock);
  }
  else
  {
   // Вычисляем абсолютное время с учетом таймаута
   abs_time.tv_sec += timeout / 1000;
   long nsec_add = (timeout % 1000) * 1000000L;
   abs_time.tv_nsec += nsec_add;
   
   // Обработка переполнения наносекунд
   if(abs_time.tv_nsec >= 1000000000L)
   {
    abs_time.tv_sec += abs_time.tv_nsec / 1000000000L;
    abs_time.tv_nsec = abs_time.tv_nsec % 1000000000L;
   }
   
   res = pthread_rwlock_timedrdlock(&m_rwlock, &abs_time);
  }
 }

 if(res == 0)
  return true;
 else if(res == ETIMEDOUT)
  return false;
 else
 {
  // Другие ошибки (EINVAL, EDEADLK и т.д.)
  return false;
 }
}

bool UGenericMutexGcc::shared_unlock() noexcept
{
 int res = pthread_rwlock_unlock(&m_rwlock);
 if(res != 0)
 {
  // Ошибка разблокировки - возможно, мьютекс не был заблокирован этим потоком
  // В noexcept функции не можем выбросить исключение
  return false;
 }
 return true;
}

bool UGenericMutexGcc::exclusive_lock(unsigned timeout)
{
 int res;

 if(timeout == RDK_MUTEX_TIMEOUT)
 {
  res = pthread_rwlock_wrlock(&m_rwlock);
 }
 else
 {
  struct timespec abs_time;
  if(clock_gettime(CLOCK_REALTIME, &abs_time) != 0)
  {
   // Ошибка получения времени - используем обычную блокировку
   res = pthread_rwlock_wrlock(&m_rwlock);
  }
  else
  {
   // Вычисляем абсолютное время с учетом таймаута
   abs_time.tv_sec += timeout / 1000;
   long nsec_add = (timeout % 1000) * 1000000L;
   abs_time.tv_nsec += nsec_add;
   
   // Обработка переполнения наносекунд
   if(abs_time.tv_nsec >= 1000000000L)
   {
    abs_time.tv_sec += abs_time.tv_nsec / 1000000000L;
    abs_time.tv_nsec = abs_time.tv_nsec % 1000000000L;
   }
   
   res = pthread_rwlock_timedwrlock(&m_rwlock, &abs_time);
  }
 }

 if(res == 0)
  return true;
 else if(res == ETIMEDOUT)
  return false;
 else
 {
  // Другие ошибки (EINVAL, EDEADLK и т.д.)
  return false;
 }
}

bool UGenericMutexGcc::exclusive_unlock() noexcept
{
 int res = pthread_rwlock_unlock(&m_rwlock);
 if(res != 0)
 {
  // Ошибка разблокировки - возможно, мьютекс не был заблокирован этим потоком
  // В noexcept функции не можем выбросить исключение
  return false;
 }
 return true;
}



UGenericEventGcc::UGenericEventGcc() noexcept
{
 Event = neosmart::CreateEvent(true, true);
}

UGenericEventGcc::~UGenericEventGcc() noexcept
{
 if(Event)
 {
  neosmart::DestroyEvent(Event);
  Event = nullptr;
 }
}

bool UGenericEventGcc::set() noexcept
{
 if(!Event)
  return false;
 neosmart::SetEvent(Event);
 return true;
}

bool UGenericEventGcc::reset() noexcept
{
 if(!Event)
  return false;
 neosmart::ResetEvent(Event);
 return true;
}

bool UGenericEventGcc::wait(unsigned wait_time)
{
 if(!Event)
  return false;
 if(neosmart::WaitForEvent(Event, wait_time) == WAIT_TIMEOUT)
  return false;
 return true;
}

UGenericEventGcc::UGenericEventGcc(const UGenericEventGcc &copy) = delete;

UGenericEventGcc& UGenericEventGcc::operator = (const UGenericEventGcc &copy) = delete;

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

#endif // _WIN32

#endif // UGENERIC_MUTEX_GCC_CPP

