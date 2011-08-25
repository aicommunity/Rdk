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
#include "..\Serialize\USerStorageXML.h"
#include "../Serialize/USerStorageBinary.h"

namespace RDK {

using namespace std;

#pragma warning( disable : 4700)
// ����� ������������ �������
class UIProperty
{
public:
 // ����� ���������� ��������� ��� ��������
 virtual const string& GetName(void) const=0;

 // ����� ���������� ��������� ��� ������-��������� ��������
 virtual string GetOwnerName(void) const=0;

 // ����� ���������� �������� �������� � �����
 virtual bool Save(Serialize::USerStorage *storage)=0;

 // ����� ������ �������� �������� �� ������
 virtual bool Load(Serialize::USerStorage *storage)=0;
};

// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T,typename OwnerT>
class UVProperty: public UIProperty
{
friend OwnerT;
protected: // ���� ������� �����-������
typedef T (OwnerT::*GetterT)(void) const;
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterT)(T);
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // ������
// �������� ��������
OwnerT* Owner;

// ������ �����-������
GetterT Getter;
GetterRT GetterR;
SetterT Setter;
SetterRT SetterR;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVProperty(OwnerT * const owner, SetterT setmethod , GetterT getmethod) :
  Owner(owner), Getter(getmethod), Setter(setmethod), SetterR(0), GetterR(0) { };
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  Owner(owner), GetterR(getmethod), SetterR(setmethod), Setter(0), Getter(0) { };
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
// �������������
void Init(OwnerT * const owner, SetterT setmethod, GetterT getmethod)
{ Owner = owner; Getter = getmethod; Setter = setmethod; SetterR=0; };
void Init(OwnerT * const owner, SetterRT setmethod, GetterT getmethod)
{ Owner = owner; Getter = getmethod; SetterR = setmethod; Setter=0; };

// ������� �������� ��� �������� ������� ������ � ���������
const T Get(void)
{
 if(Getter)
  return (Owner->*Getter)();

 if(GetterR)
  return (Owner->*GetterR)();
};

// ��������� �������� ��� �������� ������� ������ � ���������
bool Set(T value)
{ return (Owner->*Setter)(value); };

bool Set(const T &value)
{ return (Owner->*SetterR)(value); };

T operator () (void)
{
 if(Owner)
 {
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
 if(Owner && Setter)
  (Owner->*Setter)(value);
 else
 if(Owner && SetterR)
  (Owner->*SetterR)(value);

 return *this;
};

UVProperty& operator = (const UVProperty &v)
{
 if(Owner)
 {
  if(Getter)
  {
   *this=(Owner->*Getter)();
   return *this;
  }

  if(GetterR)
  {
   *this=(Owner->*GetterR)();
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
virtual const string& GetName(void) const
{
 static string name;
 name=reinterpret_cast<UAContainer* const>(Owner)->FindPropertyName(this);
 if(name == ForbiddenName)
  name=reinterpret_cast<UAContainer* const>(Owner)->FindStateName(this);
 return name;
};

// ����� ���������� ��������� ��� ������-��������� ��������
virtual string GetOwnerName(void) const
{
 return typeid(Owner).name();
};

// ����� ���������� �������� �������� � �����
virtual bool Save(Serialize::USerStorage *storage)
{
 Serialize::USerStorageBinary * binary=dynamic_cast<Serialize::USerStorageBinary *>(storage);
 if(binary)
 {
  *binary<<(*this)();
  return true;
 }

 Serialize::USerStorageXML * xml=dynamic_cast<Serialize::USerStorageXML *>(storage);
 if(xml)
 {
  xml->AddNode(GetName());
  Serialize::operator << (*xml,(*this)());
  xml->SelectUp();
  return true;
 }

// return storage->Save((*this)());
// Serialize::operator << (*storage,(*this)());
 return false;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(Serialize::USerStorage *storage)
{
 T temp;
 //Serialize::operator >> (*storage,temp);
// if(!storage || !storage->Load(temp))
//  return false;
 Serialize::USerStorageBinary * binary=dynamic_cast<Serialize::USerStorageBinary *>(storage);
 if(binary)
 {
  Serialize::operator >> (*binary,temp);
  *this=temp;
  return true;
 }

 Serialize::USerStorageXML * xml=dynamic_cast<Serialize::USerStorageXML *>(storage);
 if(xml)
 {
  if(!xml->SelectNode(GetName()))
   return false;
  Serialize::operator >> (*xml,temp);
  *this=temp;
  xml->SelectUp();
  return true;
 }

 return false;
};
// -----------------------------
};

/* ************************************************************************* */
// ����� - �������� � ��������� ������
/* ************************************************************************* */
template<typename T,typename OwnerT>
class UProperty: public UVProperty<T,OwnerT>
{
friend OwnerT;
protected:
// ������
T v;

public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UProperty(OwnerT * const owner, UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), v() { };

UProperty(OwnerT * const owner, UVProperty<T,OwnerT>::SetterRT setmethod)
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

 if(Owner)
 {
  if((Setter && !(Owner->*Setter)(value)) ||
     (SetterR && !(Owner->*SetterR)(value)))
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
virtual bool Save(Serialize::USerStorage *storage)
{
 Serialize::operator << (*storage,(*this)());
 return true;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(Serialize::USerStorage *storage)
{
 T temp;
 Serialize::operator >> (*storage,temp);
 *this=temp;
 return true;
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
friend OwnerT;
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
UCProperty(OwnerT * const owner, UVProperty<TC,OwnerT>::SetterT setmethod=0)
 : UVProperty<TC,OwnerT>(owner, setmethod, 0), v(), VSetter(0), VSetterR(0) { };

UCProperty(OwnerT * const owner, UVProperty<TC,OwnerT>::SetterRT setmethod)
 : UVProperty<TC,OwnerT>(owner, setmethod, 0), v(), VSetter(0), VSetterR(0) { };

//����������� ������������� ��� ��������� ��������
UCProperty(OwnerT * const owner, VSetterT setmethod)
 : UVProperty<TC,OwnerT>(owner,(UVProperty<TC,OwnerT>::SetterT)0,0), v()
{ VSetter=setmethod; };

UCProperty(OwnerT * const owner, VSetterRT setmethod)
 : UVProperty<TC,OwnerT>(owner,(UVProperty<TC,OwnerT>::SetterT)0,0), v()
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
 if(VSetterR && !(Owner->*VSetterR)(value))
  return false;

 if(i<0 || i>v.size())
  return false;

 v[i]=value;

 return true;
};

bool operator () (int i, TV value)
{
 if(VSetter && !(Owner->*VSetter)(value))
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

 if(Owner)
 {
  if(VSetter)
  {
   TC::const_iterator I,J;
   I=value.begin(); J=value.end();
   while(I != J)
   {
    if(!(Owner->*VSetter)(*I))
     return *this;

    ++I;
   }
  }
  else
  if(VSetterR)
  {
   TC::const_iterator I,J;
   I=value.begin(); J=value.end();
   while(I != J)
   {
    if(!(Owner->*VSetterR)(*I))
     return *this;

    ++I;
   }
  }
  else
  {
   if((Setter && !(Owner->*Setter)(value)) ||
     (SetterR && !(Owner->*SetterR)(value)))
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
protected:
TV& operator [] (int i)
{ return v[i]; };
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
public:
// ����� ���������� �������� �������� � �����
virtual bool Save(Serialize::USerStorage *storage)
{
 Serialize::operator << (*storage,(*this)());
 return true;
};

// ����� ������ �������� �������� �� ������
virtual bool Load(Serialize::USerStorage *storage)
{
 TC temp;
 Serialize::operator >> (*storage,temp);
 *this=temp;
 return true;
};
// -----------------------------

};
#pragma warning( default : 4700)
/* ************************************************************************* */



}

#endif

