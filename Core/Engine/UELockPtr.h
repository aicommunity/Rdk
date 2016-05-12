#ifndef UELockPtrH
#define UELockPtrH

#include "UEPtr.h"
#include "../System/UGenericMutex.h"

namespace RDK {

template<typename T>
class UELockPtr: public UEPtr<T>
{
/// Мьютекс для блокировки
UGenericMutex* Mutex;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UELockPtr(void);
UELockPtr(UGenericMutex* mutex);
UELockPtr(UGenericMutex* mutex, T* pdata);
UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata);
UELockPtr(UELockPtr<T> &p);
UELockPtr(UELockPtr<T> const &p);
virtual ~UELockPtr(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
public:
UELockPtr<T>& operator = (UELockPtr<T> const &p);
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
template<typename T>
UELockPtr<T>::UELockPtr(void)
 : Mutex(0)
{

}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex)
 : Mutex(mutex)
{
 Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, T* pdata)
 : UEPtr<T>(pdata), Mutex(mutex)
{
 Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata)
 : UEPtr<T>(pdata.Get()), Mutex(mutex)
{
 Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UELockPtr<T> &p)
 : UEPtr<T>(p), Mutex(p.Mutex)
{
 p.PData=0;
 p.Mutex=0;
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
UELockPtr<T>& UELockPtr<T>::operator = (const UELockPtr<T> &p)
{
 this->PData=p;
 Mutex=p.Mutex;
 p.ForceForget();
 return *this;
};

template<typename T>
UELockPtr<T>::UELockPtr(UELockPtr<T> const &p)
 : UEPtr<T>(p), Mutex(p.Mutex)
{
 const_cast<UELockPtr<T>&>(p).ForceForget();
}
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
}

#endif
