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

/// Класс - база для свойств с сериализацией
template<typename T>
class UPropertyBase: virtual public UIPropertyInputBase, virtual public UIPropertyOutputBase
{
public:
/// Тип метода, обеспечивающего прямой доступ к данным свойства
typedef const T& (UPropertyBase<T>::*PGetDataValue)(void) const;
typedef bool (UPropertyBase<T>::*PSetDataValue)(const T &value);
protected: // Данные
// Имя свойства
std::string Name;

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

/// Указатель на метод, обеспечивающий прямой доступ к данным свойства
PGetDataValue GetDataValuePtr;
PSetDataValue SetDataValuePtr;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyBase(const std::string &name, bool dynamic_prop_flag=false)
 : GetDataValuePtr(0), SetDataValuePtr(0), Name(name), Mutex(UCreateMutex()), UpdateTime(0), DynamicPropertyFlag(dynamic_prop_flag), CurrentInputIndex(0)
{
}

virtual ~UPropertyBase(void)
{
 UDestroyMutex(Mutex);
 Mutex=0;
}
// -----------------------------

protected:
virtual bool AFinalizeConnectToOutput(UIPropertyOutput *output_property)
{
 GetDataValuePtr=static_cast<PGetDataValue>(&UPropertyBase<T>::GetDataValueInput);
 SetDataValuePtr=static_cast<PSetDataValue>(&UPropertyBase<T>::SetDataValueInput);
 return true;
}

virtual bool AFinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(!IsConnected())
 {
  GetDataValuePtr=static_cast<PGetDataValue>(&UPropertyBase<T>::GetDataValueLocal);
  SetDataValuePtr=static_cast<PSetDataValue>(&UPropertyBase<T>::SetDataValueLocal);
 }
 return true;
}

// Возвращает ссылку на данные
virtual const T& GetData(void) const
{
 return (this->*GetDataValuePtr)();
}

// Модифицирует данные
virtual bool SetData(const T& data)
{
 return (this->*SetDataValuePtr)(data);
}

// Возвращает ссылку на данные
virtual const T& GetDataValueLocal(void) const=0;

// Модифицирует данные
virtual bool SetDataValueLocal(const T& data)=0;

// Возвращает ссылку на данные
virtual const T& GetDataValueInput(void) const
{
 return dynamic_pointer_cast<UPropertyBase<T> >(GetConnectedProperty(CurrentInputIndex))->GetData();
}

// Модифицирует данные
virtual bool SetDataValueInput(const T& data)
{
 return dynamic_pointer_cast<UPropertyBase<T> >(GetConnectedProperty(CurrentInputIndex))->SetData(data);
}

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

public:
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

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const
{
 return Name;
};
};

// Класс - базовое свойство с владельцем
template<typename T,class OwnerT>
class UPropertyBaseOwner: public UPropertyBase<T>
{
protected:
// Владелец свойства
OwnerT* Owner;

/// Тип свойства
unsigned int Type;

protected: // Данные

//T* RawDataPtr;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyBaseOwner(const std::string &name, OwnerT * const owner, unsigned int type, bool dynamic_prop_flag=false) :
  Owner(owner), Type(type), /*RawDataPtr(0),*/ UPropertyBase<T>(name, dynamic_prop_flag)
{
	if(Owner)
	{
	 reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(this);
	}
}
			/*
UPropertyBaseOwner(const std::string &name, OwnerT * const owner, unsigned int type, T * const pdata, bool dynamic_prop_flag=false) :
  Owner(owner), Type(type), RawDataPtr(pdata), UPropertyBase<T>(name,dynamic_prop_flag)
{
    if(Owner)
    {
	 reinterpret_cast<UComponent* const>(Owner)->AddLookupProperty(this);
    }
}                   */

virtual ~UPropertyBaseOwner(void)
{
    if(Owner)
    {
	 reinterpret_cast<UComponent* const>(Owner)->DelLookupProperty(this->Name,true);
	}
}
// -----------------------------

// -----------------------------
// Метод управления параметрами
// -----------------------------
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
virtual const UComponent* GetOwner(void) const
{
 return dynamic_cast<const UComponent*>(Owner);
}

virtual UComponent* GetOwner(void)
{
 return dynamic_cast<UComponent*>(Owner);
}

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
  res&=this->DisconnectAllOutputs();

 // Если свойство перестает быть выходом то отключаем всех к кому оно подключено
 if((Type & ptOutput) && !(value & ptOutput))
  res&=this->DisconnectAllInputs();

 if(res)
  Type=value;
 else
  return false;

 return true;
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
// -----------------------------
};

// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T,class OwnerT>
class UPropertyVirtual: public UPropertyBaseOwner<T,OwnerT>
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
UPropertyVirtual(const std::string &name, OwnerT * const owner, unsigned int type, SetterRT setmethod , GetterRT getmethod, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(getmethod), SetterR(setmethod), UPropertyBaseOwner<T,OwnerT>(name, owner, dynamic_prop_flag)
{
}
						   /*
UPropertyVirtual(const std::string &name, OwnerT * const owner, unsigned int type, T * const pdata, bool dynamic_prop_flag=false) :
  CheckEqualsFlag(true), GetterR(0), SetterR(0), UPropertyBaseOwner<T,OwnerT>(name,owner,dynamic_prop_flag)
{
}
                                      */
virtual ~UPropertyVirtual(void)
{
}
// -----------------------------

// -----------------------------
// Метод управления параметрами
// -----------------------------
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
virtual const T& GetDataValueLocal(void) const
{
 return (this->Owner->*GetterR)();
 /*
 if(this->Owner && GetterR)
 {
  if(GetterR)
   return (this->Owner->*GetterR)();
 }

 throw UIProperty::EPropertyZeroPtr(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());
 */
};

// Установка значения
virtual bool SetDataValueLocal(const T &value)
{
 return (this->Owner->*SetterR)(value);
					   /*
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
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

  if(CheckEqualsFlag && (this->Owner->*GetterR)() == value)
   return;

  if(!SetterR)
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

  if(this->Owner && SetterR)
  {
   if(!(this->Owner->*SetterR)(value))
	throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

   this->RenewUpdateTime();
  }
 }             */
};

// Модифицирует данные
virtual bool SetData(const T& data)
{
 if(CheckEqualsFlag && GetData() == data)
  return true;

 if(!(this->*SetDataValuePtr)(data))
  throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());

 this->RenewUpdateTime();
 return true;
}
// -----------------------------

// -----------------------------
// Методы управления
// -----------------------------
public:
const T& v(void) const
{
 return this->GetData();
};

void v(const T &value)
{
 SetData(value);
};
/*
const T& operator () (void) const
{
 return this->GetData();
};

T& operator () (void)
{
 return const_cast<T&>(this->GetData());
};*/
/// deprecated
const T* operator -> (void) const
{ return &this->GetData(); };

/// deprecated
const T& operator * (void) const
{ return this->GetData(); };
  /*
const T& operator [] (int index) const
{ return this->GetData(index); };
	*/

// Оператор присваивания
UPropertyVirtual<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UPropertyVirtual<T,OwnerT>& operator = (const UPropertyVirtual<T,OwnerT> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};


template<typename T,class OwnerT>
bool operator != (const UPropertyVirtual<T,OwnerT> &prop, const T &v)
{ return prop.v() != v; };

template<typename T,class OwnerT>
bool operator != (const T &v, const UPropertyVirtual<T,OwnerT> &prop)
{ return prop.v() != v; };

template<typename T,class OwnerT>
bool operator == (const UPropertyVirtual<T,OwnerT> &prop, const T &v)
{ return prop.v() == v; };

template<typename T,class OwnerT>
bool operator == (const T &v, const UPropertyVirtual<T,OwnerT> &prop)
{ return prop.v() == v; };

template<typename T,class OwnerT>
bool operator < (const UPropertyVirtual<T,OwnerT> &prop, const T &v)
{ return prop.v() < v; };

template<typename T,class OwnerT>
bool operator < (const T &v, const UPropertyVirtual<T,OwnerT> &prop)
{ return prop.v() < v; };

/* ************************************************************************* */
// Класс - свойство со значением внутри
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type>
class UPropertyLocal: public UPropertyVirtual<T,OwnerT>
{
protected:
//protected:
// Данные
mutable T Value;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyLocal(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyVirtual<T,OwnerT>(name,owner, type, setmethod, 0, dynamic_prop_flag), Value() { };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
protected:
// Возврат значения
virtual const T& GetDataValueLocal(void) const
{
 return Value;
};
	 /*
virtual T& GetData(void)
{
 return v;
};            */


virtual bool SetDataValueLocal(const T &value)
{
 Value=value;
 return true;
};

// Модифицирует данные
virtual bool SetData(const T& data)
{
 if(this->CheckEqualsFlag && Value == data)
  return true;

 if(this->SetterR && !(this->Owner->*SetterR)(data))
  throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());
 SetDataValueLocal(data);
 this->RenewUpdateTime();
 return true;
}

public:
// TODO: прямой доступ
T& v(void)
{
 return this->Value;
};

const T& operator [] (int index) const
{ return this->GetData()[index]; };

T& operator [] (int index)
{ return GetData()[index]; }; // TODO: Временно разрешено для переезда на новый обмен

// Оператор присваивания
UPropertyLocal<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UPropertyLocal<T,OwnerT,type>& operator = (const UPropertyLocal<T,OwnerT,type> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};

template<typename T,class OwnerT, unsigned int type>
class UProperty: public UPropertyLocal<T,OwnerT,type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UProperty(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<T,OwnerT,type>(name,owner, setmethod, dynamic_prop_flag) { };
// -----------------------------

// Оператор присваивания
UProperty<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

template<typename T, typename V, class OwnerT, unsigned int type>
UProperty<T,OwnerT,type>& operator = (const V &value)
{
 this->SetData(value);
 return *this;
};

UProperty<T,OwnerT,type>& operator = (const UProperty<T,OwnerT,type> &v)
{
 this->SetData(v.GetData());
 return *this;
};
// -----------------------------
};

// Класс - свойство-контейнер со значением внутри
template<typename T, typename RangeT, typename OwnerT, unsigned int type>
class UPropertyRangeLocal: public UPropertyVirtual<T,OwnerT>
{
protected: // Типы методов ввода-вывода
typedef bool (OwnerT::*VSetterRT)(const RangeT&);

protected: // Данные
// Методы ввода-вывода
VSetterRT VSetterR;

RangeT Value;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyRangeLocal(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyVirtual<T,OwnerT>(name, owner, type, setmethod, 0, dynamic_prop_flag), VSetterR(0)
{
 this->SetNumConnectionsLimit(-1);
};

UPropertyRangeLocal(const std::string &name, OwnerT * const owner, VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyVirtual<T,OwnerT>(name, owner, type, (typename UPropertyVirtual<T,OwnerT>::SetterRT)0,0, dynamic_prop_flag)
{
 VSetterR=setmethod;
 this->SetNumConnectionsLimit(-1);
};
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
protected:
// Возврат значения
virtual const T& GetDataLocal(void) const
{
 RangeT::const_iterator I=Value.begin();
 std::advance(I,this->CurrentInputIndex);
 return *I;
};

virtual void SetDataLocal(const T &value)
{
 RangeT::iterator I=Value.begin();
 std::advance(I,this->CurrentInputIndex);

 if(this->CheckEqualsFlag && *I == value)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UPropertyVirtual<T,OwnerT>::GetOwnerName(),UPropertyVirtual<T,OwnerT>::GetName());
 }

 *I=value;
 this->RenewUpdateTime();
};

public:
const T& operator [] (int index) const
{
 this->CurrentInputIndex=index;
 return this->GetData();
};

T& operator [] (int index) // TODO: Временно разрешено для переезда на новый обмен
{ return Value[index]; };

const RangeT& GetRangeData(void) const
{ return Value; };

UPropertyRangeLocal<T,RangeT, OwnerT,type>& operator = (const RangeT &value)
{
 typename RangeT::const_iterator I=value.begin();
 this->CurrentInputIndex=0;
 for(;I != value.end(); ++I)
 {
//  this->CurrentInputIndex
  this->SetData(*I);
  ++this->CurrentInputIndex;  // TODO: нет контроля выхода за границу
 }
 return *this;
};

UPropertyRangeLocal<T,RangeT, OwnerT,type>& operator = (const UPropertyRangeLocal<T,RangeT, OwnerT,type> &v)
{
 *this=v.Value;
 return *this;
};

public:
/// Финальные действия по связыванию входа со свойством output_property
virtual bool FinalizeConnectToOutput(UIPropertyOutput *output_property)
{
 if(!UIPropertyInputBase::FinalizeConnectToOutput(output_property))
  return false;
 Value.resize(this->GetNumConnections());
 return true;
}

/// Финальные действия по уничтожению связи со свойством output_property
virtual bool FinalizeDisconnectFromOutput(UIPropertyOutput *output_property, int c_index)
{
 if(!UIPropertyInputBase::FinalizeDisconnectFromOutput(output_property,c_index))
  return false;

 typename RangeT::iterator I=Value.begin();
 std::advance(I,c_index);
 Value.erase(I);
 return true;
}
// -----------------------------

};


// Класс - свойство-контейнер со значением внутри
template<typename T, typename RangeT, typename OwnerT, unsigned int type>
class UPropertyRange: public UPropertyRangeLocal<T,RangeT,OwnerT,type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<T,RangeT, OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};

UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyRangeLocal<T, RangeT, OwnerT, type>::VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<T,RangeT, OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
UPropertyRange<T,RangeT, OwnerT,type>& operator = (const RangeT &value)
{
 static_cast<UPropertyRangeLocal<T,RangeT, OwnerT,type>& >(*this)=value;
 return *this;
};

UPropertyRange<T,RangeT, OwnerT,type>& operator = (const UPropertyRange<T,RangeT, OwnerT,type> &v)
{
 static_cast<UPropertyRangeLocal<T,RangeT, OwnerT,type>& >(*this)=v.Value;
 return *this;
};
// -----------------------------
};

template<typename T,class OwnerT>
class UPropertyInput: public UPropertyBaseOwner<T,OwnerT>
{
protected:
// Данные
mutable T Value;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyInput(const std::string &name, OwnerT * const owner, bool dynamic_prop_flag=false)
 : UPropertyBaseOwner<T,OwnerT>(name,owner, ptPubInput, dynamic_prop_flag), Value() { };
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
protected:
// Возврат значения
virtual const T& GetDataValueLocal(void) const
{
 return Value;
};

virtual bool SetDataValueLocal(const T &value)
{
 return false;
};

public:
// Возвращает ссылку на данные
virtual const T& GetData(void) const
{
 if(IsConnected())
 {
  UEPtr<UPropertyBase<T> > prop=dynamic_pointer_cast<UPropertyBase<T> >(GetConnectedProperty(CurrentInputIndex));
  UEPtr<UNet> owner=prop->GetOwner();
  return *dynamic_pointer_cast<T>(owner);
 }
 else
 {
  return GetDataValueLocal();
 }
}

// Модифицирует данные
virtual bool SetData(const T& data)
{
 return false;
}

// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 return true;
};

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 return true;
};

// Метод сравнивает тип этого свойства с другим свойством
virtual bool CompareLanguageType(const UIProperty &dt) const
{
 const UIPropertyOutput* out=dynamic_cast<const UIPropertyOutput*>(&dt);
 if(!out)
  return false;
 return (dynamic_cast<const T*>(out->GetOwner()))?true:false;
}

// Метод изменяет тип свойства
bool ChangeType(unsigned int value)
{
 return false;
}

const T* operator -> (void) const
{ return &this->GetData(); };

T* operator -> (void)
{ return const_cast<T*>(&this->GetData()); };

const T& operator * (void) const
{ return this->GetData(); };

T& operator * (void)
{ return const_cast<T&>(this->GetData()); };
};

#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif
/* ************************************************************************* */



}

#endif

