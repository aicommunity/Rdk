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
friend class UComponent;
protected: // Типы методов ввода-вывода

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVBaseLProperty(const string &name, OwnerT * const owner)
 : UVBaseProperty<T,OwnerT>(owner)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

UVBaseLProperty(const string &name, OwnerT * const owner, T * const pdata)
 : UVBaseProperty<T,OwnerT>(owner, pdata)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------
};



// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UVLProperty: public UVProperty<T,OwnerT>
{
friend class UComponent;
protected: // Типы методов ввода-вывода

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
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
// Методы управления
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

// Оператор присваивания
UVLProperty& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UVLProperty& operator = (const UVLProperty &v)
{
 this->SetData(v.GetData());
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
/*ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
  */
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

// -----------------------------
// Операторы доступа
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

// Оператор присваивания
ULProperty& operator = (const T &value)
{
 this->SetData(value);
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
/*UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UCProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
  */
UCLProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UCProperty<T,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };

//Конструктор инициализации для отдельных значений
/*UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterT setmethod)
 : UCProperty<T,OwnerT>(owner,setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
  */
UCLProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterRT setmethod)
 : UCProperty<T,OwnerT>(owner,setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); };
// -----------------------------

public: // Исключения
// Выход за границы массива C (container) property
struct EPropertyRangeError: public EPropertyError
{
 int MinValue, MaxValue, ErrorValue;
public:
EPropertyRangeError(const std::string &owner_name, const std::string &property_name,
	 int min_value, int max_value, int error_value)
: EPropertyError(owner_name, property_name),
  MinValue(min_value), MaxValue(max_value), ErrorValue(error_value) {};


// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return EPropertyError::CreateLogMessage()+std::string(" MinValue=")+
  sntoa(MinValue)+std::string(" MaxValue=")+sntoa(MaxValue)+
  std::string(" ErrorValue=")+sntoa(ErrorValue);
}
};


// -----------------------------
// Операторы доступа
// -----------------------------
// Чтение элемента контейнера
const typename UCProperty<T,OwnerT>::TV& operator () (int i) const
{
 if(i<0 || i>=this->v.size())
  throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
	0,int(this->v.size()),i);

 return this->v[i];
};

// Запись элемента контейнера
bool operator () (int i, const typename UCProperty<T,OwnerT>::TV &value)
{
 if(UVProperty<T,OwnerT>::VSetterR && !(this->Owner->*(UVProperty<T,OwnerT>::VSetterR)(value)))
  throw EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

 if(i<0 || i>=this->v.size())
  throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
	0,int(this->v.size()),i);

 this->v[i]=value;

 return true;
};

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

// Оператор присваивания
UCLProperty& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UCLProperty& operator = (const UCLProperty &value)
{
 this->SetData(value.GetData());
 return *this;
};
// -----------------------------

// -----------------------------
// Скрытые операторы доступа только для дружественного класса
// -----------------------------
public:
typename UCProperty<T,OwnerT>::TV& operator [] (int i)
{ return this->v[i]; };

const typename UCProperty<T,OwnerT>::TV& operator [] (int i) const
{ return this->v[i]; };
// -----------------------------
};
/* ************************************************************************* */

}
#endif

