#ifndef UGENERIC_MUTEX_QT_CPP
#define UGENERIC_MUTEX_QT_CPP
// ---------------------------------------------------------------------------

#include "../UGenericMutex.h"
#include <QReadWriteLock>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInt>

//#define RDK_NATIVE_QT_EVENTS

class RDK_LIB_TYPE UGenericMutexQt: public UGenericMutex
{
private:
QReadWriteLock m_mutex;

public:
UGenericMutexQt();
virtual ~UGenericMutexQt();

virtual bool wait(int timeout);

virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool shared_unlock(void);

virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
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

bool UGenericMutexQt::shared_lock(unsigned timeout)
{
 if(timeout == RDK_MUTEX_TIMEOUT)
  m_mutex.lockForRead();
 else
 {
  return m_mutex.tryLockForRead(timeout);
 }

    return true;
}

bool UGenericMutexQt::shared_unlock(void)
{
    m_mutex.unlock();
    return true;
}

bool UGenericMutexQt::exclusive_lock(unsigned timeout)
{
 if(timeout == RDK_MUTEX_TIMEOUT)
  m_mutex.lockForWrite();
 else
 {
  return m_mutex.tryLockForWrite(timeout);
 }

 return true;
}

bool UGenericMutexQt::exclusive_unlock(void)
{
    m_mutex.unlock();
    return true;
}


// ---------------------------------------------------------------------------
#ifdef RDK_NATIVE_QT_EVENTS
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
#else
    #if defined(_WIN32)

#include <Windows.h>

class RDK_LIB_TYPE UGenericEventQt: public UGenericEvent
{
protected:
HANDLE Event;

public:
UGenericEventQt();
virtual ~UGenericEventQt();

virtual bool set(void);
virtual bool reset(void);
virtual bool wait(unsigned wait_time);


private:
UGenericEventQt(const UGenericEventQt &copy);
UGenericEventQt& operator = (const UGenericEventQt &copy);
};
UGenericEventQt::UGenericEventQt()
{
 Event=CreateEvent(0,TRUE,TRUE,0);
}

UGenericEventQt::~UGenericEventQt()
{
 if(Event)
  CloseHandle(Event);
}

bool UGenericEventQt::set(void)
{
 SetEvent(Event);
 return true;
}

bool UGenericEventQt::reset(void)
{
 ResetEvent(Event);
 return true;
}

bool UGenericEventQt::wait(unsigned wait_time)
{
 if(WaitForSingleObject(Event,wait_time) == WAIT_TIMEOUT)
  return false;
 return true;
}

UGenericEventQt::UGenericEventQt(const UGenericEventQt &copy)
{

}

UGenericEventQt& UGenericEventQt::operator = (const UGenericEventQt &copy)
{
 return *this;
}

    #elif defined(__GNUC__)
#include "../Gcc/pevents.h" // got from https://github.com/NeoSmart/PEvents

class RDK_LIB_TYPE UGenericEventQt: public UGenericEvent
{
protected:
neosmart::neosmart_event_t Event;

public:
 UGenericEventQt();
 virtual ~UGenericEventQt();

 virtual bool set(void);
 virtual bool reset(void);
 virtual bool wait(unsigned wait_time);


private:
 UGenericEventQt(const UGenericEventQt &copy);
 UGenericEventQt& operator = (const UGenericEventQt &copy);
};

UGenericEventQt::UGenericEventQt()
{
 Event=neosmart::CreateEvent(true,true);
 //Event=CreateEvent(0,FALSE,TRUE,0);

 // Может быть удобно реализовать с помощью
 // condition variables и pthread_cond_timedwait
}

UGenericEventQt::~UGenericEventQt()
{
 neosmart::DestroyEvent(Event);
}

bool UGenericEventQt::set(void)
{
 neosmart::SetEvent(Event);
 return true;
}

bool UGenericEventQt::reset(void)
{
 neosmart::ResetEvent(Event);
 return true;
}

bool UGenericEventQt::wait(unsigned wait_time)
{
 if(neosmart::WaitForEvent(Event,wait_time) == WAIT_TIMEOUT)
  return false;
 return true;
// return false;
}

UGenericEventQt::UGenericEventQt(const UGenericEventQt &copy)
{

}

UGenericEventQt& UGenericEventQt::operator = (const UGenericEventQt &copy)
{
 return *this;
}


    #endif
#endif

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
