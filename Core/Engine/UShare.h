#ifndef UShareH
#define UShareH

#include "UADataComponent.h"

namespace RDK {

template<typename T,class MainOwnerT>
class UVShare: public UIShare
{
protected: // ������
// ��������� �� ����������� ������
//T* Pointer;
UVProperty<T,MainOwnerT>* VProperty;

// ��������� ����� ������ ���� ��������� �� ��������
T& RData;

// ��� ������������� ��������
std::string PropertyName;

// �������� ��������
UADataComponent* Owner;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UVShare(const std::string &property_name, UADataComponent* owner, T &rdata)
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
// ������ ���������� �������
// --------------------------
// ����� ������������� ������ ��������
virtual bool Init(UEPtr<UADataComponent> main_owner)
{
 UEPtr<MainOwnerT> main_owner_cast=dynamic_pointer_cast<MainOwnerT>(main_owner);

 UADataComponent::VariableMapCIteratorT I=main_owner_cast->GetPropertiesList().find(PropertyName);
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

// ����� ��������������� ������ ��������
virtual bool UnInit(void)
{
 VProperty=0;
 return true;
}

const T& Get(void) const
{
 return (VProperty)?VProperty->Get():RData;//(Pointer)?*Pointer:RData;
}

void Set(const T& data)
{
 if(VProperty)
  *VProperty=data;
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
}            */
// --------------------------

// --------------------------
// ���������
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
protected: // ������
// ��������� �� ����������� ������
//T* Pointer;
UVProperty<T,MainOwnerT>* VProperty;

// ��������� ����� ������ ���� ��������� �� ��������
T Data;

// ��� ������������� ��������
std::string PropertyName;

// �������� ��������
UADataComponent* Owner;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UShare(const std::string &property_name, UADataComponent* owner)
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
// ������ ���������� �������
// --------------------------
// ����� ������������� ������ ��������
virtual bool Init(UEPtr<UADataComponent> main_owner)
{
 UEPtr<MainOwnerT> main_owner_cast=dynamic_pointer_cast<MainOwnerT>(main_owner);

 UADataComponent::VariableMapCIteratorT I=main_owner_cast->GetPropertiesList().find(PropertyName);
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

// ����� ��������������� ������ ��������
virtual bool UnInit(void)
{
 VProperty=0;
}

const T Get(void) const
{
 return (VProperty)?VProperty->Get():Data;
// return (Pointer)?*Pointer:Data;
}

void Set(const T& data)
{
 if(VProperty)
  *VProperty=data;
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
  /*
const T& GetShared(void) const
{
 return (VProperty)?VProperty->Get():Data;
}

void SetShared(const T& data)
{
 if(VProperty)
  *VProperty=data;
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
}    */
// --------------------------

// --------------------------
// ���������
// --------------------------
UShare<T,MainOwnerT>& operator = (const UShare<T,MainOwnerT> &copy)
{
//s Pointer=copy.Pointer;
 VProperty=copy.VProperty;
 Data=copy.Data;

 return *this;
}
// --------------------------

};



}

#endif
