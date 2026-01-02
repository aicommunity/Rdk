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
// Функции доступа к данным
// Модифицирует данные
// Возвращает данные из свойства
virtual const T& GetData(void) const=0;

// Устанавливает данные
virtual void SetData(const T& data)=0;

// Возвращает тип данных для свойства свойства
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}

// Сравнивает тип для этого свойства в другом свойстве
virtual bool CompareLanguageType(const UIProperty &dt) const
{
 return GetLanguageType() == dt.GetLanguageType();
}

// Возвращает тип данных для свойства элемента для массива элементов
virtual const type_info& GetElemLanguageType(void) const
{
 return typeid(T);
}

// Сравнивает тип для этого элемента в другом свойстве (для массива элементов)
virtual bool CompareElemLanguageType(const UIProperty &dt) const
{
 return GetElemLanguageType() == dt.GetElemLanguageType();
}

// Метод сохранения значения свойства в поток
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

// Метод возвращает указатель на область памяти, содержащую данные свойства
virtual const void* GetMemoryArea(void)
{
 return &GetData();
}

// Метод чтения значения свойства данных свойства из области памяти
// Читает данные из переданной области памяти для свойства свойства
// Возвращает успешность операции чтения и установки значения для свойства
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
// Функции работы со временем
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
// -----------------------------

// -----------------------------
// Функции времени обновления для свойства свойства
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
// Функции работы со временем обновления
// --------------------------
/// Обновляет время обновления свойства свойства
void RenewUpdateTime(void)
{
 UpdateTime=GetCurrentStartupTime();
}

/// Возвращает время обновления на ноль
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
// Указатель на владельца
OwnerT* Owner;

// --------------------------
UComponent::VariableMapCIteratorT Variable;

public: //Конструктор инициализации.
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализирует.
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
// Метод установки переменной
// -----------------------------
// Метод устанавливает значение свойства из итератора-хранилища данных на имя
// Устанавливает в соответствующее значение
virtual void SetVariable(UComponent::VariableMapCIteratorT &var)
{
 Variable=var;
}

// Метод возвращает указатель компонента-хранилища данных
virtual UContainer* GetOwner(void) const
{
 return dynamic_cast<UContainer*>(Owner);
}

// Метод возвращает указатель на имя свойства
virtual const std::string& GetName(void) const
{
 return Variable->first;
}

// Метод возвращает тип свойства
virtual unsigned int GetType(void) const
{
 return Variable->second.Type;
}

// Метод возвращает указатель на имя компонента-хранилища данных
virtual std::string GetOwnerName(void) const
{
 return (Owner)?Owner->GetName():std::string("");
}

// Метод возвращает указатель на имя класса-хранилища данных
virtual std::string GetOwnerClassName(void) const
{
 return typeid(Owner).name();
}
// Не содержит данного внутри себя
};

// Метод - дружественный класс
//friend class OwnerT;
template<typename T,class OwnerT>
class UVProperty: public UVBaseProperty<T, OwnerT>
{
//friend class OwnerT;
public: // Данные
typedef const T& (OwnerT::*GetterRT)(void) const;
typedef bool (OwnerT::*SetterRT)(const T&);

protected: // Указатель
// Указатель геттер-указатель
GetterRT GetterR;
SetterRT SetterR;

protected:
// --------------------------
UVBaseDataProperty<T>* ExternalDataSource;

protected:
/*Getter(0), Setter(0), */
bool IsConnectedFlag;

/// Указатель на подключенные выходы
std::vector<UIPropertyOutput*> ConnectedOutputs;

/*Getter(0), Setter(0), */
mutable UVBaseDataProperty<T>* CachedConnectedOutput;

protected: // Local storage (from UPropertyLocal)
// -----------------------------
bool CheckEqualsFlag;

// Привязка внешней ссылки как источника данных
mutable T v;

public: // Указатель
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Конструктор для свойств без регистрации в компоненте
UVProperty(OwnerT * const owner, SetterRT setmethod , GetterRT getmethod) :
  UVBaseProperty<T, OwnerT>(owner), GetterR(getmethod), SetterR(setmethod), ExternalDataSource(0),
  IsConnectedFlag(false), CachedConnectedOutput(0), CheckEqualsFlag(true), v()
{
}

// Конструктор для свойств с внешними данными
UVProperty(OwnerT * const owner, T * const pdata, SetterRT setmethod=0) :
  UVBaseProperty<T, OwnerT>(owner, pdata), GetterR(0), SetterR(setmethod), ExternalDataSource(0),
  IsConnectedFlag(false), CachedConnectedOutput(0), CheckEqualsFlag(true), v()
{
}

// Конструктор для свойств с регистрацией в компоненте (объединенная функциональность UPropertyLocal)
UVProperty(const string &name, OwnerT * const owner, unsigned int type, SetterRT setmethod=0) :
  UVBaseProperty<T, OwnerT>(owner), GetterR(0), SetterR(setmethod), ExternalDataSource(0),
  IsConnectedFlag(false), CachedConnectedOutput(0), CheckEqualsFlag(true), v()
{
 dynamic_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false);
}
// -----------------------------

// -----------------------------
//  
// -----------------------------
///  
virtual UItem* GetItem(int index=0);

/// Возвращает имя для подключенного выхода
virtual std::string GetItemOutputName(int index=0) const;

/// Возвращает имя для подключенного компонента
virtual std::string GetItemName(int index=0) const;

/// Возвращает строку для подключенного компонента
virtual std::string GetItemFullName(int index=0) const;

/// Обновляет время выхода в лог (optimized)
inline void ApplyOutputUpdateTime(void) const
{
 // Lazy update: only update if connected output's time is newer
 if (IsConnectedFlag && !ConnectedOutputs.empty())
 {
  ULongTime outputTime = ConnectedOutputs[0]->GetUpdateTime();
  if (outputTime > this->UpdateTime)
   this->UpdateTime = outputTime;
 }
}

/* ************************************************************************* */
bool IsConnected(void) const
{
 return IsConnectedFlag;
}

/// Возвращает true, если в подключенном выходе есть новые данные
virtual bool IsNewData(void) const
{
 return (!ConnectedOutputs.empty())?this->ConnectedOutputs[0]->GetUpdateTime()>this->UpdateTime:true;
}
//protected:


// -----------------------------
// Функции работы
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

// Оператор присваивания
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
/// Get data implementation (optimized with fast-path)
inline const T& GetData(void) const
{
 // Fast path for unconnected properties (most common case) - ~99% of calls
#ifndef _MSC_VER
 if ([[likely]] (!IsConnectedFlag && !this->ExternalDataSource))
#else
 if (!IsConnectedFlag && !this->ExternalDataSource)
#endif
  return v;

#ifndef _MSC_VER
 if ([[unlikely]] (this->ExternalDataSource))
#else
 if (this->ExternalDataSource)
#endif
  return this->ExternalDataSource->GetData();

#ifndef _MSC_VER
 if ([[unlikely]] (IsConnectedFlag))
#else
 if (IsConnectedFlag)
#endif
 {
  // Cache typed pointer on first access
  if (!CachedConnectedOutput && !ConnectedOutputs.empty())
   CachedConnectedOutput = dynamic_cast<UVBaseDataProperty<T>*>(ConnectedOutputs[0]);
  
  if (CachedConnectedOutput)
  {
   // Cache data with update time check to avoid unnecessary copies
   ULongTime outputTime = CachedConnectedOutput->GetUpdateTime();
   if (outputTime > this->UpdateTime)
   {
    v = CachedConnectedOutput->GetData();
    this->UpdateTime = outputTime;
   }
  }
 }

 return v;
}

// Устанавливает указатель на данные входа (optimized with early exits)
virtual void SetData(const T &value)
{
 if (this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if (IsConnectedFlag)
  return;

 if (CheckEqualsFlag && value == v)
  return;

 if (this->Owner)
 {
  if (this->SetterR && !(this->Owner->*(this->SetterR))(value))
   throw UIProperty::EPropertySetterFail(this->GetOwnerName(),this->GetName());
 }

 v=value;
 this->RenewUpdateTime();
 return;
}

// Устанавливает значение напрямую, минуя сеттер (для использования в сеттерах для предотвращения рекурсии)
public:
void SetDataDirect(const T &value)
{
 if (this->ExternalDataSource)
 {
  this->ExternalDataSource->SetData(value);
  return;
 }

 if (IsConnectedFlag)
  return;

 if (CheckEqualsFlag && value == v)
  return;

 v=value;
 this->RenewUpdateTime();
 return;
}

public:
// -----------------------------
// Функции работы с подключенными выходами
// -----------------------------
bool AttachTo(UVBaseDataProperty<T>* prop)
{
 bool res=UVBaseDataProperty<T>::AttachTo(prop);
 if(res)
 {
  IsConnectedFlag=true;
 }
 return res;
}

void DetachFrom(void)
{
 IsConnectedFlag=false;
 CachedConnectedOutput = 0; // Clear cache
 UVBaseDataProperty<T>::DetachFrom();
}

// Метод возвращает число указателей
int GetNumPointers(void) const
{
 return int(ConnectedOutputs.size());
}

// Устанавливает указатель на указатель выход
bool SetPointer(int index, UIPropertyOutput* property)
{
 IsConnectedFlag=true;
 // Cache typed pointer for optimization
 CachedConnectedOutput = dynamic_cast<UVBaseDataProperty<T>*>(property);
 ConnectedOutputs.assign(1,property);
 this->ResetUpdateTime();
 return true;
}

/// Возвращает указатель на указатель
bool ResetPointer(int index, UIPropertyOutput* property)
{
 if(!ConnectedOutputs.empty() && ConnectedOutputs[0] == property)
 {
  IsConnectedFlag=false;
  CachedConnectedOutput = 0; // Clear cache
  ConnectedOutputs.clear();
  return true;
 }
 return false;
}
// -----------------------------

/* ************************************************************************* */
};

class UItem;
class UConnector;
class UContainer;

/* ************************************************************************* */


/// Метод чтения значения свойства из подключенного выхода
/// https://stackoverflow.com/questions/60608588/specializing-a-template-for-a-container-of-type-t
template<typename T,class OwnerT, unsigned int type, bool = is_iterable<T>::value>
class UProperty;


template<typename T, typename OwnerT, unsigned int type>
class UProperty<T, OwnerT, type, false>: public UVProperty<T,OwnerT>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализирует
UProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
    : UVProperty<T,OwnerT>(name, owner, type, setmethod)
{ }

protected:
UProperty(const UProperty<T,OwnerT,type> &v) {}
// -----------------------------

// -----------------------------
// Выход за границы массива C (container) property
// -----------------------------
public:
// Оператор присваивания
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
// Метод - свойство-контейнер на подключенном выходе
/* ************************************************************************* */
template<typename T, typename OwnerT, unsigned int type>
class UProperty<T, OwnerT, type, true>: public UVProperty<T,OwnerT>
{
public: // Если свойство вектор-указатель
typedef typename T::value_type TV;
typedef bool (OwnerT::*VSetterRT)(const TV&);

protected: // Указатель
// Указатель вектор-указатель
VSetterRT VSetterR;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
public:
UProperty(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(name, owner, type, setmethod), VSetterR(0)
{
 this->IoType = static_cast<unsigned int>(ipRange) | static_cast<unsigned int>(ipData);
}

UProperty(const string &name, OwnerT * const owner, typename UProperty<T,OwnerT,type>::VSetterRT setmethod)
 : UVProperty<T,OwnerT>(name, owner, type, (typename UVProperty<T,OwnerT>::SetterRT)0), VSetterR(setmethod)
{
 this->IoType = static_cast<unsigned int>(ipRange) | static_cast<unsigned int>(ipData);
}
// -----------------------------

// -----------------------------
// Функции доступа
// -----------------------------
// Функции доступа
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
// Устанавливает указатель на указатель выход
bool SetPointer(int index, UIPropertyOutput* property)
{
 if(index<0)
  return false;

 if(int(this->ConnectedOutputs.size())<=index)
 {
  size_t new_size=index+1;
  if(has_resize<T>::value)
   this->v.resize(new_size);
  else
   throw std::runtime_error("resize doesn't support");
  this->ConnectedOutputs.resize(new_size,0);
 }
 this->ConnectedOutputs[index]=property;
 this->IsConnectedFlag=true;
 return true;
}

/// Возвращает указатель на указатель
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
   this->IsConnectedFlag=false;
  return true;
 }

 return false;
}
// -----------------------------

public: // Ошибка

// Метод для ошибки ошибки C (container) property
struct EPropertyRangeError: public UIProperty::EPropertyError
{
int MinValue, MaxValue, ErrorValue;
public:
EPropertyRangeError(const std::string &owner_name, const std::string &property_name, int min_value, int max_value, int error_value)
 : UIProperty::EPropertyError(owner_name, property_name),
   MinValue(min_value), MaxValue(max_value), ErrorValue(error_value) {}


// Создает строку сообщения для ошибки
virtual std::string CreateLogMessage(void) const
{
 return UIProperty::EPropertyError::CreateLogMessage()+std::string(" MinValue=")+
        sntoa(MinValue)+std::string(" MaxValue=")+sntoa(MaxValue)+
        std::string(" ErrorValue=")+sntoa(ErrorValue);
}
};

public:
// -----------------------------
// Функции доступа
// -----------------------------
// Возвращает значение элемента массива
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

// Устанавливает значение элемента массива
bool operator () (size_t i, const typename UProperty<T, OwnerT, type, true>::TV &value)
{
 if(VSetterR && !(this->Owner->*VSetterR)(value))
  throw UIProperty::EPropertySetterFail(this->GetOwnerName(),this->GetName());

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

// Оператор присваивания
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
// Функции работы со временем
// --------------------------
// Метод сравнивает тип для этого элемента в другом свойстве (для массива элементов)
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
 // Оператор присваивания
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

