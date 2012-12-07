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

#ifdef __BORLANDC__
#pragma warning( disable : 4700)
#endif

// Класс - база для свойств
template<typename T,class OwnerT>
class UVBaseProperty: public UIProperty
{
//friend class OwnerT;
protected: // Типы методов ввода-вывода

protected: // Данные
// Владелец свойства
OwnerT* Owner;

// Указатель на итератор-хранилище данных об этом свойстве в родительском компоненте
UADataComponent::VariableMapCIteratorT Variable;

// Прямой доступ к данным
T* PData;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVBaseProperty(OwnerT * const owner) :
  Owner(owner), PData(0)
{
   if(Owner)
	Variable=Owner->FindPropertyVariable(this);
}

UVBaseProperty(OwnerT * const owner, T * const pdata) :
  Owner(owner), PData(pdata)
{
   if(Owner)
	Variable=Owner->FindPropertyVariable(this);
}
// -----------------------------

// -----------------------------
// Методы управления
// -----------------------------
// Инициализация
/*void Init(OwnerT * const owner)
{
 Owner = owner; PData=0;
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}

void Init(OwnerT * const owner, T * const pdata)
{
 Owner = owner; PData=pdata;
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}
  */
// -----------------------------

// -----------------------------
// Методы сериализации
// -----------------------------
// Метод устанавливает значение указателя на итератор-хранилище данных об этом
// свойстве в родительском компоненте
virtual void SetVariable(UADataComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const
{
 return Variable->first;
};

// Метод возвращает тип свойства
virtual unsigned int GetType(void) const
{
 return Variable->second.Type;
};

// Метод возвращает строковое имя класса-владельца свойства
virtual std::string GetOwnerName(void) const
{
 return typeid(Owner).name();
};
// -----------------------------
};



// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T,class OwnerT>
class UVProperty: public UVBaseProperty<T,OwnerT>
{
//friend class OwnerT;
protected: // Типы методов ввода-вывода
typedef T (OwnerT::*GetterT)(void) const;
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterT)(T);
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // Данные
// Методы ввода-вывода
GetterT Getter;
SetterT Setter;
GetterRT GetterR;
SetterRT SetterR;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVProperty(OwnerT * const owner, SetterT setmethod , GetterT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), Getter(getmethod), Setter(setmethod), GetterR(0), SetterR(0)
{
}

UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), Getter(0), Setter(0), GetterR(getmethod), SetterR(setmethod)
{
}

UVProperty(OwnerT * const owner, T * const pdata) :
  UVBaseProperty<T,OwnerT>(owner,pdata), Getter(0), Setter(0), GetterR(0), SetterR(0)
{
}
// -----------------------------

// -----------------------------
// Методы управления
// -----------------------------
/*// Инициализация
void Init(OwnerT * const owner, SetterT setmethod, GetterT getmethod)
{
 UVBaseProperty::Init(owner);
 Getter = getmethod; Setter = setmethod; SetterR=0;
}
void Init(OwnerT * const owner, SetterRT setmethod, GetterT getmethod)
{
 UVBaseProperty::Init(owner);
 Getter = getmethod; SetterR = setmethod; Setter=0;
}
void Init(OwnerT * const owner, T * const pdata)
{
 UVBaseProperty::Init(owner,pdata);
 Getter = 0; GetterR=0; SetterR = 0; Setter=0;
}        */

// Возврат значения
virtual T Get(void)
{
 return T(*this);
};

// Установка значения
virtual bool Set(const T &value)
{
 *this=value;
 return true;
};

operator T (void) const
{
 if(this->Owner)
 {
  if(this->PData)
   return *this->PData;

  if(Getter)
   return (this->Owner->*Getter)();

  if(GetterR)
   return (this->Owner->*GetterR)();
 }

 //T val;
 return T();
};

// Оператор присваивания
UVProperty& operator = (const T &value)
{
 if(this->PData)
  *this->PData=value;

 if(this->Owner && Setter)
  (this->Owner->*Setter)(value);
 else
 if(this->Owner && SetterR)
  (this->Owner->*SetterR)(value);

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
// Методы сериализации
// -----------------------------
// Метод устанавливает значение указателя на итератор-хранилище данных об этом
// свойстве в родительском компоненте
virtual void SetVariable(UADataComponent::VariableMapCIteratorT &var)
{
 this->Variable=var;
}

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const
{
 return this->Variable->first;
};

// Метод возвращает тип свойства
virtual unsigned int GetType(void) const
{
 return this->Variable->second.Type;
};

// Метод возвращает строковое имя класса-владельца свойства
virtual std::string GetOwnerName(void) const
{
 return typeid(this->Owner).name();
};

// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
 UEPtr<Serialize::USerStorageBinary> binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<Get();
  return true;
 }

 UEPtr<Serialize::USerStorageXML> xml=dynamic_pointer_cast<Serialize::USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->Create(GetName());
   Serialize::operator << (*xml,Get());
   xml->SelectUp();
   return true;
  }
  else
  {
   xml->AddNode(GetName());
   Serialize::operator << (*xml,Get());
   xml->SelectUp();
   return true;
  }
 }

 return false;
};

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<Serialize::USerStorage>  storage, bool simplemode=false)
{
 T temp;

 UEPtr<Serialize::USerStorageBinary> binary=dynamic_pointer_cast<Serialize::USerStorageBinary>(storage);
 if(binary)
 {
  Serialize::operator >> (*binary,temp);
  Set(temp);
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
   Set(temp);
   xml->SelectUp();
   return true;
  }
  else
  {
   if(!xml->SelectNode(GetName()))
	return false;
   Serialize::operator >> (*xml,temp);
   Set(temp);
   xml->SelectUp();
   return true;
  }
 }

 return false;
};
// -----------------------------
};

/* ************************************************************************* */
// Класс - свойство с значением внутри
/* ************************************************************************* */
template<typename T,class OwnerT>
class UProperty: public UVProperty<T,OwnerT>
{
public:
//protected:
// Данные
T v;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
UProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), v() { this->PData=&v; };

UProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), v() { this->PData=&v; };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Возврат значения
virtual T Get(void)
{
 return v;
};

virtual bool Set(const T &value)
{
 (*this)=value;
 return true;
};

const T& operator () (void) const
{ return v; };

operator T (void) const
{ return v; };

// Оператор присваивания
UProperty& operator = (const T &value)
{
 if(v == value)
  return *this;

 if(this->Owner)
 {
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
};
/* ************************************************************************* */

/* ************************************************************************* */
// Класс - свойство-контейнер со значением внутри
/* ************************************************************************* */
template<typename T, typename OwnerT>
class UCProperty: public UVProperty<T,OwnerT>
{
//friend class OwnerT;
protected: // Типы методов ввода-вывода
typedef typename T::value_type TV;

//typedef T (OwnerT::*VGetterT)(void) const;
typedef bool (OwnerT::*VSetterT)(TV);
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // Данные
// Методы ввода-вывода
VSetterT VSetter;
VSetterRT VSetterR;

public:
// Данные
T v;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
UCProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), VSetter(0), VSetterR(0), v() { this->PData=&v; };

UCProperty(OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), VSetter(0), VSetterR(0), v() { this->PData=&v; };

//Конструктор инициализации для отдельных значений
UCProperty(OwnerT * const owner, VSetterT setmethod)
 : UVProperty<T,OwnerT>(owner,(typename UVProperty<T,OwnerT>::SetterT)0,0), v()
{ VSetter=setmethod; this->PData=&v; };

UCProperty(OwnerT * const owner, VSetterRT setmethod)
 : UVProperty<T,OwnerT>(owner,(typename UVProperty<T,OwnerT>::SetterT)0,0), v()
{ VSetterR=setmethod; this->PData=&v; };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
const T& operator () (void) const
{ return v; };

// Чтение элемента контейнера
const TV& operator () (int i) const
{ return v[i]; };

// Запись элемента контейнера
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

 // Оператор присваивания
UCProperty& operator = (const T &value)
{
 if(v == value)
  return *this;

 if(this->Owner)
 {
  if(VSetter)
  {
   typename T::const_iterator I,J;
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
   typename T::const_iterator I,J;
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

// Оператор задания размера контейнера
UCProperty& operator = (size_t size)
{
 v.resize(size);
 return *this;
};

T* operator -> (void)
{ return &v; };

T& operator * (void)
{ return v; };
// -----------------------------

// -----------------------------
// Скрытые операторы доступа только для дружественного класса
// -----------------------------
public:
TV& operator [] (int i)
{ return v[i]; };
// -----------------------------
};
#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */



}

#endif

