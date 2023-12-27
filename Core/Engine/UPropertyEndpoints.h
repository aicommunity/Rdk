//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "UProperty.h"
#include <vector>
//---------------------------------------------------------------------------

namespace RDK {

/// Специализация: свойства - вектора
template<typename V, typename OwnerT, unsigned int type>
class UProperty<std::vector<V>, OwnerT, type>: public UPropertyLocal<std::vector<V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<std::vector<V>,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<std::vector<V>,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<std::vector<V>,OwnerT,type> &v) {}

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

UProperty<std::vector<V>,OwnerT,type>& operator = (const std::vector<V> &value)
{
 this->SetData(value);
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
class UProperty<std::list<V>,OwnerT,type>: public UPropertyLocal<std::list<V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<std::list<V>,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<std::list<V>,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<std::list<V>,OwnerT,type> &v) {}

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
UProperty<std::list<V>,OwnerT,type>& operator = (const std::list<V> &value)
{
 this->SetData(value);
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
class UProperty<std::map<T,V>,OwnerT,type>: public UPropertyLocal<std::map<T,V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<std::map<T,V>,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<std::map<T,V>,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<std::map<T,V>,OwnerT,type> &v) {}

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
UProperty<std::map<T,V>,OwnerT,type>& operator = (const std::map<T,V> &value)
{
 this->SetData(value);
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
class UProperty<MDMatrix<V>,OwnerT,type>: public UPropertyLocal<MDMatrix<V>,OwnerT>

{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<MDMatrix<V>,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<MDMatrix<V>,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<MDMatrix<V>,OwnerT,type> &v) {}

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

void Resize(int rows, int cols, V def_value)
{
 this->v.Resize(rows,cols, def_value);
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
UProperty<MDMatrix<V>,OwnerT,type>& operator = (const MDMatrix<V> &value)
{
 this->SetData(value);
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
const MDMatrix<V> operator - (const UProperty<MDMatrix<V>,OwnerT, type> &v1,const UProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1.v-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const MDMatrix<V> &v1,const UProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const UProperty<MDMatrix<V>,OwnerT, type> &v1,const MDMatrix<V> &v2)
{
 return v1.v-v2;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator + (const UProperty<MDMatrix<V>,OwnerT, type> &v1,const UProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1.v+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator + (const MDMatrix<V> &v1,const UProperty<MDMatrix<V>,OwnerT, type> &v2)
{
 return v1+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator + (const UProperty<MDMatrix<V>,OwnerT, type> &v1,const MDMatrix<V> &v2)
{
 return v1.v+v2;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const UProperty<MDMatrix<V>,OwnerT, type> &v1,const UProperty<V,OwnerT, type> &v2)
{
 return v1.v-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const MDMatrix<V> &v1,const UProperty<V,OwnerT, type> &v2)
{
 return v1-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDMatrix<V> operator - (const UProperty<MDMatrix<V>,OwnerT, type> &v1,const V &v2)
{
 return v1.v-v2;
}


/// Специализация: свойства - MDVector
template<typename V, typename OwnerT, unsigned int type>
class UProperty<MDVector<V>,OwnerT,type>: public UPropertyLocal<MDVector<V>,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<MDVector<V>,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<MDVector<V>,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<MDVector<V>,OwnerT,type> &v) {}

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
UProperty<MDVector<V>,OwnerT,type>& operator = (const MDVector<V> &value)
{
 this->SetData(value);
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
const MDVector<V> operator - (const UProperty<MDVector<V>,OwnerT, type> &v1,const UProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1.v-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator - (const MDVector<V> &v1,const UProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1-v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator - (const UProperty<MDVector<V>,OwnerT, type> &v1,const MDVector<V> &v2)
{
 return v1.v-v2;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator + (const UProperty<MDVector<V>,OwnerT, type> &v1,const UProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1.v+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator + (const MDVector<V> &v1,const UProperty<MDVector<V>,OwnerT, type> &v2)
{
 return v1+v2.v;
}

template<typename V, typename OwnerT, unsigned int type>
const MDVector<V> operator + (const UProperty<MDVector<V>,OwnerT, type> &v1,const MDVector<V> &v2)
{
 return v1.v+v2;
}



/// Специализация: свойства - double
template<typename OwnerT, unsigned int type>
class UProperty<double, OwnerT,type>: public UPropertyLocal<double,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<double,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<double,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<double,OwnerT,type> &v) {}

public:

// -----------------------------
// Операторы
// -----------------------------
public:
// Оператор присваивания
UProperty<double,OwnerT, type>& operator = (const double &value)
{
 this->SetData(value);
 return *this;
}

operator double (void) const
{
 return GetData();
}

const double& operator () (void) const
{
 return this->GetData();
}

// -----------------------------

};

/// Специализация: свойства - int
template<typename OwnerT, unsigned int type>
class UProperty<int,OwnerT,type>: public UPropertyLocal<int,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<int,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<int,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<int,OwnerT,type> &v) {}

public:

operator int (void) const
{ return this->v; }

int& operator ++ (void)
{
 if(this->SetterR)
  this->SetData(this->v+1);
 else
  ++this->v;
 return this->v;
}

int operator ++ (int)
{
 int temp=this->v;
 if(this->SetterR)
  this->SetData(this->v+1);
 else
  this->v++;
 return temp;
}

int& operator -- (void)
{
 if(this->SetterR)
  this->SetData(this->v-1);
 else
  --this->v;
 return this->v;
}

int operator -- (int)
{
 int temp=this->v;
 if(this->SetterR)
  this->SetData(this->v-1);
 else
  this->v--;
 return temp;
}

UProperty<int,OwnerT,type>& operator = (const int &value)
{
 this->SetData(value);
 return *this;
}

const int& operator () (void) const
{
 return this->GetData();
}

};

/// Специализация: свойства - unsigned int
template<typename OwnerT, unsigned int type>
class UProperty<unsigned int,OwnerT,type>: public UPropertyLocal<unsigned int,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<unsigned int,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<unsigned int,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<unsigned int,OwnerT,type> &v) {}

public:

operator unsigned int (void) const
{ return this->v; }


unsigned int& operator ++ (void)
{
 if(this->SetterR)
  this->SetData(this->v+1);
 else
  ++this->v;
 return this->v;
}

unsigned int operator ++ (int)
{
 int temp=this->v;
 if(this->SetterR)
  this->SetData(this->v+1);
 else
  this->v++;
 return temp;
}

unsigned int& operator -- (void)
{
 if(this->SetterR)
  this->SetData(this->v-1);
 else
  --this->v;
 return this->v;
}

unsigned int operator -- (int)
{
 unsigned int temp=this->v;
 if(this->SetterR)
  this->SetData(this->v-1);
 else
  this->v--;
 return temp;
}

UProperty<unsigned int,OwnerT,type>& operator = (const unsigned int &value)
{
 this->SetData(value);
 return *this;
}

const unsigned int& operator () (void) const
{
 return this->GetData();
}

};

/// Специализация: свойства - int
template<typename OwnerT, unsigned int type>
class UProperty<unsigned long int,OwnerT,type>: public UPropertyLocal<unsigned long int,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<unsigned long int,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<unsigned long int,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<size_t,OwnerT,type> &v) {}

public:

operator unsigned long int (void) const
{ return this->v; }

unsigned long int& operator ++ (void)
{
 if(this->SetterR)
  this->SetData(this->v+1);
 else
  ++this->v;
 return this->v;
}

unsigned long int operator ++ (int)
{
 unsigned long int temp=this->v;
 if(this->SetterR)
  this->SetData(this->v+1);
 else
  this->v++;
 return temp;
}

unsigned long int& operator -- (void)
{
 if(this->SetterR)
  this->SetData(this->v-1);
 else
  --this->v;
 return this->v;
}

unsigned long int operator -- (int)
{
 unsigned long int temp=this->v;
 if(this->SetterR)
  this->SetData(this->v-1);
 else
  this->v--;
 return temp;
}

UProperty<unsigned long int,OwnerT,type>& operator = (const unsigned long int &value)
{
 this->SetData(value);
 return *this;
};

const unsigned long int& operator () (void) const
{
 return this->GetData();
}

};


/// Специализация: свойства - unsigned long long
template<typename OwnerT, unsigned int type>
class UProperty<unsigned long long,OwnerT,type>: public UPropertyLocal<unsigned long long,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<unsigned long long,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<unsigned long long,OwnerT>(name, owner, setmethod)
{ }

protected:
UProperty(const UProperty<unsigned long long,OwnerT,type> &v) {}

public:

operator unsigned long long (void) const
{ return this->v; }


unsigned long long& operator ++ (void)
{
 if(this->SetterR)
  SetData(this->v+1);
 else
  ++this->v;
 return this->v;
}

unsigned long long operator ++ (int)
{
 int temp=this->v;
 if(this->SetterR)
  SetData(this->v+1);
 else
  this->v++;
 return temp;
}

unsigned long long& operator -- (void)
{
 if(this->SetterR)
  SetData(this->v-1);
 else
  --this->v;
 return this->v;
}

unsigned long long operator -- (int)
{
 unsigned int temp=this->v;
 if(this->SetterR)
  SetData(this->v-1);
 else
  this->v--;
 return temp;
}

UProperty<unsigned long long,OwnerT,type>& operator = (const unsigned long long &value)
{
 this->SetData(value);
 return *this;
}

const unsigned long long& operator () (void) const
{
 return this->GetData();
}

};
/*
/// Специализация: свойства - bool
template<typename OwnerT, unsigned int type>
class UProperty<bool,OwnerT,type>: public UPropertyLocal<bool,OwnerT>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<bool,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<bool,OwnerT>(owner, setmethod)
{ reinterpret_cast<UComponent* const>(owner)->AddLookupProperty(name,type,this,false); }

protected:
UProperty(const UProperty<bool,OwnerT,type> &v) {}

public:


operator bool (void) const
{ return this->v; }

UProperty<bool,OwnerT,type>& operator = (const bool &value)
{
 this->SetData(value);
 return *this;
};

UProperty<bool,OwnerT, type>& operator = (const UProperty<bool,OwnerT, type> &v)
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
class UPropertyLocalRange<V, std::list<V>,OwnerT, type>: public UPropertyLocalRangeLocal<V,std::list<V>,OwnerT,type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyLocalRange(const std::string &name, OwnerT * const owner, typename UPropertyLocalVirtual<V,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocalRangeLocal<V,std::list<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};

UPropertyLocalRange(const std::string &name, OwnerT * const owner, typename UPropertyLocalRangeLocal<V, std::list<V>, OwnerT, type>::VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyLocalRangeLocal<V,std::list<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
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

UPropertyLocalRange<V,std::list<V>, OwnerT,type>& operator = (const std::list<V> &value)
{
 static_cast<UPropertyLocalRangeLocal<V,std::list<V>, OwnerT,type>& >(*this)=value;
 return *this;
};

UPropertyLocalRange<V,std::list<V>, OwnerT,type>& operator = (const UPropertyLocalRange<V,std::list<V>, OwnerT,type> &v)
{
 static_cast<UPropertyLocalRangeLocal<V,std::list<V>, OwnerT,type>& >(*this)=this->v.Value;
 return *this;
};
// -----------------------------
};

// Класс - свойство-контейнер со значением внутри
template<typename V, typename OwnerT, unsigned int type>
class UPropertyLocalRange<V, std::vector<V>,OwnerT, type>: public UPropertyLocalRangeLocal<V,std::vector<V>,OwnerT,type>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UPropertyLocalRange(const std::string &name, OwnerT * const owner, typename UPropertyLocalVirtual<V,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocalRangeLocal<V,std::vector<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
{
};

UPropertyLocalRange(const std::string &name, OwnerT * const owner, typename UPropertyLocalRangeLocal<V, std::list<V>, OwnerT, type>::VSetterRT setmethod, bool dynamic_prop_flag=false)
 : UPropertyLocalRangeLocal<V,std::vector<V>,OwnerT, type>(name, owner, setmethod, dynamic_prop_flag)
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

UPropertyLocalRange<V,std::vector<V>, OwnerT,type>& operator = (const std::vector<V> &value)
{
 static_cast<UPropertyLocalRangeLocal<V,std::vector<V>, OwnerT,type>& >(*this)=value;
 return *this;
};

UPropertyLocalRange<V,std::vector<V>, OwnerT,type>& operator = (const UPropertyLocalRange<V,std::vector<V>, OwnerT,type> &v)
{
 static_cast<UPropertyLocalRangeLocal<V,std::vector<V>, OwnerT,type>& >(*this)=this->v.Value;
 return *this;
};
// -----------------------------
};
              */
	   /*
/// deprecated
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UProperty: public UPropertyLocalLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocalLocal<T,OwnerT,type>(name, owner, setmethod) //ipSingle | ipData
{
};

// Оператор присваивания
UProperty<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};


};

template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UPropertyLocalParam: public UPropertyLocalLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyLocalParam(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocalLocal<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// Оператор присваивания
UPropertyLocalParam<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubState>
class UPropertyLocalState: public UPropertyLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyLocalState(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipData
{
};

// Оператор присваивания
UPropertyLocalState<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyLocalOutputData: public UPropertyLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyLocalOutputData(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// Оператор присваивания
UPropertyLocalOutputData<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};             */
/*
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyLocalInput: public UPropertyLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyLocalInput(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipComp
{
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyLocalInputData: public UPropertyLocal<T,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyLocalInputData(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipDatas
{
};

};

template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyLocalInputDataRange: public UPropertyLocalRange<T,RangeT,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyLocalInputDataRange(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocalRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

/// deprecated
template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyLocalInputDataC: public UPropertyLocalRange<T,RangeT,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UPropertyLocalInputDataC(const string &name, OwnerT * const owner, typename UPropertyLocalVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocalRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};   */

}
#endif
