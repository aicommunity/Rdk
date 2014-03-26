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

/// Блокировщик
UGenericMutexLocker* Locker;

// Счетчик ссылок
long* Counter;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UELockPtr(void);
UELockPtr(UGenericMutex* mutex);
UELockPtr(UGenericMutex* mutex, T* pdata);
UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata);
UELockPtr(const UELockPtr<T> &p);
virtual ~UELockPtr(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
UELockPtr<T>& operator = (const UELockPtr<T> &p);

//UELockPtr<T>& operator = (T *p);
// --------------------------

};

// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<typename T>
UELockPtr<T>::UELockPtr(void)
 : Mutex(0), Locker(0), Counter(new long(1))
{

}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex)
 : Mutex(mutex), Locker(new UGenericMutexLocker(mutex)), Counter(new long(1))
{

}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, T* pdata)
 : UEPtr<T>(pdata), Mutex(mutex), Locker(new UGenericMutexLocker(mutex)), Counter(new long(1))
{

}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata)
 : UEPtr<T>(pdata.operator ->()), Mutex(mutex), Locker(new UGenericMutexLocker(mutex)), Counter(new long(1))
{

}

template<typename T>
UELockPtr<T>::UELockPtr(const UELockPtr<T> &p)
 : UEPtr<T>(p), Mutex(p.Mutex), Locker(p.Locker), Counter(p.Counter)
{
 if(Counter)
  ++(*Counter);
}

template<typename T>
UELockPtr<T>::~UELockPtr(void)
{
 Mutex=0;
 if(Counter && --(*Counter) == 0)
 {
  delete Counter;
  Counter=0;
  if(Locker)
  {
   delete Locker;
   Locker=0;
  }
 }
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
template<typename T>
UELockPtr<T>& UELockPtr<T>::operator = (const UELockPtr<T> &p)
{
 if(Counter)
 {
  if(--(*Counter) == 0)
  {
   delete Counter;
   Counter=0;
   if(Locker)
   {
	delete Locker;
	Locker=0;
   }
  }
 }
 PData=p.PData;

 {
  Counter=p.Counter;
  ++(*Counter);
 }

 return *this;
};
/*
template<typename T>
UELockPtr<T>& UELockPtr<T>::operator = (T *p)
{
 if(Counter)
 {
  if(--(*Counter) == 0)
  {
   delete Counter;
   Counter=0;
   if(Locker)
   {
	delete Locker;
	Locker=0;
   }
  }
 }
 PData=p;

 Counter=new long(1);

 return *this;
};  */
// --------------------------


}

#endif
