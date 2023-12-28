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
// ������ ������ � ������
mutable T* PData;

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
 : PData(pdata),IoType(ipSingle | ipData), Mutex(UCreateMutex()), UpdateTime(0)
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
virtual const T& GetData(void) const
{
 if(PData)
  return *PData;
 throw EPropertyZeroPtr(GetOwnerName(),GetName());
}

// ������������ ������
virtual void SetData(const T& data)
{
 (PData)?*PData=data:throw EPropertyZeroPtr(GetOwnerName(),GetName());
 RenewUpdateTime();
}

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

/*
operator T* (void)
{
    return this->PData;
}*/
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
// ������ ���������� �������
// --------------------------
/*
/// ���������� true, ���� �� ������������ ������ ����� ������
virtual bool IsNewData(void) const
{
 return true;
}

/// ���������� true ���� ���� ����� �����������
virtual bool IsConnected(void) const
{
 return false;
}*/
// --------------------------


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
UIPropertyOutput *ConnectedOutput;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), /*Getter(0), Setter(0), */GetterR(getmethod), SetterR(setmethod), ExternalDataSource(0)
{
    IsConnectedFlag=false;
    ConnectedOutput = 0;
}

UVProperty(OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UVBaseProperty<T,OwnerT>(owner,pdata), /*Getter(0), Setter(0), */GetterR(0), SetterR(setmethod), ExternalDataSource(0)
{
    IsConnectedFlag=false;
    ConnectedOutput = 0;
}
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 if(ExternalDataSource)
  return ExternalDataSource->GetData();

 if(this->Owner)
 {
  if(this->PData)
   return *this->PData;

  if(GetterR)
   return (this->Owner->*GetterR)();
 }

 throw UIProperty::EPropertyZeroPtr(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
}

// ��������� ��������
virtual void SetData(const T &value)
{
 if(ExternalDataSource)
 {
  ExternalDataSource->SetData(value);
  return;
 }

 if(this->PData && !SetterR)
 {
  *this->PData=value;
  this->RenewUpdateTime();
 }

 if(this->Owner && SetterR)
 {
  if(!(this->Owner->*SetterR)(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

  if(this->PData)
  {
   *this->PData=value;
   this->RenewUpdateTime();
  }
 }
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
/// ��������� ����� ������ � �����
void ApplyOutputUpdateTime(void) const
{
    if(ConnectedOutput)
        this->UpdateTime=ConnectedOutput->GetUpdateTime();
}

// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
    return IsConnectedFlag;
}

/// ���������� true, ���� �� ������������ ������ ����� ������
virtual bool IsNewData(void) const
{
    return (this->ConnectedOutput)?this->ConnectedOutput->GetUpdateTime()>this->UpdateTime:true;
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
 this->PData=&v;
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
 {
  return this->ExternalDataSource->GetData();
 }

 return (IsConnectedFlag)?*this->PData:v;
}

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if(CheckEqualsFlag && value == v)
  return;

 if(IsConnectedFlag)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

  v=value;
  this->RenewUpdateTime();
  return;
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
        this->PData=const_cast<T*>(&this->ExternalDataSource->GetData());
        IsConnectedFlag=true;
    }
    return res;
}

void DetachFrom(void)
{
    this->PData=&v;
    IsConnectedFlag=false;
    UVProperty<T,OwnerT>::DetachFrom();
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
    if(IsConnectedFlag)
     return this->PData;
    return 0;
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, UIPropertyOutput* property)
{
    this->PData=const_cast<T*>(&dynamic_cast<UVBaseDataProperty<T>*>(property)->GetData());
    IsConnectedFlag=true;
    this->ConnectedOutput=property;
    this->ResetUpdateTime();
    return true;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, UIPropertyOutput* property)
{
    if(this->ConnectedOutput == property)
    {
        this->PData=&v;
        IsConnectedFlag=false;
        ConnectedOutput=0;
        return true;
    }
    return false;
}


/*
T* operator -> (void) const
{
    this->ApplyOutputUpdateTime();
    return (IsConnectedFlag)?this->PData:&(this->v);
}

T& operator * (void)
{
    this->ApplyOutputUpdateTime();
    return (IsConnectedFlag)?*this->PData:v;
}

operator T* (void) const
{
    this->ApplyOutputUpdateTime();
    return (IsConnectedFlag)?this->PData:&(this->v);
}
*/
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
class UCPropertyLocal: public UVProperty<T,OwnerT>
{
public: // ���� ������� �����-������
typedef typename T::value_type TV;
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // ������
// ������ �����-������
VSetterRT VSetterR;

/// ���� �������� �������� �������� �� ��������� ����������� ��������
bool CheckEqualsFlag;

public:
// ������
T v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
public:
UCPropertyLocal(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), VSetterR(0), CheckEqualsFlag(true), v()
{
 this->PData=&v;
 dynamic_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false);
}

UCPropertyLocal(const string &name, OwnerT * const owner, typename UCPropertyLocal<T,OwnerT>::VSetterRT setmethod)
 : UVProperty<T,OwnerT>(owner,(typename UVProperty<T,OwnerT>::SetterRT)0,0), CheckEqualsFlag(true), v()
{
 VSetterR=setmethod; this->PData=&v;
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
 return (this->ExternalDataSource)?this->ExternalDataSource->GetData():v;
};

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

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

public: // ����������

// ����� �� ������� ������� C (container) property
struct EPropertyRangeError: public UIProperty::EPropertyError
{
int MinValue, MaxValue, ErrorValue;
public:
EPropertyRangeError(const std::string &owner_name, const std::string &property_name,
                    int min_value, int max_value, int error_value)
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
const typename UCPropertyLocal<T,OwnerT, type>::TV& operator () (int i) const
{
    if(i<0 || i>=this->v.size())
        throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                                  0,int(this->v.size()),i);

    return this->v[i];
}

// ������ �������� ����������
bool operator () (int i, const typename UCPropertyLocal<T,OwnerT, type>::TV &value)
{
    if(UVProperty<T,OwnerT>::VSetterR && !(this->Owner->*(UVProperty<T,OwnerT>::VSetterR)(value)))
        throw EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

    if(i<0 || i>=this->v.size())
        throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                                  0,int(this->v.size()),i);

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
// -----------------------------

// -----------------------------
// ������� ��������� ������� ������ ��� �������������� ������
// -----------------------------
public:
typename UCPropertyLocal<T,OwnerT, type>::TV& operator [] (int i)
{ return this->v[i]; }

const typename UCPropertyLocal<T,OwnerT, type>::TV& operator [] (int i) const
{ return this->v[i]; }
// -----------------------------
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


// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCBase: public UCProperty<std::vector<T*>,OwnerT,type>
{
protected:
/// ��������� ����������, ��������������, ���� ��� ��������� �����������
std::vector<T*> Local;

/// ��������� �� ������������ �����
std::vector<UIPropertyOutput*> ConnectedOutputs;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputCBase(const string &name, OwnerT * const owner, int input_type)
    : UCProperty<std::vector<T*>,OwnerT,type>(name, owner)
{
    this->IoType=input_type;
}

~UPropertyInputCBase(void)
{
    for(size_t i=0;i<Local.size();i++)
        delete Local[i];
}
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
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

// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
    return !this->v.empty();
}

// ����� ���������� �� ������
int GetNumPointers(void) const
{
    return int(this->v.size());
}

// ���������� ��������� �� ������ �����
void const * GetPointer(int index) const
{
    if(int(this->v.size())<=index)
#if defined(__BORLANDC__) && !defined(__clang__)
        //  return 0;
        throw UCLProperty<std::vector<T*>,OwnerT, type>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
                                                                               0,int(this->v.size()),index);
#else
        throw typename UCLProperty<std::vector<T*>,OwnerT, type>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
                                                                                        0,int(this->v.size()),index);
#endif
    return this->v[index];
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, UIPropertyOutput* property)
{
    if(index<0)
        return false;

    if(int(this->v.size())<=index || (this->v.size() != Local.size()))
    {
        size_t new_size=index+1;
        this->v.resize(new_size);
        size_t old_size=Local.size();
        for(size_t i=new_size;i<old_size;i++)
            delete Local[i];
        Local.resize(new_size);
        ConnectedOutputs.resize(new_size,0);

        for(size_t i=old_size;i<new_size;i++)
            Local[i]=new T;
    }
    this->v[index]=const_cast<T*>(&dynamic_cast<UVBaseDataProperty<T>*>(property)->GetData());
    ConnectedOutputs.resize(this->v.size());
    ConnectedOutputs[index]=property;
    return true;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, UIPropertyOutput* property)
{
    if(int(this->v.size())>index && index >=0)
    {
        this->v.erase(this->v.begin()+index);
        if(int(Local.size())>index)
        {
            delete Local[index];
            Local.erase(Local.begin()+index);
        }
        if(int(this->ConnectedOutputs.size())>index)
            this->ConnectedOutputs.erase(ConnectedOutputs.begin()+index);
        return true;
    }
    return false;
}

/// ������� ��� ���������
void ClearAllPointers(void)
{
    for(size_t i=0;i<Local.size();i++)
    {
        delete Local[i];
    }
    Local.clear();
    ConnectedOutputs.clear();
    this->v.clear();
}

T* operator [] (int i)
{
    if(int(this->v.size())<=i)
#if defined(__BORLANDC__) && !defined(__clang__)
        throw UCLProperty<std::vector<T*>,OwnerT, type>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
                                                                               0,int(this->v.size()),i);
#else
        throw typename UCLProperty<std::vector<T*>,OwnerT, type>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
                                                                                        0,int(this->v.size()),i);
#endif

    return (this->v[i])?this->v[i]:Local[i];
}

const T* operator [] (int i) const
{
    if(int(this->v.size())<=i)
#ifdef __BORLANDC__
        throw UCLProperty<std::vector<T*>,OwnerT, type>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
                                                                               0,int(this->v.size()),i);
#else
        throw typename UCLProperty<std::vector<T*>,OwnerT, type>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
                                                                                        0,int(this->v.size()),i);
#endif

    return (this->v[i])?this->v[i]:Local[i];
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
/// ���������� ��� ������������� ����������
virtual UItem* GetItem(int index)
{
    if(int(this->ConnectedOutputs.size())>index && index >=0)
    {
        return reinterpret_cast<UItem*>(ConnectedOutputs[index]->GetOwner());
    }
    return 0;
}

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(int index) const
{
    if(int(this->ConnectedOutputs.size())>index && index >=0)
    {
        return ConnectedOutputs[index]->GetName();
    }
    return std::string();
}

/// ���������� ��� ������������� ����������
virtual std::string GetItemName(int index) const
{
    if(int(this->ConnectedOutputs.size())>index && index >=0)
    {
        return ConnectedOutputs[index]->GetOwner()->GetName();
    }
    return std::string();
}

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(int index) const
{
    if(int(this->ConnectedOutputs.size())>index && index >=0)
    {
        return ConnectedOutputs[index]->GetOwner()->GetFullName();
    }
    return std::string();
}

protected:
/// ���������� ��� ������������� ����������
virtual UItem* GetItem(void)
{
    return 0;
}
// --------------------------

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCData: public UPropertyInputCBase<T,OwnerT,type>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputCData(const string &name, OwnerT * const owner)
    : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{ }

/// Deprecated
UPropertyInputCData(const string &name, OwnerT * const owner, int index)
    : UPropertyInputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{ }

// -----------------------------
};



template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using ULProperty = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using UCLProperty = UCProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputData = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputCData = UCProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
using UPropertyInputData = UProperty<T, OwnerT, type>;


#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif

}

#endif

