#ifndef UEPTR_H
#define UEPTR_H

#include "../Utilities/UPtr.h"
//#include "UEnvException.h"

namespace RDK {

/* ��������� c ��������� ������ (������ ������ shared_ptr) */
template<typename T>
class UESharedPtr
{
public: // ����������
class UFEUsingZeroPtr: public T::IException {};

//////////////////////////
protected:
T* PData;

// ������� ������
long* Counter;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UESharedPtr(void);
UESharedPtr(T* pdata);
UESharedPtr(const UESharedPtr<T> &p);
template<typename Y> UESharedPtr(UESharedPtr<Y> &p)
: PData(dynamic_cast<T*>(p.Get())),
  Counter(p.GetPCounter())
{
 if(Counter)
  ++(*Counter);
};
~UESharedPtr(void);
// --------------------------

// --------------------------
// ���������
// --------------------------
UESharedPtr<T>& operator = (const UESharedPtr<T> &p);

UESharedPtr<T>& operator = (T *p);

//UESharedPtr<T>& operator = (UEPtr<T> &p);

//UESharedPtr<T>& operator = (UEPtr<T> &p);

bool operator == (const UESharedPtr<T> &p);

bool operator != (const UESharedPtr<T>&p);

//bool operator == (const T *p);

//bool operator != (const T *p);

bool operator ! (void) const;

T* operator -> (void) const;

T& operator * (void);

operator T* (void) const;


// ���������� ����� ������ �� ������
long GetCounter(void) const;

long* GetPCounter(void) const;

// ������� ��������� �� ��������  �������
void Clear(void);

T* Get(void) const;

// �������������� ����� ���������
UESharedPtr<T>& Init(long* counter, T* pdata);
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
protected:
// --------------------------
};

/* ������� ��������� (������ ������ weak_ptr) */
template<typename T>
class UEPtr
{
protected:
T* PData;

public: // ����������
class UFEUsingZeroPtr: public T::IException {};

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UEPtr(void);
UEPtr(T* pdata);
UEPtr(const UEPtr<T> &p);
UEPtr(const UESharedPtr<T> &p);
//template<typename Y> UEPtr(Y* p) : PData(p){};
template<typename Y> UEPtr(UEPtr<Y> &p) : /*PData(p.Get()){};*/PData(dynamic_cast<T*>(p.Get())){};
template<typename Y> UEPtr(UESharedPtr<Y> &p) : /*PData(p.Get()){};*/PData(dynamic_cast<T*>(p.Get())){};
~UEPtr(void);
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
T* Get(void) const;
// --------------------------

// --------------------------
// ���������
// --------------------------
UEPtr<T>& operator = (UEPtr<T> &p);

UEPtr<T>& operator = (UESharedPtr<T> &p);
 /*
template<typename Y> UEPtr<T>& operator = (UESharedPtr<Y> &p)
{
 PData=p.get();
 return *this;
}  */

UEPtr<T>& operator = (T *p);

//bool operator == (const UEPtr<T> &p);

//bool operator != (const UEPtr<T> &p);

//bool operator == (const T *p);

//bool operator != (const T *p);

bool operator ! (void) const;

T* operator -> (void) const;

T& operator * (void);

operator T* (void) const;
// --------------------------
};




/* class UEPtr */
// --------------------------
// ������������ � �����������
// --------------------------
template<typename T>
UEPtr<T>::UEPtr(void)
: PData(0)
{
};

template<typename T>
UEPtr<T>::UEPtr(T* pdata)
: PData(pdata)
{
// PData=pdata;
};

template<typename T>
UEPtr<T>::UEPtr(const UEPtr<T> &p)
: PData(p.PData)
{
// PData=p.PData;
};

template<typename T>
UEPtr<T>::UEPtr(const UESharedPtr<T> &p)
: PData(p.Get())
{

}
/*
template<typename T, typename Y>
UEPtr<T>::UEPtr(Y* p)
 : PData(p)
{
} */


template<typename T>
UEPtr<T>::~UEPtr(void)
{
// PData=0;
};
// --------------------------

// --------------------------
// ������ ������� � ������
// --------------------------
template<typename T>
T* UEPtr<T>::Get(void) const
{
 return PData;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
template<typename T>
UEPtr<T>& UEPtr<T>::operator = (UEPtr<T> &p)
{
 PData=p.PData;
 return *this;
};

template<typename T>
UEPtr<T>& UEPtr<T>::operator = (UESharedPtr<T> &p)
{
 PData=p.get();
 return *this;
}


template<typename T>
UEPtr<T>& UEPtr<T>::operator = (T *p)
{
 PData=p;
 return *this;
};
/*
template<typename T>
bool UEPtr<T>::operator == (const UEPtr<T> &p)
{ return (PData == p.PData)?true:false; };

template<typename T>
bool UEPtr<T>::operator != (const UEPtr<T> &p)
{ return (PData != p.PData)?true:false; };

template<typename T>
bool UEPtr<T>::operator == (const T *p)
{ return (PData == p)?true:false; };

template<typename T>
bool UEPtr<T>::operator != (const T *p)
{ return (PData != p)?true:false; };
*/
template<typename T>
bool UEPtr<T>::operator ! (void) const
{ return (PData)?false:true; };

template<typename T>
T* UEPtr<T>::operator -> (void) const
{
 return PData;
};

template<typename T>
T& UEPtr<T>::operator * (void)
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return *PData;
};

template<typename T>
UEPtr<T>::operator T* (void) const
{
 return PData;
}
// --------------------------


// Class UESharedPtr
// --------------------------
// ������������ � �����������
// --------------------------
template<typename T>
UESharedPtr<T>::UESharedPtr(void)
: Counter(new long(1)),PData(0)
{
};

template<typename T>
UESharedPtr<T>::UESharedPtr(T* pdata)
: PData(pdata),
  Counter(new long(1))
{
};

template<typename T>
UESharedPtr<T>::UESharedPtr(const UESharedPtr<T> &p)
: PData(p),
  Counter(p.Counter)
{
 if(Counter)
  ++(*Counter);
};

template<typename T>
UESharedPtr<T>::~UESharedPtr(void)
{
 if(Counter && --(*Counter) == 0)
 {
  delete Counter;
  Counter=0;
  if(PData)
   delete PData;
 }
};
// --------------------------

// --------------------------
// ���������
// --------------------------
template<typename T>
UESharedPtr<T>& UESharedPtr<T>::operator = (const UESharedPtr<T> &p)
{
 if(Counter)
 {
  if(--(*Counter) == 0)
  {
   delete Counter;
   Counter=0;
   if(PData)
	delete PData;
  }
 }
 PData=p.PData;

// if(PData)
 {
  Counter=p.Counter;
  ++(*Counter);
 }
// else
//  Counter=new long(1);

 return *this;
};

template<typename T>
UESharedPtr<T>& UESharedPtr<T>::operator = (T *p)
{
 if(Counter)
 {
  if(--(*Counter) == 0)
  {
   delete Counter;
   Counter=0;
   if(PData)
	delete PData;
  }
 }
 PData=p;

 if(PData)
  Counter=new long(1);
 else
  Counter=new long(1);

 return *this;
};

template<typename T>
T& UESharedPtr<T>::operator * (void)
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return *PData;
};

template<typename T>
bool UESharedPtr<T>::operator == (const UESharedPtr<T> &p)
{ return (PData == p.PData)?true:false; };

template<typename T>
bool UESharedPtr<T>::operator != (const UESharedPtr<T> &p)
{ return (PData != p.PData)?true:false; };
/*
template<typename T>
bool UESharedPtr<T>::operator == (const T *p)
{ return (PData == p)?true:false; };

template<typename T>
bool UESharedPtr<T>::operator != (const T *p)
{ return (PData != p)?true:false; };
*/
template<typename T>
bool UESharedPtr<T>::operator ! (void) const
{ return (PData)?false:true; };

template<typename T>
T* UESharedPtr<T>::operator -> (void) const
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return PData;
};


template<typename T>
UESharedPtr<T>::operator T* (void) const
{
 return PData;
}


// ���������� ����� ������ �� ������
template<typename T>
long UESharedPtr<T>::GetCounter(void) const
{ return *Counter; };

template<typename T>
long* UESharedPtr<T>::GetPCounter(void) const
{ return Counter; };

// ������� ��������� �� ��������  �������
template<typename T>
void UESharedPtr<T>::Clear(void)
{
 if(Counter && --(*Counter) == 0)
 {
  delete Counter;
  Counter=0;
  if(PData)
   delete PData;
 }
}

template<typename T>
T* UESharedPtr<T>::Get(void) const
{
 return PData;
}


// �������������� ����� ���������
template<typename T>
UESharedPtr<T>& UESharedPtr<T>::Init(long* counter, T* pdata)
{
 Clear();
 Counter=counter;
 ++(*Counter);
 PData=pdata;
 return *this;
}
// --------------------------


// -----------------------------------------------------------
// ����������  �����
// -----------------------------------------------------------
template<class T, class U>
UESharedPtr<T> static_pointer_cast(UESharedPtr<U> const & r)
{
 UESharedPtr<T> result;
 result.Init(r.GetPCounter(),static_cast<T*>(r.Get()));
// result.reset(static_cast<T*>(r.get()));
 return result;
}

template<class T, class U>
UESharedPtr<T> const_pointer_cast(UESharedPtr<U> const & r)
{
 UESharedPtr<T> result;
 result.Init(r.GetPCounter(),const_cast<T*>(r.Get()));
 return result;
}

template<class T, class U>
UESharedPtr<T> dynamic_pointer_cast(UESharedPtr<U> const & r)
{
 UESharedPtr<T> result;
 result.Init(r.GetPCounter(),dynamic_cast<T*>(r.Get()));
 //result.reset(dynamic_cast<T*>(r.get()));
 return result;
}

template<class T, class U>
UEPtr<T> static_pointer_cast(UEPtr<U> const & r)
{
 UEPtr<T> result;
 result=static_cast<T*>(r.Get());
 return result;
}

template<class T, class U>
UEPtr<T> const_pointer_cast(UEPtr<U> const & r)
{
 UEPtr<T> result;
 result=const_cast<T*>(r.Get());
 return result;
}

template<class T, class U>
UEPtr<T> dynamic_pointer_cast(UEPtr<U> const & r)
{
 UEPtr<T> result;
 result=dynamic_cast<T*>(r.Get());
 return result;
}
// -----------------------------------------------------------

}

#endif

