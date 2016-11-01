#ifndef UGENERIC_MUTEX_QT_CPP
#define UGENERIC_MUTEX_QT_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
#include <QReadWriteLock>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInt>

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
    //m_mutex.unlock();
    //m_mutex.try

    /*try
    {
        m_mutex.unlock();
    }
    catch(...)
    {

    }*/
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
    m_mutex.lockForRead();
    return true;
}

bool UGenericMutexQt::shared_unlock(void)
{
    m_mutex.unlock();
    return true;
}

bool UGenericMutexQt::exclusive_lock(void)
{
    m_mutex.lockForWrite();
    return true;
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
    //QMutex mutex;
    QWaitCondition condition;
    QAtomicInt isReseted;

public:
    UGenericEventQt();
    virtual ~UGenericEventQt();

    virtual bool set(void);
    virtual bool reset(void);
    virtual bool wait(unsigned wait_time);
};

UGenericEventQt::UGenericEventQt()
{
    isReseted = 0;
}

UGenericEventQt::~UGenericEventQt()
{

}

bool UGenericEventQt::set(void)
{
    condition.wakeAll();
    //isReseted = 0;
    return true;
}

bool UGenericEventQt::reset(void)
{
    isReseted = 1;
    return true;
}

bool UGenericEventQt::wait(unsigned wait_time)
{
    if(!isReseted) return false;
    QMutex mutex;
    mutex.lock();
    bool b = condition.wait(&mutex, wait_time);
    if(b)
    {
        isReseted = 0;
    }
    mutex.unlock();
    return b;
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
