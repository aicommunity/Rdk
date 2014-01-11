// ---------------------------------------------------------------------------

#ifndef GenericMutexH
#define GenericMutexH

class UGenericMutex
{
private:
//	void* m_UnlockEvent;

public:
UGenericMutex() {};
virtual ~UGenericMutex() {};

virtual bool lock()=0;
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
	~UGenericMutexLocker();
};
// ---------------------------------------------------------------------------

#endif
