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


// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T, typename OwnerT>
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
UVLProperty(const string &name, unsigned int type, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod ,
								typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,this,type,false); };
UVLProperty(const string &name, unsigned int type, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod ,
								typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,this,type,false); };
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
template<typename T, typename OwnerT>
class ULProperty: public UProperty<T,OwnerT>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
ULProperty(const string &name, unsigned int type, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

ULProperty(const string &name, unsigned int type, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
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
template<typename TC, typename OwnerT>
class UCLProperty: public UCProperty<TC,OwnerT>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
UCLProperty(const string &name, unsigned int type, OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterT setmethod=0)
 : UCProperty<TC,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UCLProperty(const string &name, unsigned int type, OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterRT setmethod)
 : UCProperty<TC,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

//Конструктор инициализации для отдельных значений
UCLProperty(const string &name, unsigned int type, OwnerT * const owner, typename UCProperty<TC,OwnerT>::VSetterT setmethod)
 : UCProperty<TC,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UCLProperty(const string &name, unsigned int type, OwnerT * const owner, typename UCProperty<TC,OwnerT>::VSetterRT setmethod)
 : UCProperty<TC,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Оператор присваивания
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

// Оператор задания размера контейнера
UCLProperty& operator = (size_t size)
{
 (*static_cast<UCProperty<TC,OwnerT>* >(this)) = size;
 return *this;
};
// -----------------------------
};
/* ************************************************************************* */


/* ************************************************************************* */
/* ************************************************************************* */
/* Свойства для управления переменными состояния                              */
/* ************************************************************************* */
/* ************************************************************************* */
	 /*
// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T, typename OwnerT>
class UVLStateProperty: public UVProperty<T,OwnerT>
{
friend class UADataComponent;
protected: // Типы методов ввода-вывода

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVLStateProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod ,
        typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };
UVLStateProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod ,
        typename UVProperty<T,OwnerT>::GetterT getmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, getmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };
// -----------------------------

// -----------------------------
// Методы управления
// -----------------------------
// Оператор присваивания
UVLStateProperty& operator = (const T &value)
{
 (*static_cast<UVProperty<T,OwnerT>* >(this)) = value;
 return *this;
};

UVLStateProperty& operator = (const UVLStateProperty &v)
{
 (*static_cast<UVProperty<T,OwnerT>* >(this)) = v.Get();
 return *this;
};
// -----------------------------
};
				*/
/* ************************************************************************* */
// Класс - свойство с значением внутри
/* ************************************************************************* */
/*
template<typename T, typename OwnerT>
class ULStateProperty: public UProperty<T,OwnerT>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
ULStateProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };

ULStateProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Оператор присваивания
ULStateProperty& operator = (const T &value)
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
/*
template<typename TC, typename OwnerT>
class UCLStateProperty: public UCProperty<TC,OwnerT>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
UCLStateProperty(const string &name, OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterT setmethod=0)
 : UCProperty<TC,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };

UCLStateProperty(const string &name, OwnerT * const owner, typename UVProperty<TC,OwnerT>::SetterRT setmethod)
 : UCProperty<TC,OwnerT>(owner, setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };

//Конструктор инициализации для отдельных значений
UCLStateProperty(const string &name, OwnerT * const owner, typename UCProperty<TC,OwnerT>::VSetterT setmethod)
 : UCProperty<TC,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };

UCLStateProperty(const string &name, OwnerT * const owner, typename UCProperty<TC,OwnerT>::VSetterRT setmethod)
 : UCProperty<TC,OwnerT>(owner,setmethod)
{ reinterpret_cast<UADataComponent* const>(owner)->AddLookupState(name,this,false); };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Оператор присваивания
UCLStateProperty& operator = (const TC &value)
{
 (*static_cast<UCProperty<TC,OwnerT>* >(this)) = value;
 return *this;
};

UCLStateProperty& operator = (const UCLStateProperty &value)
{
 (*static_cast<UCProperty<TC,OwnerT>* >(this)) = value;
 return *this;
};

// Оператор задания размера контейнера
UCLStateProperty& operator = (size_t size)
{
 (*static_cast<UCProperty<TC,OwnerT>* >(this)) = size;
 return *this;
};
// -----------------------------
};
/* ************************************************************************* */


}
#endif

