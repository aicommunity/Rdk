#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

enum UInputPropertyTypes { ipSingle=1, ipRange=2, ipList=3 };

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UInputProperty: public UVLProperty<T,OwnerT,type>, public UIInputProperty
{
protected:
// Тип входа
UInputPropertyTypes InputType;

// Диапазон индексов входов
int MinRange, MaxRange;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UInputProperty(const string &name, OwnerT * const owner, UInputPropertyTypes input_type=ipSingle, int min_range=0, int max_range=-1)
 : UVLProperty<T,OwnerT,type>(name, owner, (T * const)0), InputType(input_type), MinRange(min_range), MaxRange(max_range)
{ };
// -----------------------------

// -----------------------------
// Методы управления параметрами
// -----------------------------
// Тип входа
UInputPropertyTypes GetInputType(void) const
{
 return InputType;
}

bool CheckRange(int index)
{
 if(InputType == ipSingle && MinRange == index)
  return true;
 return false;
}
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает указатель на данные входа
void const * GetPointer(void) const
{
 return PData;
}

// Устанавливает указатель на данные входа
bool SetPointer(void* value)
{
 PData=reinterpret_cast<T*>(value);
 return true;
}

bool operator ! (void) const
{ return (PData)?false:true; };

T* operator -> (void) const
{
 if(!PData)
  return 0;
//  throw EUsingZeroPtr();


 return PData;
};

T& operator * (void)
{
// if(!PData)
  //throw EUsingZeroPtr();

 return *PData;
};

operator T* (void) const
{
 return PData;
}

// --------------------------
};


}
#endif
