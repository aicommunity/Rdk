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
#include <windows.h>

class RDK_LIB_TYPE UGenericMutexAnsi: public UGenericMutex
{
private:
boost::shared_mutex Mutex;

boost::thread::id Id;

DWORD Pid;

//boost::unique_lock< boost::shared_mutex > * UniqueLock;

//boost::shared_lock< boost::shared_mutex > * SharedLock;

public:
UGenericMutexAnsi();
virtual ~UGenericMutexAnsi();

virtual bool shared_lock(void);
virtual bool shared_unlock(void);

virtual bool exclusive_lock(void);
virtual bool exclusive_unlock(void);

};


UGenericMutexAnsi::UGenericMutexAnsi()
{
 //m_UnlockEvent = CreateEvent(0, TRUE, TRUE, 0);
}

UGenericMutexAnsi::~UGenericMutexAnsi()
{
/* if(SharedLock)
 {
  delete SharedLock;
  SharedLock=0;
 }

 if(UniqueLock)
 {
  delete UniqueLock;
  UniqueLock=0;
 }  */
}

bool UGenericMutexAnsi::shared_lock(void)
{
// Mutex.lock_shared();
 boost::system_time pt(boost::get_system_time());
 if(!Mutex.timed_lock_shared(pt + boost::posix_time::milliseconds(10000)))
 {
  return false;
 }
 return true;
}

bool UGenericMutexAnsi::shared_unlock(void)
{
 Mutex.unlock_shared();
 return true;
}

bool UGenericMutexAnsi::exclusive_lock(void)
{
// boost::thread::id self = boost::this_thread::get_id();
 if(Mutex.try_lock())
 {
//  Id = self;
  return true;
 }

// if(Id == self)
//  return true;

// Mutex.lock();
 boost::system_time pt(boost::get_system_time());
 if(!Mutex.timed_lock(pt + boost::posix_time::milliseconds(10000)))
 {
  return false;
 }
 // Id = self;
 return true;
}

bool UGenericMutexAnsi::exclusive_unlock(void)
{
 Mutex.unlock();
// Id=boost::thread::id();
 return true;
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
