#ifndef UGENERIC_MUTEX_CPP
#define UGENERIC_MUTEX_CPP

#include "UGenericMutex.h"

#ifdef RDK_MUTEX_DEADLOCK_DEBUG
RDK_LIB_TYPE std::map<int,TUThreadInfo> GlobalThreadInfoMap;
#endif

UGenericMutex::UGenericMutex() noexcept
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
: DebugId(-1)
#endif
{}

UGenericMutex::~UGenericMutex() noexcept
{}

UGenericEvent::UGenericEvent() noexcept
{

}

UGenericEvent::~UGenericEvent() noexcept
{

}

UGenericMutexExclusiveLocker::UGenericMutexExclusiveLocker(UGenericMutex *m)
 : m_mutex(nullptr), m_locked(false)
{
 if(m)
 {
  m_locked = m->exclusive_lock();
  if(m_locked)
  {
   m_mutex = m;
  }
  else
  {
   // Блокировка не удалась - не сохраняем указатель, чтобы не разблокировать в деструкторе
   m_mutex = nullptr;
  }
 }
 else
 {
  m_mutex = nullptr;
 }
}

UGenericMutexExclusiveLocker::~UGenericMutexExclusiveLocker() noexcept
{
 if(m_mutex && m_locked)
 {
  m_mutex->exclusive_unlock();
 }
}



UGenericMutexSharedLocker::UGenericMutexSharedLocker(UGenericMutex *m)
 : m_mutex(nullptr), m_locked(false)
{
 if(m)
 {
  m_locked = m->shared_lock();
  if(m_locked)
  {
   m_mutex = m;
  }
  else
  {
   // Блокировка не удалась - не сохраняем указатель, чтобы не разблокировать в деструкторе
   m_mutex = nullptr;
  }
 }
 else
 {
  m_mutex = nullptr;
 }
}

UGenericMutexSharedLocker::~UGenericMutexSharedLocker() noexcept
{
 if(m_mutex && m_locked)
 {
  m_mutex->shared_unlock();
 }
}


UGenericEvent* UCreateEvent(bool initial_state)
{
 if(initial_state)
  return UCreateEvent();

 UGenericEvent* event=UCreateEvent();
 event->reset();
 return event;
}

#endif

