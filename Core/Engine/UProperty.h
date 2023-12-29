/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */


#ifndef UPROPERTY_H
#define UPROPERTY_H

#include <string>
#include <sstream>
#include <typeinfo>
#include "../Serialize/USerStorageXML.h"
#include "../Serialize/USerStorageBinary.h"
#include "../Serialize/UXMLStdSerialize.h"
#include "../Serialize/UBinaryStdSerialize.h"
#include "../Graphics/UGraphicsBinarySerialize.h"
#include "../Graphics/UGraphicsXMLSerialize.h"
#include "../Engine/UBinaryEnvSerialize.h"
#include "../Engine/UXMLEnvSerialize.h"
#include "../Math/MUXMLSerialize.h"
#include "../Math/MUBinarySerialize.h"
#include "UComponent.h"
#include "../System/rdk_system.h"

namespace RDK {

using namespace std;

#ifdef __BORLANDC__
#pragma warning( disable : 4700)
#endif

// ����� - ���� ��� �������
template<typename T>
class UVBaseDataProperty: public UIPropertyOutput
{
protected: // ������
// ��� �����
int IoType;

protected: // ������ �������������
/// ������� ����� ��������
UGenericMutex *Mutex;

/// ����� ���������� �������� (��)
mutable ULongTime UpdateTime;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
explicit UVBaseDataProperty(T * const pdata)
 : IoType(ipSingle | ipData), Mutex(UCreateMutex()), UpdateTime(0)
{
}

virtual ~UVBaseDataProperty(void)
{
 UDestroyMutex(Mutex);
 Mutex=0;
}
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
// ���������� ������ �� ������
virtual const T& GetData(void) const=0;

// ������������ ������
virtual void SetData(const T& data)=0;

// ���������� �������� ��� ��������� ��������
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}

// ����� ���������� ��� ����� �������� � ������ ���������
virtual bool CompareLanguageType(const UIProperty &dt) const
{
 return GetLanguageType() == dt.GetLanguageType();
}

// ���������� �������� ��� ��������� �������� ��� ������ ��������
virtual const type_info& GetElemLanguageType(void) const
{
 return typeid(T);
}

// ����� ���������� ��� ����� �������� � ������ ��������� (�� ������ ��������)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return GetElemLanguageType() == dt.GetElemLanguageType();
}

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<USerStorage>  storage, bool simplemode=false)
{
/*
 UEPtr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<GetData();
  return true;
 }
 */
 UEPtr<USerStorageXML> xml=dynamic_pointer_cast<USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->Create(GetName());
   try
   {
    operator << (*xml,GetData());
   }
   catch(UIProperty::EPropertyZeroPtr &)
   {
   }
   xml->SelectUp();
   return true;
  }
  else
  {
   xml->AddNode(GetName());
   try
   {
    operator << (*xml,GetData());
   }
   catch(UIProperty::EPropertyZeroPtr &)
   {
   }
   xml->SelectUp();
   return true;
  }
 }

 return false;
}

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 T temp;
/*
 UEPtr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  operator >> (*binary,temp);
  SetData(temp);
  return true;
 }*/

 UEPtr<USerStorageXML> xml=dynamic_pointer_cast<USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->SelectRoot();
   if(xml->GetNodeName() != GetName())
    return false;
   operator >> (*xml,temp);
   try
   {
    SetData(temp);
    xml->SelectUp();
   }
   catch(...)
   {
    xml->SelectUp();
   }
   return true;
  }
  else
  {
   if(!xml->SelectNode(GetName()))
    return false;
   operator >> (*xml,temp);
   try
   {
    SetData(temp);
    xml->SelectUp();
   }
   catch(...)
   {
    xml->SelectUp();
   }
   return true;
  }
 }

 return false;
}

// ����� ���������� ��������� �� ������� ������, ���������� ������ ��������
virtual const void* GetMemoryArea(void)
{
 return &GetData();
}

// ����� �������� �������� ������ �������� �� ������� ������
// �������� ���������� ����������� ��������� ���� ������
// ������� ��������� ���������� � ��������� �� ����������� ��� ������
bool ReadFromMemory(const void *buffer)
{
 if(!buffer)
  return false;

 const T* temp=(const T*)buffer;
 SetData(*temp);
 return true;
}
// -----------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���
virtual int GetIoType(void) const
{
 return IoType;
}

virtual ULongTime GetUpdateTime(void) const
{
// UGenericLocker locker(Mutex);
 return UpdateTime;
}

virtual void SetUpdateTime(ULongTime value)
{
// UGenericLocker locker(Mutex);
 UpdateTime=value;
}
// --------------------------

// -----------------------------
// �������� ������� ������ ��� ��������� ������
// -----------------------------
virtual bool AttachTo(UVBaseDataProperty<T>* prop)
{
 return false;
}

virtual void DetachFrom(void)
{
}
// -----------------------------

protected:
// --------------------------
// ������� ������ ���������� �������
// --------------------------
/// ��������� ����� ��������� ������ ��������
void RenewUpdateTime(void)
{
 UpdateTime=GetCurrentStartupTime();
}

/// ���������� ����� ���������� �� ����
void ResetUpdateTime(void)
{
 UpdateTime=0;
}
// --------------------------
};


// ����� - ���� ��� �������
template<typename T,class OwnerT>
class UVBaseProperty: public UVBaseDataProperty<T>
{
protected: // ������
// �������� ��������
OwnerT* Owner;

// ��������� �� ��������-��������� ������ �� ���� �������� � ������������ ����������
UComponent::VariableMapCIteratorT Variable;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
explicit UVBaseProperty(OwnerT * const owner) :
  UVBaseDataProperty<T>(0), Owner(owner)
{
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}

UVBaseProperty(OwnerT * const owner, T * const pdata) :
  UVBaseDataProperty<T>(pdata), Owner(owner)
{
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
// ����� ������������� �������� ��������� �� ��������-��������� ������ �� ����
// �������� � ������������ ����������
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

// ����� ���������� ��������� ����������-��������� ��������
virtual UContainer* GetOwner(void) const
{
 return dynamic_cast<UContainer*>(Owner);
}

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const
{
 return Variable->first;
}

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const
{
 return Variable->second.Type;
}

// ����� ���������� ��������� ��� ����������-��������� ��������
virtual std::string GetOwnerName(void) const
{
 return (Owner)?Owner->GetName():std::string("");
}

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerClassName(void) const
{
 return typeid(Owner).name();
}
// -----------------------------
};

// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T,class OwnerT>
class UVProperty: public UVBaseProperty<T,OwnerT>
{
//friend class OwnerT;
public: // ���� ������� �����-������
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // ������
// ������ �����-������
GetterRT GetterR;
SetterRT SetterR;

protected:
/// ������ �� ������� ��������-�������� ������
UVBaseDataProperty<T>* ExternalDataSource;

protected:
/// ���� ������� �����������
bool IsConnectedFlag;

/// ��������� �� ������������ �����
std::vector<UIPropertyOutput*> ConnectedOutputs;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), /*Getter(0), Setter(0), */GetterR(getmethod), SetterR(setmethod), ExternalDataSource(0)
{
    IsConnectedFlag=false;
}

UVProperty(OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UVBaseProperty<T,OwnerT>(owner,pdata), /*Getter(0), Setter(0), */GetterR(0), SetterR(setmethod), ExternalDataSource(0)
{
    IsConnectedFlag=false;
}
// -----------------------------

// -----------------------------
// �������� ������� ������ ��� ��������� ������
// -----------------------------
bool AttachTo(UVBaseDataProperty<T>* prop)
{
 if(!prop)
  return false;
 ExternalDataSource=prop;
 this->UpdateConnectedPointers();
 return true;
}

void DetachFrom(void)
{
 ExternalDataSource=0;
 this->UpdateConnectedPointers();
}
// -----------------------------

// -----------------------------
// ������ ���������� ����������� �������
// -----------------------------
/// ���������� ��� ������������� ����������
virtual UItem* GetItem(int index=0)
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return reinterpret_cast<UItem*>(ConnectedOutputs[index]->GetOwner());
 }
 return 0;
}

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(int index=0) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetName();
 }
 return std::string();
}

/// ���������� ��� ������������� ����������
virtual std::string GetItemName(int index=0) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetOwner()->GetName();
 }
 return std::string();
}

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(int index=0) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetOwner()->GetFullName();
 }
 return std::string();
}

/// ��������� ����� ������ � �����
void ApplyOutputUpdateTime(void) const
{
 if(!ConnectedOutputs.empty())
  this->UpdateTime=ConnectedOutputs[0]->GetUpdateTime();
}

// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return IsConnectedFlag;
}

/// ���������� true, ���� �� ������������ ������ ����� ������
virtual bool IsNewData(void) const
{
 return (!ConnectedOutputs.empty())?this->ConnectedOutputs[0]->GetUpdateTime()>this->UpdateTime:true;
}
// -----------------------------


// -----------------------------
// ������ ����������
// -----------------------------
operator T (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

const T& operator () (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

T* operator -> (void)
{
 ApplyOutputUpdateTime();
 return const_cast<T*>(&this->GetData());
}

const T* operator -> (void) const
{
 ApplyOutputUpdateTime();
 return &this->GetData();
}

T& operator * (void)
{
 ApplyOutputUpdateTime();
 return const_cast<T&>(this->GetData());
}

const T& operator * (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

// �������� ������������
UVProperty<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}
// -----------------------------
};

class UItem;
class UConnector;

/* ************************************************************************* */
// ����� - �������� � ��������� ������
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type=ptPubParameter>
class UPropertyLocal: public UVProperty<T,OwnerT>
{
protected:
/// ���� �������� �������� �������� �� ��������� ����������� ��������
bool CheckEqualsFlag;

public:
//protected:
// ������
mutable T v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
public:
UPropertyLocal(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), CheckEqualsFlag(true), v()
{
// this->PData=&v;
 dynamic_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false);
}
// -----------------------------

// -----------------------------
// ����� ���������� �����������
// -----------------------------
/// ���� �������� �������� �������� �� ��������� ����������� ��������
bool IsCheckEquals(void) const
{
 return CheckEqualsFlag;
}

void SetCheckEquals(bool value)
{
 CheckEqualsFlag=value;
}
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 return (IsConnectedFlag)?dynamic_cast<UVBaseDataProperty<T>*>(this->ConnectedOutputs[0])->GetData():v;
}

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if(IsConnectedFlag)
  return;

 if(CheckEqualsFlag && value == v)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
 }

 v=value;
 this->RenewUpdateTime();
 return;
}
// -----------------------------

// -----------------------------
// ������ ���������� ����������� �������
// -----------------------------
bool AttachTo(UVBaseDataProperty<T>* prop)
{
 bool res=UVProperty<T,OwnerT>::AttachTo(prop);
 if(res)
 {
//  this->PData=const_cast<T*>(&this->ExternalDataSource->GetData());
  IsConnectedFlag=true;
 }
 return res;
}

void DetachFrom(void)
{
// this->PData=&v;
 IsConnectedFlag=false;
 UVProperty<T,OwnerT>::DetachFrom();
}

// ����� ���������� �� ������
int GetNumPointers(void) const
{
 return int(this->ConnectedOutputs.size());
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, UIPropertyOutput* property)
{
 //this->PData=const_cast<T*>(&dynamic_cast<UVBaseDataProperty<T>*>(property)->GetData());
 IsConnectedFlag=true;
 this->ConnectedOutputs.assign(1,property);
 this->ResetUpdateTime();
 return true;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(!this->ConnectedOutputs.empty() && this->ConnectedOutputs[0] == property)
 {
//  this->PData=&v;
  IsConnectedFlag=false;
  ConnectedOutputs.clear();
  return true;
 }
 return false;
}
// -----------------------------

};
/* ************************************************************************* */


/// �������� ����� �������� �� ��������� ������
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UProperty: public UPropertyLocal<T,OwnerT,type>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<T,OwnerT,type> &v) {}
// -----------------------------

// -----------------------------
// ���������
// -----------------------------
public:
// �������� ������������
UProperty<T,OwnerT, type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UProperty<T,OwnerT, type>& operator = (const UProperty<T,OwnerT, type> &v)
{
 this->SetData(v.GetData());
 return *this;
}

operator T (void) const
{
 return this->GetData();
}

const T& operator () (void) const
{
 return this->GetData();
}
// -----------------------------
};


/* ************************************************************************* */
// ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UCPropertyLocal: public UPropertyLocal<T,OwnerT,type>
{
public: // ���� ������� �����-������
typedef typename T::value_type TV;
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // ������
// ������ �����-������
VSetterRT VSetterR;

public:
// --------------------------
// ������������ � �����������
// --------------------------
public:
UCPropertyLocal(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod), VSetterR(0)
{
 this->IoType = ipRange | ipData;
}

UCPropertyLocal(const string &name, OwnerT * const owner, typename UCPropertyLocal<T,OwnerT>::VSetterRT setmethod)
 : UPropertyLocal<T,OwnerT,type>(name, owner,(typename UVProperty<T,OwnerT>::SetterRT)0)
{
 VSetterR=setmethod;
}
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 return (IsConnectedFlag)?dynamic_cast<UVBaseDataProperty<T>*>(this->ConnectedOutputs[0])->GetData():v;
};

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if(IsConnectedFlag)
  return;

 if(CheckEqualsFlag && v == value)
  return;

 if(this->Owner)
 {
  if(VSetterR)
  {
   typename T::const_iterator I,J;
   I=value.begin(); J=value.end();
   while(I != J)
   {
    if(!(this->Owner->*VSetterR)(*I))
     throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

    ++I;
   }
  }
  else
  {
   if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
    throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
  }
 }

 v=value;
 this->RenewUpdateTime();
}
// -----------------------------

// -----------------------------
// ������������� ��������� �� ������ �����
bool SetPointer(int index, UIPropertyOutput* property)
{
 if(index<0)
  return false;

 if(int(ConnectedOutputs.size())<=index)
 {
  size_t new_size=index+1;
  this->v.resize(new_size);
  ConnectedOutputs.resize(new_size,0);
 }
 ConnectedOutputs[index]=property;
 return true;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(int(this->v.size())>index && index >=0)
 {
  auto it = this->v.begin();
  std::advance(it,index);
  this->v.erase(it);
 }

 if(int(this->ConnectedOutputs.size())>index && index >= 0)
 {
  this->ConnectedOutputs.erase(ConnectedOutputs.begin()+index);
  return true;
 }

 return false;
}
// -----------------------------

public: // ����������

// ����� �� ������� ������� C (container) property
struct EPropertyRangeError: public UIProperty::EPropertyError
{
int MinValue, MaxValue, ErrorValue;
public:
EPropertyRangeError(const std::string &owner_name, const std::string &property_name, int min_value, int max_value, int error_value)
 : UIProperty::EPropertyError(owner_name, property_name),
   MinValue(min_value), MaxValue(max_value), ErrorValue(error_value) {}


// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return UIProperty::EPropertyError::CreateLogMessage()+std::string(" MinValue=")+
        sntoa(MinValue)+std::string(" MaxValue=")+sntoa(MaxValue)+
        std::string(" ErrorValue=")+sntoa(ErrorValue);
}
};

public:
// -----------------------------
// ��������� �������
// -----------------------------
// ������ �������� ����������
const typename UCPropertyLocal<T,OwnerT, type>::TV& operator () (size_t i) const
{
 const T& data = GetData();
 if(i>=data.size())
  throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                               0,int(data.size()),int(i));

 return data[i];
}

// ������ �������� ����������
bool operator () (size_t i, const typename UCPropertyLocal<T,OwnerT, type>::TV &value)
{
 if(UVProperty<T,OwnerT>::VSetterR && !(this->Owner->*(UVProperty<T,OwnerT>::VSetterR)(value)))
  throw EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

 if(IsConnectedFlag)
  return false;

 if(i>=this->v.size())
  throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                               0,int(this->v.size()),int(i));

 this->v[i]=value;
 this->RenewUpdateTime();

 return true;
}

operator T (void) const
{
 return this->GetData();
}

const T& operator () (void) const
{
 return this->GetData();
}

T* operator -> (void)
{ return const_cast<T*>(&this->GetData()); }

const T* operator -> (void) const
{ return &this->GetData(); }

T& operator * (void)
{ return const_cast<T&>(this->GetData()); }

const T& operator * (void) const
{ return this->GetData(); }

typename UCPropertyLocal<T,OwnerT, type>::TV& operator [] (size_t i)
{ return const_cast<UCPropertyLocal<T,OwnerT, type>::TV&>((*this)(i)); }

const typename UCPropertyLocal<T,OwnerT, type>::TV& operator [] (size_t i) const
{ return (*this)(i); }

// �������� ������������
UCPropertyLocal& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UCPropertyLocal& operator = (const UCPropertyLocal &value)
{
 this->SetData(value.GetData());
 return *this;
}

// --------------------------
// ������ ���������� �������
// --------------------------
// ����� ���������� ��� ����� �������� � ������ ��������� (�� ������ ��������)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return (GetElemLanguageType() == dt.GetElemLanguageType()) || (typeid(T::value_type) == dt.GetElemLanguageType());
}
// --------------------------
};


// �������� ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UCProperty: public UCPropertyLocal<T,OwnerT, type>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UCProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UCPropertyLocal<T,OwnerT, type>(name, owner, setmethod)
{ }

//����������� ������������� ��� ��������� ��������
UCProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterRT setmethod)
    : UCPropertyLocal<T,OwnerT, type>(name, owner,setmethod)
{ }

protected:
UCProperty(const UCProperty<T,OwnerT, type> &v) {}
// -----------------------------

public:
// �������� ������������
UCProperty& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UCProperty& operator = (const UCProperty &value)
{
 this->SetData(value.GetData());
 return *this;
}
};

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using ULProperty = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using UCLProperty = UCProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputData = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputCData = UCProperty<std::vector<T>, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
using UPropertyInputData = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
using UPropertyInputCData = UCProperty<std::vector<T>, OwnerT, type>;


#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif

}

#endif

