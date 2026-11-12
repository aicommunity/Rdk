#ifndef UGENERIC_MUTEX_GCC_CPP
#define UGENERIC_MUTEX_GCC_CPP
// ---------------------------------------------------------------------------

#ifndef _WIN32

#include "../UGenericMutex.h"
#include <pthread.h>
#include <iostream>
#include <cerrno>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <map>
#include <unistd.h>
#include <sys/syscall.h>
#include "pevents.h" // got from https://github.com/NeoSmart/PEvents

class RDK_LIB_TYPE UGenericMutexGcc: public UGenericMutex
{
private:
 pthread_mutex_t m_mutex; // Рекурсивный мьютекс для exclusive lock
 pthread_cond_t m_condition; // Условная переменная для ожидания
 pthread_t m_exclusive_owner; // Поток, владеющий exclusive блокировкой
 int m_exclusive_count; // Счетчик рекурсивных exclusive блокировок
 int m_shared_count; // Количество активных shared блокировок
 std::map<pthread_t, int> m_shared_readers; // Счетчики shared блокировок по потокам

public:
 UGenericMutexGcc() noexcept;
 virtual ~UGenericMutexGcc() noexcept;

 virtual bool shared_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
 virtual bool shared_unlock() noexcept;

 virtual bool exclusive_lock(unsigned timeout=RDK_MUTEX_TIMEOUT);
 virtual bool exclusive_unlock() noexcept;

private:
 pthread_t get_current_thread_id() const;
 UGenericMutexGcc(const UGenericMutexGcc &copy) = delete;
 UGenericMutexGcc& operator = (const UGenericMutexGcc &copy) = delete;
};

class RDK_LIB_TYPE UGenericEventGcc: public UGenericEvent
{
protected:
neosmart::neosmart_event_t Event;

public:
 UGenericEventGcc() noexcept;
 virtual ~UGenericEventGcc() noexcept;

 virtual bool set() noexcept;
 virtual bool reset() noexcept;
 virtual bool wait(unsigned wait_time);


private:
 UGenericEventGcc(const UGenericEventGcc &copy);
 UGenericEventGcc& operator = (const UGenericEventGcc &copy);
};

// Глобальная константа для "пустого" pthread_t (используется для проверки инициализации)
// Должна быть объявлена перед использованием в конструкторе
static const pthread_t g_empty_pthread = {0};

// Проверяет, является ли pthread_t "пустым" (не инициализированным)
static bool is_pthread_empty(const pthread_t& pt)
{
 return pthread_equal(pt, g_empty_pthread) != 0;
}

UGenericMutexGcc::UGenericMutexGcc() noexcept
 : m_exclusive_count(0), m_shared_count(0)
{
 // Инициализируем pthread_t как "пустой" (копируем заранее обнуленный)
 m_exclusive_owner = g_empty_pthread;
 
 pthread_mutexattr_t attr;
 pthread_mutexattr_init(&attr);
 pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
 
 int result = pthread_mutex_init(&m_mutex, &attr);
 pthread_mutexattr_destroy(&attr);
 
 if(result == 0)
 {
  result = pthread_cond_init(&m_condition, nullptr);
 }
 
 // Если инициализация не удалась, это критическая ошибка системы
 if(result != 0)
 {
  std::abort(); // Критическая ошибка - прерываем выполнение
 }
}

UGenericMutexGcc::~UGenericMutexGcc() noexcept
{
 pthread_cond_destroy(&m_condition);
 pthread_mutex_destroy(&m_mutex);
}

pthread_t UGenericMutexGcc::get_current_thread_id() const
{
 return pthread_self();
}

bool UGenericMutexGcc::shared_lock(unsigned timeout)
{
 pthread_t thread_id = get_current_thread_id();
 
 if(pthread_mutex_lock(&m_mutex) != 0)
  return false;
 
 // Если текущий поток уже имеет exclusive блокировку, разрешаем shared
 // (рекурсивная блокировка, как в Qt версии)
 if(!is_pthread_empty(m_exclusive_owner) && pthread_equal(m_exclusive_owner, thread_id) && m_exclusive_count > 0)
 {
  m_shared_count++;
  m_shared_readers[thread_id]++;
  pthread_mutex_unlock(&m_mutex);
  return true;
 }
 
 // Ждем, пока не будет exclusive блокировки (только если она не принадлежит текущему потоку)
 struct timespec abs_time;
 bool use_timeout = (timeout != RDK_MUTEX_TIMEOUT);
 
 if(use_timeout)
 {
  if(clock_gettime(CLOCK_REALTIME, &abs_time) != 0)
  {
   pthread_mutex_unlock(&m_mutex);
   return false;
  }
  abs_time.tv_sec += timeout / 1000;
  long nsec_add = (timeout % 1000) * 1000000L;
  abs_time.tv_nsec += nsec_add;
  if(abs_time.tv_nsec >= 1000000000L)
  {
   abs_time.tv_sec += abs_time.tv_nsec / 1000000000L;
   abs_time.tv_nsec = abs_time.tv_nsec % 1000000000L;
  }
 }
 
 // Ждем, пока exclusive блокировка не будет освобождена (если она принадлежит другому потоку)
 while(m_exclusive_count > 0 && (is_pthread_empty(m_exclusive_owner) || !pthread_equal(m_exclusive_owner, thread_id)))
 {
  if(use_timeout)
  {
   int res = pthread_cond_timedwait(&m_condition, &m_mutex, &abs_time);
   if(res == ETIMEDOUT)
   {
    pthread_mutex_unlock(&m_mutex);
    return false;
   }
   else if(res != 0)
   {
    pthread_mutex_unlock(&m_mutex);
    return false;
   }
  }
  else
  {
   pthread_cond_wait(&m_condition, &m_mutex);
  }
 }
 
 m_shared_count++;
 m_shared_readers[thread_id]++;
 pthread_mutex_unlock(&m_mutex);
 return true;
}

bool UGenericMutexGcc::shared_unlock() noexcept
{
 pthread_t thread_id = get_current_thread_id();
 
 if(pthread_mutex_lock(&m_mutex) != 0)
  return false;
 
 auto it = m_shared_readers.find(thread_id);
 if(it == m_shared_readers.end() || it->second <= 0)
 {
  pthread_mutex_unlock(&m_mutex);
  return false;
 }
 
 it->second--;
 if(it->second == 0)
 {
  m_shared_readers.erase(it);
 }
 m_shared_count--;
 
 pthread_cond_broadcast(&m_condition);
 pthread_mutex_unlock(&m_mutex);
 return true;
}

bool UGenericMutexGcc::exclusive_lock(unsigned timeout)
{
 pthread_t thread_id = get_current_thread_id();
 
 if(pthread_mutex_lock(&m_mutex) != 0)
  return false;
 
 // Рекурсивная блокировка: если текущий поток уже владеет exclusive блокировкой
 if(!is_pthread_empty(m_exclusive_owner) && pthread_equal(m_exclusive_owner, thread_id))
 {
  m_exclusive_count++;
  pthread_mutex_unlock(&m_mutex);
  return true;
 }
 
 // Проверяем, есть ли у текущего потока shared блокировки
 auto shared_it = m_shared_readers.find(thread_id);
 int current_thread_shared_count = (shared_it != m_shared_readers.end()) ? shared_it->second : 0;
 
 // Если у текущего потока есть shared блокировки, нужно проверить, есть ли другие блокировки
 // Если только текущий поток имеет shared блокировки, можно преобразовать их в exclusive
 bool has_other_locks = false;
 if(m_exclusive_count > 0)
 {
  // Есть exclusive блокировка от другого потока
  has_other_locks = true;
 }
 else if(m_shared_count > current_thread_shared_count)
 {
  // Есть shared блокировки от других потоков
  has_other_locks = true;
 }
 
 // Если нет других блокировок и у текущего потока есть shared блокировки, преобразуем их
 if(!has_other_locks && current_thread_shared_count > 0)
 {
  // Удаляем shared блокировки текущего потока
  m_shared_count -= current_thread_shared_count;
  m_shared_readers.erase(shared_it);
  
  // Устанавливаем exclusive блокировку
  m_exclusive_owner = thread_id;
  m_exclusive_count = 1;
  pthread_mutex_unlock(&m_mutex);
  return true;
 }
 
 // Ждем, пока не будет других блокировок
 struct timespec abs_time;
 bool use_timeout = (timeout != RDK_MUTEX_TIMEOUT);
 
 if(use_timeout)
 {
  if(clock_gettime(CLOCK_REALTIME, &abs_time) != 0)
  {
   pthread_mutex_unlock(&m_mutex);
   return false;
  }
  abs_time.tv_sec += timeout / 1000;
  long nsec_add = (timeout % 1000) * 1000000L;
  abs_time.tv_nsec += nsec_add;
  if(abs_time.tv_nsec >= 1000000000L)
  {
   abs_time.tv_sec += abs_time.tv_nsec / 1000000000L;
   abs_time.tv_nsec = abs_time.tv_nsec % 1000000000L;
  }
 }
 
 // Ждем, пока не будет других блокировок (исключая shared блокировки текущего потока)
 while(m_exclusive_count > 0 || (m_shared_count > current_thread_shared_count))
 {
  if(use_timeout)
  {
   int res = pthread_cond_timedwait(&m_condition, &m_mutex, &abs_time);
   if(res == ETIMEDOUT)
   {
    pthread_mutex_unlock(&m_mutex);
    return false;
   }
   else if(res != 0)
   {
    pthread_mutex_unlock(&m_mutex);
    return false;
   }
  }
  else
  {
   pthread_cond_wait(&m_condition, &m_mutex);
  }
  
  // Пересчитываем количество shared блокировок текущего потока (на случай изменений)
  shared_it = m_shared_readers.find(thread_id);
  current_thread_shared_count = (shared_it != m_shared_readers.end()) ? shared_it->second : 0;
 }
 
 // Если у текущего потока были shared блокировки, удаляем их
 if(current_thread_shared_count > 0)
 {
  m_shared_count -= current_thread_shared_count;
  m_shared_readers.erase(thread_id);
 }
 
 m_exclusive_owner = thread_id;
 m_exclusive_count = 1;
 pthread_mutex_unlock(&m_mutex);
 return true;
}

bool UGenericMutexGcc::exclusive_unlock() noexcept
{
 pthread_t thread_id = get_current_thread_id();
 
 if(pthread_mutex_lock(&m_mutex) != 0)
  return false;
 
 // Проверяем, что текущий поток владеет блокировкой
 if(is_pthread_empty(m_exclusive_owner) || !pthread_equal(m_exclusive_owner, thread_id) || m_exclusive_count <= 0)
 {
  pthread_mutex_unlock(&m_mutex);
  return false;
 }
 
 m_exclusive_count--;
 if(m_exclusive_count == 0)
 {
  m_exclusive_owner = g_empty_pthread;
  pthread_cond_broadcast(&m_condition);
 }
 
 pthread_mutex_unlock(&m_mutex);
 return true;
}



UGenericEventGcc::UGenericEventGcc() noexcept
{
 Event = neosmart::CreateEvent(true, true);
}

UGenericEventGcc::~UGenericEventGcc() noexcept
{
 if(Event)
 {
  neosmart::DestroyEvent(Event);
  Event = nullptr;
 }
}

bool UGenericEventGcc::set() noexcept
{
 if(!Event)
  return false;
 neosmart::SetEvent(Event);
 return true;
}

bool UGenericEventGcc::reset() noexcept
{
 if(!Event)
  return false;
 neosmart::ResetEvent(Event);
 return true;
}

bool UGenericEventGcc::wait(unsigned wait_time)
{
 if(!Event)
  return false;
 if(neosmart::WaitForEvent(Event, wait_time) == WAIT_TIMEOUT)
  return false;
 return true;
}

UGenericEventGcc::UGenericEventGcc(const UGenericEventGcc &copy) = delete;

UGenericEventGcc& UGenericEventGcc::operator = (const UGenericEventGcc &copy) = delete;

// ---------------------------------------------------------------------------
UGenericMutex* UCreateMutex(void)
{
 return new UGenericMutexGcc;
}

void UDestroyMutex(UGenericMutex* mutex)
{
 if(mutex)
  delete mutex;
}

UGenericEvent* UCreateEvent(void)
{
 return new UGenericEventGcc;
}

void UDestroyEvent(UGenericEvent* event)
{
 if(event)
  delete event;
}

#endif // _WIN32

#endif // UGENERIC_MUTEX_GCC_CPP

