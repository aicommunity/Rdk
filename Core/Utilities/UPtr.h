#ifndef UPTR_H
#define UPTR_H

#include "UException.h"

namespace RDK {

/* Простой указатель (слабый аналог weak_ptr) */
template<typename T>
class UPtr
{
protected:
T* PData;

public: // Исключения
class UFEUsingZeroPtr/*: public UFatalException */{};

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPtr(void)
: PData(0)
{
};

explicit UPtr(T* pdata)
: PData(pdata)
{
// PData=pdata;
};

UPtr(const UPtr<T> &p)
: PData(p.PData)
{
// PData=p.PData;
};

virtual ~UPtr(void)
{
 PData=0;
};
// --------------------------

// --------------------------
// Методы доступа
// --------------------------
T* Get(void) const
{
 return PData;
}
// --------------------------

// --------------------------
// Операторы
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

bool operator == (const UPtr<T> &p) const
{ return (PData == p.PData)?true:false; };

bool operator != (const UPtr<T> &p) const
{ return (PData != p.PData)?true:false; };

bool operator == (const T *p) const
{ return (PData == p)?true:false; };

bool operator != (const T *p) const
{ return (PData != p)?true:false; };

operator bool (void) const
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

operator T* (void) const
{
 return PData;
}
// --------------------------
};

/* Указатель c подсчетом ссылок (слабый аналог shared_ptr) */
template<typename T>
class USharedPtr: public UPtr<T>
{
// Счетчик ссылок
long* Counter;

public: // Исключения
//class UFEZeroPtr: public UFatalException {};

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
USharedPtr(void)
: UPtr<T>(0),Counter(0)
{
};

explicit USharedPtr(T* pdata)
: UPtr<T>(pdata),
  Counter(new long(1))
{
};

USharedPtr(const USharedPtr<T> &p)
: UPtr<T>(p.PData),
  Counter(p.Counter)
{
 if(!UPtr<T>::PData)
  Counter=0;
 else
  ++(*Counter);
};

virtual ~USharedPtr(void)
{
 if(!UPtr<T>::PData)
 {
  if(Counter)
   delete Counter; // Заглушка!! Это по идее не нужно
  Counter=0;
 }
 else
 if(Counter && --(*Counter) <= 0)
 {
  delete Counter;
  Counter=0;
  if(UPtr<T>::PData)
   delete UPtr<T>::PData;
 }
};
// --------------------------

// --------------------------
// Операторы
// --------------------------
USharedPtr<T>& operator = (const USharedPtr<T> &p)
{
 if(Counter)
 {
  if(--(*Counter) <= 0)
  {
   delete Counter;
   Counter=0;
   if(UPtr<T>::PData)
   {
	delete UPtr<T>::PData;
	UPtr<T>::PData=0;
   }
  }
 }
 UPtr<T>::PData=p.PData;

 if(UPtr<T>::PData)
 {
  Counter=p.Counter;
  ++(*Counter);
 }
 else
  Counter=0;

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
   if(UPtr<T>::PData)
   {
	delete UPtr<T>::PData;
	UPtr<T>::PData=0;
   }
  }
 }
 UPtr<T>::PData=p;

 if(UPtr<T>::PData)
  Counter=new long(1);
 else
  Counter=0;

 return *this;
};


USharedPtr<T>& operator = (UPtr<T> &p)
{
 return operator = (p.PData);
};


const T* operator -> (void) const
{
 return UPtr<T>::PData;
};


protected:
operator T* (void) const
{
 return UPtr<T>::PData;
}
public:
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

// Возвращает число ссылок на объект
long GetCounter(void) const
{ return *Counter; };

long* GetPCounter(void) const
{ return Counter; };

// Очищает указатель от текущего  объекта
void Clear(void)
{
 if(Counter && --(*Counter) <= 0)
 {
  delete Counter;
  Counter=0;
  if(UPtr<T>::PData)
  {
   delete UPtr<T>::PData;
   UPtr<T>::PData=0;
  }
 }
}
/*
// Инициализирует умный указатель
USharedPtr<T>& Init(long* counter, T* pdata)
{
 Clear();
 Counter=counter;
 ++(*Counter);
 UPtr<T>::PData=pdata;
} */
// --------------------------

// --------------------------
// Скрытые методы управления
// --------------------------
protected:
// --------------------------
};


// Операторы преобразования указателей через динамическое приведение типов
template<typename T, typename T2>
USharedPtr<T>& operator << (USharedPtr<T> &ptr1, const USharedPtr<T2> &ptr2)
{
 ptr1.Init(ptr2.GetPCounter(),dynamic_cast<T*>(ptr2.GetPData()));

 return ptr1;
}

// Операторы преобразования указателей через динамическое приведение типов
template<typename T, typename T2>
USharedPtr<T>& PtrConvert (USharedPtr<T> &ptr1, USharedPtr<T2> &ptr2)
{
 //ptr1=ptr2;
/*if( ptr1.Counter)
 {
  if(--(* ptr1.Counter) <= 0)
  {
   delete  ptr1.Counter;
	ptr1.Counter=0;
   if( ptr1.PData)
	delete  ptr1.PData;
  }
 }
  ptr1.PData= ptr2.PData;

 if(ptr1.PData)
 {
  ptr1.Counter=ptr2.Counter;
  ++(*ptr1.Counter);
 }
 else
  ptr1.Counter=new long(0);

 return *this;
*/
 return ptr1;
}

}

#endif

