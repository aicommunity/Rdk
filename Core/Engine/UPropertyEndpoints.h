//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "UProperty.h"
//---------------------------------------------------------------------------

namespace RDK {

template<typename V, typename OwnerT, unsigned int type>
class UProperty<std::vector<V>,OwnerT,type>: public UPropertyLocal<std::vector<V>,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<std::vector<V>,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<std::vector<V>,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag) //ipSingle | ipData
{
};

/// Оператор доступа по индексу
V& operator [] (size_t index)
{
 return v[index];
}

const V& operator [] (size_t index) const
{
 return v[index];
}

void resize(size_t size)
{
 v.resize(size);
}

void assign(size_t size, const V &val)
{
 v.assign(size,val);
}

std::vector<V>::const_iterator begin(void) const
{
 return v.begin();
}

std::vector<V>::iterator begin(void)
{
 return v.begin();
}

std::vector<V>::const_iterator end(void) const
{
 return v.end();
}

std::vector<V>::iterator end(void)
{
 return v.end();
}

/// Оператор присваивания
UProperty<std::vector<V>,OwnerT,type>& operator = (const std::vector<V> &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename V, typename OwnerT, unsigned int type>
class UProperty<MDMatrix<V>,OwnerT,type>: public UPropertyLocal<MDMatrix<V>,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<MDMatrix<V>,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<MDMatrix<V>,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag) //ipSingle | ipData
{
};

/// Оператор доступа по индексу
V& operator [] (size_t index)
{
 return v[index];
}

const V& operator [] (size_t index) const
{
 return v[index];
}

V& operator () (int row, int col)
{
 return v(row,col);
}

const V& operator () (int row, int col) const
{
 return v(row,col);
}

void Resize(int rows, int cols)
{
 v.Resize(rows,cols);
}

void Assign(int rows, int cols, const V &val)
{
 v.Assign(rows,cols,val);
}

int GetRows(void) const
{
 return v.GetRows();
}

int GetCols(void) const
{
 return v.GetCols();
}

void ToZero(void) const
{
 v.ToZero();
}

/// Оператор присваивания
UProperty<MDMatrix<V>,OwnerT,type>& operator = (const MDMatrix<V> &value)
{
 this->SetData(value);
 return *this;
};

};

/// deprecated
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class ULProperty: public UPropertyLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod) //ipSingle | ipData
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
class UPropertyParam: public UPropertyLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyParam(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
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
UPropertyState(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
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
UPropertyOutputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
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
UPropertyInput(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
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
UPropertyInputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
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
UPropertyInputDataRange(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
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
UPropertyInputDataC(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

}
#endif
