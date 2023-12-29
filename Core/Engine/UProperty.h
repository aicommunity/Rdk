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
#include "UComponent.h"
#include "../System/rdk_system.h"

namespace RDK {

using namespace std;

#ifdef __BORLANDC__
#pragma warning( disable : 4700)
#endif

// Класс - база для свойств
template<typename T>
class UVBaseDataProperty: public UIPropertyOutput
{
protected: // Данные
// Тип входа
int IoType;

protected: // Данные синхронизации
/// Мьютекс этого свойства
UGenericMutex *Mutex;

/// Время обновления свойства (мс)
mutable ULongTime UpdateTime;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
explicit UVBaseDataProperty(T * const pdata)
 : IoType(ipSingle | ipData), Mutex(UCreateMutex()), UpdateTime(0)
{
}

virtual ~UVBaseDataProperty(void)
{
 UDestroyMutex(Mutex);
 Mutex=0;
}
// -----------------------------

// -----------------------------
// Методы сериализации
// -----------------------------
// Возвращает ссылку на данные
virtual const T& GetData(void) const=0;

// Модифицирует данные
virtual void SetData(const T& data)=0;

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

// Возвращает языковой тип хранимого свойства для одного элемента
virtual const type_info& GetElemLanguageType(void) const
{
 return typeid(T);
}

// Метод сравнивает тип этого свойства с другим свойством (по одному элементу)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return GetElemLanguageType() == dt.GetElemLanguageType();
}

// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<USerStorage>  storage, bool simplemode=false)
{
/*
 UEPtr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<GetData();
  return true;
 }
 */
 UEPtr<USerStorageXML> xml=dynamic_pointer_cast<USerStorageXML>(storage);
 if(xml)
 {
  if(simplemode)
  {
   xml->Create(GetName());
   try
   {
    operator << (*xml,GetData());
   }
   catch(UIProperty::EPropertyZeroPtr &)
   {
   }
   xml->SelectUp();
   return true;
  }
  else
  {
   xml->AddNode(GetName());
   try
   {
    operator << (*xml,GetData());
   }
   catch(UIProperty::EPropertyZeroPtr &)
   {
   }
   xml->SelectUp();
   return true;
  }
 }

 return false;
}

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<USerStorage>  storage, bool simplemode=false)
{
 T temp;
/*
 UEPtr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  operator >> (*binary,temp);
  SetData(temp);
  return true;
 }*/

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
}

// Метод возвращает указатель на область памяти, содержащую данные свойства
virtual const void* GetMemoryArea(void)
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
// Тип
virtual int GetIoType(void) const
{
 return IoType;
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
// --------------------------

// -----------------------------
// Привязка внешней ссылки как источника данных
// -----------------------------
virtual bool AttachTo(UVBaseDataProperty<T>* prop)
{
 return false;
}

virtual void DetachFrom(void)
{
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


// Класс - база для свойств
template<typename T,class OwnerT>
class UVBaseProperty: public UVBaseDataProperty<T>
{
protected: // Данные
// Владелец свойства
OwnerT* Owner;

// Указатель на итератор-хранилище данных об этом свойстве в родительском компоненте
UComponent::VariableMapCIteratorT Variable;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
explicit UVBaseProperty(OwnerT * const owner) :
  UVBaseDataProperty<T>(0), Owner(owner)
{
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}

UVBaseProperty(OwnerT * const owner, T * const pdata) :
  UVBaseDataProperty<T>(pdata), Owner(owner)
{
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}
// -----------------------------

// -----------------------------
// Методы сериализации
// -----------------------------
// Метод устанавливает значение указателя на итератор-хранилище данных об этом
// свойстве в родительском компоненте
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

// Метод возвращает указатель компонента-владельца свойства
virtual UContainer* GetOwner(void) const
{
 return dynamic_cast<UContainer*>(Owner);
}

// Метод возвращает строковое имя свойства
virtual const std::string& GetName(void) const
{
 return Variable->first;
}

// Метод возвращает тип свойства
virtual unsigned int GetType(void) const
{
 return Variable->second.Type;
}

// Метод возвращает строковое имя компонента-владельца свойства
virtual std::string GetOwnerName(void) const
{
 return (Owner)?Owner->GetName():std::string("");
}

// Метод возвращает строковое имя класса-владельца свойства
virtual std::string GetOwnerClassName(void) const
{
 return typeid(Owner).name();
}
// -----------------------------
};

// Класс - виртуальное свойство
// Не содержит данного внутри себя
template<typename T,class OwnerT>
class UVProperty: public UVBaseProperty<T,OwnerT>
{
//friend class OwnerT;
public: // Типы методов ввода-вывода
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // Данные
// Методы ввода-вывода
GetterRT GetterR;
SetterRT SetterR;

protected:
/// Ссылка на внешнее свойство-источник данных
UVBaseDataProperty<T>* ExternalDataSource;

protected:
/// Флаг наличия подключения
bool IsConnectedFlag;

/// Указатель на подключенный выход
std::vector<UIPropertyOutput*> ConnectedOutputs;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseProperty<T,OwnerT>(owner), /*Getter(0), Setter(0), */GetterR(getmethod), SetterR(setmethod), ExternalDataSource(0)
{
    IsConnectedFlag=false;
}

UVProperty(OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UVBaseProperty<T,OwnerT>(owner,pdata), /*Getter(0), Setter(0), */GetterR(0), SetterR(setmethod), ExternalDataSource(0)
{
    IsConnectedFlag=false;
}
// -----------------------------

// -----------------------------
// Привязка внешней ссылки как источника данных
// -----------------------------
bool AttachTo(UVBaseDataProperty<T>* prop)
{
 if(!prop)
  return false;
 ExternalDataSource=prop;
 this->UpdateConnectedPointers();
 return true;
}

void DetachFrom(void)
{
 ExternalDataSource=0;
 this->UpdateConnectedPointers();
}
// -----------------------------

// -----------------------------
// Методы управления подклчаемым выходом
// -----------------------------
/// Возвращает имя подключенного компонента
virtual UItem* GetItem(int index=0)
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return reinterpret_cast<UItem*>(ConnectedOutputs[index]->GetOwner());
 }
 return 0;
}

/// Возвращает имя подключенного выхода
virtual std::string GetItemOutputName(int index=0) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetName();
 }
 return std::string();
}

/// Возвращает имя подключенного компонента
virtual std::string GetItemName(int index=0) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetOwner()->GetName();
 }
 return std::string();
}

/// Возвращает полное имя подключенного компонента
virtual std::string GetItemFullName(int index=0) const
{
 if(int(this->ConnectedOutputs.size())>index && index >=0)
 {
  return ConnectedOutputs[index]->GetOwner()->GetFullName();
 }
 return std::string();
}

/// Применяет время выхода к входу
void ApplyOutputUpdateTime(void) const
{
 if(!ConnectedOutputs.empty())
  this->UpdateTime=ConnectedOutputs[0]->GetUpdateTime();
}

// Возвращает true если вход имеет подключение
bool IsConnected(void) const
{
 return IsConnectedFlag;
}

/// Возвращает true, если на подключенном выходе новые данные
virtual bool IsNewData(void) const
{
 return (!ConnectedOutputs.empty())?this->ConnectedOutputs[0]->GetUpdateTime()>this->UpdateTime:true;
}
// -----------------------------


// -----------------------------
// Методы управления
// -----------------------------
operator T (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

const T& operator () (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

T* operator -> (void)
{
 ApplyOutputUpdateTime();
 return const_cast<T*>(&this->GetData());
}

const T* operator -> (void) const
{
 ApplyOutputUpdateTime();
 return &this->GetData();
}

T& operator * (void)
{
 ApplyOutputUpdateTime();
 return const_cast<T&>(this->GetData());
}

const T& operator * (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

// Оператор присваивания
UVProperty<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}
// -----------------------------
};

class UItem;
class UConnector;

/* ************************************************************************* */
// Класс - свойство с значением внутри
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type=ptPubParameter>
class UPropertyLocal: public UVProperty<T,OwnerT>
{
protected:
/// Флаг проверки значения свойства на равенство присваевому значению
bool CheckEqualsFlag;

public:
//protected:
// Данные
mutable T v;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
public:
UPropertyLocal(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), CheckEqualsFlag(true), v()
{
// this->PData=&v;
 dynamic_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false);
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
// Операторы доступа
// -----------------------------
// Возврат значения
virtual const T& GetData(void) const
{
 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 return (IsConnectedFlag)?dynamic_cast<UVBaseDataProperty<T>*>(this->ConnectedOutputs[0])->GetData():v;
}

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if(IsConnectedFlag)
  return;

 if(CheckEqualsFlag && value == v)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
 }

 v=value;
 this->RenewUpdateTime();
 return;
}
// -----------------------------

// -----------------------------
// Методы управления подклчаемым выходом
// -----------------------------
bool AttachTo(UVBaseDataProperty<T>* prop)
{
 bool res=UVProperty<T,OwnerT>::AttachTo(prop);
 if(res)
 {
//  this->PData=const_cast<T*>(&this->ExternalDataSource->GetData());
  IsConnectedFlag=true;
 }
 return res;
}

void DetachFrom(void)
{
// this->PData=&v;
 IsConnectedFlag=false;
 UVProperty<T,OwnerT>::DetachFrom();
}

// Число указателей на данные
int GetNumPointers(void) const
{
 return int(this->ConnectedOutputs.size());
}

// Устанавливает указатель на данные входа
bool SetPointer(int index, UIPropertyOutput* property)
{
 //this->PData=const_cast<T*>(&dynamic_cast<UVBaseDataProperty<T>*>(property)->GetData());
 IsConnectedFlag=true;
 this->ConnectedOutputs.assign(1,property);
 this->ResetUpdateTime();
 return true;
}

/// Сбрасывает указатель на данные
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(!this->ConnectedOutputs.empty() && this->ConnectedOutputs[0] == property)
 {
//  this->PData=&v;
  IsConnectedFlag=false;
  ConnectedOutputs.clear();
  return true;
 }
 return false;
}
// -----------------------------

};
/* ************************************************************************* */


/// Конечный класс свойства со значением внутри
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UProperty: public UPropertyLocal<T,OwnerT,type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
UProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<T,OwnerT,type> &v) {}
// -----------------------------

// -----------------------------
// Операторы
// -----------------------------
public:
// Оператор присваивания
UProperty<T,OwnerT, type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UProperty<T,OwnerT, type>& operator = (const UProperty<T,OwnerT, type> &v)
{
 this->SetData(v.GetData());
 return *this;
}

operator T (void) const
{
 return this->GetData();
}

const T& operator () (void) const
{
 return this->GetData();
}
// -----------------------------
};


/* ************************************************************************* */
// Класс - свойство-контейнер со значением внутри
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UCPropertyLocal: public UPropertyLocal<T,OwnerT,type>
{
public: // Типы методов ввода-вывода
typedef typename T::value_type TV;
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // Данные
// Методы ввода-вывода
VSetterRT VSetterR;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
public:
UCPropertyLocal(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod), VSetterR(0)
{
 this->IoType = ipRange | ipData;
}

UCPropertyLocal(const string &name, OwnerT * const owner, typename UCPropertyLocal<T,OwnerT>::VSetterRT setmethod)
 : UPropertyLocal<T,OwnerT,type>(name, owner,(typename UVProperty<T,OwnerT>::SetterRT)0)
{
 VSetterR=setmethod;
}
// -----------------------------

// -----------------------------
// Операторы доступа
// -----------------------------
// Возврат значения
virtual const T& GetData(void) const
{
 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 return (IsConnectedFlag)?dynamic_cast<UVBaseDataProperty<T>*>(this->ConnectedOutputs[0])->GetData():v;
};

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if(IsConnectedFlag)
  return;

 if(CheckEqualsFlag && v == value)
  return;

 if(this->Owner)
 {
  if(VSetterR)
  {
   typename T::const_iterator I,J;
   I=value.begin(); J=value.end();
   while(I != J)
   {
    if(!(this->Owner->*VSetterR)(*I))
     throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

    ++I;
   }
  }
  else
  {
   if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
    throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
  }
 }

 v=value;
 this->RenewUpdateTime();
}
// -----------------------------

// -----------------------------
// Устанавливает указатель на данные входа
bool SetPointer(int index, UIPropertyOutput* property)
{
 if(index<0)
  return false;

 if(int(ConnectedOutputs.size())<=index)
 {
  size_t new_size=index+1;
  this->v.resize(new_size);
  ConnectedOutputs.resize(new_size,0);
 }
 ConnectedOutputs[index]=property;
 return true;
}

/// Сбрасывает указатель на данные
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(int(this->v.size())>index && index >=0)
 {
  auto it = this->v.begin();
  std::advance(it,index);
  this->v.erase(it);
 }

 if(int(this->ConnectedOutputs.size())>index && index >= 0)
 {
  this->ConnectedOutputs.erase(ConnectedOutputs.begin()+index);
  return true;
 }

 return false;
}
// -----------------------------

public: // Исключения

// Выход за границы массива C (container) property
struct EPropertyRangeError: public UIProperty::EPropertyError
{
int MinValue, MaxValue, ErrorValue;
public:
EPropertyRangeError(const std::string &owner_name, const std::string &property_name, int min_value, int max_value, int error_value)
 : UIProperty::EPropertyError(owner_name, property_name),
   MinValue(min_value), MaxValue(max_value), ErrorValue(error_value) {}


// Формирует строку лога об исключении
virtual std::string CreateLogMessage(void) const
{
 return UIProperty::EPropertyError::CreateLogMessage()+std::string(" MinValue=")+
        sntoa(MinValue)+std::string(" MaxValue=")+sntoa(MaxValue)+
        std::string(" ErrorValue=")+sntoa(ErrorValue);
}
};

public:
// -----------------------------
// Операторы доступа
// -----------------------------
// Чтение элемента контейнера
const typename UCPropertyLocal<T,OwnerT, type>::TV& operator () (size_t i) const
{
 const T& data = GetData();
 if(i>=data.size())
  throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                               0,int(data.size()),int(i));

 return data[i];
}

// Запись элемента контейнера
bool operator () (size_t i, const typename UCPropertyLocal<T,OwnerT, type>::TV &value)
{
 if(UVProperty<T,OwnerT>::VSetterR && !(this->Owner->*(UVProperty<T,OwnerT>::VSetterR)(value)))
  throw EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

 if(IsConnectedFlag)
  return false;

 if(i>=this->v.size())
  throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                               0,int(this->v.size()),int(i));

 this->v[i]=value;
 this->RenewUpdateTime();

 return true;
}

operator T (void) const
{
 return this->GetData();
}

const T& operator () (void) const
{
 return this->GetData();
}

T* operator -> (void)
{ return const_cast<T*>(&this->GetData()); }

const T* operator -> (void) const
{ return &this->GetData(); }

T& operator * (void)
{ return const_cast<T&>(this->GetData()); }

const T& operator * (void) const
{ return this->GetData(); }

typename UCPropertyLocal<T,OwnerT, type>::TV& operator [] (size_t i)
{ return const_cast<UCPropertyLocal<T,OwnerT, type>::TV&>((*this)(i)); }

const typename UCPropertyLocal<T,OwnerT, type>::TV& operator [] (size_t i) const
{ return (*this)(i); }

// Оператор присваивания
UCPropertyLocal& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UCPropertyLocal& operator = (const UCPropertyLocal &value)
{
 this->SetData(value.GetData());
 return *this;
}

// --------------------------
// Методы управления входами
// --------------------------
// Метод сравнивает тип этого свойства с другим свойством (по одному элементу)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return (GetElemLanguageType() == dt.GetElemLanguageType()) || (typeid(T::value_type) == dt.GetElemLanguageType());
}
// --------------------------
};


// Конечный класс - свойство-контейнер со значением внутри
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UCProperty: public UCPropertyLocal<T,OwnerT, type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации
UCProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UCPropertyLocal<T,OwnerT, type>(name, owner, setmethod)
{ }

//Конструктор инициализации для отдельных значений
UCProperty(const string &name, OwnerT * const owner, typename UCProperty<T,OwnerT>::VSetterRT setmethod)
    : UCPropertyLocal<T,OwnerT, type>(name, owner,setmethod)
{ }

protected:
UCProperty(const UCProperty<T,OwnerT, type> &v) {}
// -----------------------------

public:
// Оператор присваивания
UCProperty& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UCProperty& operator = (const UCProperty &value)
{
 this->SetData(value.GetData());
 return *this;
}
};

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using ULProperty = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using UCLProperty = UCProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputData = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputCData = UCProperty<std::vector<T>, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
using UPropertyInputData = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
using UPropertyInputCData = UCProperty<std::vector<T>, OwnerT, type>;


#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif

}

#endif

