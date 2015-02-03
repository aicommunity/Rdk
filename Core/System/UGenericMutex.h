// ---------------------------------------------------------------------------

#ifndef GenericMutexH
#define GenericMutexH

#include "../../Deploy/Include/initdll_defs.h"
#include <map>
#include <string>

#ifdef RDK_MUTEX_DEADLOCK_DEBUG
struct RDK_LIB_TYPE TUThreadInfo
{
std::string Name;

int Pid;
};

extern RDK_LIB_TYPE std::map<int,TUThreadInfo> GlobalThreadInfoMap;
#endif

class RDK_LIB_TYPE UGenericMutex
{
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
public:
int DebugId;
#endif

protected:


public:
UGenericMutex();
virtual ~UGenericMutex();

virtual bool shared_lock(void)=0;
virtual bool shared_unlock(void)=0;

virtual bool exclusive_lock(void)=0;
virtual bool exclusive_unlock(void)=0;
};

class RDK_LIB_TYPE UGenericEvent
{
protected:

public:
UGenericEvent();
virtual ~UGenericEvent();

virtual bool set(void)=0;
virtual bool reset(void)=0;
virtual bool wait(unsigned wait_time)=0;
};

RDK_LIB_TYPE UGenericMutex* UCreateMutex(void);
RDK_LIB_TYPE void UDestroyMutex(UGenericMutex* mutex);

RDK_LIB_TYPE UGenericEvent* UCreateEvent(void);
RDK_LIB_TYPE void UDestroyEvent(UGenericEvent* event);

// ---------------------------------------------------------------------------
class RDK_LIB_TYPE UGenericMutexExclusiveLocker
{
private:
UGenericMutex *m_mutex;

public:
UGenericMutexExclusiveLocker(UGenericMutex *m);
~UGenericMutexExclusiveLocker();

private:
UGenericMutexExclusiveLocker(const UGenericMutexExclusiveLocker &m);

UGenericMutexExclusiveLocker& operator = (const UGenericMutexExclusiveLocker &m);
};

class RDK_LIB_TYPE UGenericMutexSharedLocker
{
private:
UGenericMutex *m_mutex;

public:
UGenericMutexSharedLocker(UGenericMutex *m);
~UGenericMutexSharedLocker();

private:
UGenericMutexSharedLocker(const UGenericMutexSharedLocker &m);
UGenericMutexSharedLocker& operator = (const UGenericMutexSharedLocker &m);
};
// ---------------------------------------------------------------------------

#endif
