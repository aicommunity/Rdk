// ---------------------------------------------------------------------------

#ifndef GenericMutexH
#define GenericMutexH

class UGenericMutex
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

UGenericMutex* UCreateMutex(void);
void UDestroyMutex(UGenericMutex* mutex);

// ---------------------------------------------------------------------------
class UGenericMutexLocker
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
