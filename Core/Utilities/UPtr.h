#ifndef UPTR_H
#define UPTR_H

#include "UException.h"

namespace RDK {

/* ������� ��������� (������ ������ weak_ptr) */
template<typename T>
class UPtr
{
protected:
T* PData;

public: // ����������
class UFEUsingZeroPtr: public UFatalException {};

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UPtr(void)
: PData(0)
{
};

UPtr(T* pdata)
: PData(0)
{
 PData=pdata;
};

UPtr(const UPtr<T> &p)
: PData(0)
{
 PData=p.PData;
};

~UPtr(void)
{
 PData=0;
};
// --------------------------

// --------------------------
// ���������
// --------------------------
UPtr<T>& operator = (UPtr<T> &p)
{
 PData=p.PData;
 return *this;
};

UPtr<T>& operator = (T *p)
{
 PData=p;
 return *this;
};

bool operator == (const UPtr<T> &p)
{ return (PData == p.PData)?true:false; };

bool operator != (const UPtr<T> &p)
{ return (PData != p.PData)?true:false; };

bool operator == (const T *p)
{ return (PData == p)?true:false; };

bool operator != (const T *p)
{ return (PData != p)?true:false; };

operator bool (void)
{ return (PData)?true:false; };

T* operator -> (void)
{
 return PData;
};

T& operator * (void)
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return *PData;
};

// --------------------------
};

/* ��������� c ��������� ������ (������ ������ shared_ptr) */
template<typename T>
class USharedPtr: public UPtr<T>
{
// ������� ������
long* Counter;

public: // ����������
//class UFEZeroPtr: public UFatalException {};

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
USharedPtr(void)
: Counter(new long(0))
{
};

USharedPtr(T* pdata)
: UPtr<T>(pdata),
  Counter(new long(1))
{
};

USharedPtr(const USharedPtr<T> &p)
: UPtr<T>(p.PData),
  Counter(p.Counter)
{
 ++(*Counter);
};

~USharedPtr(void)
{
 if(Counter && --(*Counter) <= 0)
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
USharedPtr<T>& operator = (const USharedPtr<T> &p)
{
 if(Counter)
 {
  if(--(*Counter) <= 0)
  {
   delete Counter;
   Counter=0;
   if(PData)
    delete PData;
  }
 }
 PData=p.PData;

 if(PData)
 {
  Counter=p.Counter;
  ++(*Counter);
 }
 else
  Counter=new long(0);

 return *this;
};

USharedPtr<T>& operator = (T *p)
{
 if(Counter)
 {
  if(--(*Counter) <= 0)
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
  Counter=new long(0);

 return *this;
};


USharedPtr<T>& operator = (UPtr<T> &p)
{
 return operator = (p.PData);
};

				/*
bool operator == (const USharedPtr<T> &p)
{ return (PData == p.PData)?true:false; };

bool operator != (const USharedPtr<T> &p)
{ return (PData != p.PData)?true:false; };

bool operator == (const T *p)
{ return (PData == p)?true:false; };

bool operator != (const T *p)
{ return (PData != p)?true:false; };

operator bool (void)
{ return (PData)?true:false; };

T& operator -> (void)
{
 if(!PData)
  throw UFEZeroPtr;

 return *PData;
};

T& operator * (void)
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return *PData;
};                */

// ���������� ����� ������ �� ������
long GetCounter(void) const
{ return *Counter;};
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
protected:
// --------------------------

};


}

#endif

