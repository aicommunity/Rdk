#ifndef UGENERIC_MUTEX_CPP
#define UGENERIC_MUTEX_CPP

#include "UGenericMutex.h"

#ifdef RDK_MUTEX_DEADLOCK_DEBUG
RDK_LIB_TYPE std::map<int,TUThreadInfo> GlobalThreadInfoMap;
#endif

UGenericMutex::UGenericMutex()
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
: DebugId(-1)
#endif
{}

UGenericMutex::~UGenericMutex()
{}

UGenericEvent::UGenericEvent()
{

}

UGenericEvent::~UGenericEvent()
{

}

UGenericMutexExclusiveLocker::UGenericMutexExclusiveLocker(UGenericMutex *m)
{
 if(m)
 {
  m->exclusive_lock();
  m_mutex = m;
 }
 else
  m_mutex = 0;
}

UGenericMutexExclusiveLocker::~UGenericMutexExclusiveLocker()
{
 if(m_mutex)
  m_mutex->exclusive_unlock();
}

UGenericMutexExclusiveLocker::UGenericMutexExclusiveLocker(const UGenericMutexExclusiveLocker &m)
{};

UGenericMutexExclusiveLocker& UGenericMutexExclusiveLocker::operator = (const UGenericMutexExclusiveLocker &m)
{ return *this; };



UGenericMutexSharedLocker::UGenericMutexSharedLocker(UGenericMutex *m)
{
 if(m)
 {
  m->shared_lock();
  m_mutex = m;
 }
 else
  m_mutex = 0;
}

UGenericMutexSharedLocker::~UGenericMutexSharedLocker()
{
 if(m_mutex)
  m_mutex->shared_unlock();
}

UGenericMutexSharedLocker::UGenericMutexSharedLocker(const UGenericMutexSharedLocker &m)
{};

UGenericMutexSharedLocker& UGenericMutexSharedLocker::operator = (const UGenericMutexSharedLocker &m)
{ return *this; };


UGenericEvent* UCreateEvent(bool initial_state)
{
 if(initial_state)
  return UCreateEvent();

 UGenericEvent* event=UCreateEvent();
 event->reset();
 return event;
}

#endif

