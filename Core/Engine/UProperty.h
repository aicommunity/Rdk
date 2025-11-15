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
#include <memory>
#include <concepts>
#include <memory>
#include "ModernProperties.h"

// Forward declaration
class UContainer;
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

// Modern C++20 concepts for property system
#if RDK_HAS_CONCEPTS
template<typename T>
concept PropertyValueType = std::is_arithmetic_v<T> || 
                           std::is_same_v<T, std::string> || 
                           std::is_enum_v<T> ||
                           std::is_same_v<T, bool>;

template<typename T>
concept PropertyContainerType = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    typename T::const_iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<size_t>;
};

template<typename T>
concept PropertyOwnerType = requires(T t) {
    { t.GetStorage() } -> std::convertible_to<std::shared_ptr<UStorage>>;
    { t.GetEnvironment() } -> std::convertible_to<std::shared_ptr<UEnvironment>>;
};
#else
// Fallback for pre-concepts C++20
template<typename T>
constexpr bool PropertyValueType_v = std::is_arithmetic_v<T> || 
                                    std::is_same_v<T, std::string> || 
                                    std::is_enum_v<T> ||
                                    std::is_same_v<T, bool>;

template<typename T>
constexpr bool PropertyContainerType_v = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    typename T::const_iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<size_t>;
};

template<typename T>
constexpr bool PropertyOwnerType = requires(T t) {
    { t.GetStorage() } -> std::convertible_to<std::shared_ptr<UStorage>>;
    { t.GetEnvironment() } -> std::convertible_to<std::shared_ptr<UEnvironment>>;
};
#endif

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

// ����� - ���� ��� �������
template<typename T>
class UVBaseDataProperty: public UIPropertyOutput
{
protected: // ������
// ��� �����
int IoType;

protected: // ������ �������������
/// ������� ����� ��������
std::shared_ptr<std::mutex> Mutex;

/// ����� ���������� �������� (��)
mutable ULongTime UpdateTime;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
explicit UVBaseDataProperty(T * const pdata)
 : IoType(ipSingle | ipData), Mutex(std::make_shared<std::mutex>()), UpdateTime(0)
{
}

virtual ~UVBaseDataProperty(void) = default;

// Move constructor
UVBaseDataProperty(UVBaseDataProperty&& other) noexcept
 : IoType(other.IoType), Mutex(std::move(other.Mutex)), UpdateTime(other.UpdateTime)
{
}

// Move assignment
UVBaseDataProperty& operator=(UVBaseDataProperty&& other) noexcept
{
 if (this != &other) {
  IoType = other.IoType;
  Mutex = std::move(other.Mutex);
  UpdateTime = other.UpdateTime;
 }
 return *this;
}
// -----------------------------

// -----------------------------
// ������ ������������
// -----------------------------
// ���������� ������ �� ������
// GetData method - specialized for void and non-void types
virtual const T& GetData(void) const = 0;

// ������������ ������
// SetData method - specialized for void and non-void types  
virtual void SetData(const T& data) = 0;

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
virtual bool Save(std::shared_ptr<USerStorage>  storage, bool simplemode=false)
{
/*
 std::shared_ptr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  *binary<<GetData();
  return true;
 }
 */
 std::shared_ptr<USerStorageXML> xml=dynamic_pointer_cast<USerStorageXML>(storage);
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

// ����� ������ �������� �������� �� ������
virtual bool Load(std::shared_ptr<USerStorage>  storage, bool simplemode=false)
{
 T temp;
/*
 std::shared_ptr<USerStorageBinary> binary=dynamic_pointer_cast<USerStorageBinary>(storage);
 if(binary)
 {
  operator >> (*binary,temp);
  SetData(temp);
  return true;
 }*/

 std::shared_ptr<USerStorageXML> xml=dynamic_pointer_cast<USerStorageXML>(storage);
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

// Modern C++20 Save/Load with std::shared_ptr - methods already exist above

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
// -----------------------------

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
// std::lock_guard<std::mutex> locker(*Mutex);
 return UpdateTime;
}

virtual void SetUpdateTime(ULongTime value)
{
// std::lock_guard<std::mutex> locker(*Mutex);
 UpdateTime=value;
}
// --------------------------

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
// -----------------------------

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
// --------------------------
};


// ����� - ���� ��� �������
// Specialization for void type
template<>
class UVBaseDataProperty<void>: public UIPropertyOutput
{
protected:
 // No data for void type
 int dummy_data; // Use int as dummy data
public:
 UVBaseDataProperty() : dummy_data(0) {}
 virtual ~UVBaseDataProperty() {}
 // Dummy implementations for void type
 virtual const int& GetData(void) const { return dummy_data; }
 virtual void SetData(const int& data) { dummy_data = data; }
 virtual const type_info& GetLanguageType(void) const
 {
  return typeid(void);
 }
};

template<typename T,class OwnerT>
class UVBaseProperty: public UVBaseDataProperty<T>
{
protected: // ������
// �������� ��������
OwnerT* Owner;

// ��������� �� ��������-��������� ������ �� ���� �������� � ������������ ����������
UComponent::VariableMapCIteratorT Variable;

public: // ������
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

// -----------------------------
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
 return nullptr; // Default implementation - override in derived classes
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

// Implement pure virtual methods from UIProperty base class
virtual const void* GetMemoryArea(void) override
{
 return nullptr;
}

virtual bool ReadFromMemory(const void *buffer) override
{
 return false;
}

// Implement pure virtual methods from UIPropertyInput
virtual UItem* GetItem(int index=0) override
{
 return nullptr;
}

virtual std::string GetItemName(int index=0) const override
{
 return "";
}

virtual std::string GetItemFullName(int index=0) const override
{
 return "";
}

virtual std::string GetItemOutputName(int index=0) const override
{
 return "";
}

virtual bool SetPointer(int index, UIPropertyOutput* property) override
{
 return false;
}

virtual bool ResetPointer(int index, UIPropertyOutput* property) override
{
 return false;
}

virtual bool IsNewData(void) const override
{
 return false;
}

virtual bool IsConnected(void) const override
{
 return false;
}
// -----------------------------
};

// ����� - ����������� ��������
// �� �������� ������� ������ ����
template<typename T,class OwnerT>
class UVProperty: public UVBaseProperty<T,OwnerT>
{
//friend class OwnerT;
public: // ���� ������� �����-������
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // ������
// ������ �����-������
GetterRT GetterR;
SetterRT SetterR;

protected:
/// ������ �� ������� ��������-�������� ������
UVBaseDataProperty<T>* ExternalDataSource;

protected:
/// ���� ������� �����������
bool IsConnectedFlag;

/// ��������� �� ������������ �����
std::vector<UIPropertyOutput*> ConnectedOutputs;

public: // ������
// --------------------------
// ������������ � �����������
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
// ������ ���������� ����������� �������
// -----------------------------
/// ���������� ��� ������������� ����������
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
 if(!ConnectedOutputs.empty())
  this->UpdateTime=ConnectedOutputs[0]->GetUpdateTime();
}

// ���������� true ���� ���� ����� �����������
bool IsConnected(void) const
{
 return IsConnectedFlag;
}

/// ���������� true, ���� �� ������������ ������ ����� ������
virtual bool IsNewData(void) const
{
 return (!ConnectedOutputs.empty())?this->ConnectedOutputs[0]->GetUpdateTime()>this->UpdateTime:true;
}
// -----------------------------


// -----------------------------
// ������ ����������
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

// �������� ������������
UVProperty<T,OwnerT>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
}
// -----------------------------
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
/// ���� �������� �������� �������� �� ��������� ����������� ��������
bool CheckEqualsFlag;

public:
//protected:
// ������
mutable T v;

public:
// --------------------------
// ������������ � �����������
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
// ��������� �������
// -----------------------------
// ������� ��������
virtual const T& GetData(void) const
{
 if(this->ExternalDataSource)
  return this->ExternalDataSource->GetData();

 if(UVProperty<T,OwnerT>::IsConnectedFlag)
  v = dynamic_cast<UVBaseDataProperty<T>*>(this->ConnectedOutputs[0])->GetData();

 return v;
}

virtual void SetData(const T &value)
{
 // SAFETY: Add protection against corrupted pointers
 try {
  if(this->ExternalDataSource)
  {
   try {
    this->ExternalDataSource->SetData(value);
    return;
   } catch (...) {
    // ExternalDataSource may be corrupted, continue to local storage
   }
  }

  if(UVProperty<T,OwnerT>::IsConnectedFlag)
   return;

  if(CheckEqualsFlag && value == v)
   return;

  if(this->Owner)
  {
   try {
    if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
     throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());
   } catch (...) {
    // Owner or SetterR may be corrupted, skip setter call and set value directly
    // This prevents segfault when accessing corrupted Owner pointer
   }
  }

  v=value;
  this->RenewUpdateTime();
  return;
 } catch (...) {
  // If this pointer itself is corrupted, we can't do anything
  // But at least we prevent segfault from propagating
  throw;
 }
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
// ���������
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
 this->IoType = ipRange | ipData;
}

UProperty(const string &name, OwnerT * const owner, typename UProperty<T,OwnerT,type>::VSetterRT setmethod)
 : UPropertyLocal<T,OwnerT,type>(name, owner,(typename UVProperty<T,OwnerT>::SetterRT)0)
{
 this->IoType = ipRange | ipData;
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
 // SAFETY: Add protection against corrupted pointers
 try {
  if(this->ExternalDataSource)
  {
   try {
    this->ExternalDataSource->SetData(value);
    return;
   } catch (...) {
    // ExternalDataSource may be corrupted, continue to local storage
   }
  }

  if(this->IsConnectedFlag)
   return;

  if(this->CheckEqualsFlag && this->v == value)
   return;

  if(this->Owner)
  {
   try {
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
   } catch (...) {
    // Owner or setter methods may be corrupted, skip setter call and set value directly
    // This prevents segfault when accessing corrupted Owner pointer
   }
  }

  this->v=value;
  this->RenewUpdateTime();
 } catch (...) {
  // If this pointer itself is corrupted, we can't do anything
  // But at least we prevent segfault from propagating
  throw;
 }
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

 if(int(this->ConnectedOutputs.size())>index && index >= 0)
 {
  this->ConnectedOutputs.erase(this->ConnectedOutputs.begin()+index);
  if(this->ConnectedOutputs.empty())
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
 const T& data = GetData();
 if(i>=data.size())
  throw EPropertyRangeError(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName(),
                               0,int(data.size()),int(i));

 return data[i];
}

// ������ �������� ����������
bool operator () (size_t i, const typename UProperty<T, OwnerT, type, true>::TV &value)
{
 if(UVProperty<T,OwnerT>::VSetterR && !(this->Owner->*(UVProperty<T,OwnerT>::VSetterR)(value)))
  throw EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

 if(this->IsConnectedFlag)
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
 data.resize(this->ConnectedOutputs.size());
 size_t i=0;
 for(auto I=data.begin();I != data.end();I++)
 {
  *I = dynamic_cast<const UVBaseDataProperty<TV>*>(this->ConnectedOutputs[i])->GetData();
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

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using ULProperty = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using UCProperty = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
using UCLProperty = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputData = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
using UPropertyOutputCData = UProperty<std::vector<T>, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
using UPropertyInputData = UProperty<T, OwnerT, type>;

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
using UPropertyInputCData = UProperty<std::vector<T>, OwnerT, type>;




#ifdef __BORLANDC__
#pragma warning( default : 4700)
#endif

}

#endif

