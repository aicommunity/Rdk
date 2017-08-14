#ifndef UELockPtrH
#define UELockPtrH

#include "UEPtr.h"
#include "../System/UGenericMutex.h"

namespace RDK {

template<typename T>
class UELockPtr: protected UEPtr<T>
{
/// ������� ��� ����������
UGenericMutex* Mutex;

public:
// --------------------------
// ������������ � �����������
// --------------------------
private:
UELockPtr(void);
UELockPtr(const UEPtr<T> &p);
UELockPtr(const T* p);
UELockPtr(UEPtr<T> &p);
UELockPtr(T* p);
public:
UELockPtr(UGenericMutex* mutex);
UELockPtr(UGenericMutex* mutex, T* pdata);
UELockPtr(UGenericMutex* mutex, const UEPtr<T> &pdata);
UELockPtr(const UELockPtr<T> &p);
//UELockPtr(UELockPtr<T> const &p);
virtual ~UELockPtr(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
bool operator ! (void) const;

T* operator -> (void) const;

T& operator * (void);

T* Get(void) const;

bool operator == (const T *p) const
{ return PData == p; };

bool operator != (const T *p) const
{ return PData != p; };

private:
UELockPtr<T>& operator = (const UELockPtr<T> &p);
UEPtr<T>& operator = (const UEPtr<T> &p);
UEPtr<T>& operator = (const T *p);
UEPtr<T>& operator = (UEPtr<T> &p);
UEPtr<T>& operator = (T *p);

//operator T* (void) const;
// --------------------------

// --------------------------
// ������
// --------------------------
/// �������������� ���������� ������� �� ������ ��� ������ ����������
void ForceForget(void);
// --------------------------
};

// --------------------------
// ������������ � �����������
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
 if(Mutex)
  Mutex->exclusive_lock();
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
// ���������
// --------------------------
template<typename T>
T* UELockPtr<T>::Get(void) const
{
 return PData;
}

template<typename T>
bool UELockPtr<T>::operator ! (void) const
{ return (PData)?false:true; };

template<typename T>
T* UELockPtr<T>::operator -> (void) const
{
 if(!PData)
  throw EUsingZeroPtr();

 return PData;
};

template<typename T>
T& UELockPtr<T>::operator * (void)
{
 if(!PData)
  throw EUsingZeroPtr();

 return *PData;
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
// ������
// --------------------------
/// �������������� ���������� ������� �� ������ ��� ������ ����������
template<typename T>
void UELockPtr<T>::ForceForget(void)
{
 this->PData=0;
 Mutex=0;
}
// --------------------------
}

#endif
