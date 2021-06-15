#ifndef UELockVarH
#define UELockVarH

#include "../System/UGenericMutex.h"

namespace RDK {
///������� ���������������� ���������� � �������� ����� � ����������� �������
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
explicit UELockVar(const T pdata);
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

T* operator -> (void) const;

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
 : Data(pdata), Mutex(UCreateMutex())
{
// new UGenericMutexLocker(mutex)
}

template<typename T>
UELockVar<T>::UELockVar(const UELockVar<T> &pdata)
 : Data(pdata.Get()), Mutex(UCreateMutex())
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
 UGenericMutexSharedLocker Locker(Mutex);
 return Data;
}

// --------------------------

// --------------------------
// ���������
// --------------------------
template<typename T>
UELockVar<T>::operator T (void) const
{
 UGenericMutexSharedLocker Locker(Mutex);
 return Data;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator = (const UELockVar<T> &p)
{
 UGenericMutexExclusiveLocker Locker(Mutex);
 Data=p.Get();

 return *this;
};

template<typename T>
UELockVar<T>& UELockVar<T>::operator = (const T &p)
{
 UGenericMutexExclusiveLocker Locker(Mutex);
 Data=p;

 return *this;
};


template<typename T>
UELockVar<T>& UELockVar<T>::operator += (const T &p)
{
 UGenericMutexExclusiveLocker Locker(Mutex);
 Data+=p;

 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator ++ (int i)
{
 UGenericMutexExclusiveLocker Locker(Mutex);
 Data++;
 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator -- (int i)
{
 UGenericMutexExclusiveLocker Locker(Mutex);
 Data--;
 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator ++ (void)
{
 UGenericMutexExclusiveLocker Locker(Mutex);
 Data++;
 return *this;
}

template<typename T>
UELockVar<T>& UELockVar<T>::operator -- (void)
{
 UGenericMutexExclusiveLocker Locker(Mutex);
 Data--;
 return *this;
}

template<typename T>
bool UELockVar<T>::operator == (const T &p) const
{
 UGenericMutexSharedLocker Locker(Mutex);
 if(Data == p)
  return true;
 return false;
}

template<typename T>
bool UELockVar<T>::operator != (const T &p) const
{
 UGenericMutexSharedLocker Locker(Mutex);
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
