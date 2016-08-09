#ifndef UGENERIC_MUTEX_QT_CPP
#define UGENERIC_MUTEX_QT_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
#include <QReadWriteLock>

class RDK_LIB_TYPE UGenericMutexQt: public UGenericMutex
{
private:
QReadWriteLock m_mutex;

public:
UGenericMutexQt();
virtual ~UGenericMutexQt();

virtual bool wait(int timeout);

virtual bool shared_lock(void);
virtual bool shared_unlock(void);

virtual bool exclusive_lock(void);
virtual bool exclusive_unlock(void);
};


UGenericMutexQt::UGenericMutexQt() : m_mutex(QReadWriteLock::Recursive)
{
    //mutex.lock();
}

UGenericMutexQt::~UGenericMutexQt()
{
    //mutex.unlock();
}

bool UGenericMutexQt::wait(int timeout)
{
 /*if (WaitForSingleObject(m_UnlockEvent, timeout) != WAIT_TIMEOUT)
 {
  return true;
 } */
 return m_mutex.tryLockForWrite(timeout);
}

bool UGenericMutexQt::shared_lock(void)
{
    return m_mutex.tryLockForRead();
}

bool UGenericMutexQt::shared_unlock(void)
{
    m_mutex.unlock();
    return true;
}

bool UGenericMutexQt::exclusive_lock(void)
{
    return m_mutex.tryLockForWrite();
}

bool UGenericMutexQt::exclusive_unlock(void)
{
    m_mutex.unlock();
    return true;
}


// ---------------------------------------------------------------------------
class RDK_LIB_TYPE UGenericEventQt: public UGenericEvent
{
protected:

public:
UGenericEventQt();
virtual ~UGenericEventQt();

virtual bool set(void);
virtual bool reset(void);
virtual bool wait(unsigned wait_time);
};

UGenericEventQt::UGenericEventQt()
{
}

UGenericEventQt::~UGenericEventQt()
{

}

bool UGenericEventQt::set(void)
{
 return true; // TODO:
}

bool UGenericEventQt::reset(void)
{
 return true; // TODO:
}

bool UGenericEventQt::wait(unsigned wait_time)
{
 return true; // TODO:
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

UGenericEvent* UCreateEvent(void)
{
 return new UGenericEventQt;
}
/*
UGenericEvent* UCreateEvent(bool initial_state)
{
 UGenericEventQt* res= new UGenericEventQt;
 if(initial_state)
  res->set();
 return res;
}*/

void UDestroyEvent(UGenericEvent* event)
{
 if(event)
 {
  delete event;
 }
}
/*
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
}*/
#endif
