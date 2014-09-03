#ifndef UOUTPUT_PROPERTY_H
#define UOUTPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

// -----------------------------------------------------------------------------
// Output properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputBase: public ULProperty<T,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : ULProperty<T,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
operator T* (void)
{
 return this->PData;
}

/*bool operator ! (void) const
{
 return (GetPointer(0))?true:false;
};

T* operator -> (void)
{
 return &this->v;
};

T& operator * (void)
{
 return this->v;
};

operator T* (void)
{
 return &this->v;
}

T& Value(void)
{
 return this->v;
}

const T& Value(void) const
{
 return this->v;
}

T& operator [] (int i)
{
 return this->v;
}

const T& operator [] (int i) const
{
 return this->v;
}
       */
// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает указатель на данные выхода
void const * GetPointer(int index) const
{
 return &this->v;
}

// Устанавливает указатель на данные выхода
bool SetPointer(int index, void* value)
{
 return true;
}
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
UPropertyOutputData(const string &name, OwnerT * const owner, int min_range, int input_type=ipSingle, int max_range=-1)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{

};
// -----------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCBase: protected UCLProperty<std::vector<T>,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputCBase(const string &name, OwnerT * const owner, int min_range, int input_type, int max_range=-1)
 : UCLProperty<std::vector<T>,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
T& Value(int i)
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

const T& Value(int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

T& operator [] (int i)
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

const T& operator [] (int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCData: public UPropertyOutputCBase<T,OwnerT,type>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputCData(const string &name, OwnerT * const owner, int min_range, int input_type=ipRange, int max_range=-1)
 : UPropertyOutputCBase<T,OwnerT,type>(name, owner, min_range, input_type | ipData, max_range)
{

};
// -----------------------------
};

// -----------------------------------------------------------------------------
// Output virtual properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT>
class UVPropertyOutputBase: public UVProperty<T,OwnerT>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVPropertyOutputBase(OwnerT * const owner, T* data, int min_range, int input_type, int max_range=-1)
 : UVProperty<T,OwnerT>(owner,data)
{
 this->IoType=input_type;
 this->MinRange=min_range;
 this->MaxRange=max_range;
};
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------

// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает указатель на данные выхода
void const * GetPointer(int index) const
{
 return this->PData;
}

// Устанавливает указатель на данные выхода
bool SetPointer(int index, void* value)
{
 return true;
}
// --------------------------
};

template<typename T, typename OwnerT>
class UVPropertyOutputData: public UVPropertyOutputBase<T,OwnerT>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVPropertyOutputData(OwnerT * const owner, T* data, int min_range, int input_type=ipSingle, int max_range=-1)
 : UVPropertyOutputBase<T,OwnerT>(owner, data, min_range, input_type | ipData, max_range)
{

};
// -----------------------------
};

}
#endif
