#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

class UPropertyInputBase
{
protected:
// Тип входа
int InputType;

// Диапазон индексов входов
int MinRange, MaxRange;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputBase(int input_type=ipDataSingle, int min_range=0, int max_range=-1)
 : InputType(input_type), MinRange(min_range), MaxRange(max_range)
{ };
// -----------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UVBaseLProperty<T,OwnerT,type>, public UPropertyInputBase, public UIInputProperty
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInput(const string &name, OwnerT * const owner, int input_type=ipSingle, int min_range=0, int max_range=-1)
 : UVBaseLProperty<T,OwnerT,type>(name, owner, (T * const)0), UPropertyInputBase(input_type | ipComp, min_range, max_range)
{ };
// -----------------------------

// -----------------------------
// Методы управления параметрами
// -----------------------------
// Тип входа
virtual int GetInputType(void) const
{
 return InputType;
}

virtual bool CheckRange(int index)
{
 return ((InputType == ipDataSingle || InputType == ipCompSingle) && MinRange == index);
}
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает указатель на данные входа
void const * GetPointer(void) const
{
 return this->PData;
}

// Устанавливает указатель на данные входа
bool SetPointer(void* value)
{
 this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 if(!this->PData)
  return 0;
//  throw EUsingZeroPtr();


 return this->PData;
};

T& operator * (void)
{
// if(!PData)
  //throw EUsingZeroPtr();

 return *this->PData;
};

operator T* (void) const
{
 return this->PData;
}

// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}

// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UVLProperty<T,OwnerT,type>, public UPropertyInputBase, public UIInputProperty
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputData(const string &name, OwnerT * const owner, int input_type=ipSingle, int min_range=0, int max_range=-1)
 : UVLProperty<T,OwnerT,type>(name, owner, (T * const)0), UPropertyInputBase(input_type | ipData, min_range, max_range)
{ };
// -----------------------------

// -----------------------------
// Методы управления параметрами
// -----------------------------
// Тип входа
virtual int GetInputType(void) const
{
 return InputType;
}

virtual bool CheckRange(int index)
{
 return ((InputType == ipDataSingle || InputType == ipCompSingle) && MinRange == index);
}
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает указатель на данные входа
void const * GetPointer(void) const
{
 return this->PData;
}

// Устанавливает указатель на данные входа
bool SetPointer(void* value)
{
 this->PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (this->PData)?false:true; };

T* operator -> (void) const
{
 if(!this->PData)
  return 0;
//  throw EUsingZeroPtr();


 return this->PData;
};

T& operator * (void)
{
// if(!PData)
  //throw EUsingZeroPtr();

 return *this->PData;
};

operator T* (void) const
{
 return this->PData;
}
// --------------------------
};


}
#endif
