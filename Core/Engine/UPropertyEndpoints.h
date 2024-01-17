//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "UProperty.h"
//---------------------------------------------------------------------------

namespace RDK {

/// Специализация: свойства - map
template<typename T, typename V, typename OwnerT, unsigned int type>
class UProperty<std::map<T,V>,OwnerT,type, true>: public UPropertyLocal<std::map<T,V>,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<std::map<T,V>,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<std::map<T,V>,OwnerT,type>(name, owner, setmethod)
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
class UProperty<MDMatrix<V>,OwnerT,type, false>: public UPropertyLocal<MDMatrix<V>,OwnerT,type>

{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<MDMatrix<V>,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<MDMatrix<V>,OwnerT,type>(name, owner, setmethod)
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
class UProperty<MDVector<V>,OwnerT,type, false>: public UPropertyLocal<MDVector<V>,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<MDVector<V>,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<MDVector<V>,OwnerT,type>(name, owner, setmethod)
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
class UProperty<double, OwnerT,type, false>: public UPropertyLocal<double,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<double,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<double,OwnerT,type>(name, owner, setmethod)
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
class UProperty<int,OwnerT,type, false>: public UPropertyLocal<int,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<int,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<int,OwnerT,type>(name, owner, setmethod)
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
class UProperty<unsigned int,OwnerT,type, false>: public UPropertyLocal<unsigned int,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<unsigned int,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<unsigned int,OwnerT,type>(name, owner, setmethod)
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
class UProperty<unsigned long int,OwnerT,type, false>: public UPropertyLocal<unsigned long int,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<unsigned long int,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<unsigned long int,OwnerT,type>(name, owner, setmethod)
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
class UProperty<unsigned long long,OwnerT,type, false>: public UPropertyLocal<unsigned long long,OwnerT,type>
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
//Конструктор инициализации.
UProperty(const string &name, OwnerT * const owner, typename UPropertyLocal<unsigned long long,OwnerT,type>::SetterRT setmethod=0)
 : UPropertyLocal<unsigned long long,OwnerT,type>(name, owner, setmethod)
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

}
#endif
