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

// Класс - базовое виртуальное свойство
// Не содержит данного внутри себя
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UVBaseLProperty: public UVBaseProperty<T,OwnerT>
{
friend class UADataComponent;
protected: // Типы методов ввода-вывода

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVBaseLProperty(const string &name, OwnerT * const owner)
 : UVBaseProperty<T,OwnerT>(owner)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UVBaseLProperty(const string &name, OwnerT * const owner, T * const pdata)
 : UVBaseProperty<T,OwnerT>(owner, pdata)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------
};



// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UVLProperty: public UVProperty<T,OwnerT>
{
friend class UADataComponent;
protected: // Типы методов ввода-вывода

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod ,
								typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
UVLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod ,
								typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UVLProperty(const string &name, OwnerT * const owner, T * const pdata)
 : UVProperty<T,OwnerT>(owner, pdata)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// Методы управления
// -----------------------------
// Оператор присваивания
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
// Класс - свойство с значением внутри
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class ULProperty: public UProperty<T,OwnerT>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Оператор присваивания
ULProperty& operator = (const T &value)
{
 (*static_cast<UProperty<T,OwnerT>* >(this)) = value;
 return *this;
};
// -----------------------------

};
/* ************************************************************************* */

/* ************************************************************************* */
// Класс - свойство-контейнер со значением внутри
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UCLProperty: public UCProperty<T,OwnerT>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UCProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UCProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

//Конструктор инициализации для отдельных значений
UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterT setmethod)
 : UCProperty<T,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterRT setmethod)
 : UCProperty<T,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Оператор присваивания
UCLProperty& operator = (const T &value)
{
 (*static_cast<UCProperty<T,OwnerT>* >(this)) = value;
 return *this;
};

UCLProperty& operator = (const UCLProperty &value)
{
 (*static_cast<UCProperty<T,OwnerT>* >(this)) = value;
 return *this;
};

// Оператор задания размера контейнера
UCLProperty& operator = (size_t size)
{
 (*static_cast<UCProperty<T,OwnerT>* >(this)) = size;
 return *this;
};
// -----------------------------
};
/* ************************************************************************* */

}
#endif

