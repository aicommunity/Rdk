//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "UProperty.h"
//---------------------------------------------------------------------------

namespace RDK {

/// deprecated
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class ULProperty: public UProperty<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) //ipSingle | ipData
{
};

// Оператор присваивания
ULProperty<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};


};

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UPropertyParam: public UProperty<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyParam(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// Оператор присваивания
UPropertyParam<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubState>
class UPropertyState: public UProperty<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyState(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipData
{
};

// Оператор присваивания
UPropertyState<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputData: public UProperty<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// Оператор присваивания
UPropertyOutputData<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UProperty<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInput(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipComp
{
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UProperty<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipDatas
{
};

};

template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputDataRange: public UPropertyRange<T,RangeT,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputDataRange(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

/// deprecated
template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputDataC: public UPropertyRange<T,RangeT,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyInputDataC(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

}
#endif
