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
#include "UADataComponent.h"

namespace RDK {

using namespace std;

#pragma warning( disable : 4700)
// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T,class OwnerT>
class UVProperty: public UIProperty
{
//friend class OwnerT;
protected: // ���� ������� �����-������
typedef T (OwnerT::*GetterT)(void) const;
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterT)(T);
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // ������
// �������� ��������
OwnerT* Owner;

// ������ ������ � ������
T* PData;

// ������ �����-������
GetterT Getter;
SetterT Setter;
GetterRT GetterR;
SetterRT SetterR;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVProperty(OwnerT * const owner, SetterT setmethod , GetterT getmethod) :
  Owner(owner), PData(0), Getter(getmethod), Setter(setmethod), GetterR(0), SetterR(0) { };
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  Owner(owner), PData(0), Getter(0), Setter(0), GetterR(getmethod), SetterR(setmethod)   { };
UVProperty(OwnerT * const owner, T * const pdata) :
  Owner(owner), PData(pdata), Getter(0), Setter(0), GetterR(0), SetterR(0)   { };
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
// �������������
void Init(OwnerT * const owner, SetterT setmethod, GetterT getmethod)
{ Owner = owner; PData=0; Getter = getmethod; Setter = setmethod; SetterR=0; };
void Init(OwnerT * const owner, SetterRT setmethod, GetterT getmethod)
{ Owner = owner; PData=0; Getter = getmethod; SetterR = setmethod; Setter=0; };
void Init(OwnerT * const owner, T * const pdata)
{ Owner = owner; PData=pdata; Getter = 0; GetterR=0; SetterR = 0; Setter=0; };

// ������� �������� ��� �������� ������� ������ � ���������
const T Get(void)
{
 if(PData)
  return *PData;

 if(Getter)
  return (Owner->*Getter)();

 if(GetterR)
  return (Owner->*GetterR)();

 return T();
};

// ��������� �������� ��� �������� ������� ������ � ���������
bool Set(T value)
{ return (PData)?(*PData):((Owner->*Setter)(value)); };

bool Set(const T &value)
{ return (PData)?(*PData):((Owner->*SetterR)(value)); };

T operator () (void)
{
 if(Owner)
 {
  if(PData)
   return *PData;

  if(Getter)
   return (Owner->*Getter)();

  if(GetterR)
   return (Owner->*GetterR)();
 }

 T val;
 return val;
};

operator T (void) const
{
 if(Owner)
 {
  if(PData)
   return *PData;

  if(Getter)
   return (Owner->*Getter)();

  if(GetterR)
   return (Owner->*GetterR)();
 }

 T val;
 return val;
};

// �������� ������������
UVProperty& operator = (const T &value)
{
 if(PData)
  *PData=value;

 if(Owner && Setter)
  (Owner->*Setter)(value);
 else
 if(Owner && SetterR)
  (Owner->*SetterR)(value);

 return *this;
};

UVProperty& operator = (const UVProperty &v)
{
 if(v.Owner)
 {
  if(v.PData)
   (*this)=*v.PData;

  if(v.Getter)
  {
   *this=(v.Owner->*Getter)();
   return *this;
  }

  if(v.GetterR)
  {
   *this=(v.Owner->*GetterR)();
   return *this;
  }
 }

 return *this;
};
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const
{
 static std::string name;
 name=reinterpret_cast<UADataComponent* const>(Owner)->FindPropertyName(this);
 if(name == ForbiddenName)
  name=reinterpret_cast<UADataComponent* const>(Owner)->FindStateName(this);
 return name;
};

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerName(void) const
{
 return typeid(Owner).name();
};

// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
 UEPtr<Serialize::USerStorageBinary> binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<(*this)();
  return true;
 }

 UEPtr<Serialize::USerStorageXML> xml=dynamic_pointer_cast<Serialize::USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->Create(GetName());
   Serialize::operator << (*xml,(*this)());
   xml->SelectUp();
   return true;
  }
  else
  {
   xml->AddNode(GetName());
   Serialize::operator << (*xml,(*this)());
   xml->SelectUp();
   return true;
  }
 }

 return false;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
 T temp;

 UEPtr<Serialize::USerStorageBinary> binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  Serialize::operator >> (*binary,temp);
  *this=temp;
  return true;
 }

 UEPtr<Serialize::USerStorageXML> xml=dynamic_pointer_cast<Serialize::USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->SelectRoot();
   if(xml->GetNodeName() != GetName())
	return false;
   Serialize::operator >> (*xml,temp);
   *this=temp;
   xml->SelectUp();
   return true;
  }
  else
  {
   if(!xml->SelectNode(GetName()))
	return false;
   Serialize::operator >> (*xml,temp);
   *this=temp;
   xml->SelectUp();
   return true;
  }
 }

 return false;
};
// -----------------------------
};

/* ************************************************************************* */
// ����� - �������� � ��������� ������
/* ************************************************************************* */
template<typename T,class OwnerT>
class UProperty: public UVProperty<T,OwnerT>
{
//friend class OwnerT;
public:
//protected:
// ������
T v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), v() { };

UProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), v() { };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
const T& operator () (void) const
{ return v; };

operator T (void) const
{ return v; };

// �������� ������������
UProperty& operator = (const T &value)
{
 if(v == value)
  return *this;

 if(this->Owner)
 {
//  if((this->Setter && !(*(this->Setter))(value)) ||
//	 (this->SetterR && !(*(this->SetterR))(value)))
//	  if((this->Setter && !(this->Owner->*Setter)(value)) ||
//	     (this->SetterR && !(this->Owner->*SetterR)(value)))
	  if((this->Setter && !(this->Owner->*(this->Setter))(value)) ||
		 (this->SetterR && !(this->Owner->*(this->SetterR))(value)))
	   return *this;

  v=value;
  return *this;
 }

 v=value;
 return *this;
};

UProperty& operator = (const UProperty &value)
{ return (*this)=value.v; };

T* operator -> (void)
{ return &v; };

T& operator * (void)
{ return v; };
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
/* Serialize::operator << (*storage,(*this)());
 return true;
*/
 UEPtr<Serialize::USerStorageBinary>   binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<(*this)();
  return true;
 }

 UEPtr<Serialize::USerStorageXML> xml=dynamic_pointer_cast<Serialize::USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->Create(this->GetName());
   Serialize::operator << (*xml,(*this)());
   xml->SelectUp();
   return true;
  }
  else
  {
   xml->AddNode(this->GetName());
   Serialize::operator << (*xml,(*this)());
   xml->SelectUp();
   return true;
  }
 }

// return storage->Save((*this)());
// Serialize::operator << (*storage,(*this)());
 return false;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
/* T temp;
 Serialize::operator >> (*storage,temp);
 *this=temp;
 return true;*/
 T temp;
 //Serialize::operator >> (*storage,temp);
// if(!storage || !storage->Load(temp))
//  return false;
 UEPtr<Serialize::USerStorageBinary>   binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  Serialize::operator >> (*binary,temp);
  *this=temp;
  return true;
 }

 UEPtr<Serialize::USerStorageXML> xml=dynamic_pointer_cast<Serialize::USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->SelectRoot();
   if(xml->GetNodeName() != this->GetName())
	return false;
   Serialize::operator >> (*xml,temp);
   *this=temp;
   xml->SelectUp();
   return true;
  }
  else
  {
   if(!xml->SelectNode(this->GetName()))
	return false;
   Serialize::operator >> (*xml,temp);
   *this=temp;
   xml->SelectUp();
   return true;
  }
 }

 return false;
};
// -----------------------------
};
/* ************************************************************************* */

/* ************************************************************************* */
// ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename TC, typename OwnerT>
class UCProperty: public UVProperty<TC,OwnerT>
{
//friend class OwnerT;
protected: // ���� ������� �����-������
typedef typename TC::value_type TV;

//typedef T (OwnerT::*VGetterT)(void) const;
typedef bool (OwnerT::*VSetterT)(TV);
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // ������
// ������ �����-������
VSetterT VSetter;
VSetterRT VSetterR;

protected:
// ������
TC v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UCProperty(OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterT setmethod=0)
 : UVProperty<TC,OwnerT>(owner, setmethod, 0), v(), VSetter(0), VSetterR(0) { };

UCProperty(OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterRT setmethod)
 : UVProperty<TC,OwnerT>(owner, setmethod, 0), v(), VSetter(0), VSetterR(0) { };

//����������� ������������� ��� ��������� ��������
UCProperty(OwnerT * const owner, VSetterT setmethod)
 : UVProperty<TC,OwnerT>(owner,(typename UVProperty<TC,OwnerT>::SetterT)0,0), v()
{ VSetter=setmethod; };

UCProperty(OwnerT * const owner, VSetterRT setmethod)
 : UVProperty<TC,OwnerT>(owner,(typename UVProperty<TC,OwnerT>::SetterT)0,0), v()
{ VSetterR=setmethod; };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
const TC& operator () (void) const
{ return v; };

// ������ �������� ����������
const TV& operator () (int i) const
{ return v[i]; };

// ������ �������� ����������
bool operator () (int i, const TV &value)
{
 if(VSetterR && !(this->Owner->*VSetterR)(value))
  return false;

 if(i<0 || i>v.size())
  return false;

 v[i]=value;

 return true;
};

bool operator () (int i, TV value)
{
 if(VSetter && !(this->Owner->*VSetter)(value))
  return false;

 if(i<0 || i>v.size())
  return false;

 v[i]=value;

 return true;
};

 // �������� ������������
UCProperty& operator = (const TC &value)
{
 if(v == value)
  return *this;

 if(this->Owner)
 {
  if(VSetter)
  {
   typename TC::const_iterator I,J;
   I=value.begin(); J=value.end();
   while(I != J)
   {
    if(!(this->Owner->*VSetter)(*I))
     return *this;

    ++I;
   }
  }
  else
  if(VSetterR)
  {
   typename TC::const_iterator I,J;
   I=value.begin(); J=value.end();
   while(I != J)
   {
    if(!(this->Owner->*VSetterR)(*I))
     return *this;

    ++I;
   }
  }
  else
  {
   if((this->Setter && !(this->Owner->*(this->Setter))(value)) ||
	 (this->SetterR && !(this->Owner->*(this->SetterR))(value)))
	return *this;
  }
 }

 v=value;
 return *this;
};

UCProperty& operator = (const UCProperty &value)
{ return (*this)=value.v; };

// �������� ������� ������� ����������
UCProperty& operator = (size_t size)
{
 v.resize(size);
 return *this;
};

TC* operator -> (void)
{ return &v; };

TC& operator * (void)
{ return v; };
// -----------------------------

// -----------------------------
// ������� ��������� ������� ������ ��� �������������� ������
// -----------------------------
public:
TV& operator [] (int i)
{ return v[i]; };
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
public:
// ����� ���������� �������� �������� � �����
virtual bool Save(UEPtr<Serialize::USerStorage> storage, bool simplemode=false)
{
/* Serialize::operator << (*storage,(*this)());
 return true;
 */
 UEPtr<Serialize::USerStorageBinary>   binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<(*this)();
  return true;
 }

 UEPtr<Serialize::USerStorageXML> xml=dynamic_pointer_cast<Serialize::USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->Create(this->GetName());
   Serialize::operator << (*xml,(*this)());
   xml->SelectUp();
   return true;
  }
  else
  {
   xml->AddNode(this->GetName());
   Serialize::operator << (*xml,(*this)());
   xml->SelectUp();
   return true;
  }
 }

// return storage->Save((*this)());
// Serialize::operator << (*storage,(*this)());
 return false;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(UEPtr<Serialize::USerStorage> storage, bool simplemode=false)
{
/*
 TC temp;
 Serialize::operator >> (*storage,temp);
 *this=temp;
 return true;
 */

 TC temp;
 //Serialize::operator >> (*storage,temp);
// if(!storage || !storage->Load(temp))
//  return false;
 UEPtr<Serialize::USerStorageBinary> binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  Serialize::operator >> (*binary,temp);
  *this=temp;
  return true;
 }

 UEPtr<Serialize::USerStorageXML> xml=dynamic_pointer_cast<Serialize::USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->SelectRoot();
   if(xml->GetNodeName() != this->GetName())
	return false;
   Serialize::operator >> (*xml,temp);
   *this=temp;
   xml->SelectUp();
   return true;
  }
  else
  {
   if(!xml->SelectNode(this->GetName()))
	return false;
   Serialize::operator >> (*xml,temp);
   *this=temp;
   xml->SelectUp();
   return true;
  }
 }

 return false;
};
// -----------------------------

};
#pragma warning( default : 4700)
/* ************************************************************************* */



}

#endif

