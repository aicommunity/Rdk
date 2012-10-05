/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru
Version:        3.0.0

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


// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UVLProperty: public UVProperty<T,OwnerT>
{
friend class UADataComponent;
protected: // ���� ������� �����-������

protected: // ������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod ,
								typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
UVLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod ,
								typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// ������ ����������
// -----------------------------
// �������� ������������
UVLProperty& operator = (const T &value)
{
 (*static_cast<UVProperty<T,OwnerT>* >(this)) = value;
 return *this;
};

UVLProperty& operator = (const UVLProperty &v)
{
 (*static_cast<UVProperty<T,OwnerT>* >(this)) = v;
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
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
// �������� ������������
ULProperty& operator = (const T &value)
{
 (*static_cast<UProperty<T,OwnerT>* >(this)) = value;
 return *this;
};
// -----------------------------

};
/* ************************************************************************* */

/* ************************************************************************* */
// ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename TC, typename OwnerT, unsigned int type=ptPubParameter>
class UCLProperty: public UCProperty<TC,OwnerT>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterT setmethod=0)
 : UCProperty<TC,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterRT setmethod)
 : UCProperty<TC,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

//����������� ������������� ��� ��������� ��������
UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<TC,OwnerT>::VSetterT setmethod)
 : UCProperty<TC,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<TC,OwnerT>::VSetterRT setmethod)
 : UCProperty<TC,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
// �������� ������������
UCLProperty& operator = (const TC &value)
{
 (*static_cast<UCProperty<TC,OwnerT>* >(this)) = value;
 return *this;
};

UCLProperty& operator = (const UCLProperty &value)
{
 (*static_cast<UCProperty<TC,OwnerT>* >(this)) = value;
 return *this;
};

// �������� ������� ������� ����������
UCLProperty& operator = (size_t size)
{
 (*static_cast<UCProperty<TC,OwnerT>* >(this)) = size;
 return *this;
};
// -----------------------------
};
/* ************************************************************************* */

}
#endif

