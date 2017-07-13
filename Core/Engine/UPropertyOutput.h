#ifndef UOUTPUT_PROPERTY_H
#define UOUTPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UItem.h"

namespace RDK {

// -----------------------------------------------------------------------------
// Output properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutput: virtual public ULProperty<T,OwnerT,type>, virtual public UIPropertyOutputBase
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutput(const string &name, OwnerT * const owner, int input_type=ipSingle | ipData, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : ULProperty<T,OwnerT,type>(name, owner, setmethod)
{
};
// -----------------------------

// --------------------------
// Методы управления указателем
// --------------------------
operator T* (void)
{
 return this->PData;
}
// --------------------------
// Методы управления указателем
// --------------------------
// Возвращает указатель на данные выхода
void const * GetPointer(int index) const
{
 return &this->v;
}
/*
// Устанавливает указатель на данные выхода
bool SetPointer(int index, void* value, UIProperty* output)
{
 return true;
} */
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputC: virtual protected UCLProperty<std::vector<T>,OwnerT,type>, virtual public UIPropertyOutputBase
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputC(const string &name, OwnerT * const owner, int input_type=ipRange | ipData, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UCLProperty<std::vector<T>,OwnerT,type>(name, owner, setmethod)
{
// this->IoType=input_type;
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
			/*
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputData: virtual public UPropertyOutputBase<T,OwnerT,type>
{
// --------------------------
// Конструкторы и деструкторы
// --------------------------
public: // Методы
//Конструктор инициализации.
UPropertyOutputData(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : ULProperty<T,OwnerT,type>(name, owner, setmethod),
   UPropertyOutputBase<T,OwnerT,type>(name, owner, ipSingle | ipData, setmethod)
{

};

/// Deprecated
UPropertyOutputData(const string &name, OwnerT * const owner, int index, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, ipSingle | ipData, setmethod)
{

};
// -----------------------------
};                 */
				/*
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCData: virtual public UPropertyOutputCBase<T,OwnerT,type>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputCData(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyOutputCBase<T,OwnerT,type>(name, owner, ipRange | ipData, setmethod)
{

};


/// Deprecated
UPropertyOutputCData(const string &name, OwnerT * const owner, int index, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyOutputCBase<T,OwnerT,type>(name, owner, ipRange | ipData, setmethod)
{

};
// -----------------------------
};                 */

// -----------------------------------------------------------------------------
// Output virtual properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT>
class UVPropertyOutput: virtual public UVProperty<T,OwnerT>, /*public UPropertyIOBase, */virtual public UIPropertyOutputBase
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVPropertyOutput(OwnerT * const owner, T* data, int input_type, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(owner,data, setmethod)
{
// this->IoType=input_type;
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
		   /*
// Устанавливает указатель на данные выхода
bool SetPointer(int index, void* value, UIProperty* output)
{
 return true;
}            */
// --------------------------
};
				/*
template<typename T, typename OwnerT>
class UVPropertyOutputData: virtual public UVPropertyOutputBase<T,OwnerT>
{
protected:

public: // Методы

// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UVPropertyOutputData(OwnerT * const owner, T* data, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVPropertyOutputBase<T,OwnerT>(owner, data, ipSingle | ipData, setmethod)
{

};

/// Deprecated
UVPropertyOutputData(OwnerT * const owner, T* data, int index, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVPropertyOutputBase<T,OwnerT>(owner, data, ipSingle | ipData, setmethod)
{

};
// -----------------------------
};         */


#define UPropertyOutputData UPropertyOutput
#define UPropertyOutputCData UPropertyOutputC
#define UVPropertyOutputData UVPropertyOutput

template<typename T, typename OwnerT, unsigned int type>
std::ostream& operator << (std::ostream &stream, UPropertyOutputData<T,OwnerT,type> &output)
{
 using namespace std;
 stream<<"Output "<<output.GetOwnerName()<<":"<<output.GetName();
 stream<<endl;
 stream<<"Data:"<<endl;
 stream<<*output;
 stream<<"--------------------";
 return stream;
}


}
#endif
