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

/// Флаг, указывающий, что это динамическое свойтво
bool DynamicPropertyFlag;

mutable int CurrentInputIndex;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyBase(const std::string &name, OwnerT * const owner, bool dynamic_prop_flag=false)
 : Name(name), Owner(owner), Type(type), Mutex(UCreateMutex()), UpdateTime(0), DynamicPropertyFlag(dynamic_prop_flag), CurrentInputIndex(0)
{
   if(Owner)
   {
	reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(this);
//	Variable=Owner->FindPropertyVariable(this);
   }
}

virtual ~UPropertyBase(void)
{
   if(Owner)
   {
	reinterpret_cast<UComponent* const>(Owner)->DelLookupProperty(Name,true);
//	Variable=Owner->FindPropertyVariable(this);
   }
 UDestroyMutex(Mutex);
 Mutex=0;
}
// -----------------------------

// -----------------------------
// Методы сериализации
// -----------------------------
protected:
// Возвращает ссылку на данные
virtual const T& GetData(void) const
{
 if(IsConnected())
 {
  return dynamic_pointer_cast<UPropertyBase<T,OwnerT, type> >(GetConnectedProperty(CurrentInputIndex))->GetData();
 }
 else
 {
  return GetDataLocal();
 }
}

// Модифицирует данные
virtual void SetData(const T& data)
{
 if(IsConnected())
 {
  dynamic_pointer_cast<UPropertyBase<T,OwnerT, type> >(GetConnectedProperty(CurrentInputIndex))->SetData(data);
 }
 else
 {
  SetDataLocal(data);
 }
}

// Возвращает ссылку на данные
virtual const T& GetDataLocal(void) const=0;

// Модифицирует данные
virtual void SetDataLocal(const T& data)=0;

public:
// Метод возвращает тип свойства
virtual unsigned int GetType(void) const
{
 return Type;
}

// Метод изменяет тип свойства
bool ChangeType(unsigned int value)
{
 bool res(true);
 // Если свойство перестает быть входом то отключаем всех кто к нему подключен
 if((Type & ptInput) && !(value & ptInput))
  res&=DisconnectAllOutputs();

 // Если свойство перестает быть выходом то отключаем всех к кому оно подключено
 if((Type & ptOutput) && !(value & ptOutput))
  res&=DisconnectAllInputs();

 if(res)
  Type=value;
 else
  return false;

 return true;
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
virtual const void* GetMemoryArea(void) const
{
 return &GetData();
}

// Метод копирует значение данных свойства из области памяти
// штатными средствами копирования реального типа данных
// входной указатель приводится к указателю на необходимый тип данных
bool ReadFromMemory(const void *buffer)
{
 if(!buffer)
  return false;

 const T* temp=(const T*)buffer;
 SetData(*temp);
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

/// Устанавливает флаг, указывающий, что это динамическое свойство, и должно быть удалено при разрушении компонента-владельца
virtual void SetDynamicPropertyFlag(void)
{
 DynamicPropertyFlag=true;
}

/// Возвращает состояние флага, указывающего, что это динамическое свойство
virtual bool IsDynamicPropertyFlag(void) const
{
 return DynamicPropertyFlag;
}

// Возвращает только маску типа свойства
unsigned int GetPropertyType(void) const
{
 return Type & 0x000000FF;
}

// Возвращает только маску группы свойства
unsigned int GetPropertyGroup(void) const
{
 return Type & 0xFFFFFF00;
}

// Возвращает строковое имя типа свойства
std::string GetPropertyTypeName(void) const
{
 return UComponent::GetPropertyTypeNameByType(GetPropertyType());
}

// Проверяет соответствие типа и группы свойства маске
bool CheckMask(unsigned int mask) const
{
 return (GetPropertyType() & mask) && (GetPropertyGroup() & mask);
}


// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const
{
 return Name;
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

T* RawDataPtr;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyVirtual(const std::string &name, OwnerT * const owner, SetterRT setmethod , GetterRT getmethod, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(getmethod), SetterR(setmethod), RawDataPtr(0), UPropertyBase<T,OwnerT,type>(name, owner, dynamic_prop_flag)
{
}

UPropertyVirtual(const std::string &name, OwnerT * const owner, T * const pdata, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(0), SetterR(0), RawDataPtr(pdata), UPropertyBase<T,OwnerT,type>(name,owner,dynamic_prop_flag)
{
}

virtual ~UPropertyVirtual(void)
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
protected:
// Возврат значения
virtual const T& GetDataLocal(void) const
{
 if(this->Owner && GetterR)
 {
  if(GetterR)
   return (this->Owner->*GetterR)();
 }
 else
 if(RawDataPtr)
  return *RawDataPtr;

 throw UIProperty::EPropertyZeroPtr(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());
};

// Установка значения
virtual void SetDataLocal(const T &value)
{
 if(RawDataPtr)
 {
  if(CheckEqualsFlag && *RawDataPtr == value)
   return;

  *RawDataPtr=value;
  this->RenewUpdateTime();
 }
 else
 {
  if(!GetterR)
   throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

  if(CheckEqualsFlag && (this->Owner->*GetterR)() == value)
   return;

  if(!SetterR)
   throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

  if(this->Owner && SetterR)
  {
   if(!(this->Owner->*SetterR)(value))
	throw UIProperty::EPropertySetterFail(UPropertyBase<T,OwnerT,type>::GetOwnerName(),UPropertyBase<T,OwnerT,type>::GetName());

   this->RenewUpdateTime();
  }
 }
};
// -----------------------------

// -----------------------------
// Методы управления
// -----------------------------
public:
operator T (void) const
{
 return this->GetData();
};

const T& operator () (void) const
{
 return this->GetData();
};
				   /*
T* operator -> (void)
{ return const_cast<T*>(&this->GetData()); };
                     */
const T* operator -> (void) const
{ return &this->GetData(); };

/*T& operator * (void)
{ return const_cast<T&>(this->GetData()); };
  */
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

/* ************************************************************************* */
// Класс - свойство со значением внутри
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type>
class UProperty: public UPropertyVirtual<T,OwnerT,type>
{
protected:
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
protected:
// Возврат значения
virtual const T& GetDataLocal(void) const
{
 return v;
};
	 /*
virtual T& GetData(void)
{
 return v;
};            */


virtual void SetDataLocal(const T &value)
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

public:
const T& operator [] (int index) const
{ return GetData()[index]; };
				 /*
T& operator [] (int index)
{ return GetData()[index]; };   */
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
 : UPropertyVirtual<T,OwnerT,type>(name, owner, setmethod, 0), VSetterR(0)
{
 SetNumConnectionsLimit(-1);
};

UPropertyRange(const std::string &name, OwnerT * const owner, VSetterRT setmethod)
 : UProperty<T,OwnerT,type>(name, owner,(typename UPropertyVirtual<T,OwnerT,type>::SetterRT)0,0)
{
 VSetterR=setmethod;
 SetNumConnectionsLimit(-1);
};
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
protected:
// Возврат значения
virtual const T& GetDataLocal(void) const
{
 return v[CurrentInputIndex];
};
/*
virtual T& GetData(void)
{
 return v[index];
};        */

virtual void SetDataLocal(const T &value)
{
 if(CheckEqualsFlag && v[CurrentInputIndex] == value)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT,type>::GetOwnerName(),UPropertyVirtual<T,OwnerT,type>::GetName());
 }

 v[CurrentInputIndex]=value;
 this->RenewUpdateTime();
};

public:
const T& operator [] (int index) const
{
 CurrentInputIndex=index;
 return GetData();
};
/*
T& operator [] (int index)
{ return v[index]; };
								  */
public:
/// Финальные действия по связыванию входа со свойством output_property
virtual bool FinalizeConnectToOutput(UIPropertyOutput *output_property)
{
 if(!UIPropertyInputBase::FinalizeConnectToOutput(output_property))
  return false;
 v.resize(GetNumConnections());
 return true;
}

/// Финальные действия по уничтожению связи со свойством output_property
virtual bool FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(!UIPropertyInputBase::FinalizeDisconnectFromOutput(output_property,c_index))
  return false;
 v.erase(v.begin()+c_index);
 return true;
}
// -----------------------------

};
#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */



}

#endif

