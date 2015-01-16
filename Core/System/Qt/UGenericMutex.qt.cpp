#ifndef UGENERIC_MUTEX_WIN_CPP
#define UGENERIC_MUTEX_WIN_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
#include <QMutex>

class RDK_LIB_TYPE UGenericMutexQt: public UGenericMutex
{
private:
QMutex m_mutex;

public:
UGenericMutexQt();
virtual ~UGenericMutexQt();

virtual bool lock(int lock_id=-1);
virtual bool unlock();
virtual bool wait(int timeout);
};


UGenericMutexQt::UGenericMutexQt()
{
    //mutex.lock();
}

UGenericMutexQt::~UGenericMutexQt()
{
    //mutex.unlock();
}

bool UGenericMutexQt::lock(int lock_id)
{
 m_mutex.lock();
 return true;
}

bool UGenericMutexQt::unlock()
{
 m_mutex.unlock();
 return true;
}

bool UGenericMutexQt::wait(int timeout)
{
 /*if (WaitForSingleObject(m_UnlockEvent, timeout) != WAIT_TIMEOUT)
 {
  return true;
 } */
 return m_mutex.tryLock(timeout);
}
// ---------------------------------------------------------------------------
UGenericMutex* UCreateMutex(void)
{
 return new UGenericMutexQt;
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

UGenericMutexLocker::~UGenericMutexLocker()
{
 if(m_mutex)
  m_mutex->unlock();
}
#endif
