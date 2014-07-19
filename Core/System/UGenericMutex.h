// ---------------------------------------------------------------------------

#ifndef GenericMutexH
#define GenericMutexH

#include "../../Deploy/Include/initdll_defs.h"

class RDK_LIB_TYPE UGenericMutex
{
protected:
int LockId;
//	void* m_UnlockEvent;

public:
UGenericMutex(): LockId(-1) {};
virtual ~UGenericMutex() {};

virtual bool lock(int lock_id=-1)=0;
virtual bool unlock()=0;
virtual bool wait(int timeout)=0;
};

RDK_LIB_TYPE UGenericMutex* UCreateMutex(void);
RDK_LIB_TYPE void UDestroyMutex(UGenericMutex* mutex);

// ---------------------------------------------------------------------------
class RDK_LIB_TYPE UGenericMutexLocker
{
private:
UGenericMutex *m_mutex;

public:
UGenericMutexLocker(UGenericMutex *m);
UGenericMutexLocker(UGenericMutex *m, int lock_id);
~UGenericMutexLocker();

private:
UGenericMutexLocker(const UGenericMutexLocker &m)
{};

UGenericMutexLocker& operator = (const UGenericMutexLocker &m)
{ return *this; };
};
// ---------------------------------------------------------------------------

#endif
