#ifndef UELockPtrH
#define UELockPtrH

#include "UEPtr.h"
#include "../System/UGenericMutex.h"

namespace RDK {

template<typename T>
class UELockPtr: protected UEPtr<T>
{
/// Мьютекс для блокировки
UGenericMutex* Mutex;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
private:
UELockPtr(void);
UELockPtr(const UEPtr<T> &p);
UELockPtr(const T* p);
UELockPtr(UEPtr<T> &p);
UELockPtr(T* p);
public:
explicit UELockPtr(UGenericMutex* mutex);
UELockPtr(UGenericMutex* mutex, T* pdata);
UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata);
UELockPtr(UGenericMutex* mutex, T* pdata, unsigned timeout);
UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata, unsigned timeout);
UELockPtr(const UELockPtr<T> &p);
//UELockPtr(UELockPtr<T> const &p);
virtual ~UELockPtr(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool operator ! (void) const;

operator bool (void) const;

T* operator -> (void) const;

T& operator * (void);

T* Get(void) const;

bool operator == (const T *p) const
{ return this->PData == p; };

bool operator != (const T *p) const
{ return this->PData != p; };

private:
UELockPtr<T>& operator = (const UELockPtr<T> &p);
UEPtr<T>& operator = (const UEPtr<T> &p);
UEPtr<T>& operator = (const T *p);
UEPtr<T>& operator = (UEPtr<T> &p);
UEPtr<T>& operator = (T *p);

//operator T* (void) const;
// --------------------------

// --------------------------
// Методы
// --------------------------
/// Принудительное отключение обертки от данных без снятия блокировки
void ForceForget(void);
// --------------------------
};

// --------------------------
// Конструкторы и деструкторы
// --------------------------
/*
template<typename T>
UELockPtr<T>::UELockPtr(void)
 : Mutex(0)
{

} */

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex)
 : Mutex(mutex)
{
 if(Mutex)
  Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, T* pdata)
 : UEPtr<T>(pdata), Mutex(mutex)
{
 if(Mutex)
  Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata)
 : UEPtr<T>(pdata.Get()), Mutex(mutex)
{
 if(Mutex)
  Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, T* pdata, unsigned timeout)
 : UEPtr<T>(pdata), Mutex(mutex)
{
 if(Mutex)
 {
  if(!Mutex->exclusive_lock(timeout))
   this->PData=0;
 }
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata, unsigned timeout)
 : UEPtr<T>(pdata.Get()), Mutex(mutex)
{
 if(Mutex)
 {
  if(!Mutex->exclusive_lock(timeout))
   this->PData=0;
 }
}

template<typename T>
UELockPtr<T>::UELockPtr(const UELockPtr<T> &p)
 : UEPtr<T>(p), Mutex(p.Mutex)
{
 const_cast<UELockPtr<T>&>(p).ForceForget();
}

template<typename T>
UELockPtr<T>::~UELockPtr(void)
{
 if(Mutex)
  Mutex->exclusive_unlock();
 Mutex=0;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
template<typename T>
T* UELockPtr<T>::Get(void) const
{
 return this->PData;
}

template<typename T>
bool UELockPtr<T>::operator ! (void) const
{ return (this->PData)?false:true; };

template<typename T>
UELockPtr<T>::operator bool (void) const
{ return (this->PData)?true:false; };


template<typename T>
T* UELockPtr<T>::operator -> (void) const
{
 if(!this->PData)
#if defined(__GNUC__)
  throw typename UELockPtr<T>::EUsingZeroPtr();
#else
  throw UELockPtr<T>::EUsingZeroPtr();
#endif

 return this->PData;
};

template<typename T>
T& UELockPtr<T>::operator * (void)
{
 if(!this->PData)
#if defined(__GNUC__)
  throw typename UELockPtr<T>::EUsingZeroPtr();
#else
  throw UELockPtr<T>::EUsingZeroPtr();
#endif

 return *this->PData;
};

template<typename T>
UELockPtr<T>& UELockPtr<T>::operator = (const UELockPtr<T> &p)
{
 this->PData=p;
 Mutex=p.Mutex;
 p.ForceForget();
 return *this;
};
				  /*
template<typename T>
UELockPtr<T>::UELockPtr(UELockPtr<T> const &p)
 : UEPtr<T>(p), Mutex(p.Mutex)
{
 const_cast<UELockPtr<T>&>(p).ForceForget();
}                   */
// --------------------------

// --------------------------
// Методы
// --------------------------
/// Принудительное отключение обертки от данных без снятия блокировки
template<typename T>
void UELockPtr<T>::ForceForget(void)
{
 this->PData=0;
 Mutex=0;
}
// --------------------------

// =============================================================================
// UESharedLockPtr - RAII wrapper for read-only (shared) mutex locks
// Allows multiple readers to access data simultaneously
// =============================================================================
template<typename T>
class UESharedLockPtr: protected UEPtr<T>
{
/// Мьютекс для блокировки
UGenericMutex* Mutex;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
private:
UESharedLockPtr(void);
UESharedLockPtr(const UEPtr<T> &p);
UESharedLockPtr(const T* p);
UESharedLockPtr(UEPtr<T> &p);
UESharedLockPtr(T* p);
public:
explicit UESharedLockPtr(UGenericMutex* mutex);
UESharedLockPtr(UGenericMutex* mutex, T* pdata);
UESharedLockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata);
UESharedLockPtr(UGenericMutex* mutex, T* pdata, unsigned timeout);
UESharedLockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata, unsigned timeout);
UESharedLockPtr(const UESharedLockPtr<T> &p);
virtual ~UESharedLockPtr(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
bool operator ! (void) const;

operator bool (void) const;

const T* operator -> (void) const;

const T& operator * (void) const;

const T* Get(void) const;

bool operator == (const T *p) const
{ return this->PData == p; };

bool operator != (const T *p) const
{ return this->PData != p; };

private:
UESharedLockPtr<T>& operator = (const UESharedLockPtr<T> &p);
UEPtr<T>& operator = (const UEPtr<T> &p);
UEPtr<T>& operator = (const T *p);
UEPtr<T>& operator = (UEPtr<T> &p);
UEPtr<T>& operator = (T *p);

// --------------------------
// Методы
// --------------------------
/// Принудительное отключение обертки от данных без снятия блокировки
void ForceForget(void);
// --------------------------
};

// --------------------------
// Конструкторы и деструкторы UESharedLockPtr
// --------------------------
template<typename T>
UESharedLockPtr<T>::UESharedLockPtr(UGenericMutex* mutex)
 : Mutex(mutex)
{
 if(Mutex)
  Mutex->shared_lock();
}

template<typename T>
UESharedLockPtr<T>::UESharedLockPtr(UGenericMutex* mutex, T* pdata)
 : UEPtr<T>(pdata), Mutex(mutex)
{
 if(Mutex)
  Mutex->shared_lock();
}

template<typename T>
UESharedLockPtr<T>::UESharedLockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata)
 : UEPtr<T>(pdata.Get()), Mutex(mutex)
{
 if(Mutex)
  Mutex->shared_lock();
}

template<typename T>
UESharedLockPtr<T>::UESharedLockPtr(UGenericMutex* mutex, T* pdata, unsigned timeout)
 : UEPtr<T>(pdata), Mutex(mutex)
{
 if(Mutex)
 {
  if(!Mutex->shared_lock(timeout))
   this->PData=0;
 }
}

template<typename T>
UESharedLockPtr<T>::UESharedLockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata, unsigned timeout)
 : UEPtr<T>(pdata.Get()), Mutex(mutex)
{
 if(Mutex)
 {
  if(!Mutex->shared_lock(timeout))
   this->PData=0;
 }
}

template<typename T>
UESharedLockPtr<T>::UESharedLockPtr(const UESharedLockPtr<T> &p)
 : UEPtr<T>(p), Mutex(p.Mutex)
{
 const_cast<UESharedLockPtr<T>&>(p).ForceForget();
}

template<typename T>
UESharedLockPtr<T>::~UESharedLockPtr(void)
{
 if(Mutex)
  Mutex->shared_unlock();
 Mutex=0;
}
// --------------------------

// --------------------------
// Операторы UESharedLockPtr
// --------------------------
template<typename T>
const T* UESharedLockPtr<T>::Get(void) const
{
 return this->PData;
}

template<typename T>
bool UESharedLockPtr<T>::operator ! (void) const
{ return (this->PData)?false:true; };

template<typename T>
UESharedLockPtr<T>::operator bool (void) const
{ return (this->PData)?true:false; };

template<typename T>
const T* UESharedLockPtr<T>::operator -> (void) const
{
 if(!this->PData)
#if defined(__GNUC__)
  throw typename UESharedLockPtr<T>::EUsingZeroPtr();
#else
  throw UESharedLockPtr<T>::EUsingZeroPtr();
#endif

 return this->PData;
};

template<typename T>
const T& UESharedLockPtr<T>::operator * (void) const
{
 if(!this->PData)
#if defined(__GNUC__)
  throw typename UESharedLockPtr<T>::EUsingZeroPtr();
#else
  throw UESharedLockPtr<T>::EUsingZeroPtr();
#endif

 return *this->PData;
};

template<typename T>
UESharedLockPtr<T>& UESharedLockPtr<T>::operator = (const UESharedLockPtr<T> &p)
{
 this->PData=p;
 Mutex=p.Mutex;
 p.ForceForget();
 return *this;
};
// --------------------------

// --------------------------
// Методы UESharedLockPtr
// --------------------------
template<typename T>
void UESharedLockPtr<T>::ForceForget(void)
{
 this->PData=0;
 Mutex=0;
}
// --------------------------
}

#endif
