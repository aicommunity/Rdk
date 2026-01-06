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
SRWLOCK m_lock;
CONDITION_VARIABLE m_condition;
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
DWORD m_exclusive_owner; // ID потока, владеющего exclusive блокировкой
int m_shared_count; // Количество shared блокировок (для отладки)
#endif

public:
UGenericMutexWin();
virtual ~UGenericMutexWin() noexcept;

virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool shared_unlock() noexcept;

virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
virtual bool exclusive_unlock() noexcept;

private:
UGenericMutexWin(const UGenericMutexWin &copy) = delete;
UGenericMutexWin& operator = (const UGenericMutexWin &copy) = delete;
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

UGenericMutexWin::UGenericMutexWin() noexcept
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
 : m_exclusive_owner(0), m_shared_count(0)
#endif
{
 InitializeSRWLock(&m_lock);
 InitializeConditionVariable(&m_condition);
}

UGenericMutexWin::~UGenericMutexWin() noexcept
{
 // SRWLOCK и CONDITION_VARIABLE не требуют явного освобождения
 // Проверка на заблокированный мьютекс невозможна без дополнительной логики
}

bool UGenericMutexWin::shared_lock(unsigned timeout)
{
using namespace std;
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
 DWORD current_pid = GetCurrentThreadId();
#endif

 if(timeout == RDK_MUTEX_TIMEOUT)
 {
  AcquireSRWLockShared(&m_lock);
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
  m_shared_count++;
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<current_pid<<" "<<name<<": shared_lock"<<endl;
	file.flush();
   }
  }
#endif
  return true;
 }
 else
 {
  // Для таймаута используем TryAcquireSRWLockShared с polling
  DWORD start_time = GetTickCount();
  DWORD elapsed = 0;
  bool acquired = false;

  while(elapsed < timeout)
  {
   if(TryAcquireSRWLockShared(&m_lock))
   {
    acquired = true;
    break;
   }
   
   // Небольшая задержка перед следующей попыткой
   DWORD remaining = timeout - elapsed;
   DWORD sleep_time = (remaining < 10) ? remaining : 10;
   if(sleep_time > 0)
   {
    Sleep(sleep_time);
   }
   
   elapsed = GetTickCount() - start_time;
   // Защита от переполнения GetTickCount
   if(elapsed > timeout)
    break;
  }

  if(!acquired)
  {
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
   if(DebugId>=0)
   {
    fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
    if(file)
    {
	 std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	 string name;
	 if(I != GlobalThreadInfoMap.end())
	  name=I->second.Name;
	 file<<current_pid<<" "<<name<<": shared_lock timeout"<<endl;
	 file.flush();
    }
   }
#endif
   return false;
  }

#ifdef RDK_MUTEX_DEADLOCK_DEBUG
  m_shared_count++;
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<current_pid<<" "<<name<<": shared_lock"<<endl;
	file.flush();
   }
  }
#endif
  return true;
 }
}

bool UGenericMutexWin::shared_unlock() noexcept
{
using namespace std;
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
 DWORD current_pid = GetCurrentThreadId();
 if(m_shared_count <= 0)
 {
  // Попытка разблокировать незаблокированный мьютекс
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<current_pid<<" "<<name<<": shared_unlock failed - not locked"<<endl;
	file.flush();
   }
  }
  return false;
 }
 m_shared_count--;
 if(DebugId>=0)
 {
  fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
  if(file)
  {
   std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
   string name;
   if(I != GlobalThreadInfoMap.end())
	name=I->second.Name;
   file<<current_pid<<" "<<name<<": shared_unlock"<<endl;
   file.flush();
  }
 }
#endif
 ReleaseSRWLockShared(&m_lock);
 WakeAllConditionVariable(&m_condition);
 return true;
}

bool UGenericMutexWin::exclusive_lock(unsigned timeout)
{
using namespace std;
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
 DWORD current_pid = GetCurrentThreadId();
#endif

 if(timeout == RDK_MUTEX_TIMEOUT)
 {
  AcquireSRWLockExclusive(&m_lock);
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
  m_exclusive_owner = current_pid;
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<current_pid<<" "<<name<<": exclusive_lock"<<endl;
	file.flush();
   }
  }
#endif
  return true;
 }
 else
 {
  // Для таймаута используем TryAcquireSRWLockExclusive с polling
  DWORD start_time = GetTickCount();
  DWORD elapsed = 0;
  bool acquired = false;

  while(elapsed < timeout)
  {
   if(TryAcquireSRWLockExclusive(&m_lock))
   {
    acquired = true;
    break;
   }
   
   // Небольшая задержка перед следующей попыткой
   DWORD remaining = timeout - elapsed;
   DWORD sleep_time = (remaining < 10) ? remaining : 10;
   if(sleep_time > 0)
   {
    Sleep(sleep_time);
   }
   
   elapsed = GetTickCount() - start_time;
   // Защита от переполнения GetTickCount
   if(elapsed > timeout)
    break;
  }

  if(!acquired)
  {
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
   if(DebugId>=0)
   {
    fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
    if(file)
    {
	 std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	 string name;
	 if(I != GlobalThreadInfoMap.end())
	  name=I->second.Name;
	 file<<current_pid<<" "<<name<<": exclusive_lock timeout"<<endl;
	 file.flush();
    }
   }
#endif
   return false;
  }

#ifdef RDK_MUTEX_DEADLOCK_DEBUG
  m_exclusive_owner = current_pid;
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	file<<current_pid<<" "<<name<<": exclusive_lock"<<endl;
	file.flush();
   }
  }
#endif
  return true;
 }
}

bool UGenericMutexWin::exclusive_unlock() noexcept
{
using namespace std;
#ifdef RDK_MUTEX_DEADLOCK_DEBUG
 DWORD current_pid = GetCurrentThreadId();
 if(m_exclusive_owner != current_pid && m_exclusive_owner != 0)
 {
  // Попытка разблокировать мьютекс, заблокированный другим потоком
  if(DebugId>=0)
  {
   fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
   if(file)
   {
	std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
	string name;
	if(I != GlobalThreadInfoMap.end())
	 name=I->second.Name;
	I=GlobalThreadInfoMap.find(m_exclusive_owner);
	string owner_name;
	if(I != GlobalThreadInfoMap.end())
	 owner_name=I->second.Name;
	file<<current_pid<<" "<<name<<": exclusive_unlock failed - owned by "<<m_exclusive_owner<<" "<<owner_name<<endl;
	file.flush();
   }
  }
  return false;
 }
 m_exclusive_owner = 0;
 if(DebugId>=0)
 {
  fstream file((RDK::sntoa(DebugId,2)+".lock.txt").c_str(),ios::out | ios::app);
  if(file)
  {
   std::map<int,TUThreadInfo>::const_iterator I=GlobalThreadInfoMap.find(current_pid);
   string name;
   if(I != GlobalThreadInfoMap.end())
	name=I->second.Name;
   file<<current_pid<<" "<<name<<": exclusive_unlock"<<endl;
   file.flush();
  }
 }
#endif
 ReleaseSRWLockExclusive(&m_lock);
 WakeAllConditionVariable(&m_condition);
 return true;
}


// ---------------------------------------------------------------------------
UGenericEventWin::UGenericEventWin() noexcept
{
 Event = CreateEvent(nullptr, TRUE, TRUE, nullptr);
 if(!Event)
 {
  // В случае ошибки Event будет nullptr, что будет проверяться в методах
 }
}

UGenericEventWin::~UGenericEventWin() noexcept
{
 if(Event)
 {
  CloseHandle(Event);
  Event = nullptr;
 }
}

bool UGenericEventWin::set() noexcept
{
 if(!Event)
  return false;
 return SetEvent(Event) != FALSE;
}

bool UGenericEventWin::reset() noexcept
{
 if(!Event)
  return false;
 return ResetEvent(Event) != FALSE;
}

bool UGenericEventWin::wait(unsigned wait_time)
{
 if(!Event)
  return false;
 DWORD result = WaitForSingleObject(Event, wait_time);
 if(result == WAIT_TIMEOUT)
  return false;
 return result == WAIT_OBJECT_0;
}

UGenericEventWin::UGenericEventWin(const UGenericEventWin &copy) = delete;

UGenericEventWin& UGenericEventWin::operator = (const UGenericEventWin &copy) = delete;

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

