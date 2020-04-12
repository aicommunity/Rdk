//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "ULocalProperty.h"
#include <vector>
//---------------------------------------------------------------------------

namespace RDK {

/// Специализация: свойства - вектора
template<typename V, typename OwnerT, unsigned int type>
class ULProperty<std::vector<V>, OwnerT, type>: public UProperty<std::vector<V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<std::vector<V>,OwnerT>::SetterRT setmethod=0)
 : UProperty<std::vector<V>,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<std::vector<V>,OwnerT,type> &v) {}

public:

/// Оператор доступа по индексу
typename std::vector<V>::reference operator [] (size_t index)
{
 return this->v[index];
}

const typename std::vector<V>::const_reference operator [] (size_t index) const
{
 return this->v[index];
}

bool empty(void) const
{
 return this->v.empty();
}

void resize(size_t size, const V &val)
{
 this->v.resize(size,val);
}

void resize(size_t size)
{
 this->v.resize(size);
}

void assign(size_t size, const V &val)
{
 this->v.assign(size,val);
}

typename std::vector<V>::const_iterator begin(void) const
{
 return this->v.begin();
}

typename std::vector<V>::iterator begin(void)
{
 return this->v.begin();
}

typename std::vector<V>::const_iterator end(void) const
{
 return this->v.end();
}

typename std::vector<V>::iterator end(void)
{
 return this->v.end();
}

V& back(void)
{
 return this->v.back();
}

V& front(void)
{
 return this->v.front();
}

const V& back(void) const
{
 return this->v.back();
}

const V& front(void) const
{
 return this->v.front();
}

size_t size(void) const
{
 return this->v.size();
}

ULProperty<std::vector<V>,OwnerT,type>& operator = (const std::vector<V> &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<std::vector<V>,OwnerT, type>& operator = (const ULProperty<std::vector<V>,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

operator std::vector<V> (void) const
{
 return this->GetData();
}

const std::vector<V>& operator () (void) const
{
 return this->GetData();
}


};

/// Специализация: свойства - список
template<typename V, typename OwnerT, unsigned int type>
class ULProperty<std::list<V>,OwnerT,type>: public UProperty<std::list<V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<std::list<V>,OwnerT>::SetterRT setmethod=0)
 : UProperty<std::list<V>,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<std::list<V>,OwnerT,type> &v) {}

public:

/// Оператор доступа по индексу
typename std::list<V>::reference operator [] (size_t index)
{
 return this->v[index];
}

const typename std::list<V>::const_reference operator [] (size_t index) const
{
 return this->v[index];
}

bool empty(void) const
{
 return this->v.empty();
}

typename std::list<V>::const_iterator begin(void) const
{
 return this->v.begin();
}

typename std::list<V>::iterator begin(void)
{
 return this->v.begin();
}

typename std::list<V>::const_iterator end(void) const
{
 return this->v.end();
}

typename std::list<V>::iterator end(void)
{
 return this->v.end();
}

V& back(void)
{
 return this->v.back();
}

V& front(void)
{
 return this->v.front();
}

const V& back(void) const
{
 return this->v.back();
}

const V& front(void) const
{
 return this->v.front();
}

void push_back(const V &value)
{
 this->v.push_back(value);
}

size_t size(void) const
{
 return this->v.size();
}

/// Оператор присваивания
ULProperty<std::list<V>,OwnerT,type>& operator = (const std::list<V> &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<std::list<V>,OwnerT, type>& operator = (const ULProperty<std::list<V>,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

operator std::list<V> (void) const
{
 return this->GetData();
}

const std::list<V>& operator () (void) const
{
 return this->GetData();
}


};

/// Специализация: свойства - map
template<typename T, typename V, typename OwnerT, unsigned int type>
class ULProperty<std::map<T,V>,OwnerT,type>: public UProperty<std::map<T,V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<std::map<T,V>,OwnerT>::SetterRT setmethod=0)
 : UProperty<std::map<T,V>,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<std::map<T,V>,OwnerT,type> &v) {}

public:

/// Оператор доступа по индексу
typename std::map<T,V>::reference operator [] (T index)
{
 return this->v[index];
}

const typename std::map<T,V>::const_reference operator [] (T index) const
{
 return this->v[index];
}

bool empty(void) const
{
 return this->v.empty();
}

typename std::map<T,V>::const_iterator begin(void) const
{
 return this->v.begin();
}

typename std::map<T,V>::iterator begin(void)
{
 return this->v.begin();
}

typename std::map<T,V>::const_iterator end(void) const
{
 return this->v.end();
}

typename std::map<T,V>::iterator end(void)
{
 return this->v.end();
}

V& back(void)
{
 return this->v.back();
}

V& front(void)
{
 return this->v.front();
}

const V& back(void) const
{
 return this->v.back();
}

const V& front(void) const
{
 return this->v.front();
}

void push_back(const V &value)
{
 this->v.push_back(value);
}

size_t size(void) const
{
 return this->v.size();
}

/// Оператор присваивания
ULProperty<std::map<T,V>,OwnerT,type>& operator = (const std::map<T,V> &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<std::map<T,V>,OwnerT, type>& operator = (const ULProperty<std::map<T,V>,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

operator std::map<T,V> (void) const
{
 return this->GetData();
}

const std::map<T,V>& operator () (void) const
{
 return this->GetData();
}

};

/// Специализация: свойства - MDMatrix
template<typename V, typename OwnerT, unsigned int type>
class ULProperty<MDMatrix<V>,OwnerT,type>: public UProperty<MDMatrix<V>,OwnerT>

{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<MDMatrix<V>,OwnerT>::SetterRT setmethod=0)
 : UProperty<MDMatrix<V>,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<MDMatrix<V>,OwnerT,type> &v) {}

public:

/// Оператор доступа по индексу
V& operator [] (size_t index)
{
 return this->v[index];
}

const V& operator [] (size_t index) const
{
 return this->v[index];
}

V& operator () (int row, int col)
{
 return this->v(row,col);
}

const V& operator () (int row, int col) const
{
 return this->v(row,col);
}

/// Возвращает число элементов по всем размерностям
MMatrixSize GetMatrixSize(void) const
{
 return this->v.GetMatrixSize();
}

void Resize(int rows, int cols)
{
 this->v.Resize(rows,cols);
}

void Resize(const MMatrixSize &size)
{
 this->v.Resize(size);
}

void Assign(int rows, int cols, const V &val)
{
 this->v.Assign(rows,cols,val);
}

void Assign(int rows, int cols, const V *val)
{
 this->v.Assign(rows,cols,val);
}

bool Assign(const MMatrixSize &size, const V *data)
{
 return this->v.Assign(size,data);
}

bool Assign(const MMatrixSize &size, V value)
{
 return this->v.Assign(size,value);
}

bool Assign(const MMatrixSize &size, const void *data)
{
 return this->v.Assign(size,data);
}

int GetRows(void) const
{
 return this->v.GetRows();
}

int GetCols(void) const
{
 return this->v.GetCols();
}

void ToZero(void) const
{
 this->v.ToZero();
}

/// Оператор присваивания
ULProperty<MDMatrix<V>,OwnerT,type>& operator = (const MDMatrix<V> &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<MDMatrix<V>,OwnerT, type>& operator = (const ULProperty<MDMatrix<V>,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

operator MDMatrix<V> (void) const
{
 return this->GetData();
}

const MDMatrix<V>& operator () (void) const
{
 return this->GetData();
}

};

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const ULProperty<MDMatrix<V>,OwnerT, type> &v1,const ULProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1.v-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const MDMatrix<V> &v1,const ULProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const ULProperty<MDMatrix<V>,OwnerT, type> &v1,const MDMatrix<V> &v2)
{
 return v1.v-v2;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator + (const ULProperty<MDMatrix<V>,OwnerT, type> &v1,const ULProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1.v+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator + (const MDMatrix<V> &v1,const ULProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator + (const ULProperty<MDMatrix<V>,OwnerT, type> &v1,const MDMatrix<V> &v2)
{
 return v1.v+v2;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const ULProperty<MDMatrix<V>,OwnerT, type> &v1,const ULProperty<V,OwnerT, type> &v2)
{
 return v1.v-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const MDMatrix<V> &v1,const ULProperty<V,OwnerT, type> &v2)
{
 return v1-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const ULProperty<MDMatrix<V>,OwnerT, type> &v1,const V &v2)
{
 return v1.v-v2;
}


/// Специализация: свойства - MDVector
template<typename V, typename OwnerT, unsigned int type>
class ULProperty<MDVector<V>,OwnerT,type>: public UProperty<MDVector<V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<MDVector<V>,OwnerT>::SetterRT setmethod=0)
 : UProperty<MDVector<V>,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<MDVector<V>,OwnerT,type> &v) {}

public:

/// Оператор доступа по индексу
V& operator [] (size_t index)
{
 return this->v[index];
}

const V& operator [] (size_t index) const
{
 return this->v[index];
}

V& operator () (int index)
{
 return this->v(index);
}

const V& operator () (int index) const
{
 return this->v(index);
}

/// Возвращает число элементов по всем размерностям
MMatrixSize GetMatrixSize(void) const
{
 return this->v.GetMatrixSize();
}

void Resize(int size)
{
 this->v.Resize(size);
}

void Resize(const MMatrixSize &size)
{
 this->v.Resize(size);
}

void Resize(int size, V def_value)
{
 this->v.Resize(size,def_value);
}

void Assign(int size, V data)
{
 this->v.Assign(size, data);
}

void Assign(int size, const void *data)
{
 this->v.Assign(size, data);
}

void Assign(int size, const V &val)
{
 this->v.Assign(size,val);
}

void Assign(int size, const V *val)
{
 this->v.Assign(size,val);
}

bool Assign(const MMatrixSize &size, const V *data)
{
 return this->v.Assign(size,data);
}

bool Assign(const MMatrixSize &size, V value)
{
 return this->v.Assign(size,value);
}

bool Assign(const MMatrixSize &size, const void *data)
{
 return this->v.Assign(size,data);
}

int GetSize(void) const
{
 return this->v.GetSize();
}

void ToZero(void) const
{
 this->v.ToZero();
}

/// Оператор присваивания
ULProperty<MDVector<V>,OwnerT,type>& operator = (const MDVector<V> &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<MDVector<V>,OwnerT, type>& operator = (const ULProperty<MDVector<V>,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

operator MDVector<V> (void) const
{
 return this->GetData();
}

const MDVector<V>& operator () (void) const
{
 return this->GetData();
}

};

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator - (const ULProperty<MDVector<V>,OwnerT, type> &v1,const ULProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1.v-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator - (const MDVector<V> &v1,const ULProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator - (const ULProperty<MDVector<V>,OwnerT, type> &v1,const MDVector<V> &v2)
{
 return v1.v-v2;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator + (const ULProperty<MDVector<V>,OwnerT, type> &v1,const ULProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1.v+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator + (const MDVector<V> &v1,const ULProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator + (const ULProperty<MDVector<V>,OwnerT, type> &v1,const MDVector<V> &v2)
{
 return v1.v+v2;
}



/// Специализация: свойства - double
template<typename OwnerT, unsigned int type>
class ULProperty<double,OwnerT,type>: public UProperty<double,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<double,OwnerT>::SetterRT setmethod=0)
 : UProperty<double,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<double,OwnerT,type> &v) {}

public:

// -----------------------------
// Операторы
// -----------------------------
public:
// Оператор присваивания
ULProperty<double,OwnerT, type>& operator = (const double &value)
{
 this->SetData(value);
 return *this;
}

ULProperty<double,OwnerT, type>& operator = (const ULProperty<double,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

operator double (void) const
{
 return this->GetData();
}

const double& operator () (void) const
{
 return this->GetData();
}

// -----------------------------

};

/// Специализация: свойства - int
template<typename OwnerT, unsigned int type>
class ULProperty<int,OwnerT,type>: public UProperty<int,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<int,OwnerT>::SetterRT setmethod=0)
 : UProperty<int,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<int,OwnerT,type> &v) {}

public:

operator int (void) const
{ return this->v; }

int operator ++ (void)
{
 if(this->SetterR)
  SetData(this->v+1);
 else
  ++this->v;
 return this->v;
}

int operator ++ (int)
{
 int temp=this->v;
 if(this->SetterR)
  SetData(this->v+1);
 else
  this->v++;
 return temp;
}

int operator -- (void)
{
 if(this->SetterR)
  SetData(this->v-1);
 else
  ++this->v;
 return this->v;
}

int operator -- (int)
{
 int temp=this->v;
 if(this->SetterR)
  SetData(this->v-1);
 else
  this->v--;
 return temp;
}

ULProperty<int,OwnerT,type>& operator = (const int &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<int,OwnerT, type>& operator = (const ULProperty<int,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

const int& operator () (void) const
{
 return this->GetData();
}

};

/// Специализация: свойства - int
template<typename OwnerT, unsigned int type>
class ULProperty<unsigned int,OwnerT,type>: public UProperty<unsigned int,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<unsigned int,OwnerT>::SetterRT setmethod=0)
 : UProperty<unsigned int,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<unsigned int,OwnerT,type> &v) {}

public:

operator unsigned int (void) const
{ return this->v; }


unsigned int operator ++ (void)
{
 if(this->SetterR)
  SetData(this->v+1);
 else
  ++this->v;
 return this->v;
}

unsigned int operator ++ (int)
{
 int temp=this->v;
 if(this->SetterR)
  SetData(this->v+1);
 else
  this->v++;
 return temp;
}

unsigned int operator -- (void)
{
 if(this->SetterR)
  SetData(this->v-1);
 else
  ++this->v;
 return this->v;
}

unsigned int operator -- (int)
{
 unsigned int temp=this->v;
 if(this->SetterR)
  SetData(this->v-1);
 else
  this->v--;
 return temp;
}

ULProperty<unsigned int,OwnerT,type>& operator = (const unsigned int &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<unsigned int,OwnerT, type>& operator = (const ULProperty<unsigned int,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

const unsigned int& operator () (void) const
{
 return this->GetData();
}

};
/*
/// Специализация: свойства - bool
template<typename OwnerT, unsigned int type>
class ULProperty<bool,OwnerT,type>: public UProperty<bool,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
ULProperty(const string &name, OwnerT * const owner, typename UVProperty<bool,OwnerT>::SetterRT setmethod=0)
 : UProperty<bool,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
ULProperty(const ULProperty<bool,OwnerT,type> &v) {}

public:


operator bool (void) const
{ return this->v; }

ULProperty<bool,OwnerT,type>& operator = (const bool &value)
{
 this->SetData(value);
 return *this;
};

ULProperty<bool,OwnerT, type>& operator = (const ULProperty<bool,OwnerT, type> &v)
{
 this->SetData(this->v.GetData());
 return *this;
}

const bool& operator () (void) const
{
 return this->GetData();
}

};*/
                      /*
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
 static_cast<UPropertyRangeLocal<V,std::list<V>, OwnerT,type>& >(*this)=this->v.Value;
 return *this;
};
// -----------------------------
};

// Класс - свойство-контейнер со значением внутри
template<typename V, typename OwnerT, unsigned int type>
class UPropertyRange<V, std::vector<V>,OwnerT, type>: public UPropertyRangeLocal<V,std::vector<V>,OwnerT,type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyVirtual<V,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<V,std::vector<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};

UPropertyRange(const std::string &name, OwnerT * const owner, typename UPropertyRangeLocal<V, std::list<V>, OwnerT, type>::VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyRangeLocal<V,std::vector<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
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

void erase(typename std::vector<V>::iterator it)
{
 this->Value.erase(it);
}

size_t size(void) const
{
 return this->Value.size();
}

void resize(size_t size)
{
 this->Value.resize(size);
}

void resize(size_t size, const V &value)
{
 this->Value.resize(size,value);
}

void assign(size_t size, const V &value)
{
 this->Value.assign(size,value);
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

void push_back(const V &value)
{
 Value.push_back(value);
}

UPropertyRange<V,std::vector<V>, OwnerT,type>& operator = (const std::vector<V> &value)
{
 static_cast<UPropertyRangeLocal<V,std::vector<V>, OwnerT,type>& >(*this)=value;
 return *this;
};

UPropertyRange<V,std::vector<V>, OwnerT,type>& operator = (const UPropertyRange<V,std::vector<V>, OwnerT,type> &v)
{
 static_cast<UPropertyRangeLocal<V,std::vector<V>, OwnerT,type>& >(*this)=this->v.Value;
 return *this;
};
// -----------------------------
};
              */
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
