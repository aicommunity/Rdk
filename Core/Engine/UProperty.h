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
#include <iterator>
#include <type_traits>
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

#ifdef _MSC_VER
#pragma warning(disable:4172)
#endif

namespace RDK {

namespace detail
{
 // To allow ADL with custom begin/end
 using std::begin;
 using std::end;

 template <typename T>
 auto is_iterable_impl(int)
 -> decltype (
     begin(std::declval<T&>()) != end(std::declval<T&>()), // begin/end and operator !=
     void(), // Handle evil operator ,
     ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
     void(*begin(std::declval<T&>())), // operator*
     std::true_type{});

 template <typename T>
 std::false_type is_iterable_impl(...);
}

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(0));

template <typename T, typename = int>
struct has_resize : std::false_type { };

template <typename T>
struct has_resize <T, decltype( (void) std::declval<T>().resize(1), 0)> : std::true_type { };

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
// Constructor with optional mutex creation
explicit UVBaseDataProperty(T * const pdata, bool needs_mutex = false)
 : IoType(static_cast<unsigned int>(ipSingle) | static_cast<unsigned int>(ipData)), Mutex(needs_mutex ? UCreateMutex() : nullptr), UpdateTime(0)
{
}

virtual ~UVBaseDataProperty(void)
{
 if(Mutex)
 {
  UDestroyMutex(Mutex);
  Mutex=0;
 }
}
// -----------------------------

// -----------------------------
// ������ ������������
// Модифицирует данные
// ���������� ������ �� ������
virtual const T& GetData(void) const=0;

// ������������ ������
virtual void SetData(const T& data)=0;

// ���������� �������� ��� ��������� ��������
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}

// ����� ���������� ��� ����� �������� � ������ ���������
virtual bool CompareLanguageType(const UIProperty &dt) const
{
 return GetLanguageType() == dt.GetLanguageType();
}

// ���������� �������� ��� ��������� �������� ��� ������ ��������
virtual const type_info& GetElemLanguageType(void) const
{
 return typeid(T);
}

// ����� ���������� ��� ����� �������� � ������ ��������� (�� ������ ��������)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return GetElemLanguageType() == dt.GetElemLanguageType();
}

// ����� ���������� �������� �������� � �����
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

// Method reads property value from stream
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

// ����� ���������� ��������� �� ������� ������, ���������� ������ ��������
virtual const void* GetMemoryArea(void)
{
 return &GetData();
}

// ����� �������� �������� ������ �������� �� ������� ������
// �������� ���������� ����������� ��������� ���� ������
// ������� ��������� ���������� � ��������� �� ����������� ��� ������
bool ReadFromMemory(const void *buffer)
{
 if(!buffer)
  return false;

 const T* temp=(const T*)buffer;
 SetData(*temp);
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���
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
// -----------------------------

// -----------------------------
// �������� ������� ������ ��� ��������� ������
// -----------------------------
virtual bool AttachTo(UVBaseDataProperty<T>* prop)
{
 return false;
}

virtual void DetachFrom(void)
{
}
// Скрытые методы управления данными

protected:
// --------------------------
// ������� ������ ���������� �������
// --------------------------
/// ��������� ����� ��������� ������ ��������
void RenewUpdateTime(void)
{
 UpdateTime=GetCurrentStartupTime();
}

/// ���������� ����� ���������� �� ����
void ResetUpdateTime(void)
{
 UpdateTime=0;
}
// Класс - база для свойств
};


// Данные
template<typename T,class OwnerT>
class UVBaseProperty: public UVBaseDataProperty<T>
{
protected: // Указатель на итератор-хранилище данных об этом свойстве в родительском компоненте
// �������� ��������
OwnerT* Owner;

// --------------------------
UComponent::VariableMapCIteratorT Variable;

public: //Конструктор инициализации.
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
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

// свойстве в родительском компоненте
// ������ ������������
// -----------------------------
// ����� ������������� �������� ��������� �� ��������-��������� ������ �� ����
// �������� � ������������ ����������
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

// ����� ���������� ��������� ����������-��������� ��������
virtual UContainer* GetOwner(void) const
{
 return dynamic_cast<UContainer*>(Owner);
}

// ����� ���������� ��������� ��� ��������
virtual const std::string& GetName(void) const
{
 return Variable->first;
}

// ����� ���������� ��� ��������
virtual unsigned int GetType(void) const
{
 return Variable->second.Type;
}

// ����� ���������� ��������� ��� ����������-��������� ��������
virtual std::string GetOwnerName(void) const
{
 return (Owner)?Owner->GetName():std::string("");
}

// ����� ���������� ��������� ��� ������-��������� ��������
virtual std::string GetOwnerClassName(void) const
{
 return typeid(Owner).name();
}
// Не содержит данного внутри себя
};

// ����� - ����������� ��������
//friend class OwnerT;
template<typename T,class OwnerT>
class UVProperty: public UVBaseDataProperty<T>
{
//friend class OwnerT;
public: // Данные
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // ������
// ������ �����-������
GetterRT GetterR;
SetterRT SetterR;

protected: // Owner and variable (from UVBaseProperty)
/// Флаг наличия подключения
OwnerT* Owner;

/// Указатель на подключенный выход
UComponent::VariableMapCIteratorT Variable;

protected:
// --------------------------
UVBaseDataProperty<T>* ExternalDataSource;

protected:
/*Getter(0), Setter(0), */
bool IsConnectedFlag;

/// ��������� �� ������������ �����
std::vector<UIPropertyOutput*> ConnectedOutputs;

/*Getter(0), Setter(0), */
mutable UVBaseDataProperty<T>* CachedConnectedOutput;

protected: // Local storage (from UPropertyLocal)
// -----------------------------
bool CheckEqualsFlag;

// Привязка внешней ссылки как источника данных
mutable T v;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseDataProperty<T>(0), Owner(owner), GetterR(getmethod), SetterR(setmethod), ExternalDataSource(0),
  IsConnectedFlag(false), CachedConnectedOutput(0), CheckEqualsFlag(true), v()
{
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}

UVProperty(OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UVBaseDataProperty<T>(pdata), Owner(owner), GetterR(0), SetterR(setmethod), ExternalDataSource(0),
  IsConnectedFlag(false), CachedConnectedOutput(0), CheckEqualsFlag(true), v()
{
 if(Owner)
  Variable=Owner->FindPropertyVariable(this);
}
// -----------------------------

/// Возвращает имя подключенного выхода
// �������� ������� ������ ��� ��������� ������
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
// Property information methods (from UVBaseProperty)
// -----------------------------
/// Возвращает true, если на подключенном выходе новые данные
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

/// Get owner component as UContainer
virtual UContainer* GetOwner(void) const
{
 return dynamic_cast<UContainer*>(Owner);
}

/// Get property name
virtual const std::string& GetName(void) const
{
 return Variable->first;
}

/// Get property type
virtual unsigned int GetType(void) const
{
 return Variable->second.Type;
}

/// Get owner component name
virtual std::string GetOwnerName(void) const
{
 return (Owner)?Owner->GetName():std::string("");
}

/// Get owner component class name
virtual std::string GetOwnerClassName(void) const
{
 return typeid(Owner).name();
}
// -----------------------------

// -----------------------------
//  
// -----------------------------
///  
virtual UItem* GetItem(int index=0);

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(int index=0) const;

/// ���������� ��� ������������� ����������
virtual std::string GetItemName(int index=0) const;

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(int index=0) const;

/// ��������� ����� ������ � �����
void ApplyOutputUpdateTime(void) const
{
 // Lazy update: only update if connected output's time is newer
 if(IsConnectedFlag && ConnectedOutputs[0]->GetUpdateTime() > this->UpdateTime)
  this->UpdateTime=ConnectedOutputs[0]->GetUpdateTime();
}

/* ************************************************************************* */
bool IsConnected(void) const
{
 return IsConnectedFlag;
}

/// ���������� true, ���� �� ������������ ������ ����� ������
virtual bool IsNewData(void) const
{
 return (!ConnectedOutputs.empty())?this->ConnectedOutputs[0]->GetUpdateTime()>this->UpdateTime:true;
}
//protected:


// -----------------------------
// ������ ����������
// --------------------------
inline operator T (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

const T& operator () (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

inline T* operator -> (void)
{
 ApplyOutputUpdateTime();
 return const_cast<T*>(&this->GetData());
}

inline const T* operator -> (void) const
{
 ApplyOutputUpdateTime();
 return &this->GetData();
}

inline T& operator * (void)
{
 ApplyOutputUpdateTime();
 return const_cast<T&>(this->GetData());
}

inline const T& operator * (void) const
{
 ApplyOutputUpdateTime();
 return this->GetData();
}

// �������� ������������
inline UVProperty<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}
// -----------------------------

// -----------------------------
// Check equals flag methods (from UPropertyLocal)
// -----------------------------
/// Check if value equals check is enabled
bool IsCheckEquals(void) const
{
 return CheckEqualsFlag;
}

/// Set value equals check flag
void SetCheckEquals(bool value)
{
 CheckEqualsFlag=value;
}
// -----------------------------

// -----------------------------
// Data access methods (from UPropertyLocal)
// -----------------------------
/// Get data implementation (from UPropertyLocal)
inline const T& GetData(void) const
{
 // Fast path for unconnected properties
 if(!IsConnectedFlag && !this->ExternalDataSource)
  return v;

 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 if(IsConnectedFlag)
 {
  //  this->PData=const_cast<T*>(&this->ExternalDataSource->GetData());
  if(!CachedConnectedOutput && !ConnectedOutputs.empty())
   CachedConnectedOutput = dynamic_cast<UVBaseDataProperty<T>*>(ConnectedOutputs[0]);
  
  if(CachedConnectedOutput)
  {
   // Cache data with update time check to avoid unnecessary copies
   ULongTime outputTime = CachedConnectedOutput->GetUpdateTime();
   if(outputTime > this->UpdateTime)
   {
    v = CachedConnectedOutput->GetData();
    this->UpdateTime = outputTime;
   }
  }
 }

 return v;
}

// Устанавливает указатель на данные входа
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
   throw UIProperty::EPropertySetterFail(this->GetOwnerName(),this->GetName());
 }

 v=value;
 this->RenewUpdateTime();
 return;
}
/* ************************************************************************* */
};

class UItem;
class UConnector;
class UContainer;

/* ************************************************************************* */
// ����� - �������� � ��������� ������
/* ************************************************************************* */
template<typename T,class OwnerT, unsigned int type>
class UPropertyLocal: public UVProperty<T,OwnerT>
{
protected:
// Конструкторы и деструкторы
bool CheckEqualsFlag;

public:
//protected:
// ������
mutable T v;

public:
// -----------------------------
// ������������ � �����������
// -----------------------------
public:
UPropertyLocal(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(owner, setmethod, 0), CheckEqualsFlag(true), v()
{
// this->PData=&v;
 dynamic_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false);
}
// -----------------------------

// -----------------------------
// ����� ���������� �����������
// -----------------------------
/// ���� �������� �������� �������� �� ��������� ����������� ��������
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
// -----------------------------
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 if(UVProperty<T,OwnerT>::IsConnectedFlag)
 {
  // Типы методов ввода-вывода
  if(!UVProperty<T,OwnerT>::CachedConnectedOutput && !UVProperty<T,OwnerT>::ConnectedOutputs.empty())
   UVProperty<T,OwnerT>::CachedConnectedOutput = dynamic_cast<UVBaseDataProperty<T>*>(UVProperty<T,OwnerT>::ConnectedOutputs[0]);
  
  if(UVProperty<T,OwnerT>::CachedConnectedOutput)
  {
   // Cache data with update time check to avoid unnecessary copies
   ULongTime outputTime = UVProperty<T,OwnerT>::CachedConnectedOutput->GetUpdateTime();
   if(outputTime > this->UpdateTime)
   {
    v = UVProperty<T,OwnerT>::CachedConnectedOutput->GetData();
    this->UpdateTime = outputTime;
   }
  }
 }

 return v;
}

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if(UVProperty<T,OwnerT>::IsConnectedFlag)
  return;

 if(CheckEqualsFlag && value == v)
  return;

 if(this->Owner)
 {
  if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(this->GetOwnerName(),this->GetName());
 }

 v=value;
 this->RenewUpdateTime();
 return;
}
// -----------------------------

// -----------------------------
// ������ ���������� ����������� �������
// -----------------------------
bool AttachTo(UVBaseDataProperty<T>* prop)
{
 bool res=UVProperty<T,OwnerT>::AttachTo(prop);
 if(res)
 {
//  this->PData=const_cast<T*>(&this->ExternalDataSource->GetData());
  UVProperty<T,OwnerT>::IsConnectedFlag=true;
 }
 return res;
}

void DetachFrom(void)
{
// this->PData=&v;
 UVProperty<T,OwnerT>::IsConnectedFlag=false;
 UVProperty<T,OwnerT>::CachedConnectedOutput = 0; // Clear cache
 UVProperty<T,OwnerT>::DetachFrom();
}

// ����� ���������� �� ������
int GetNumPointers(void) const
{
 return int(this->ConnectedOutputs.size());
}

// ������������� ��������� �� ������ �����
bool SetPointer(int index, UIPropertyOutput* property)
{
 //this->PData=const_cast<T*>(&dynamic_cast<UVBaseDataProperty<T>*>(property)->GetData());
 UVProperty<T,OwnerT>::IsConnectedFlag=true;
 // Cache typed pointer for optimization
 UVProperty<T,OwnerT>::CachedConnectedOutput = dynamic_cast<UVBaseDataProperty<T>*>(property);
 UVProperty<T,OwnerT>::ConnectedOutputs.assign(1,property);
 this->ResetUpdateTime();
 return true;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(!this->ConnectedOutputs.empty() && this->ConnectedOutputs[0] == property)
 {
//  this->PData=&v;
  UVProperty<T,OwnerT>::IsConnectedFlag=false;
  UVProperty<T,OwnerT>::CachedConnectedOutput = 0; // Clear cache
  UVProperty<T,OwnerT>::ConnectedOutputs.clear();
  return true;
 }
 return false;
}
// -----------------------------

};
/* ************************************************************************* */


/// �������� ����� �������� �� ��������� ������
/// https://stackoverflow.com/questions/60608588/specializing-a-template-for-a-container-of-type-t
template<typename T,class OwnerT, unsigned int type, bool = is_iterable<T>::value>
class UProperty;


template<typename T, typename OwnerT, unsigned int type>
class UProperty<T, OwnerT, type, false>: public UPropertyLocal<T,OwnerT,type>
{
public:
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������
UProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<T,OwnerT,type> &v) {}
// -----------------------------

// -----------------------------
// Выход за границы массива C (container) property
// -----------------------------
public:
// �������� ������������
UProperty& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UProperty& operator = (const UProperty<T,OwnerT, type> &v)
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
// ����� - ��������-��������� �� ��������� ������
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type>
class UProperty<T, OwnerT, type, true>: public UPropertyLocal<T,OwnerT,type>
{
public: // ���� ������� �����-������
typedef typename T::value_type TV;
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // ������
// ������ �����-������
VSetterRT VSetterR;

public:
// --------------------------
// ������������ � �����������
// --------------------------
public:
UProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod), VSetterR(0)
{
 this->IoType = static_cast<unsigned int>(ipRange) | static_cast<unsigned int>(ipData);
}

UProperty(const string &name, OwnerT * const owner, typename UProperty<T,OwnerT,type>::VSetterRT setmethod)
 : UPropertyLocal<T,OwnerT,type>(name, owner,(typename UVProperty<T,OwnerT>::SetterRT)0)
{
 this->IoType = static_cast<unsigned int>(ipRange) | static_cast<unsigned int>(ipData);
 VSetterR=setmethod;
}
// -----------------------------

// -----------------------------
// ��������� �������
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 if(this->IsConnectedFlag)
  UpdateLocalInputData(this->v);

 return this->v;
}

virtual void SetData(const T &value)
{
 if(this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if(this->IsConnectedFlag)
  return;

 if(this->CheckEqualsFlag && this->v == value)
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
     throw UIProperty::EPropertySetterFail(this->GetOwnerName(),this->GetName());

    ++I;
   }
  }
  else
  {
   if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
    throw UIProperty::EPropertySetterFail(this->GetOwnerName(),this->GetName());
  }
 }

 this->v=value;
 this->RenewUpdateTime();
}
// -----------------------------

// -----------------------------
// ������������� ��������� �� ������ �����
bool SetPointer(int index, UIPropertyOutput* property)
{
 if(index<0)
  return false;

 if(int(UVProperty<T,OwnerT>::ConnectedOutputs.size())<=index)
 {
  size_t new_size=index+1;
  if(has_resize<T>::value)
   this->v.resize(new_size);
  else
   throw std::runtime_error("resize doesn't support");
  this->ConnectedOutputs.resize(new_size,0);
 }
 this->ConnectedOutputs[index]=property;
 UVProperty<T,OwnerT>::IsConnectedFlag=true;
 return true;
}

/// ���������� ��������� �� ������
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(int(this->v.size())>index && index >=0)
 {
  auto it = this->v.begin();
  std::advance(it,index);
  this->v.erase(it);
 }

 if(int(UVProperty<T,OwnerT>::ConnectedOutputs.size())>index && index >= 0)
 {
  UVProperty<T,OwnerT>::ConnectedOutputs.erase(UVProperty<T,OwnerT>::ConnectedOutputs.begin()+index);
  if(UVProperty<T,OwnerT>::ConnectedOutputs.empty())
   UVProperty<T,OwnerT>::IsConnectedFlag=false;
  return true;
 }

 return false;
}
// -----------------------------

public: // ����������

// ����� �� ������� ������� C (container) property
struct EPropertyRangeError: public UIProperty::EPropertyError
{
int MinValue, MaxValue, ErrorValue;
public:
EPropertyRangeError(const std::string &owner_name, const std::string &property_name, int min_value, int max_value, int error_value)
 : UIProperty::EPropertyError(owner_name, property_name),
   MinValue(min_value), MaxValue(max_value), ErrorValue(error_value) {}


// ��������� ������ ���� �� ����������
virtual std::string CreateLogMessage(void) const
{
 return UIProperty::EPropertyError::CreateLogMessage()+std::string(" MinValue=")+
        sntoa(MinValue)+std::string(" MaxValue=")+sntoa(MaxValue)+
        std::string(" ErrorValue=")+sntoa(ErrorValue);
}
};

public:
// -----------------------------
// ��������� �������
// -----------------------------
// ������ �������� ����������
const typename UProperty<T, OwnerT, type, true>::TV& operator () (size_t i) const
{
 const T& data_ref = this->GetData();
 if(i>=data_ref.size())
  throw EPropertyRangeError(this->GetOwnerName(),this->GetName(),
                               0,int(data_ref.size()),int(i));

 // GetData() returns reference to member 'v', not a temporary
 // For std::vector<bool>, operator[] returns a proxy object, not a direct reference
 // This is safe because the proxy object is valid as long as the vector exists
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
#endif
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4172)
#endif
 return data_ref[i];
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

// ������ �������� ����������
bool operator () (size_t i, const typename UProperty<T, OwnerT, type, true>::TV &value)
{
 if(UVProperty<T,OwnerT>::VSetterR && !(this->Owner->*(UVProperty<T,OwnerT>::VSetterR)(value)))
  throw EPropertySetterFail(this->GetOwnerName(),this->GetName());

 if(this->IsConnectedFlag)
  return false;

 if(i>=this->v.size())
  throw EPropertyRangeError(this->GetOwnerName(),this->GetName(),
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

typename UProperty<T, OwnerT, type, true>::TV& operator [] (size_t i)
{ return const_cast<UProperty<T, OwnerT, type, true>::TV&>((*this)(i)); }

const typename UProperty<T, OwnerT, type, true>::TV& operator [] (size_t i) const
{ return (*this)(i); }

// �������� ������������
UProperty& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}

UProperty& operator = (const UProperty &value)
{
 this->SetData(value.GetData());
 return *this;
}

bool empty(void) const
{
 return this->GetData().empty();
}

typename T::const_iterator begin(void) const
{
 return this->GetData().begin();
}

typename T::iterator begin(void)
{
 return const_cast<T&>(this->GetData()).begin();
}

typename T::const_iterator end(void) const
{
 return this->GetData().end();
}

typename T::iterator end(void)
{
 return const_cast<T&>(this->GetData()).end();
}

T& back(void)
{
 return const_cast<T&>(this->GetData()).back();
}

T& front(void)
{
 return const_cast<T&>(this->GetData()).front();
}

const T& back(void) const
{
 return this->GetData().back();
}

const T& front(void) const
{
 return this->GetData().front();
}

void push_back(const TV &value)
{
 const_cast<T&>(this->GetData()).push_back(value);
}

size_t size(void) const
{
 return this->GetData().size();
}

void resize(size_t size, const TV &val)
{
 if(has_resize<T>::value)
  this->v.resize(size,val);
 else
  throw std::runtime_error("resize doesn't support");
}

void resize(size_t size)
{
 if(has_resize<T>::value)
  this->v.resize(size);
 else
  throw std::runtime_error("resize doesn't support");
}

void assign(size_t size, const TV &val)
{
 if(has_resize<T>::value)
  this->v.assign(size,val);
 else
  throw std::runtime_error("resize doesn't support");
}

// --------------------------
// ������ ���������� �������
// --------------------------
// ����� ���������� ��� ����� �������� � ������ ��������� (�� ������ ��������)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return (this->GetElemLanguageType() == dt.GetElemLanguageType()) || (typeid(TV) == dt.GetElemLanguageType());
}
// --------------------------

protected:
 const T& UpdateLocalInputData(T& data) const
{
 data.resize(UVProperty<T,OwnerT>::ConnectedOutputs.size());
 size_t i=0;
 for(auto I=data.begin();I != data.end();I++)
 {
  *I = dynamic_cast<const UVBaseDataProperty<TV>*>(UVProperty<T,OwnerT>::ConnectedOutputs[i])->GetData();
  ++i;
 }
 return data;
}
};

template <typename OwnerT, unsigned int type>
class UProperty<std::string, OwnerT, type, true>
    : public UProperty<std::string, OwnerT, type, false>
{
  using base = UProperty<std::string, OwnerT, type, false>;
  using base::base;

public:
 // �������� ������������
 UProperty& operator = (const std::string &value)
 {
  this->SetData(value);
  return *this;
 }

 UProperty& operator = (const UProperty &value)
 {
  this->SetData(value.GetData());
  return *this;
 }
};

// Deprecated aliases have been removed. Use UProperty<T, OwnerT, type> directly.
// All property types (Parameters, States, Inputs, Outputs) now use unified UProperty.
// For container properties, use UProperty<std::vector<T>, OwnerT, type>.




#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif

}

#endif

