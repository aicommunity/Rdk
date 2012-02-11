#ifndef UShareH
#define UShareH

#include "UAContainer.h"

namespace RDK {

template<typename T,class MainOwnerT>
class UVShare
{
protected: // Данные
// Указатель на расшаренные данные
T* Pointer;

// Локальная копия данных если указатель не доступен
T& RData;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVShare(T &rdata)
: Pointer(0), RData(rdata)
{
}

virtual ~UVShare(void)
{
 Pointer=0;
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
const T& Get(void) const
{
 return (Pointer)?*Pointer:RData;
}

void Set(const T& data)
{
 if(Pointer)
  *Pointer=data;
 else
  RData=data;
}

const T& GetLocal(void) const
{
 return RData;
}

void SetLocal(const T& data)
{
 RData=data;
}

const T& GetShared(void) const
{
 if(Pointer)
  return *Pointer;

 return RData;
}

void SetShared(const T& data)
{
 if(Pointer)
  *Pointer=data;
 else
  RData=data;
}

const T* GetPointer(void) const
{
 return Pointer;
}

void SetPointer(UAComponent *owner, T* pointer)
{
 MainOwnerT *mainowner=dynamic_cast<MainOwnerT*>(owner);

 if(mainowner)
 {
  Pointer=pointer;
  if(Pointer)
   *Pointer=RData;
 }
 else
  Pointer=0;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
UVShare<T,MainOwnerT>& operator = (const UVShare<T,MainOwnerT> &copy)
{
 Pointer=copy.Pointer;
 RData=copy.RData;

 return *this;
}
// --------------------------

};


template<typename T,class MainOwnerT>
class UShare
{
protected: // Данные
// Указатель на расшаренные данные
T* Pointer;

// Локальная копия данных если указатель не доступен
T Data;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UShare(void)
{
 Pointer=0;
}

virtual ~UShare(void)
{
 Pointer=0;
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
const T& Get(void) const
{
 return (Pointer)?*Pointer:Data;
}

void Set(const T& data)
{
 if(Pointer)
  *Pointer=data;
 else
  Data=data;
}

const T& GetLocal(void) const
{
 return Data;
}

void SetLocal(const T& data)
{
 Data=data;
}

const T& GetShared(void) const
{
 if(Pointer)
  return *Pointer;

 return Data;
}

void SetShared(const T& data)
{
 if(Pointer)
  *Pointer=data;
 else
  Data=data;
}

const T* GetPointer(void) const
{
 return Pointer;
}

void SetPointer(UAComponent *owner, T* pointer)
{
 MainOwnerT *mainowner=dynamic_cast<MainOwnerT*>(owner);

 if(mainowner)
 {
  Pointer=pointer;
  if(Pointer)
   *Pointer=Data;
 }
 else
  Pointer=0;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
UShare<T,MainOwnerT>& operator = (const UShare<T,MainOwnerT> &copy)
{
 Pointer=copy.Pointer;
 Data=copy.Data;

 return *this;
}
// --------------------------

};



}

#endif
