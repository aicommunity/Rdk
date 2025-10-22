#ifndef UELockPtrH
#define UELockPtrH

#include <memory>
#include "../System/UGenericMutex.h"

namespace RDK {

template<typename T>
class UELockPtr
{
/// ������� ��� ����������
UGenericMutex* Mutex;
/// The underlying shared_ptr
std::shared_ptr<T> PData;

/// Exception class for zero pointer access
class EUsingZeroPtr : public std::exception {
public:
    const char* what() const noexcept override {
        return "UELockPtr: Attempting to use zero pointer";
    }
};

public:
// --------------------------
// ������������ � �����������
// --------------------------
private:
UELockPtr(void);
UELockPtr(const std::shared_ptr<T> &p);
UELockPtr(const T* p);
UELockPtr(std::shared_ptr<T> &p);
UELockPtr(T* p);
public:
explicit UELockPtr(UGenericMutex* mutex);
UELockPtr(UGenericMutex* mutex, T* pdata);
UELockPtr(UGenericMutex* mutex, const std::shared_ptr<T> &pdata);
UELockPtr(UGenericMutex* mutex, T* pdata, unsigned timeout);
UELockPtr(UGenericMutex* mutex, const std::shared_ptr<T> &pdata, unsigned timeout);
UELockPtr(const UELockPtr<T> &p);
//UELockPtr(UELockPtr<T> const &p);
virtual ~UELockPtr(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
bool operator ! (void) const;

operator bool (void) const;

T* operator -> (void) const;

T& operator * (void);

T* Get(void) const;

bool operator == (const T *p) const
{ return this->PData.get() == p; };

bool operator != (const T *p) const
{ return this->PData.get() != p; };

private:
UELockPtr<T>& operator = (const UELockPtr<T> &p);
std::shared_ptr<T>& operator = (const std::shared_ptr<T> &p);
std::shared_ptr<T>& operator = (const T *p);
std::shared_ptr<T>& operator = (std::shared_ptr<T> &p);
std::shared_ptr<T>& operator = (T *p);

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
 if(Mutex)
  Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, T* pdata)
 : Mutex(mutex), PData(pdata)
{
 if(Mutex)
  Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, const std::shared_ptr<T> &pdata)
 : Mutex(mutex), PData(pdata)
{
 if(Mutex)
  Mutex->exclusive_lock();
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, T* pdata, unsigned timeout)
 : Mutex(mutex), PData(pdata)
{
 if(Mutex)
 {
  if(!Mutex->exclusive_lock(timeout))
   this->PData=nullptr;
 }
}

template<typename T>
UELockPtr<T>::UELockPtr(UGenericMutex* mutex, const std::shared_ptr<T> &pdata, unsigned timeout)
 : Mutex(mutex), PData(pdata)
{
 if(Mutex)
 {
  if(!Mutex->exclusive_lock(timeout))
   this->PData=nullptr;
 }
}

template<typename T>
UELockPtr<T>::UELockPtr(const UELockPtr<T> &p)
 : std::shared_ptr<T>(p), Mutex(p.Mutex)
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
 return this->PData.get();
}

template<typename T>
bool UELockPtr<T>::operator ! (void) const
{ return !this->PData; };

template<typename T>
UELockPtr<T>::operator bool (void) const
{ return (bool)this->PData; };


template<typename T>
T* UELockPtr<T>::operator -> (void) const
{
 if(!this->PData)
#if defined(__GNUC__)
  throw typename UELockPtr<T>::EUsingZeroPtr();
#else
  throw UELockPtr<T>::EUsingZeroPtr();
#endif

 return this->PData.get();
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
 : std::shared_ptr<T>(p), Mutex(p.Mutex)
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
