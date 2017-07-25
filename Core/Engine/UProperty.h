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

// ����� - ���� ��� �������
template<typename T,class OwnerT, unsigned int type>
class UPropertyBase: virtual public UIPropertyInputBase, virtual public UIPropertyOutputBase
{
protected: // ������
// ��� ��������
std::string Name;

// �������� ��������
OwnerT* Owner;

/// ��� ��������
unsigned int Type;

// ������ ������ � ������
//mutable T* PData;

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

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyBase(const std::string &name, OwnerT * const owner, bool dynamic_prop_flag=false)
 : Name(name), Owner(owner), Type(type), Mutex(UCreateMutex()), UpdateTime(0), DynamicPropertyFlag(dynamic_prop_flag), CurrentInputIndex(0)
{
   if(Owner)
   {
	reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(this);
//	Variable=Owner->FindPropertyVariable(this);
   }
}

virtual ~UPropertyBase(void)
{
   if(Owner)
   {
	reinterpret_cast<UComponent* const>(Owner)->DelLookupProperty(Name,true);
//	Variable=Owner->FindPropertyVariable(this);
   }
 UDestroyMutex(Mutex);
 Mutex=0;
}
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
protected:
// ���������� ������ �� ������
virtual const T& GetData(void) const
{
 if(IsConnected())
 {
  return dynamic_pointer_cast<UPropertyBase<T,OwnerT, type> >(GetConnectedProperty(CurrentInputIndex))->GetData();
 }
 else
 {
  return GetDataLocal();
 }
}

// ������������ ������
virtual void SetData(const T& data)
{
 if(IsConnected())
 {
  dynamic_pointer_cast<UPropertyBase<T,OwnerT, type> >(GetConnectedProperty(CurrentInputIndex))->SetData(data);
 }
 else
 {
  SetDataLocal(data);
 }
}

// ���������� ������ �� ������
virtual const T& GetDataLocal(void) const=0;

// ������������ ������
virtual void SetDataLocal(const T& data)=0;

public:
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
  res&=DisconnectAllOutputs();

 // ���� �������� ��������� ���� ������� �� ��������� ���� � ���� ��� ����������
 if((Type & ptOutput) && !(value & ptOutput))
  res&=DisconnectAllInputs();

 if(res)
  Type=value;
 else
  return false;

 return true;
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
// -----------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
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
// ������ ������������
// -----------------------------
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


// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const
{
 return Name;
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

/// ���������� ��������� �� ��������� ��������
virtual const UNet* GetOwner(void) const
{
 return dynamic_cast<const UNet*>(Owner);
}

virtual UNet* GetOwner(void)
{
 return dynamic_cast<UNet*>(Owner);
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

// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T,class OwnerT, unsigned int type>
class UPropertyVirtual: public UPropertyBase<T,OwnerT,type>
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

T* RawDataPtr;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyVirtual(const std::string &name, OwnerT * const owner, SetterRT setmethod , GetterRT getmethod, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(getmethod), SetterR(setmethod), RawDataPtr(0), UPropertyBase<T,OwnerT,type>(name, owner, dynamic_prop_flag)
{
}

UPropertyVirtual(const std::string &name, OwnerT * const owner, T * const pdata, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(0), SetterR(0), RawDataPtr(pdata), UPropertyBase<T,OwnerT,type>(name,owner,dynamic_prop_flag)
{
}

virtual ~UPropertyVirtual(void)
{

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
// ������ ����������
// -----------------------------
protected:
// ������� ��������
virtual const T& GetDataLocal(void) const
{
 if(this->Owner && GetterR)
 {
  if(GetterR)
   return (this->Owner->*GetterR)();
 }
 else
 if(RawDataPtr)
  return *RawDataPtr;

 throw UIProperty::EPropertyZeroPtr(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());
};

// ��������� ��������
virtual void SetDataLocal(const T &value)
{
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
   throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

  if(CheckEqualsFlag && (this->Owner->*GetterR)() == value)
   return;

  if(!SetterR)
   throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

  if(this->Owner && SetterR)
  {
   if(!(this->Owner->*SetterR)(value))
	throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

   this->RenewUpdateTime();
  }
 }
};
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
public:
operator T (void) const
{
 return this->GetData();
};

const T& operator () (void) const
{
 return this->GetData();
};
				   /*
T* operator -> (void)
{ return const_cast<T*>(&this->GetData()); };
                     */
const T* operator -> (void) const
{ return &this->GetData(); };

/*T& operator * (void)
{ return const_cast<T&>(this->GetData()); };
  */
const T& operator * (void) const
{ return this->GetData(); };

const T& operator [] (int index) const
{ return this->GetData(index); };

// �������� ������������
UPropertyVirtual<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UPropertyVirtual<T,OwnerT,type>& operator = (const UPropertyVirtual<T,OwnerT,type> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};

/* ************************************************************************* */
// ����� - �������� �� ��������� ������
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type>
class UProperty: public UPropertyVirtual<T,OwnerT,type>
{
protected:
//protected:
// ������
mutable T v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UProperty(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyVirtual<T,OwnerT,type>(name,owner, setmethod, 0), v() { };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
protected:
// ������� ��������
virtual const T& GetDataLocal(void) const
{
 return v;
};
	 /*
virtual T& GetData(void)
{
 return v;
};            */


virtual void SetDataLocal(const T &value)
{
 if(CheckEqualsFlag && v == value)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT,type>::GetOwnerName(),UPropertyVirtual<T,OwnerT,type>::GetName());
 }

 v=value;
 this->RenewUpdateTime();
};

public:
const T& operator [] (int index) const
{ return GetData()[index]; };
				 /*
T& operator [] (int index)
{ return GetData()[index]; };   */
// -----------------------------
};
/* ************************************************************************* */

/* ************************************************************************* */
// ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename T, typename RangeT, typename OwnerT, unsigned int type>
class UPropertyRange: public UPropertyVirtual<T,OwnerT,type>
{
protected: // ���� ������� �����-������
typedef bool (OwnerT::*VSetterRT)(const RangeT&);

protected: // ������
// ������ �����-������
VSetterRT VSetterR;

RangeT v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod)
 : UPropertyVirtual<T,OwnerT,type>(name, owner, setmethod, 0), VSetterR(0)
{
 SetNumConnectionsLimit(-1);
};

UPropertyRange(const std::string &name, OwnerT * const owner, VSetterRT setmethod)
 : UProperty<T,OwnerT,type>(name, owner,(typename UPropertyVirtual<T,OwnerT,type>::SetterRT)0,0)
{
 VSetterR=setmethod;
 SetNumConnectionsLimit(-1);
};
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
protected:
// ������� ��������
virtual const T& GetDataLocal(void) const
{
 return v[CurrentInputIndex];
};
/*
virtual T& GetData(void)
{
 return v[index];
};        */

virtual void SetDataLocal(const T &value)
{
 if(CheckEqualsFlag && v[CurrentInputIndex] == value)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT,type>::GetOwnerName(),UPropertyVirtual<T,OwnerT,type>::GetName());
 }

 v[CurrentInputIndex]=value;
 this->RenewUpdateTime();
};

public:
const T& operator [] (int index) const
{
 CurrentInputIndex=index;
 return GetData();
};
/*
T& operator [] (int index)
{ return v[index]; };
								  */
public:
/// ��������� �������� �� ���������� ����� �� ��������� output_property
virtual bool FinalizeConnectToOutput(UIPropertyOutput *output_property)
{
 if(!UIPropertyInputBase::FinalizeConnectToOutput(output_property))
  return false;
 v.resize(GetNumConnections());
 return true;
}

/// ��������� �������� �� ����������� ����� �� ��������� output_property
virtual bool FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(!UIPropertyInputBase::FinalizeDisconnectFromOutput(output_property,c_index))
  return false;
 v.erase(v.begin()+c_index);
 return true;
}
// -----------------------------

};
#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */



}

#endif

