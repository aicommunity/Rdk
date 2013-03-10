/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2012.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru
Version:        1.0.0

This file - part of the project: RDK

File License:        BSD License
Project License:    BSD License
See file license.txt for more information
*********************************************************** */

#ifndef ULocalProperty_H
#define ULocalProperty_H

#include <string>
#include <sstream>
#include "UProperty.h"

namespace RDK {

using namespace std;

// ����� - ������� ����������� ��������
// �� �������� ������� ������ ����
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UVBaseLProperty: public UVBaseProperty<T,OwnerT>
{
friend class UComponent;
protected: // ���� ������� �����-������

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVBaseLProperty(const string &name, OwnerT * const owner)
 : UVBaseProperty<T,OwnerT>(owner)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UVBaseLProperty(const string &name, OwnerT * const owner, T * const pdata)
 : UVBaseProperty<T,OwnerT>(owner, pdata)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------
};



// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UVLProperty: public UVProperty<T,OwnerT>
{
friend class UComponent;
protected: // ���� ������� �����-������

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
/*UVLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod ,
								typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
*/
UVLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod ,
								typename UVProperty<T,OwnerT>::GetterRT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UVLProperty(const string &name, OwnerT * const owner, T * const pdata, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(owner, pdata,setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
operator T (void) const
{
 return GetData();
};

const T& operator () (void) const
{
 return GetData();
};

T* operator -> (void)
{ return const_cast<T*>(&GetData()); };

const T* operator -> (void) const
{ return &GetData(); };

T& operator * (void)
{ return const_cast<T&>(GetData()); };

const T& operator * (void) const
{ return GetData(); };

// �������� ������������
UVLProperty& operator = (const T &value)
{
 (*static_cast<UVProperty<T,OwnerT>* >(this)) = value;
 return *this;
};

UVLProperty& operator = (const UVLProperty &v)
{
 SetData(v.GetData());
 return *this;
};
// -----------------------------
};

/* ************************************************************************* */
// ����� - �������� � ��������� ������
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class ULProperty: public UProperty<T,OwnerT>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
/*ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
  */
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
operator T (void) const
{
 return GetData();
};

const T& operator () (void) const
{
 return GetData();
};

T* operator -> (void)
{ return const_cast<T*>(&GetData()); };

const T* operator -> (void) const
{ return &GetData(); };

T& operator * (void)
{ return const_cast<T&>(GetData()); };

const T& operator * (void) const
{ return GetData(); };

// �������� ������������
ULProperty& operator = (const T &value)
{
 SetData(value);
 return *this;
};
// -----------------------------

};
/* ************************************************************************* */

/* ************************************************************************* */
// ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UCLProperty: public UCProperty<T,OwnerT>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
/*UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UCProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
  */
UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UCProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

//����������� ������������� ��� ��������� ��������
/*UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterT setmethod)
 : UCProperty<T,OwnerT>(owner,setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
  */
UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterRT setmethod)
 : UCProperty<T,OwnerT>(owner,setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
// ������ �������� ����������
const typename UCProperty<T,OwnerT>::TV& operator () (int i) const
{ return v[i]; };

// ������ �������� ����������
bool operator () (int i, const typename UCProperty<T,OwnerT>::TV &value)
{
 if(VSetterR && !(this->Owner->*VSetterR)(value))
  return false;

 if(i<0 || i>v.size())
  return false;

 v[i]=value;

 return true;
};

operator T (void) const
{
 return GetData();
};

const T& operator () (void) const
{
 return GetData();
};

T* operator -> (void)
{ return const_cast<T*>(&GetData()); };

const T* operator -> (void) const
{ return &GetData(); };

T& operator * (void)
{ return const_cast<T&>(GetData()); };

const T& operator * (void) const
{ return GetData(); };

// �������� ������������
UCLProperty& operator = (const T &value)
{
 SetData(value);
 return *this;
};

UCLProperty& operator = (const UCLProperty &value)
{
 SetData(value.GetData());
 return *this;
};
// -----------------------------

// -----------------------------
// ������� ��������� ������� ������ ��� �������������� ������
// -----------------------------
public:
typename UCProperty<T,OwnerT>::TV& operator [] (int i)
{ return v[i]; };

const typename UCProperty<T,OwnerT>::TV& operator [] (int i) const
{ return v[i]; };
// -----------------------------
};
/* ************************************************************************* */

}
#endif

