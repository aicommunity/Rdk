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

/// ��������� ��������� PData
virtual void UpdatePData(void* data)
{
 PData=(T*)data;
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

// --------------------------
// ������ ���������� ����������
// --------------------------
/*
/// ���������� ��������� �� ������
virtual void const* GetPointer(int index) const
{
 return 0;
}

/// ������������� ��������� �� ������
virtual bool SetPointer(int index, void* value, UIProperty* output)
{
 return false;
}

/// ���������� ��������� �� ������
virtual bool ResetPointer(int index, void* value)
{
 return false;
}
*/
/// �������� ��������� �������-������
virtual void UpdateConnectedPointers(void)
{
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
/// �������������� ������
virtual void Init(UItem* item, const std::string &output_name)
{
}

/// ���������������� ������
virtual void UnInit(void)
{
}

/// ���������� true, ���� �� ������������ ������ ����� ������
virtual bool IsNewData(void) const
{
 return true;
}

/// ���������� true ���� ���� ����� �����������
virtual bool IsConnected(void) const
{
 return false;
}
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
};

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const
{
 return Variable->second.Type;
};

// ����� ���������� ��������� ��� ����������-��������� ��������
virtual std::string GetOwnerName(void) const
{
 return (Owner)?Owner->GetName():std::string("");
};

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerClassName(void) const
{
 return typeid(Owner).name();
};
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

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), /*Getter(0), Setter(0), */GetterR(getmethod), SetterR(setmethod), ExternalDataSource(0)
{
}

UVProperty(OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UVBaseProperty<T,OwnerT>(owner,pdata), /*Getter(0), Setter(0), */GetterR(0), SetterR(setmethod), ExternalDataSource(0)
{
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
// ������ ����������
// -----------------------------
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
UVProperty<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}
// -----------------------------
};


/// ����� - ����������� ��������
/// �� �������� ������� ������ ����
/// Setter � Getter ������������ ������ �� ��������
/// ������� �������� �������������� ����� ���������� �����
/*
template<typename T,class OwnerT>
class UVSProperty: public UVBaseProperty<T,OwnerT>
{
protected: // ���� ������� �����-������
typedef T (OwnerT::*GetterT)(void) const;
typedef bool (OwnerT::*SetterT)(T);

protected: // ������
// ������ �����-������
GetterT Getter;
SetterT Setter;

private:
mutable T Temp;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVSProperty(OwnerT * const owner, SetterT setmethod , GetterT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), Getter(getmethod), Setter(setmethod)
{
}

UVSProperty(OwnerT * const owner, T * const pdata, SetterT setmethod=0) :
  UVBaseProperty<T,OwnerT>(owner,pdata), Getter(0), Setter(setmethod)
{
}
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 if(this->Owner)
 {
  if(this->PData)
   return *this->PData;

  if(Getter)
   return Temp=(this->Owner->*Getter)();
 }

 throw UIProperty::EPropertyZeroPtr(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
};

// ��������� ��������
virtual void SetData(const T &value)
{
 if(this->PData && !Setter)
 {
  *this->PData=value;
  this->RenewUpdateTime();
 }

 if(this->Owner && Setter)
 {
  if(!(this->Owner->*Setter)(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

  if(this->PData)
  {
   *this->PData=value;
   this->RenewUpdateTime();
  }
 }
};
// -----------------------------
};*/


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
 return v;
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
};
/* ************************************************************************* */


/// �������� ����� �������� �� ��������� ������
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UProperty: public UPropertyLocal<T,OwnerT>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UPropertyLocal<T,OwnerT>(name, owner, setmethod)
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

};


// �������� ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UCProperty: public UCPropertyLocal<T,OwnerT>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UCProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UCPropertyLocal<T,OwnerT>(name, owner, setmethod)
{ }

//����������� ������������� ��� ��������� ��������
UCProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterRT setmethod)
    : UCPropertyLocal<T,OwnerT>(name, owner,setmethod)
{ }

protected:
UCProperty(const UCProperty<T,OwnerT> &v) {}
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


// -----------------------------
// ��������� �������
// -----------------------------
// ������ �������� ����������
const typename UCProperty<T,OwnerT>::TV& operator () (int i) const
{
    if(i<0 || i>=this->v.size())
        throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                                  0,int(this->v.size()),i);

    return this->v[i];
}

// ������ �������� ����������
bool operator () (int i, const typename UCProperty<T,OwnerT>::TV &value)
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
// -----------------------------

// -----------------------------
// ������� ��������� ������� ������ ��� �������������� ������
// -----------------------------
public:
typename UCProperty<T,OwnerT>::TV& operator [] (int i)
{ return this->v[i]; }

const typename UCProperty<T,OwnerT>::TV& operator [] (int i) const
{ return this->v[i]; }
// -----------------------------
};


#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */

template<typename T, typename OwnerT, unsigned int type>
std::ostream& operator << (std::ostream &stream, UPropertyLocal<T,OwnerT, type> &property)
{
    using namespace std;
    stream<<"Property "<<property.GetOwnerName()<<":"<<property.GetName();
    stream<<endl;
    stream<<"Data:"<<endl;
    stream<<*property;
    stream<<"--------------------";
    return stream;
}

}

#endif

