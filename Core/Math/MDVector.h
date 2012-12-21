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

// --------------------------
// Операторы управления данными
// --------------------------
// Возвращает длину вектора
int GetSize(void) const;

// Меняет длину вектора
void Resize(int size);

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
  memset(MDMatrix<T>::Data1D,0,GetRows()*sizeof(T));
 else
 {
  T* p=MDMatrix<T>::Data1D;
  for(unsigned i=0;i<GetRows();i++)
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

// --------------------------
// Операторы управления данными
// --------------------------
// Возвращает длину вектора
template<class T>
int MDVector<T>::GetSize(void) const
{
 return Rows;
}

// Меняет длину вектора
template<class T>
void MDVector<T>::Resize(int size)
{
 MDMatrix<T>::Resize(size,1);
}

// Оператор присваивания
template<class T>
MDVector<T>& MDVector<T>::operator = (const MDVector<T> &copy)
{
 Resize(copy.GetRows());
 memcpy(MDMatrix<T>::Data1D,copy.Data1D,sizeof(T)*GetRows());
 return *this;
};

template<class T>
MDVector<T>& MDVector<T>::operator = (const MDMatrix<T> &copy)
{
 Resize(copy.GetRows());
 memcpy(MDMatrix<T>::Data1D,copy.Data1D,sizeof(T)*GetRows());
 return *this;
}

template<class T>
MDVector<T>& MDVector<T>::operator = (T value)
{
 T* pm1=MDMatrix<T>::Data1D;

 for(unsigned i=0;i<GetRows();i++)
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
 for(int k=0;k<GetRows();k++)
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

