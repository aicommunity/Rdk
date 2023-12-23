#ifndef UOUTPUT_PROPERTY_H
#define UOUTPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UConnector.h"

namespace RDK {

// -----------------------------------------------------------------------------
// Output properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputBase: public UProperty<T,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputBase(const string &name, OwnerT * const owner, int input_type, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner,setmethod)
{
 this->IoType=input_type;
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
 return &this->GetData();
}

// Устанавливает указатель на данные выхода
bool SetPointer(int index, void* value, UIProperty* output)
{
 return true;
}

/// Обновить указатели свойств-входов
void UpdateConnectedPointers(void)
{
 UEPtr<UConnector> item=dynamic_cast<UConnector*>(this->Owner);
 size_t num_inputs=item->GetNumActiveOutputs(this->GetName());
 for(size_t i=0;i<num_inputs;i++)
 {
  UIProperty* property(0);
  item->FindConnectedProperty(this->GetName(), int(i), property);
  if(!property)
   continue;

  property->SetPointer(int(i), const_cast<void*>(GetPointer(0)), this);
 }
}
// --------------------------

// --------------------------
// Методы управления выходами
// --------------------------
/// Возвращает число подключенных входов
virtual size_t GetNumConnectors(void) const
{
 return UIPropertyOutput::GetNumConnectors();
}

/// Возвращает указатель на компонент-приемник
virtual UConnector* GetConnector(int index)
{
 return UIPropertyOutput::GetConnector(index);
}

/// Возвращает имя подключенного входа компонента-приемника
virtual std::string GetConnectorInputName(int index) const
{
 return UIPropertyOutput::GetConnectorInputName(index);
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
UPropertyOutputData(const string &name, OwnerT * const owner, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, ipSingle | ipData, setmethod)
{

};

/// Deprecated
UPropertyOutputData(const string &name, OwnerT * const owner, int index, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, ipSingle | ipData, setmethod)
{

};
// -----------------------------

/// Оператор присваивания
UPropertyOutputData<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

UPropertyOutputData<T,OwnerT, type>& operator = (const UPropertyOutputData<T,OwnerT, type> &v)
{
 this->SetData(v.GetData());
 return *this;
}
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCBase: public UCLProperty<std::vector<T>,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyOutputCBase(const string &name, OwnerT * const owner, int input_type, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UCLProperty<std::vector<T>,OwnerT,type>(name, owner, setmethod)
{
 this->IoType=input_type;
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
UVPropertyOutputBase(OwnerT * const owner, T* data, int input_type, typename UVProperty<T,OwnerT>::SetterRT setmethod=0)
 : UVProperty<T,OwnerT>(owner,data, setmethod)
{
 this->IoType=input_type;
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
bool SetPointer(int index, void* value, UIProperty* output)
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
};


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
