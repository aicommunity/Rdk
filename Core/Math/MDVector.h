/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 1997.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MDVECTOR_H
#define MDVECTOR_H

#include "MDMatrix.h"

namespace RDK {

template<class T>
class MDVector: public MDMatrix<T>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MDVector(void);
MDVector(int size, T defvalue=0);
MDVector(T xv, T yv, T zv=0);
MDVector(T xv, T yv, T zv, T dv);
MDVector(const MDVector<T> &copy);
MDVector(const MDMatrix<T> &copy);
//MDVector(const T* data);
~MDVector(void);
// --------------------------

// -----------------------------------
// Общие методы доступа к параметрам матрицы
// -----------------------------------
/// Возвращает размерность матрицы
virtual int GetDimensions(void) const;

/// Возвращает число элементов по всем размерностям
virtual MMatrixSize GetMatrixSize(void) const;

/// Устанавливает число элементов по всем размерностям
virtual bool Resize(const MMatrixSize &size);
// -----------------------------------

// --------------------------
// Операторы управления данными
// --------------------------
// Меняет длину вектора
void Resize(int size);
void Resize(int size, T def_value);

void Assign(int size, const T *data);
void Assign(int size, T data);


// Оператор присваивания
MDVector<T>& operator = (const MDVector<T> &copy);
MDVector<T>& operator = (const MDMatrix<T> &copy);
//MDVector<T>& operator = (const T* data);
MDVector<T>& operator = (T value);

// Доступ к элементу
T& operator () (int i);
const T& operator () (int i) const;
// --------------------------

};

// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T>
MDVector<T>::MDVector(void)
{
};

template<class T>
MDVector<T>::MDVector(int size, T defvalue)
{
 MDMatrix<T>::Resize(size,1);
 if(defvalue == 0)
  memset(MDMatrix<T>::Data1D,0,this->GetRows()*sizeof(T));
 else
 {
  T* p=MDMatrix<T>::Data1D;
  for(int i=0;i<MDMatrix<T>::GetRows();i++)
   *p++=defvalue;
 }
}

template<class T>
MDVector<T>::MDVector(T xv, T yv, T zv)
{
 MDMatrix<T>::Resize(3,1);
 MDMatrix<T>::x=xv; MDMatrix<T>::y=yv; MDMatrix<T>::z=zv;
}

template<class T>
MDVector<T>::MDVector(T xv, T yv, T zv, T dv)
{
 MDMatrix<T>::Resize(4,1);
 MDMatrix<T>::x=xv; MDMatrix<T>::y=yv; MDMatrix<T>::z=zv; MDMatrix<T>::d=dv;
}


template<class T>
MDVector<T>::MDVector(const MDVector<T> &copy)
    : MDMatrix<T>(copy)
{ /**this=copy;*/ };

template<class T>
MDVector<T>::MDVector(const MDMatrix<T> &copy)
{ *this=copy; };
/*
template<class T>
MDVector<T>::MDVector(const T* data)
{ *copy=data; };
  */
template<class T>
MDVector<T>::~MDVector(void) {};
// --------------------------

// -----------------------------------
// Общие методы доступа к параметрам матрицы
// -----------------------------------
/// Возвращает размерность матрицы
template<class T>
int MDVector<T>::GetDimensions(void) const
{
 return 1;
}

/// Возвращает число элементов по всем размерностям
template<class T>
MMatrixSize MDVector<T>::GetMatrixSize(void) const
{
 std::vector<int> dims;
 dims.assign(1,Rows);
 MMatrixSize size(dims);

 return size;
}

/// Устанавливает число элементов по всем размерностям
template<class T>
bool MDVector<T>::Resize(const MMatrixSize &size)
{
 if(size.GetDimensions() != 1)
  return false;
 Resize(size[0]);
 return true;
}
// -----------------------------------

// --------------------------
// Операторы управления данными
// --------------------------
// Меняет длину вектора
template<class T>
void MDVector<T>::Resize(int size)
{
 MDMatrix<T>::Resize(size,1);
}

template<class T>
void MDVector<T>::Resize(int size, T def_value)
{
 MDMatrix<T>::Resize(size,1,def_value);
}

template<class T>
void MDVector<T>::Assign(int size, const T *data)
{
 MDMatrix<T>::Assign(size,1,data);
}

template<class T>
void MDVector<T>::Assign(int size, T data)
{
 MDMatrix<T>::Assign(size,1,data);
}


// Оператор присваивания
template<class T>
MDVector<T>& MDVector<T>::operator = (const MDVector<T> &copy)
{
 Resize(copy.GetRows());
 memcpy(MDMatrix<T>::Data1D,copy.Data1D,sizeof(T)*this->GetRows());
 return *this;
};

template<class T>
MDVector<T>& MDVector<T>::operator = (const MDMatrix<T> &copy)
{
 Resize(copy.GetRows());
 if(copy.GetCols() > 0)
  memcpy(MDMatrix<T>::Data1D,copy.Data1D,sizeof(T)*this->GetRows());
 else
  memset(MDMatrix<T>::Data1D,0,sizeof(T)*this->GetRows());
 return *this;
}

template<class T>
MDVector<T>& MDVector<T>::operator = (T value)
{
 T* pm1=MDMatrix<T>::Data1D;

 for(unsigned i=0;i<this->GetRows();i++)
  *pm1++ = value;
 return *this;
};

// Доступ к элементу
template<class T>
T& MDVector<T>::operator () (int i)
{
 return *(MDMatrix<T>::Data1D+i);
}

template<class T>
const T& MDVector<T>::operator () (int i) const
{
 return *(MDMatrix<T>::Data1D+i);
}
// --------------------------

// --------------------------
// Арифметические операторы
// --------------------------
// Скалярное произведение векторов
template<class T>
double operator * (const MDVector<T> &M1, const MDVector<T> &M2)
{
 double res=0;
 for(int k=0;k<MDMatrix<T>::GetRows();k++)
 {
  res+=M1.Data[k][0]*M2.Data[k][0];
 }

 return res;
}

// Векторное произведение векторов
template<class T>
MDVector<T> operator ^ (const MDVector<T> &u, const MDVector<T> &v)
{
 return MDVector<T>(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}
// --------------------------

}
#endif

