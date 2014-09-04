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
#include "../Math/MUXMLSerialize.h"
#include "../Math/MUBinarySerialize.h"
#include "UComponent.h"

namespace RDK {

using namespace std;

#ifdef __BORLANDC__
#pragma warning( disable : 4700)
#endif

// ����� - ���� ��� �������
template<typename T>
class UVBaseDataProperty: public UIProperty
{
protected: // ������
// ������ ������ � ������
mutable T* PData;

// ��� �����
int IoType;

// �������� �������� ������
int MinRange, MaxRange;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVBaseDataProperty(void)
 : PData(0),IoType(0),MinRange(0), MaxRange(-1)
{
}

UVBaseDataProperty(T * const pdata)
 : PData(pdata),IoType(0),MinRange(0), MaxRange(-1)
{
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
};

// ������������ ������
virtual void SetData(const T& data)
{
 (PData)?*PData=data:throw EPropertyZeroPtr(GetOwnerName(),GetName());
};

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
 UEPtr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<GetData();
  return true;
 }

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

 UEPtr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  operator >> (*binary,temp);
  SetData(temp);
  return true;
 }

 UEPtr<USerStorageXML> xml=dynamic_pointer_cast<USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->SelectRoot();
   if(xml->GetNodeName() != GetName())
	return false;
   operator >> (*xml,temp);
   SetData(temp);
   xml->SelectUp();
   return true;
  }
  else
  {
   if(!xml->SelectNode(GetName()))
	return false;
   operator >> (*xml,temp);
   SetData(temp);
   xml->SelectUp();
   return true;
  }
 }

 return false;
};

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

virtual bool CheckRange(int index)
{
 if(IoType & ipSingle)
  return (MinRange == index && MaxRange<0) | (index>=MinRange && index<=MaxRange);
 else
 if(IoType & ipRange)
  return (MinRange <= index && MaxRange<0) | (index>=MinRange && index<=MaxRange);

 return false;
}

// �������� �������� ������
virtual int GetMinRange(void)
{ return UVBaseDataProperty<T>::MinRange; };

virtual int GetMaxRange(void)
{ return UVBaseDataProperty<T>::MaxRange; };
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
/// ���������� ��������� �� ������
virtual void const* GetPointer(int index) const
{
 return 0;
}

/// ������������� ��������� �� ������
virtual bool SetPointer(int index, void* value)
{
 return false;
}
// --------------------------

};


// ����� - ���� ��� �������
template<typename T,class OwnerT>
class UVBaseProperty: public UVBaseDataProperty<T>
{
//friend class OwnerT;
protected: // ���� ������� �����-������

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
UVBaseProperty(OwnerT * const owner) :
  Owner(owner)
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
protected: // ���� ������� �����-������
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // ������
// ������ �����-������
GetterRT GetterR;
SetterRT SetterR;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), /*Getter(0), Setter(0), */GetterR(getmethod), SetterR(setmethod)
{
}

UVProperty(OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UVBaseProperty<T,OwnerT>(owner,pdata), /*Getter(0), Setter(0), */GetterR(0), SetterR(setmethod)
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

  if(GetterR)
   return (this->Owner->*GetterR)();
 }

 throw UIProperty::EPropertyZeroPtr(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
};

// ��������� ��������
virtual void SetData(const T &value)
{
 if(this->PData && !SetterR)
  *this->PData=value;

 if(this->Owner && SetterR)
 {
  if(!(this->Owner->*SetterR)(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

  if(this->PData)
   *this->PData=value;
 }
};
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
operator T (void) const
{
 return this->GetData();
};

const T& operator () (void) const
{
 return this->GetData();
};

T* operator -> (void)
{ return const_cast<T*>(&this->GetData()); };

const T* operator -> (void) const
{ return &this->GetData(); };

T& operator * (void)
{ return const_cast<T&>(this->GetData()); };

const T& operator * (void) const
{ return this->GetData(); };

bool operator ! (void) const
{
 return (&this->GetData())?true:false;
};



// �������� ������������
UVProperty<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UVProperty<T,OwnerT>& operator = (const UVProperty<T,OwnerT> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};


/// ����� - ����������� ��������
/// �� �������� ������� ������ ����
/// Setter � Getter ������������ ������ �� ��������
/// ������� �������� �������������� ����� ���������� �����
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
  *this->PData=value;

 if(this->Owner && Setter)
 {
  if(!(this->Owner->*Setter)(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

  if(this->PData)
   *this->PData=value;
 }
};
// -----------------------------
};

/* ************************************************************************* */
// ����� - �������� � ��������� ������
/* ************************************************************************* */
template<typename T,class OwnerT>
class UProperty: public UVProperty<T,OwnerT>
{
public:
//protected:
// ������
mutable T v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), v() { this->PData=&v; };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 return v;
};

virtual void SetData(const T &value)
{
 if(v == value)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

  v=value;
  return;
 }

 v=value;
 return;
};
// -----------------------------
};
/* ************************************************************************* */

/* ************************************************************************* */
// ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename T, typename OwnerT>
class UCProperty: public UVProperty<T,OwnerT>
{
protected: // ���� ������� �����-������
typedef typename T::value_type TV;
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // ������
// ������ �����-������
VSetterRT VSetterR;

public:
// ������
T v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UCProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), VSetterR(0), v() { this->PData=&v; };

UCProperty(OwnerT * const owner, VSetterRT setmethod)
 : UVProperty<T,OwnerT>(owner,(typename UVProperty<T,OwnerT>::SetterRT)0,0), v()
{ VSetterR=setmethod; this->PData=&v; };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
virtual void SetData(const T &value)
{
 if(v == value)
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
};
// -----------------------------
};
#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */



}

#endif

