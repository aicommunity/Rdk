#ifndef UGENERIC_MUTEX_ANSI_CPP
#define UGENERIC_MUTEX_ANSI_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
//#include <boost/thread/thread_id.hpp>
//#include <boost/bind.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/exception.hpp>
#include <windows.h>

class RDK_LIB_TYPE UGenericMutexAnsi: public UGenericMutex
{
private:
boost::shared_mutex Mutex;

public:
UGenericMutexAnsi();
virtual ~UGenericMutexAnsi() noexcept;

virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool shared_unlock() noexcept;

virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool exclusive_unlock() noexcept;

private:
UGenericMutexAnsi(const UGenericMutexAnsi &copy) = delete;
UGenericMutexAnsi& operator = (const UGenericMutexAnsi &copy) = delete;
};


UGenericMutexAnsi::UGenericMutexAnsi()
{
 // boost::shared_mutex инициализируется автоматически
}

UGenericMutexAnsi::~UGenericMutexAnsi() noexcept
{
 // boost::shared_mutex автоматически освобождается при уничтожении
}

bool UGenericMutexAnsi::shared_lock(unsigned timeout)
{
 try
 {
  if(timeout == RDK_MUTEX_TIMEOUT)
  {
   Mutex.lock_shared();
   return true;
  }
  else
  {
   boost::system_time pt(boost::get_system_time());
   if(!Mutex.timed_lock_shared(pt + boost::posix_time::milliseconds(timeout)))
   {
    return false;
   }
   return true;
  }
 }
 catch(...)
 {
  // Boost может выбросить исключение при ошибке блокировки
  return false;
 }
}

bool UGenericMutexAnsi::shared_unlock() noexcept
{
 try
 {
  Mutex.unlock_shared();
  return true;
 }
 catch(...)
 {
  // В noexcept функции не можем выбросить исключение
  // Boost обычно не бросает исключения из unlock_shared, но на всякий случай
  return false;
 }
}

bool UGenericMutexAnsi::exclusive_lock(unsigned timeout)
{
 try
 {
  if(timeout == RDK_MUTEX_TIMEOUT)
  {
   Mutex.lock();
   return true;
  }
  else
  {
   // Сначала пробуем быструю блокировку
   if(Mutex.try_lock())
   {
    return true;
   }
   
   // Если не получилось, используем таймаут
   boost::system_time pt(boost::get_system_time());
   if(!Mutex.timed_lock(pt + boost::posix_time::milliseconds(timeout)))
   {
    return false;
   }
   return true;
  }
 }
 catch(...)
 {
  // Boost может выбросить исключение при ошибке блокировки
  return false;
 }
}

bool UGenericMutexAnsi::exclusive_unlock() noexcept
{
 try
 {
  Mutex.unlock();
  return true;
 }
 catch(...)
 {
  // В noexcept функции не можем выбросить исключение
  // Boost обычно не бросает исключения из unlock, но на всякий случай
  return false;
 }
}

// ---------------------------------------------------------------------------
UGenericMutex* UCreateMutex(void)
{
 return new UGenericMutexAnsi;
}

void UDestroyMutex(UGenericMutex* mutex)
{
 if(mutex)
  delete mutex;
}

#endif
