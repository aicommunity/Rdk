#ifndef UGENERIC_MUTEX_WIN_CPP
#define UGENERIC_MUTEX_WIN_CPP
// ---------------------------------------------------------------------------

#include <windows.h>
#include "../UGenericMutex.h"
#include "../../../Deploy/Include/initdll_defs.h"
#include "../../Utilities/USupport.h"
#include <fstream>

class RDK_LIB_TYPE UGenericMutexWin: public UGenericMutex
{
private:
void* m_UnlockEvent;

DWORD Pid;

public:
UGenericMutexWin();
virtual ~UGenericMutexWin();

virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool shared_unlock(void);

virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool exclusive_unlock(void);

private:
UGenericMutexWin(const UGenericMutexWin &copy);
UGenericMutexWin& operator = (const UGenericMutexWin &copy);

};

class RDK_LIB_TYPE UGenericEventWin: public UGenericEvent
{
protected:
HANDLE Event;

public:
UGenericEventWin();
virtual ~UGenericEventWin();

virtual bool set(void);
virtual bool reset(void);
virtual bool wait(unsigned wait_time);


private:
UGenericEventWin(const UGenericEventWin &copy);
UGenericEventWin& operator = (const UGenericEventWin &copy);
};

UGenericMutexWin::UGenericMutexWin()
 : Pid(0)
{
 m_UnlockEvent = CreateMutex(0, FALSE, 0);
}

UGenericMutexWin::~UGenericMutexWin()
{
 BOOL res=CloseHandle(m_UnlockEvent);
// if(!res)
//  throw 1; // TODO: !!!
}

bool UGenericMutexWin::shared_lock(unsigned timeout)
{
using namespace std;
 if(!m_UnlockEvent)
  return false;

#ifdef RDK_MUTEX_DEADLOCK_DEBUG
 DWORD res=WaitForSingleObject(m_UnlockEvent, 60000);
#else
 DWORD res=WaitForSingleObject(m_UnlockEvent, (timeout==RDK_MUTEX_TIMEOUT)?INFINITE:timeout);
#endif
 if (res == WAIT_OBJECT_0)
 {
  #ifdef RDK_MUTEX_DEADLOCK_DEBUG
  Pid=GetCurrentThreadId();
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(Pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<Pid<<" "<<name<<": lock"<<endl;
	file.flush();
   }
  }
  #endif
  return true;
 }
 else
 {
  #ifdef RDK_MUTEX_DEADLOCK_DEBUG
  Pid=GetCurrentThreadId();
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(Pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<Pid<<" "<<name<<": deadlock. res="<<RDK::sntoa(res)<<endl;
	file.flush();
	file.close();
   }
  }
  #endif
  return false;
 }
 return false;
}

bool UGenericMutexWin::shared_unlock(void)
{
using namespace std;
 if(!m_UnlockEvent)
  return true;

 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(Pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<Pid<<" "<<name<<": unlock"<<endl;
	file.flush();
   }
  }
 #endif
 BOOL res=ReleaseMutex(m_UnlockEvent);

 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 DWORD error=GetLastError();
 #endif
 if(res != TRUE)
 {
  #ifdef RDK_MUTEX_DEADLOCK_DEBUG
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(Pid);
	string old_name, new_name;
	if(I != GlobalThreadInfoMap.end())
	 old_name=I->second.Name;

	int new_id=GetCurrentThreadId();
	I=GlobalThreadInfoMap.find(new_id);
	if(I != GlobalThreadInfoMap.end())
	 new_name=I->second.Name;

	file<<"fail unlock old PID="<<RDK::sntoa(Pid)<<" "<<old_name<<" curr Pid="<<RDK::sntoa(new_id)<<" "<<new_name<<" Error="<<RDK::sntoa(error)<<endl;
	file.flush();
	file.close();
   }
  }
  #endif
  return false;
 }
 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 Pid=0;
 #endif

 return true;
}

bool UGenericMutexWin::exclusive_lock(unsigned timeout)
{
 return shared_lock(timeout);
}

bool UGenericMutexWin::exclusive_unlock()
{
 return shared_unlock();
}

UGenericMutexWin::UGenericMutexWin(const UGenericMutexWin &copy)
{

}

UGenericMutexWin& UGenericMutexWin::operator = (const UGenericMutexWin &copy)
{
 return *this;
}


// ---------------------------------------------------------------------------
UGenericEventWin::UGenericEventWin()
{
 Event=CreateEvent(0,TRUE,TRUE,0);
}

UGenericEventWin::~UGenericEventWin()
{
 if(Event)
  CloseHandle(Event);
}

bool UGenericEventWin::set(void)
{
 SetEvent(Event);
 return true;
}

bool UGenericEventWin::reset(void)
{
 ResetEvent(Event);
 return true;
}

bool UGenericEventWin::wait(unsigned wait_time)
{
 if(WaitForSingleObject(Event,wait_time) == WAIT_TIMEOUT)
  return false;
 return true;
}

UGenericEventWin::UGenericEventWin(const UGenericEventWin &copy)
{

}

UGenericEventWin& UGenericEventWin::operator = (const UGenericEventWin &copy)
{
 return *this;
}

// ---------------------------------------------------------------------------
UGenericMutex* UCreateMutex(void)
{
 return new UGenericMutexWin;
}

void UDestroyMutex(UGenericMutex* mutex)
{
 if(mutex)
  delete mutex;
}

UGenericEvent* UCreateEvent(void)
{
 return new UGenericEventWin;
}

void UDestroyEvent(UGenericEvent* event)
{
 if(event)
  delete event;
}


#endif
