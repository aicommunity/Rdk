#ifndef UINPUT_PROPERTY_H
#define UINPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

class UPropertyIOBase: public UIPropertyIO
{
protected:
// Тип входа
int Type;

// Диапазон индексов входов
int MinRange, MaxRange;

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyIOBase(int input_type=ipDataSingle, int min_range=0, int max_range=-1)
 : Type(input_type), MinRange(min_range), MaxRange(max_range)
{ };
// -----------------------------

// -----------------------------
// Методы управления параметрами
// -----------------------------
// Тип
virtual int GetType(void) const
{
 return Type;
}

virtual bool CheckRange(int index)
{
 return (MinRange == index && MaxRange<0) | (index>=MinRange && index<=MaxRange);
 //((Type == ipDataSingle || Type == ipCompSingle) && MinRange == index);
}

// Диапазон индексов входов
int GetMinRange(void)
{ return MinRange; };

int GetMaxRange(void)
{ return MaxRange; };
// -----------------------------

};

template<typename T, typename OwnerT, unsigned int type>
class UPropertyInputBase: public UVBaseLProperty<T,OwnerT,type>, public UPropertyIOBase
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputBase(const string &name, OwnerT * const owner, int input_type, int min_range=0, int max_range=-1)
 : UVBaseLProperty<T,OwnerT,type>(name, owner, (T * const)0), UPropertyIOBase(input_type, min_range, max_range)
{ };
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

 return this->PData;
};

T& operator * (void)
{
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
class UPropertyInput: public UPropertyInputBase<T,OwnerT,type>
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInput(const string &name, OwnerT * const owner, /*int input_type=ipSingle | ipComp, */int min_range, int max_range=-1)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, /*input_type | */ipComp, min_range, max_range)
{ };
// -----------------------------


virtual void Init(void)
{

}

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UPropertyInputBase<T,OwnerT,type>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputData(const string &name, OwnerT * const owner, /*int input_type=ipSingle, */int min_range, int max_range=-1)
 : UPropertyInputBase<T,OwnerT,type>(name, owner, /*input_type | */ipData, min_range, max_range)
{ };
// -----------------------------


virtual void Init(void)
{
 if(Owner && MinRange>=0)
 {
  Owner->SetInputDataInfo(MinRange,new UDataInfo<T>);
 }
}

};

template<typename T, typename OwnerT, unsigned int type>
class UPropertyOutputBase: public ULProperty<T,OwnerT,type>, public UPropertyIOBase
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputBase(const string &name, OwnerT * const owner, int input_type, int min_range=0, int max_range=-1)
 : ULProperty<T,OwnerT,type>(name, owner), UPropertyIOBase(input_type, min_range, max_range)
{ };
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

 return this->PData;
};

T& operator * (void)
{
 return *this->PData;
};

operator T* (void) const
{
 return this->PData;
}
	   /*
// Метод записывает значение свойства в поток
virtual bool Save(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}

// Метод читает значение свойства из потока
virtual bool Load(UEPtr<UVariableData> storage, bool simplemode=false)
{
 return true;
}        */
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputData: public UPropertyOutputBase<T,OwnerT,type>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputData(const string &name, OwnerT * const owner, /*int input_type=ipSingle, */int min_range, int max_range=-1)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, /*input_type | */ipData, min_range, max_range)
{

};
// -----------------------------

virtual void Init(void)
{
 if(Owner && MinRange>=0)
 {
  Owner->SetOutputDataAsPointer(MinRange,&this->v);
  Owner->SetOutputDataInfo(MinRange,new UDataInfo<T>);
 }
};

};


}
#endif
