#ifndef UELockVarH
#define UELockVarH

#include "../System/UGenericMutex.h"

namespace RDK {

template<typename T>
class UELockVar
{
/// ������
T Data;

/// ������� ��� ����������
UGenericMutex* Mutex;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UELockVar(void);
UELockVar(const T pdata);
UELockVar(const UELockVar<T> &pdata);
virtual ~UELockVar(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
T Get(void) const;
// --------------------------

// --------------------------
// ���������
// --------------------------
operator T (void) const;

UELockVar<T>& operator = (const UELockVar<T> &p);

UELockVar<T>& operator = (const T &p);

UELockVar<T>& operator += (const T &p);

UELockVar<T>& operator ++ (int i);
UELockVar<T>& operator -- (int i);
UELockVar<T>& operator ++ (void);
UELockVar<T>& operator -- (void);

bool operator == (const T &p) const;
bool operator != (const T &p) const;

//friend T operator + (const UELockVar<T> &p1, const T &p2);
// --------------------------
};

// --------------------------
// ������������ � �����������
// --------------------------
template<typename T>
UELockVar<T>::UELockVar(void)
 : Mutex(UCreateMutex())
{

}

template<typename T>
UELockVar<T>::UELockVar(const T pdata)
 : Mutex(UCreateMutex()), Data(pdata)
{
// new UGenericMutexLocker(mutex)
}

template<typename T>
UELockVar<T>::UELockVar(const UELockVar<T> &pdata)
 : Mutex(UCreateMutex()),Data(pdata.Get())
{

}

template<typename T>
UELockVar<T>::~UELockVar(void)
{
 if(Mutex)
 {
  UDestroyMutex(Mutex);
  Mutex=0;
 }
}
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
template<typename T>
T UELockVar<T>::Get(void) const
{
 UGenericMutexLocker Locker(Mutex);
 return Data;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
template<typename T>
UELockVar<T>::operator T (void) const
{
 UGenericMutexLocker Locker(Mutex);
 return Data;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator = (const UELockVar<T> &p)
{
 UGenericMutexLocker Locker(Mutex);
 Data=p.Get();

 return *this;
};

template<typename T>
UELockVar<T>& UELockVar<T>::operator = (const T &p)
{
 UGenericMutexLocker Locker(Mutex);
 Data=p;

 return *this;
};


template<typename T>
UELockVar<T>& UELockVar<T>::operator += (const T &p)
{
 UGenericMutexLocker Locker(Mutex);
 Data+=p;

 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator ++ (int i)
{
 UGenericMutexLocker Locker(Mutex);
 Data++;
 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator -- (int i)
{
 UGenericMutexLocker Locker(Mutex);
 Data--;
 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator ++ (void)
{
 UGenericMutexLocker Locker(Mutex);
 Data++;
 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator -- (void)
{
 UGenericMutexLocker Locker(Mutex);
 Data--;
 return *this;
}

template<typename T>
bool UELockVar<T>::operator == (const T &p) const
{
 UGenericMutexLocker Locker(Mutex);
 if(Data == p)
  return true;
 return false;
}

template<typename T>
bool UELockVar<T>::operator != (const T &p) const
{
 UGenericMutexLocker Locker(Mutex);
 if(Data != p)
  return true;
 return false;
}


/*
template<typename T>
T operator + (const UELockVar<T> &p1, const T &p2)
{
 return p1.Get()+p2;
}
  */
// --------------------------


}

#endif
