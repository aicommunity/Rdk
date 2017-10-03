//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "UProperty.h"
//---------------------------------------------------------------------------

namespace RDK {

/// Специализация: свойства - вектора
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
typename std::vector<V>::reference operator [] (size_t index)
{
 return this->Value[index];
}

const typename std::vector<V>::const_reference operator [] (size_t index) const
{
 return this->Value[index];
}

bool empty(void) const
{
 return Value.empty();
}

void resize(size_t size, const V &val)
{
 Value.resize(size,val);
}

void resize(size_t size)
{
 Value.resize(size);
}

void assign(size_t size, const V &val)
{
 Value.assign(size,val);
}

typename std::vector<V>::const_iterator begin(void) const
{
 return Value.begin();
}

typename std::vector<V>::iterator begin(void)
{
 return Value.begin();
}

typename std::vector<V>::const_iterator end(void) const
{
 return Value.end();
}

typename std::vector<V>::iterator end(void)
{
 return Value.end();
}

V& back(void)
{
 return Value.back();
}

V& front(void)
{
 return Value.front();
}

const V& back(void) const
{
 return Value.back();
}

const V& front(void) const
{
 return Value.front();
}

size_t size(void) const
{
 return Value.size();
}

/// Оператор присваивания
UProperty<std::vector<V>,OwnerT,type>& operator = (const std::vector<V> &value)
{
 this->SetData(value);
 return *this;
};

};

/// Специализация: свойства - вектора
template<typename V, typename OwnerT, unsigned int type>
class UProperty<std::list<V>,OwnerT,type>: public UPropertyLocal<std::list<V>,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<std::list<V>,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<std::list<V>,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag) //ipSingle | ipData
{
};

/// Оператор доступа по индексу
typename std::list<V>::reference operator [] (size_t index)
{
 return this->Value[index];
}

const typename std::list<V>::const_reference operator [] (size_t index) const
{
 return this->Value[index];
}

bool empty(void) const
{
 return Value.empty();
}

typename std::list<V>::const_iterator begin(void) const
{
 return Value.begin();
}

typename std::list<V>::iterator begin(void)
{
 return Value.begin();
}

typename std::list<V>::const_iterator end(void) const
{
 return Value.end();
}

typename std::list<V>::iterator end(void)
{
 return Value.end();
}

V& back(void)
{
 return Value.back();
}

V& front(void)
{
 return Value.front();
}

const V& back(void) const
{
 return Value.back();
}

const V& front(void) const
{
 return Value.front();
}

void push_back(const V &value)
{
 Value.push_back(value);
}

size_t size(void) const
{
 return Value.size();
}

/// Оператор присваивания
UProperty<std::list<V>,OwnerT,type>& operator = (const std::list<V> &value)
{
 this->SetData(value);
 return *this;
};

};

/// Специализация: свойства - MDMatrix
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
 return Value[index];
}

const V& operator [] (size_t index) const
{
 return Value[index];
}

V& operator () (int row, int col)
{
 return Value(row,col);
}

const V& operator () (int row, int col) const
{
 return Value(row,col);
}

/// Возвращает число элементов по всем размерностям
MMatrixSize GetMatrixSize(void) const
{
 return Value.GetMatrixSize();
}

void Resize(int rows, int cols)
{
 Value.Resize(rows,cols);
}

void Resize(const MMatrixSize &size)
{
 Value.Resize(size);
}

void Assign(int rows, int cols, const V &val)
{
 Value.Assign(rows,cols,val);
}

void Assign(int rows, int cols, const V *val)
{
 Value.Assign(rows,cols,val);
}

bool Assign(const MMatrixSize &size, const V *data)
{
 return Value.Assign(size,data);
}

bool Assign(const MMatrixSize &size, V value)
{
 return Value.Assign(size,value);
}

bool Assign(const MMatrixSize &size, const void *data)
{
 return Value.Assign(size,data);
}

int GetRows(void) const
{
 return Value.GetRows();
}

int GetCols(void) const
{
 return Value.GetCols();
}

void ToZero(void) const
{
 Value.ToZero();
}

/// Оператор присваивания
UProperty<MDMatrix<V>,OwnerT,type>& operator = (const MDMatrix<V> &value)
{
 this->SetData(value);
 return *this;
};

};

/// Специализация: свойства - MDVector
template<typename V, typename OwnerT, unsigned int type>
class UProperty<MDVector<V>,OwnerT,type>: public UPropertyLocal<MDVector<V>,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<MDVector<V>,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<MDVector<V>,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag) //ipSingle | ipData
{
};

/// Оператор доступа по индексу
V& operator [] (size_t index)
{
 return Value[index];
}

const V& operator [] (size_t index) const
{
 return Value[index];
}

V& operator () (int index)
{
 return Value(index);
}

const V& operator () (int index) const
{
 return Value(index);
}

/// Возвращает число элементов по всем размерностям
MMatrixSize GetMatrixSize(void) const
{
 return Value.GetMatrixSize();
}

void Resize(int size)
{
 Value.Resize(size);
}

void Resize(const MMatrixSize &size)
{
 Value.Resize(size);
}

void Assign(int size, const V &val)
{
 Value.Assign(size,val);
}

void Assign(int size, const V *val)
{
 Value.Assign(size,val);
}

bool Assign(const MMatrixSize &size, const V *data)
{
 return Value.Assign(size,data);
}

bool Assign(const MMatrixSize &size, V value)
{
 return Value.Assign(size,value);
}

bool Assign(const MMatrixSize &size, const void *data)
{
 return Value.Assign(size,data);
}

int GetSize(void) const
{
 return Value.GetSize();
}

void ToZero(void) const
{
 Value.ToZero();
}

/// Оператор присваивания
UProperty<MDVector<V>,OwnerT,type>& operator = (const MDVector<V> &value)
{
 this->SetData(value);
 return *this;
};

};

/// Специализация: свойства - double
template<typename OwnerT, unsigned int type>
class UProperty<double,OwnerT,type>: public UPropertyLocal<double,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<double,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<double,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator double (void) const
{ return Value; }

UProperty<double,OwnerT,type>& operator = (const double &value)
{
 this->SetData(value);
 return *this;
};

};

/// Специализация: свойства - int
template<typename OwnerT, unsigned int type>
class UProperty<int,OwnerT,type>: public UPropertyLocal<int,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<int,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<int,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator int (void) const
{ return Value; }

UProperty<int,OwnerT,type>& operator = (const int &value)
{
 this->SetData(value);
 return *this;
};

int operator ++ (void)
{
 if(SetterR)
  SetData(Value+1);
 else
  ++Value;
 return Value;
}

int operator ++ (int)
{
 int temp=Value;
 if(SetterR)
  SetData(Value+1);
 else
  Value++;
 return temp;
}

int operator -- (void)
{
 if(SetterR)
  SetData(Value-1);
 else
  ++Value;
 return Value;
}

int operator -- (int)
{
 int temp=Value;
 if(SetterR)
  SetData(Value-1);
 else
  Value--;
 return temp;
}

};

/// Специализация: свойства - int
template<typename OwnerT, unsigned int type>
class UProperty<unsigned int,OwnerT,type>: public UPropertyLocal<unsigned int,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<unsigned int,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<unsigned int,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator unsigned int (void) const
{ return Value; }

UProperty<unsigned int,OwnerT,type>& operator = (const unsigned int &value)
{
 this->SetData(value);
 return *this;
};

unsigned int operator ++ (void)
{
 if(SetterR)
  SetData(Value+1);
 else
  ++Value;
 return Value;
}

unsigned int operator ++ (int)
{
 int temp=Value;
 if(SetterR)
  SetData(Value+1);
 else
  Value++;
 return temp;
}

unsigned int operator -- (void)
{
 if(SetterR)
  SetData(Value-1);
 else
  ++Value;
 return Value;
}

unsigned int operator -- (int)
{
 unsigned int temp=Value;
 if(SetterR)
  SetData(Value-1);
 else
  Value--;
 return temp;
}

};

/// Специализация: свойства - bool
template<typename OwnerT, unsigned int type>
class UProperty<bool,OwnerT,type>: public UPropertyLocal<bool,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<bool,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<bool,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator bool (void) const
{ return Value; }

UProperty<bool,OwnerT,type>& operator = (const bool &value)
{
 this->SetData(value);
 return *this;
};

};

// Класс - свойство-контейнер со значением внутри
template<typename V, typename OwnerT, unsigned int type>
class UPropertyRange<V, std::list<V>,OwnerT, type>: public UPropertyRangeLocal<V,std::list<V>,OwnerT,type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<V,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<V,std::list<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};

UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyRangeLocal<V, std::list<V>, OwnerT, type>::VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<V,std::list<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};
// -----------------------------

// -----------------------------
// Методы доступа
// -----------------------------
void clear(void)
{
 this->Value.clear();
}

void erase(typename std::list<V>::iterator it)
{
 this->Value.erase(it);
}

size_t size(void) const
{
 return this->Value.size();
}

typename std::list<V>::const_iterator begin(void) const
{
 return Value.begin();
}

typename std::list<V>::iterator begin(void)
{
 return Value.begin();
}

typename std::list<V>::const_iterator end(void) const
{
 return Value.end();
}

typename std::list<V>::iterator end(void)
{
 return Value.end();
}

void push_back(const V &value)
{
 Value.push_back(value);
}

UPropertyRange<V,std::list<V>, OwnerT,type>& operator = (const std::list<V> &value)
{
 static_cast<UPropertyRangeLocal<V,std::list<V>, OwnerT,type>& >(*this)=value;
 return *this;
};

UPropertyRange<V,std::list<V>, OwnerT,type>& operator = (const UPropertyRange<V,std::list<V>, OwnerT,type> &v)
{
 static_cast<UPropertyRangeLocal<V,std::list<V>, OwnerT,type>& >(*this)=v.Value;
 return *this;
};
// -----------------------------
};


	   /*
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
                               /*
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

};             */
/*
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

};   */

}
#endif
