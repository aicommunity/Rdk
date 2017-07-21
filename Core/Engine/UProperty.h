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

// Класс - база для свойств
template<typename T,class OwnerT, unsigned int type>
class UPropertyBase: virtual public UIPropertyInputBase, virtual public UIPropertyOutputBase
{
protected: // Данные
// Имя свойства
std::string Name;

// Владелец свойства
OwnerT* Owner;

/// Тип свойства
unsigned int Type;

// Прямой доступ к данным
//mutable T* PData;

protected: // Данные синхронизации
/// Мьютекс этого свойства
UGenericMutex *Mutex;

/// Время обновления свойства (мс)
mutable ULongTime UpdateTime;

// Указатель на итератор-хранилище данных об этом свойстве в родительском компоненте
UComponent::VariableMapCIteratorT Variable;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyBase(const std::string &name, OwnerT * const owner)
 : Name(name), Owner(owner), Type(type), Mutex(UCreateMutex()), UpdateTime(0)
{
   if(Owner)
   {
	Variable=Owner->FindPropertyVariable(this);
	reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(name,type,this,false);
   }
}

UPropertyBase(const std::string &name, OwnerT * const owner, T * const pdata, int index=0)
 : Name(name), Owner(owner), Type(type), Mutex(UCreateMutex()), UpdateTime(0)
{
 if(Owner)
 {
  Variable=Owner->FindPropertyVariable(this);
  reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(name,type,this,false);
 }
 SetData(*pdata,0); // TODO: виртуальный метод еще не готов
}

virtual ~UPropertyBase(void)
{
 UDestroyMutex(Mutex);
 Mutex=0;
}
// -----------------------------

// -----------------------------
// Методы сериализации
// -----------------------------
// Возвращает ссылку на данные
virtual const T& GetData(int index=0) const=0;
/*{
 if(PData)
  return *PData;
 throw EPropertyZeroPtr(GetOwnerName(),GetName());
};  */

// Модифицирует данные
virtual void SetData(const T& data, int index=0)=0;
/*{
 (PData)?*PData=data:throw EPropertyZeroPtr(GetOwnerName(),GetName());
 RenewUpdateTime();
}; */


// Метод возвращает тип свойства
virtual unsigned int GetType(void) const
{
 return Type;
}

// Возвращает языковой тип хранимого свойства
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}

// Метод сравнивает тип этого свойства с другим свойством
virtual bool CompareLanguageType(const UIProperty &dt) const
{
 return GetLanguageType() == dt.GetLanguageType();
}

// Метод записывает значение свойства в поток
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

// Метод читает значение свойства из потока
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

// Метод возвращает указатель на область памяти, содержащую данные свойства
virtual const void* GetMemoryArea(int index=0)
{
 return &GetData(index);
}

// Метод копирует значение данных свойства из области памяти
// штатными средствами копирования реального типа данных
// входной указатель приводится к указателю на необходимый тип данных
bool ReadFromMemory(const void *buffer, int index=0)
{
 if(!buffer)
  return false;

 const T* temp=(const T*)buffer;
 SetData(*temp,index);
 return true;
}
// -----------------------------

// --------------------------
// Методы управления данными
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
// Методы сериализации
// -----------------------------
// Метод устанавливает значение указателя на итератор-хранилище данных об этом
// свойстве в родительском компоненте
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const
{
 return Variable->first;
};

// Метод возвращает строковое имя компонента-владельца свойства
virtual std::string GetOwnerName(void) const
{
 return (Owner)?Owner->GetName():std::string("");
};

// Метод возвращает строковое имя класса-владельца свойства
virtual std::string GetOwnerClassName(void) const
{
 return typeid(Owner).name();
};

/// Возвращает указатель на владелька свойства
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
// Скрытые методы управления данными
// --------------------------
/// Обновляет время изменения данных свойства
void RenewUpdateTime(void)
{
 UpdateTime=GetCurrentStartupTime();
}

/// Сбрасывает время обновления до нуля
void ResetUpdateTime(void)
{
 UpdateTime=0;
}
// --------------------------
};

// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T,class OwnerT, unsigned int type>
class UPropertyVirtual: public UPropertyBase<T,OwnerT,type>
{
protected: // Типы методов ввода-вывода
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected:
/// Флаг проверки значения свойства на равенство присваевому значению
bool CheckEqualsFlag;

protected: // Данные
// Методы ввода-вывода
GetterRT GetterR;
SetterRT SetterR;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyVirtual(const std::string &name, OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UPropertyBase<T,OwnerT,type>(name, owner), CheckEqualsFlag(true), GetterR(getmethod), SetterR(setmethod)
{
}

UPropertyVirtual(const std::string &name, OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UPropertyBase<T,OwnerT,type>(name,owner,pdata), CheckEqualsFlag(true), GetterR(0), SetterR(setmethod)
{
}
// -----------------------------

// -----------------------------
// Метод управления параметрами
// -----------------------------
/// Флаг проверки значения свойства на равенство присваевому значению
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
// Методы управления
// -----------------------------
// Возврат значения
virtual const T& GetData(int index=0) const
{
 if(this->Owner && GetterR)
 {
  if(GetterR)
   return (this->Owner->*GetterR)();
 }

 throw UIProperty::EPropertyZeroPtr(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());
};

// Установка значения
virtual void SetData(const T &value, int index=0)
{
 if(!GetterR)
  throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

 if(CheckEqualsFlag && (this->Owner->*GetterR)() == value)
  return;

 if(!SetterR)
 {
  throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());
 }

 if(this->Owner && SetterR)
 {
  if(!(this->Owner->*SetterR)(value))
   throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

  this->RenewUpdateTime();
 }
};
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

const T& operator [] (int index) const
{ return this->GetData(index); };

// Оператор присваивания
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

								   /*
/// Класс - виртуальное свойство
/// Не содержит данного внутри себя
/// Setter и Getter обеспечивают доступ по значению
/// Вовзрат значения осуществляется через внутреннюю копию
template<typename T,class OwnerT>
class UVSProperty: public UVBaseProperty<T,OwnerT>
{
protected: // Типы методов ввода-вывода
typedef T (OwnerT::*GetterT)(void) const;
typedef bool (OwnerT::*SetterT)(T);

protected: // Данные
// Методы ввода-вывода
GetterT Getter;
SetterT Setter;

private:
mutable T Temp;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
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
// Методы управления
// -----------------------------
// Возврат значения
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

// Установка значения
virtual void SetData(const T &value)
{
 if(this->PData && !Setter)
 {
  *this->PData=value;
  this->RenewUpdateTime();
 }

 if(this->Owner && Setter)
 {
  if(!(this->Owner->*Setter)(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

  if(this->PData)
  {
   *this->PData=value;
   this->RenewUpdateTime();
  }
 }
};
// -----------------------------
};                       */

/* ************************************************************************* */
// Класс - свойство со значением внутри
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type>
class UProperty: public UPropertyVirtual<T,OwnerT,type>
{
public:
//protected:
// Данные
mutable T v;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProperty(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyVirtual<T,OwnerT,type>(name,owner, setmethod, 0), v() { };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Возврат значения
virtual const T& GetData(int index=0) const
{
 return v;
};

virtual void SetData(const T &value, int index=0)
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
// -----------------------------
};
/* ************************************************************************* */

/* ************************************************************************* */
// Класс - свойство-контейнер со значением внутри
/* ************************************************************************* */
template<typename T, typename RangeT, typename OwnerT, unsigned int type>
class UPropertyRange: public UPropertyVirtual<T,OwnerT,type>
{
protected: // Типы методов ввода-вывода
typedef bool (OwnerT::*VSetterRT)(const RangeT&);

protected: // Данные
// Методы ввода-вывода
VSetterRT VSetterR;

RangeT v;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod)
 : UPropertyVirtual<T,OwnerT,type>(name, owner, setmethod, 0), VSetterR(0) { };

UPropertyRange(const std::string &name, OwnerT * const owner, VSetterRT setmethod)
 : UProperty<T,OwnerT,type>(name, owner,(typename UPropertyVirtual<T,OwnerT,type>::SetterRT)0,0)
{ VSetterR=setmethod; };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Возврат значения
virtual const T& GetData(int index=0) const
{
 return v[index];
};

virtual T& GetData(int index=0)
{
 return v[index];
};

T& operator [] (int index)
{ return this->GetData(index); };

virtual void SetData(const T &value, int index=0)
{
 if(CheckEqualsFlag && v[index] == value)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT,type>::GetOwnerName(),UPropertyVirtual<T,OwnerT,type>::GetName());
 }

 v[index]=value;
 this->RenewUpdateTime();
};
// -----------------------------

};
#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */



}

#endif

