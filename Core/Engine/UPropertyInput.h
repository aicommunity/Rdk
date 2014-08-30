#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputBase: protected ULProperty<T*,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// Временная переменная, использующаяся, если нет реального подключения
mutable T Local;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : ULProperty<T*,OwnerT,type>(name, owner)
{
 UVBaseDataProperty<T*>::IoType=input_type;
 UVBaseDataProperty<T*>::MinRange=min_range;
 UVBaseDataProperty<T*>::MaxRange=max_range;
};
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает языковой тип хранимого свойства
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}// --------------------------
};

template<typename T, typename OwnerT>
class UVPropertyInputBase: public UVProperty<T*,OwnerT>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// Временная переменная, использующаяся, если нет реального подключения
mutable T Local;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVPropertyInputBase(OwnerT * const owner, T* data, int min_range, int input_type, int max_range=-1)
 : UVProperty<T*,OwnerT>(owner, &data)
{
 UVBaseDataProperty<T*>::IoType=input_type;
 UVBaseDataProperty<T*>::MinRange=min_range;
 UVBaseDataProperty<T*>::MaxRange=max_range;
};
// -----------------------------


// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает языковой тип хранимого свойства
virtual const type_info& GetLanguageType(void) const
{
 return typeid(T);
}// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UPropertyInputBase<T,OwnerT,type>
{
protected:
//mutable T Local;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInput(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle | ipComp, int max_range=-1)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, min_range, input_type | ipComp, max_range)
{ };
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает true если вход имеет подключение
bool IsConnected(void) const
{
 return (this->v)?true:false;
}

// Возвращает указатель на данные входа
void const * GetPointer(int index) const
{
 return this->v;
}

// Устанавливает указатель на данные входа
bool SetPointer(int index, void* value)
{
 this->v=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->v)?false:true; };

T* operator -> (void) const
{
 return (this->v)?this->v:&Local;
};

T& operator * (void)
{
 return (this->v)?*this->v:Local;
};

operator T* (void) const
{
 return (this->v)?this->v:&Local;
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UPropertyInputBase<T,OwnerT,type>
{
protected:
//mutable T Local;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputData(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle | ipComp, int max_range=-1)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{ };
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает true если вход имеет подключение
bool IsConnected(void) const
{
 return (this->v)?true:false;
}

// Возвращает указатель на данные входа
void const * GetPointer(int index) const
{
 return this->v;
}

// Устанавливает указатель на данные входа
bool SetPointer(int index, void* value)
{
 this->v=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->v)?false:true; };

T* operator -> (void) const
{
 return (this->v)?this->v:&Local;
};

T& operator * (void)
{
 return (this->v)?*this->v:Local;
};

operator T* (void) const
{
 return (this->v)?this->v:&Local;
}
// --------------------------
/*
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputData(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle, int max_range=-1)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{ };
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает true если вход имеет подключение
bool IsConnected(void) const
{
 return (this->PData)?true:false;
}

// Возвращает указатель на данные входа
void const * GetPointer(int index) const
{
 return this->PData;
}

// Возврат значения
virtual const T& GetData(void) const
{
 return (this->PData)?*this->PData:this->v;
};

virtual void SetData(const T &value)
{
 if(this->PData)
 {
  if(*this->PData == value)
   return;

  if(this->Owner)
  {
   if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
	throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

   *this->PData=value;
   return;
  }

  *this->PData=value;
 }
 else
  ULProperty<T,OwnerT,type>::SetData(value);
};

// Устанавливает указатель на данные входа
bool SetPointer(int index, void* value)
{
 this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 return (this->PData)?this->PData:&this->v;
};

T& operator * (void)
{
 return (this->PData)?*this->PData:this->v;
};

operator T* (void) const
{
 return (this->PData)?this->PData:&this->v;
}
// --------------------------
*/
};


template<typename T, typename OwnerT>
class UVPropertyInputData: public UVPropertyInputBase<T,OwnerT>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVPropertyInputData(OwnerT * const owner, T *data, int min_range, int input_type=ipSingle, int max_range=-1)
 : UVPropertyInputBase<T,OwnerT>(owner, data, min_range, input_type | ipData, max_range)
{ };
// -----------------------------


// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает true если вход имеет подключение
bool IsConnected(void) const
{
 return (*this->PData)?true:false;
}

// Возвращает указатель на данные входа
void const * GetPointer(int index) const
{
 return *this->PData;
}

// Устанавливает указатель на данные входа
bool SetPointer(int index, void* value)
{
 *this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (*this->PData)?false:true; };

T* operator -> (void) const
{
 return (*this->PData)?*this->PData:&Local;
};

T& operator * (void)
{
 return (*this->PData)?**this->PData:Local;
};

operator T* (void) const
{
 return (*this->PData)?*this->PData:&Local;
}
// --------------------------
/*
// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает true если вход имеет подключение
bool IsConnected(void) const
{
 return (this->PData)?true:false;
}

// Возвращает указатель на данные входа
void const * GetPointer(int index) const
{
 return this->PData;
}

// Возврат значения
virtual const T& GetData(void) const
{
 return (this->PData)?*this->PData:throw(1); // TODO
};

virtual void SetData(const T &value)
{
 if(this->PData)
 {
  if(*this->PData == value)
   return;

  if(this->Owner)
  {
   if(this->SetterR && !(this->Owner->*(this->SetterR))(value))
	throw UIProperty::EPropertySetterFail(UVBaseProperty<T,OwnerT>::GetOwnerName(),UVBaseProperty<T,OwnerT>::GetName());

   *this->PData=value;
   return;
  }

  *this->PData=value;
 }
 else
  UVProperty<T,OwnerT>::SetData(value);
};

// Устанавливает указатель на данные входа
bool SetPointer(int index, void* value)
{
 this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 return (this->PData)?this->PData:throw(1); // TODO
};

T& operator * (void)
{
 return (this->PData)?*this->PData:throw(1); // TODO
};

operator T* (void) const
{
 return (this->PData)?this->PData:throw(1); // TODO
}
// --------------------------
*/
};
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCBase: public UCLProperty<std::vector<T*>,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyInput
{
protected:
/// Временная переменная, использующаяся, если нет реального подключения
std::vector<T> Local;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputCBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : UCLProperty<std::vector<T*>,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает указатель на данные входа
void const * GetPointer(int index) const
{
 if(int(this->v.size())<=index-this->MinRange)
 #ifdef __BORLANDC__
//  return 0;
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),index);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),index);
 #endif
 return this->v[index-this->MinRange];
}

// Устанавливает указатель на данные входа
bool SetPointer(int index, void* value)
{
 if(int(this->v.size())<=index-this->MinRange)
  this->v.resize(index-this->MinRange+1);
 this->v[index-this->MinRange]=reinterpret_cast<T*>(value);
 return true;
}

T* operator [] (int i)
{
 if(int(this->v.size())<=i-this->MinRange)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(this->GetOwnerName(),this->GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #endif
// throw EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
//	this->MinRange,int(this->v.size()+this->MinRange),i);

 return (this->v[i])?this->v[i]:&Local[i];
}

const T* operator [] (int i) const
{
 if(int(this->v.size())<=i-this->MinRange)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #else
  throw typename UCLProperty<std::vector<T*>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	this->MinRange,int(this->v.size()+this->MinRange),i);
 #endif


 return (this->v[i])?this->v[i]:&Local[i];
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputC: public UPropertyInputCBase<T,OwnerT,type>
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputC(const string &name, OwnerT * const owner, int min_range, int input_type=ipRange | ipComp, int max_range=-1)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipComp, max_range)
{ };
// -----------------------------

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


virtual void SetData(const std::vector<T*> &value)
{
 return;
};


};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputCData: public UPropertyInputCBase<T,OwnerT,type>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputCData(const string &name, OwnerT * const owner, int min_range, int input_type=ipRange, int max_range=-1)
 : UPropertyInputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{ };
// -----------------------------
};


}
#endif
