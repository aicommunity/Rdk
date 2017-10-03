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
#include "../System/rdk_system.h"
#include "UComponent.h"
#include "UPropertyImplementationBase.h"

namespace RDK {

using namespace std;

#ifdef __BORLANDC__
#pragma warning( disable : 4700)
#endif

/// ����� - ���� ��� ������� � �������������
template<typename T>
class UPropertyBase: virtual public UIPropertyInputBase, virtual public UIPropertyOutputBase
{
public:
/// ��� ������, ��������������� ������ ������ � ������ ��������
typedef const T& (UPropertyBase<T>::*PGetDataValue)(void) const;
typedef bool (UPropertyBase<T>::*PSetDataValue)(const T &value);
protected: // ������
// ��� ��������
std::string Name;

protected: // ������ �������������
/// ������� ����� ��������
UGenericMutex *Mutex;

/// ����� ���������� �������� (��)
mutable ULongTime UpdateTime;

// ��������� �� ��������-��������� ������ �� ���� �������� � ������������ ����������
UComponent::VariableMapCIteratorT Variable;

/// ����, �����������, ��� ��� ������������ �������
bool DynamicPropertyFlag;

mutable int CurrentInputIndex;

/// ��������� �� �����, �������������� ������ ������ � ������ ��������
PGetDataValue GetDataValuePtr;
PSetDataValue SetDataValuePtr;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyBase(const std::string &name, bool dynamic_prop_flag=false)
 : GetDataValuePtr(0), SetDataValuePtr(0), Name(name), Mutex(UCreateMutex()), UpdateTime(0), DynamicPropertyFlag(dynamic_prop_flag), CurrentInputIndex(0)
{
}

virtual ~UPropertyBase(void)
{
 UDestroyMutex(Mutex);
 Mutex=0;
}
// -----------------------------

protected:
virtual bool AFinalizeConnectToOutput(UIPropertyOutput *output_property)
{
 GetDataValuePtr=static_cast<PGetDataValue>(&UPropertyBase<T>::GetDataValueInput);
 SetDataValuePtr=static_cast<PSetDataValue>(&UPropertyBase<T>::SetDataValueInput);
 return true;
}

virtual bool AFinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(!IsConnected())
 {
  GetDataValuePtr=static_cast<PGetDataValue>(&UPropertyBase<T>::GetDataValueLocal);
  SetDataValuePtr=static_cast<PSetDataValue>(&UPropertyBase<T>::SetDataValueLocal);
 }
 return true;
}

// ���������� ������ �� ������
virtual const T& GetData(void) const
{
 return (this->*GetDataValuePtr)();
}

// ������������ ������
virtual bool SetData(const T& data)
{
 return (this->*SetDataValuePtr)(data);
}

// ���������� ������ �� ������
virtual const T& GetDataValueLocal(void) const=0;

// ������������ ������
virtual bool SetDataValueLocal(const T& data)=0;

// ���������� ������ �� ������
virtual const T& GetDataValueInput(void) const
{
 return dynamic_pointer_cast<UPropertyBase<T> >(GetConnectedProperty(CurrentInputIndex))->GetData();
}

// ������������ ������
virtual bool SetDataValueInput(const T& data)
{
 return dynamic_pointer_cast<UPropertyBase<T> >(GetConnectedProperty(CurrentInputIndex))->SetData(data);
}

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

public:
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

// ����� ���������� ��������� �� ������� ������, ���������� ������ ��������
virtual const void* GetMemoryArea(void) const
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

// ����� ������������� �������� ��������� �� ��������-��������� ������ �� ����
// �������� � ������������ ����������
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

/// ������������� ����, �����������, ��� ��� ������������ ��������, � ������ ���� ������� ��� ���������� ����������-���������
virtual void SetDynamicPropertyFlag(void)
{
 DynamicPropertyFlag=true;
}

/// ���������� ��������� �����, ������������, ��� ��� ������������ ��������
virtual bool IsDynamicPropertyFlag(void) const
{
 return DynamicPropertyFlag;
}

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const
{
 return Name;
};
};

// ����� - ������� �������� � ����������
template<typename T,class OwnerT>
class UPropertyBaseOwner: public UPropertyBase<T>
{
protected:
// �������� ��������
OwnerT* Owner;

/// ��� ��������
unsigned int Type;

protected: // ������

//T* RawDataPtr;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyBaseOwner(const std::string &name, OwnerT * const owner, unsigned int type, bool dynamic_prop_flag=false) :
  Owner(owner), Type(type), /*RawDataPtr(0),*/ UPropertyBase<T>(name, dynamic_prop_flag)
{
	if(Owner)
	{
	 reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(this);
	}
}
			/*
UPropertyBaseOwner(const std::string &name, OwnerT * const owner, unsigned int type, T * const pdata, bool dynamic_prop_flag=false) :
  Owner(owner), Type(type), RawDataPtr(pdata), UPropertyBase<T>(name,dynamic_prop_flag)
{
    if(Owner)
    {
	 reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(this);
    }
}                   */

virtual ~UPropertyBaseOwner(void)
{
    if(Owner)
    {
	 reinterpret_cast<UComponent* const>(Owner)->DelLookupProperty(this->Name,true);
	}
}
// -----------------------------

// -----------------------------
// ����� ���������� �����������
// -----------------------------
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

/// ���������� ��������� �� ��������� ��������
virtual const UComponent* GetOwner(void) const
{
 return dynamic_cast<const UComponent*>(Owner);
}

virtual UComponent* GetOwner(void)
{
 return dynamic_cast<UComponent*>(Owner);
}

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const
{
 return Type;
}

// ����� �������� ��� ��������
bool ChangeType(unsigned int value)
{
 bool res(true);
 // ���� �������� ��������� ���� ������ �� ��������� ���� ��� � ���� ���������
 if((Type & ptInput) && !(value & ptInput))
  res&=this->DisconnectAllOutputs();

 // ���� �������� ��������� ���� ������� �� ��������� ���� � ���� ��� ����������
 if((Type & ptOutput) && !(value & ptOutput))
  res&=this->DisconnectAllInputs();

 if(res)
  Type=value;
 else
  return false;

 return true;
}

// ���������� ������ ����� ���� ��������
unsigned int GetPropertyType(void) const
{
 return Type & 0x000000FF;
}

// ���������� ������ ����� ������ ��������
unsigned int GetPropertyGroup(void) const
{
 return Type & 0xFFFFFF00;
}

// ���������� ��������� ��� ���� ��������
std::string GetPropertyTypeName(void) const
{
 return UComponent::GetPropertyTypeNameByType(GetPropertyType());
}

// ��������� ������������ ���� � ������ �������� �����
bool CheckMask(unsigned int mask) const
{
 return (GetPropertyType() & mask) && (GetPropertyGroup() & mask);
}
// -----------------------------
};

// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T,class OwnerT>
class UPropertyVirtual: public UPropertyBaseOwner<T,OwnerT>
{
protected: // ���� ������� �����-������
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected:

/// ���� �������� �������� �������� �� ��������� ����������� ��������
bool CheckEqualsFlag;

protected: // ������
// ������ �����-������
GetterRT GetterR;
SetterRT SetterR;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyVirtual(const std::string &name, OwnerT * const owner, unsigned int type, SetterRT setmethod , GetterRT getmethod, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(getmethod), SetterR(setmethod), UPropertyBaseOwner<T,OwnerT>(name, owner, dynamic_prop_flag)
{
}
						   /*
UPropertyVirtual(const std::string &name, OwnerT * const owner, unsigned int type, T * const pdata, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(0), SetterR(0), UPropertyBaseOwner<T,OwnerT>(name,owner,dynamic_prop_flag)
{
}
                                      */
virtual ~UPropertyVirtual(void)
{
}
// -----------------------------

// -----------------------------
// ����� ���������� �����������
// -----------------------------
// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 UEPtr<USerStorageXML> xml=dynamic_pointer_cast<USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->Create(GetName());
   operator << (*xml,GetData());
   xml->SelectUp();
   return true;
  }
  else
  {
   xml->AddNode(GetName());
   operator << (*xml,GetData());
   xml->SelectUp();
   return true;
  }
 }

 return false;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 T temp;

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
};

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
// ������ ����������
// -----------------------------
protected:
// ������� ��������
virtual const T& GetDataValueLocal(void) const
{
 return (this->Owner->*GetterR)();
 /*
 if(this->Owner && GetterR)
 {
  if(GetterR)
   return (this->Owner->*GetterR)();
 }

 throw UIProperty::EPropertyZeroPtr(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());
 */
};

// ��������� ��������
virtual bool SetDataValueLocal(const T &value)
{
 return (this->Owner->*SetterR)(value);
					   /*
 if(RawDataPtr)
 {
  if(CheckEqualsFlag && *RawDataPtr == value)
   return;

  *RawDataPtr=value;
  this->RenewUpdateTime();
 }
 else
 {
  if(!GetterR)
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

  if(CheckEqualsFlag && (this->Owner->*GetterR)() == value)
   return;

  if(!SetterR)
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

  if(this->Owner && SetterR)
  {
   if(!(this->Owner->*SetterR)(value))
	throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

   this->RenewUpdateTime();
  }
 }             */
};

// ������������ ������
virtual bool SetData(const T& data)
{
 if(CheckEqualsFlag && GetData() == data)
  return true;

 if(!(this->*SetDataValuePtr)(data))
  throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

 this->RenewUpdateTime();
 return true;
}
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
public:
const T& v(void) const
{
 return this->GetData();
};

void v(const T &value)
{
 SetData(value);
};
/*
const T& operator () (void) const
{
 return this->GetData();
};

T& operator () (void)
{
 return const_cast<T&>(this->GetData());
};*/
/// deprecated
const T* operator -> (void) const
{ return &this->GetData(); };

/// deprecated
const T& operator * (void) const
{ return this->GetData(); };
  /*
const T& operator [] (int index) const
{ return this->GetData(index); };
	*/

// �������� ������������
UPropertyVirtual<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UPropertyVirtual<T,OwnerT>& operator = (const UPropertyVirtual<T,OwnerT> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};


template<typename T,class OwnerT>
bool operator != (const UPropertyVirtual<T,OwnerT> &prop, const T &v)
{ return prop.v() != v; };

template<typename T,class OwnerT>
bool operator != (const T &v, const UPropertyVirtual<T,OwnerT> &prop)
{ return prop.v() != v; };

template<typename T,class OwnerT>
bool operator == (const UPropertyVirtual<T,OwnerT> &prop, const T &v)
{ return prop.v() == v; };

template<typename T,class OwnerT>
bool operator == (const T &v, const UPropertyVirtual<T,OwnerT> &prop)
{ return prop.v() == v; };

template<typename T,class OwnerT>
bool operator < (const UPropertyVirtual<T,OwnerT> &prop, const T &v)
{ return prop.v() < v; };

template<typename T,class OwnerT>
bool operator < (const T &v, const UPropertyVirtual<T,OwnerT> &prop)
{ return prop.v() < v; };

/* ************************************************************************* */
// ����� - �������� �� ��������� ������
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type>
class UPropertyLocal: public UPropertyVirtual<T,OwnerT>
{
protected:
//protected:
// ������
mutable T Value;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyLocal(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyVirtual<T,OwnerT>(name,owner, type, setmethod, 0, dynamic_prop_flag), Value() { };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
protected:
// ������� ��������
virtual const T& GetDataValueLocal(void) const
{
 return Value;
};
	 /*
virtual T& GetData(void)
{
 return v;
};            */


virtual bool SetDataValueLocal(const T &value)
{
 Value=value;
 return true;
};

// ������������ ������
virtual bool SetData(const T& data)
{
 if(this->CheckEqualsFlag && Value == data)
  return true;

 if(this->SetterR && !(this->Owner->*SetterR)(data))
  throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());
 SetDataValueLocal(data);
 this->RenewUpdateTime();
 return true;
}

public:
// TODO: ������ ������
T& v(void)
{
 return this->Value;
};

const T& operator [] (int index) const
{ return this->GetData()[index]; };

T& operator [] (int index)
{ return GetData()[index]; }; // TODO: �������� ��������� ��� �������� �� ����� �����

// �������� ������������
UPropertyLocal<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UPropertyLocal<T,OwnerT,type>& operator = (const UPropertyLocal<T,OwnerT,type> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};

template<typename T,class OwnerT, unsigned int type>
class UProperty: public UPropertyLocal<T,OwnerT,type>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
UProperty(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<T,OwnerT,type>(name,owner, setmethod, dynamic_prop_flag) { };
// -----------------------------

// �������� ������������
UProperty<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

template<typename T, typename V, class OwnerT, unsigned int type>
UProperty<T,OwnerT,type>& operator = (const V &value)
{
 this->SetData(value);
 return *this;
};

UProperty<T,OwnerT,type>& operator = (const UProperty<T,OwnerT,type> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};

// ����� - ��������-��������� �� ��������� ������
template<typename T, typename RangeT, typename OwnerT, unsigned int type>
class UPropertyRangeLocal: public UPropertyVirtual<T,OwnerT>
{
protected: // ���� ������� �����-������
typedef bool (OwnerT::*VSetterRT)(const RangeT&);

protected: // ������
// ������ �����-������
VSetterRT VSetterR;

RangeT Value;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyRangeLocal(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyVirtual<T,OwnerT>(name, owner, type, setmethod, 0, dynamic_prop_flag), VSetterR(0)
{
 this->SetNumConnectionsLimit(-1);
};

UPropertyRangeLocal(const std::string &name, OwnerT * const owner, VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyVirtual<T,OwnerT>(name, owner, type, (typename UPropertyVirtual<T,OwnerT>::SetterRT)0,0, dynamic_prop_flag)
{
 VSetterR=setmethod;
 this->SetNumConnectionsLimit(-1);
};
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
protected:
// ������� ��������
virtual const T& GetDataLocal(void) const
{
 RangeT::const_iterator I=Value.begin();
 std::advance(I,this->CurrentInputIndex);
 return *I;
};

virtual void SetDataLocal(const T &value)
{
 RangeT::iterator I=Value.begin();
 std::advance(I,this->CurrentInputIndex);

 if(this->CheckEqualsFlag && *I == value)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());
 }

 *I=value;
 this->RenewUpdateTime();
};

public:
const T& operator [] (int index) const
{
 this->CurrentInputIndex=index;
 return this->GetData();
};

T& operator [] (int index) // TODO: �������� ��������� ��� �������� �� ����� �����
{ return Value[index]; };

const RangeT& GetRangeData(void) const
{ return Value; };

UPropertyRangeLocal<T,RangeT, OwnerT,type>& operator = (const RangeT &value)
{
 typename RangeT::const_iterator I=value.begin();
 this->CurrentInputIndex=0;
 for(;I != value.end(); ++I)
 {
//  this->CurrentInputIndex
  this->SetData(*I);
  ++this->CurrentInputIndex;  // TODO: ��� �������� ������ �� �������
 }
 return *this;
};

UPropertyRangeLocal<T,RangeT, OwnerT,type>& operator = (const UPropertyRangeLocal<T,RangeT, OwnerT,type> &v)
{
 *this=v.Value;
 return *this;
};

public:
/// ��������� �������� �� ���������� ����� �� ��������� output_property
virtual bool FinalizeConnectToOutput(UIPropertyOutput *output_property)
{
 if(!UIPropertyInputBase::FinalizeConnectToOutput(output_property))
  return false;
 Value.resize(this->GetNumConnections());
 return true;
}

/// ��������� �������� �� ����������� ����� �� ��������� output_property
virtual bool FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(!UIPropertyInputBase::FinalizeDisconnectFromOutput(output_property,c_index))
  return false;

 typename RangeT::iterator I=Value.begin();
 std::advance(I,c_index);
 Value.erase(I);
 return true;
}
// -----------------------------

};


// ����� - ��������-��������� �� ��������� ������
template<typename T, typename RangeT, typename OwnerT, unsigned int type>
class UPropertyRange: public UPropertyRangeLocal<T,RangeT,OwnerT,type>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<T,RangeT, OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};

UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyRangeLocal<T, RangeT, OwnerT, type>::VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<T,RangeT, OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
UPropertyRange<T,RangeT, OwnerT,type>& operator = (const RangeT &value)
{
 static_cast<UPropertyRangeLocal<T,RangeT, OwnerT,type>& >(*this)=value;
 return *this;
};

UPropertyRange<T,RangeT, OwnerT,type>& operator = (const UPropertyRange<T,RangeT, OwnerT,type> &v)
{
 static_cast<UPropertyRangeLocal<T,RangeT, OwnerT,type>& >(*this)=v.Value;
 return *this;
};
// -----------------------------
};

template<typename T,class OwnerT>
class UPropertyInput: public UPropertyBaseOwner<T,OwnerT>
{
protected:
// ������
mutable T Value;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyInput(const std::string &name, OwnerT * const owner, bool dynamic_prop_flag=false)
 : UPropertyBaseOwner<T,OwnerT>(name,owner, ptPubInput, dynamic_prop_flag), Value() { };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
protected:
// ������� ��������
virtual const T& GetDataValueLocal(void) const
{
 return Value;
};

virtual bool SetDataValueLocal(const T &value)
{
 return false;
};

public:
// ���������� ������ �� ������
virtual const T& GetData(void) const
{
 if(IsConnected())
 {
  UEPtr<UPropertyBase<T> > prop=dynamic_pointer_cast<UPropertyBase<T> >(GetConnectedProperty(CurrentInputIndex));
  UEPtr<UNet> owner=prop->GetOwner();
  return *dynamic_pointer_cast<T>(owner);
 }
 else
 {
  return GetDataValueLocal();
 }
}

// ������������ ������
virtual bool SetData(const T& data)
{
 return false;
}

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 return true;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 return true;
};

// ����� ���������� ��� ����� �������� � ������ ���������
virtual bool CompareLanguageType(const UIProperty &dt) const
{
 const UIPropertyOutput* out=dynamic_cast<const UIPropertyOutput*>(&dt);
 if(!out)
  return false;
 return (dynamic_cast<const T*>(out->GetOwner()))?true:false;
}

// ����� �������� ��� ��������
bool ChangeType(unsigned int value)
{
 return false;
}

const T* operator -> (void) const
{ return &this->GetData(); };

T* operator -> (void)
{ return const_cast<T*>(&this->GetData()); };

const T& operator * (void) const
{ return this->GetData(); };

T& operator * (void)
{ return const_cast<T&>(this->GetData()); };
};

#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */



}

#endif

