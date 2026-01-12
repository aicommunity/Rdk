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
virtual ~UGenericMutexQt() noexcept;

virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool shared_unlock() noexcept;

virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool exclusive_unlock() noexcept;

private:
UGenericMutexQt(const UGenericMutexQt &copy) = delete;
UGenericMutexQt& operator = (const UGenericMutexQt &copy) = delete;
};


UGenericMutexQt::UGenericMutexQt() : m_mutex(QReadWriteLock::Recursive)
{
}

UGenericMutexQt::~UGenericMutexQt() noexcept
{
 // QReadWriteLock автоматически разблокируется при уничтожении
 // Не нужно явно разблокировать в деструкторе
}

bool UGenericMutexQt::shared_lock(unsigned timeout)
{
 try
 {
  if(timeout == RDK_MUTEX_TIMEOUT)
  {
   m_mutex.lockForRead();
   return true;
  }
  else
  {
   return m_mutex.tryLockForRead(timeout);
  }
 }
 catch(...)
 {
  // Qt может выбросить исключение при ошибке блокировки
  return false;
 }
}

bool UGenericMutexQt::shared_unlock() noexcept
{
 try
 {
  m_mutex.unlock();
  return true;
 }
 catch(...)
 {
  // В noexcept функции не можем выбросить исключение
  // Qt обычно не бросает исключения из unlock, но на всякий случай
  return false;
 }
}

bool UGenericMutexQt::exclusive_lock(unsigned timeout)
{
 try
 {
  if(timeout == RDK_MUTEX_TIMEOUT)
  {
   m_mutex.lockForWrite();
   return true;
  }
  else
  {
   return m_mutex.tryLockForWrite(timeout);
  }
 }
 catch(...)
 {
  // Qt может выбросить исключение при ошибке блокировки
  return false;
 }
}

bool UGenericMutexQt::exclusive_unlock() noexcept
{
 try
 {
  m_mutex.unlock();
  return true;
 }
 catch(...)
 {
  // В noexcept функции не можем выбросить исключение
  // Qt обычно не бросает исключения из unlock, но на всякий случай
  return false;
 }
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
    UGenericEventQt() noexcept;
    virtual ~UGenericEventQt() noexcept;

    virtual bool set() noexcept;
    virtual bool reset() noexcept;
    virtual bool wait(unsigned wait_time);

private:
    UGenericEventQt(const UGenericEventQt &copy) = delete;
    UGenericEventQt& operator = (const UGenericEventQt &copy) = delete;
};

UGenericEventQt::UGenericEventQt() noexcept
{
    isReseted = 0;
}

UGenericEventQt::~UGenericEventQt() noexcept
{

}

bool UGenericEventQt::set() noexcept
{
    condition.wakeAll();
    //isReseted = 0;
    return true;
}

bool UGenericEventQt::reset() noexcept
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
    #if defined(_MSC_VER)

#include <Windows.h>

class RDK_LIB_TYPE UGenericEventQt: public UGenericEvent
{
protected:
HANDLE Event;

public:
UGenericEventQt() noexcept;
virtual ~UGenericEventQt() noexcept;

virtual bool set() noexcept;
virtual bool reset() noexcept;
virtual bool wait(unsigned wait_time);

private:
UGenericEventQt(const UGenericEventQt &copy) = delete;
UGenericEventQt& operator = (const UGenericEventQt &copy) = delete;
};
UGenericEventQt::UGenericEventQt() noexcept
{
 Event = CreateEvent(nullptr, TRUE, TRUE, nullptr);
 if(!Event)
 {
  // В случае ошибки Event будет nullptr, что будет проверяться в методах
 }
}

UGenericEventQt::~UGenericEventQt() noexcept
{
 if(Event)
 {
  CloseHandle(Event);
  Event = nullptr;
 }
}

bool UGenericEventQt::set() noexcept
{
 if(!Event)
  return false;
 return SetEvent(Event) != FALSE;
}

bool UGenericEventQt::reset() noexcept
{
 if(!Event)
  return false;
 return ResetEvent(Event) != FALSE;
}

bool UGenericEventQt::wait(unsigned wait_time)
{
 if(!Event)
  return false;
 DWORD result = WaitForSingleObject(Event, wait_time);
 if(result == WAIT_TIMEOUT)
  return false;
 return result == WAIT_OBJECT_0;
}

    #elif defined(__GNUC__)
#if defined(_WIN32)
#include <windows.h>
#endif
#include "../Gcc/pevents.h" // got from https://github.com/NeoSmart/PEvents

class RDK_LIB_TYPE UGenericEventQt: public UGenericEvent
{
protected:
neosmart::neosmart_event_t Event;

public:
 UGenericEventQt() noexcept;
 virtual ~UGenericEventQt() noexcept;

 virtual bool set() noexcept;
 virtual bool reset() noexcept;
 virtual bool wait(unsigned wait_time);

private:
 UGenericEventQt(const UGenericEventQt &copy) = delete;
 UGenericEventQt& operator = (const UGenericEventQt &copy) = delete;
};

UGenericEventQt::UGenericEventQt() noexcept
{
 Event = neosmart::CreateEvent(true, true);
}

UGenericEventQt::~UGenericEventQt() noexcept
{
 if(Event)
 {
  neosmart::DestroyEvent(Event);
  Event = nullptr;
 }
}

bool UGenericEventQt::set() noexcept
{
 if(!Event)
  return false;
 neosmart::SetEvent(Event);
 return true;
}

bool UGenericEventQt::reset() noexcept
{
 if(!Event)
  return false;
 neosmart::ResetEvent(Event);
 return true;
}

bool UGenericEventQt::wait(unsigned wait_time)
{
 if(!Event)
  return false;
 if(neosmart::WaitForEvent(Event, wait_time) == WAIT_TIMEOUT)
  return false;
 return true;
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
