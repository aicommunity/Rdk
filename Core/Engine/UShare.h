#ifndef UShareH
#define UShareH

//#include "UComponent.h"
#include "UProperty.h"

namespace RDK {

template<typename T,class MainOwnerT>
class UVShare: public UIShare
{
protected: // Данные
// Указатель на расшаренные данные
//T* Pointer;
UVProperty<T,MainOwnerT>* VProperty;

// Локальная копия данных если указатель не доступен
T& RData;

// Имя подключаемого свойства
std::string PropertyName;

// Владелец свойства
UComponent* Owner;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVShare(const std::string &property_name, UComponent* owner, T &rdata)
: VProperty(0), RData(rdata), PropertyName(property_name), Owner(owner)
{
 if(Owner)
  Owner->AddLookupShare(PropertyName,this);
}

virtual ~UVShare(void)
{
// Pointer=0;
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Метод инициализации общего свойства
virtual bool Init(UEPtr<UComponent> main_owner)
{
 UEPtr<MainOwnerT> main_owner_cast=dynamic_pointer_cast<MainOwnerT>(main_owner);

 UComponent::VariableMapCIteratorT I=main_owner_cast->GetPropertiesList().find(PropertyName);
 if(I != main_owner_cast->GetPropertiesList().end())
 {
  UVProperty<T,MainOwnerT> *prop=dynamic_pointer_cast<UVProperty<T,MainOwnerT> >(I->second.Property);
  if(prop)
   VProperty=prop;
  else
   VProperty=0;
 }
 else
  VProperty=0;

 return true;
}

// Метод деинициализации общего свойства
virtual bool UnInit(void)
{
 VProperty=0;
 return true;
}

const T& Get(void) const
{
 return (VProperty)?VProperty->GetData():RData;//(Pointer)?*Pointer:RData;
}

void Set(const T& data)
{
 if(VProperty)
  VProperty->SetData(data);
 else
  RData=data;
/*
 if(Pointer)
  *Pointer=data;
 else
  RData=data;
  */
}

const T& GetLocal(void) const
{
 return RData;
}

void SetLocal(const T& data)
{
 RData=data;
}
     /*
const T& GetShared(void) const
{
// if(Pointer)
//  return *Pointer;
 if(VProperty)
  return VProperty->Get();

 return RData;
}

void SetShared(const T& data)
{
 if(VProperty)
  *VProperty=data;
 else
  RData=data;
}

const T* GetPointer(void) const
{
 return Pointer;
}

void SetPointer(UComponent *owner, T* pointer)
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
}            */
// --------------------------

// --------------------------
// Операторы
// --------------------------
UVShare<T,MainOwnerT>& operator = (const UVShare<T,MainOwnerT> &copy)
{
// Pointer=copy.Pointer;
 VProperty=copy.VProperty;
 RData=copy.RData;

 return *this;
}
// --------------------------

};


template<typename T,class MainOwnerT>
class UShare: public UIShare
{
protected: // Данные
// Указатель на расшаренные данные
//T* Pointer;
UVProperty<T,MainOwnerT>* VProperty;

// Локальная копия данных если указатель не доступен
T Data;

// Имя подключаемого свойства
std::string PropertyName;

// Владелец свойства
UComponent* Owner;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UShare(const std::string &property_name, UComponent* owner)
: VProperty(0), PropertyName(property_name), Owner(owner)
{
 if(Owner)
  Owner->AddLookupShare(PropertyName,this);
}

virtual ~UShare(void)
{
// Pointer=0;
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Метод инициализации общего свойства
virtual bool Init(UEPtr<UComponent> main_owner)
{
 UEPtr<MainOwnerT> main_owner_cast=dynamic_pointer_cast<MainOwnerT>(main_owner);

 if(!main_owner_cast)
  return true;

 UComponent::VariableMapCIteratorT I=main_owner_cast->GetPropertiesList().find(PropertyName);
 if(I != main_owner_cast->GetPropertiesList().end())
 {
  UVProperty<T,MainOwnerT> *prop=dynamic_pointer_cast<UVProperty<T,MainOwnerT> >(I->second.Property);
  if(prop)
   VProperty=prop;
  else
   VProperty=0;
 }
 else
  VProperty=0;

 return true;
}

// Метод деинициализации общего свойства
virtual bool UnInit(void)
{
 VProperty=0;
 return true;
}

const T& Get(void) const
{
 return (VProperty)?VProperty->GetData():Data;
// return (Pointer)?*Pointer:Data;
}

void Set(const T& data)
{
 if(VProperty)
  VProperty->SetData(data);
 else
  Data=data;
/*
 if(Pointer)
  *Pointer=data;
 else
  Data=data;
  */
}

const T& GetLocal(void) const
{
 return Data;
}

void SetLocal(const T& data)
{
 Data=data;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
UShare<T,MainOwnerT>& operator = (const UShare<T,MainOwnerT> &copy)
{
// Pointer=copy.Pointer;
 VProperty=copy.VProperty;
 Data=copy.Data;

 return *this;
}
// --------------------------

};



}

#endif
