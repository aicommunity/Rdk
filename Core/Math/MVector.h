/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 1997.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MVECTOR_H
#define MVECTOR_H

#include <math.h>
#include <string.h>
#include "MMatrix.h"

namespace RDK {

template<class T, unsigned Rows>
class MVector: public MMatrix<T,Rows,1>
{
public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
MVector(void);
MVector(T defvalue);
MVector(T xv, T yv, T zv=0);
MVector(T xv, T yv, T zv, T dv);
MVector(const MVector<T,Rows> &copy);
MVector(const MMatrix<T,Rows,1> &copy);
//MVector(const T* data);
virtual ~MVector(void);
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
// Оператор присваивания
MVector<T,Rows>& operator = (const MVector<T,Rows> &copy);
MVector<T,Rows>& operator = (const MMatrix<T,Rows,1> &copy);
//MVector<T,Rows>& operator = (const T* data);
MVector<T,Rows>& operator = (T value);

//void Assign(const T *data);
void Assign(T value);

// Доступ к элементу
T& operator () (int i);
const T& operator () (int i) const;
// --------------------------

};

// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<class T, unsigned Rows>
MVector<T,Rows>::MVector(void)
{
};

template<class T, unsigned Rows>
MVector<T,Rows>::MVector(T defvalue)
{
 if(defvalue == 0)
  memset(MMatrix<T,Rows,1>::Data1D,0,Rows*sizeof(T));
 else
 {
  T* p=MMatrix<T,Rows,1>::Data1D;
  for(unsigned i=0;i<Rows;i++)
   *p++=defvalue;
 }
}

template<class T, unsigned Rows>
MVector<T,Rows>::MVector(T xv, T yv, T zv)
{
 MMatrix<T,Rows,1>::x=xv; MMatrix<T,Rows,1>::y=yv; MMatrix<T,Rows,1>::z=zv;
}

template<class T, unsigned Rows>
MVector<T,Rows>::MVector(T xv, T yv, T zv, T dv)
{
 MMatrix<T,Rows,1>::x=xv; MMatrix<T,Rows,1>::y=yv; MMatrix<T,Rows,1>::z=zv; MMatrix<T,Rows,1>::d=dv;
}


template<class T, unsigned Rows>
MVector<T,Rows>::MVector(const MVector<T,Rows> &copy)
    : MMatrix<T,Rows,1>(copy)
{ /**this=copy;*/ };

template<class T, unsigned Rows>
MVector<T,Rows>::MVector(const MMatrix<T,Rows,1> &copy)
{ *this=copy; };
/*
template<class T, unsigned Rows>
MVector<T,Rows>::MVector(const T* data)
{ *copy=data; };
  */
template<class T, unsigned Rows>
MVector<T,Rows>::~MVector(void) {};
// --------------------------


// -----------------------------------
// Общие методы доступа к параметрам матрицы
// -----------------------------------
/// Возвращает размерность матрицы
template<class T, unsigned Rows>
int MVector<T,Rows>::GetDimensions(void) const
{
 return 1;
}

/// Возвращает число элементов по всем размерностям
template<class T, unsigned Rows>
MMatrixSize MVector<T,Rows>::GetMatrixSize(void) const
{
 std::vector<int> dims;
 dims.assign(1,Rows);
 MMatrixSize size(dims);

 return size;
}

/// Устанавливает число элементов по всем размерностям
template<class T, unsigned Rows>
bool MVector<T,Rows>::Resize(const MMatrixSize &size)
{
 if(size.GetDimensions() != 1)
  return false;

 return (size[0] == Rows);
}
// -----------------------------------

// --------------------------
// Операторы управления данными
// --------------------------
// Оператор присваивания
template<class T, unsigned Rows>
MVector<T,Rows>& MVector<T,Rows>::operator = (const MVector<T,Rows> &copy)
{
 memcpy(MMatrix<T,Rows,1>::Data1D,copy.Data1D,sizeof(T)*Rows);
 return *this;
};

template<class T, unsigned Rows>
MVector<T,Rows>& MVector<T,Rows>::operator = (const MMatrix<T,Rows,1> &copy)
{
 memcpy(MMatrix<T,Rows,1>::Data1D,copy.Data1D,sizeof(T)*Rows);
 return *this;
}
  /*
template<class T, unsigned Rows>
MVector<T,Rows>& MVector<T,Rows>::operator = (const T* data)
{
 memcpy(Data1D,data,sizeof(T)*Rows);
 return *this;
};  */

template<class T, unsigned Rows>
MVector<T,Rows>& MVector<T,Rows>::operator = (T value)
{
 T* pm1=MMatrix<T,Rows,1>::Data1D;

 for(unsigned i=0;i<Rows;i++)
  *pm1++ = value;
 return *this;
};
/*
template<class T, unsigned Rows>
void MVector<T,Rows>::Assign(const T *data)
{

} */

template<class T, unsigned Rows>
void MVector<T,Rows>::Assign(T value)
{
 *this=value;
}

// Доступ к элементу
template<class T, unsigned Rows>
T& MVector<T,Rows>::operator () (int i)
{
 return *(MMatrix<T,Rows,1>::Data1D+i);
}

template<class T, unsigned Rows>
const T& MVector<T,Rows>::operator () (int i) const
{
 return *(MMatrix<T,Rows,1>::Data1D+i);
}
// --------------------------

// --------------------------
// Арифметические операторы
// --------------------------
// Скалярное произведение векторов
template<class T, unsigned Rows>
double operator * (const MVector<T,Rows> &M1, const MVector<T,Rows> &M2)
{
 double res=0;
 for(int k=0;k<Rows;k++)
 {
  res+=M1.Data[k][0]*M2.Data[k][0];
 }

 return res;
}

// Векторное произведение векторов
template<class T>
MVector<T,3> operator ^ (const MVector<T,3> &u, const MVector<T,3> &v)
{
 return MVector<T,3>(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}
// --------------------------




/*
  Класс - MVector. Определяет новый тип данных - 3D-вектор.
 Вектор можно создать след. способами:
  1. MVector<type> v; - Создаётся в-р нулевой длины.
  2. MVector<type> v( (type)num ); - Создаётся в-р все коорд.
                                               которого = num.
  3. MVector<type> v( (type)x,(type)y,(type)z );
      - Создаётся в-р с коорд. x,y,z.
  4. MVector<type> v( (MVector<type>)v1 );
      - Создаётся в-р равный в-ру v1
 --------------------------------------

 В классе перегружены следующие операции:
   ## Сервисные операторы ##
   MVector<type> []; - Доступ к коорд. в-ра по их порядк. номеру.
   ! MVector<type>; - Получение длины вектора ( возвр. type )
   MVector<type>.Normalize(); - Функция нормирования вектора.
   Normalize(MVector<type>); - Функция выдачи нормированного вектора.
   ## Операторы присваивания ##
   MVector<type> = MVector<type>; - Присваивание в-ра в-ру.
   MVector<type> = type; - Присваивание всем коорд. в-ра значения type

   ## Логические операторы ##
   MVector<type> == MVector<type>; - Проверка на равенство.
   MVector<type> != MVector<type>; - Проверка на неравенство.
   MVector<type> < type; - Проверка "все коорд. строго меньше".
   MVector<type> > type; - Проверка "все коорд. строго больше".

   ## Арифметическии операторы ##
   - MVector<type>; - Смена знака у всех коорд. в-ра.
   MVector<type> += MVector<type>; - Добавление к вектору вектора.
   MVector<type> -= MVector<type>; - Вычитание из вектора вектора.
   MVector<type> ^= MVector<type>; - Векторное домножение вектора на вектор.
   MVector<type> *= type; - Умножение в-ра на число
   MVector<type> /= type; - Деление в-ра на число
   MVector<type> /= MVector<type>; - Почленное деление в-ра на в-р
   MVector<type> + MVector<type>; - Сложение векторов.
   MVector<type> - MVector<type>; - Вычитание векторов.
   MVector<type> ^ MVector<type>; - Векторное умножение векторов.
   MVector<type> * MVector<type>; - Скалярное умножение векторов.
   MVector<type> * type; - Умножение вектора и числа справа.
   type * MVector<type>; - Умножение вектора и числа слева.
   MVector<type> / type; - Деление вектора и числа.
   MVector<type> / MVector<type>; - Почленное деление в-ров

   ## Потоковые операторы ввода-вывода ##
   ostream << MVector<type>; - Вывод в поток ( как правил cout ), в удобочитаемой форме.
   istream >> MVector<type>; - Ввод из потока ( как правил cin ), с запросом вида "v=".
   fstream << MVector<type>; - Вывод в файловый поток.
   fstream >> MVector<type>; - Ввод из файлового потока.
 ----------------------------------------
*/
    /*
template<class DataV>
class MVector
{
public: // Данные
DataV x,y,z; // Координаты в-ра.

public: // Методы
//---------------------------
// Конструкторы и деструкторы
//---------------------------
//  Создаёт в-р нулевой длины.
MVector(void)
{ x=y=z=0; };

// Создаёт в-р все коорд. которого = ksi.
MVector(DataV ksi)
{ x=y=z=ksi; };

// Создаёт в-р с коорд. xx,yy,zz.
MVector(DataV xx,DataV yy,DataV zz)
{ x=xx; y=yy; z=zz; };

// Создаёт в-р равный в-ру v
MVector(const MVector<DataV> &v)
{ x=v.x; y=v.y; z=v.z; }
//---------------------------

//---------------------------
// Перегружаемые операторы
//---------------------------
// ## Сервисные операторы ##
// Доступ к коорд. в-ра по их порядк. номеру.
DataV& operator [](int n)
{
 return *(&x+n);
}

// Получение длины вектора
DataV operator !(void) const
{
 return std::sqrt(x*x+y*y+z*z);
}
// Функция нормирования вектора.
MVector<DataV>& Normalize(void)
{
 DataV len=sqrt(x*x+y*y+z*z);
 if(len != 0) *this/=len;
 return *this;
}

// Функция выдачи нормированного вектора.
MVector<DataV> Normalize(const MVector<DataV> &v);
// #########################

// ## Операторы присваивания ##
// Присваивание в-ра в-ру.
MVector<DataV>& operator = (const MVector<DataV> &v)
{
 x=v.x; y=v.y; z=v.z;
 return *this;
}

// Присваивание всем коорд. в-ра значения DataV
MVector<DataV>& operator = (DataV f)
{
 x=y=z=f;
 return *this;
}
// ############################


// ## Логические операторы ##
// Проверка на равенство.
bool operator == (const MVector<DataV>& v) const
{
 if(x==v.x && y==v.y && z==v.z)
  return true;
 else
  return false;
}

// Проверка на неравенство.
bool operator != (const MVector<DataV>& v) const
{
 return !(*this == v);
}

// Проверка "все коорд. строго меньше".
bool operator < (DataV v)
{
 return x<v && y<v && z<v;
}

// Проверка "все коорд. строго больше".
bool operator > (DataV v)
{
 return x>v && y>v && z>v;
}
// ##########################

// ## Арифметическии операторы ##
// Смена знака у всех коорд. в-ра.
MVector<DataV> operator - (void) const
{
 return MVector<DataV> (-x,-y,-z);
}

// Добавление к вектору вектора.
MVector<DataV>& operator += (const MVector<DataV> &v)
{
 x+=v.x;
 y+=v.y;
 z+=v.z;
 return *this;
}

// Вычитание из вектора вектора.
MVector<DataV>& operator -= (const MVector<DataV> &v)
{
 x-=v.x;
 y-=v.y;
 z-=v.z;
 return *this;
}

// Векторное домножение вектора на вектор.
MVector<DataV>& operator ^= (const MVector<DataV> &v)
{
 MVector<DataV> temp(*this);

 x=temp.y*v.z-temp.z*v.y;
 y=temp.z*v.x-temp.x*v.z;
 z=temp.x*v.y-temp.y*v.x;
 return *this;
}

// Умножение в-ра на число
MVector<DataV>& operator *= (DataV f)
{
 x*=f;
 y*=f;
 z*=f;
 return *this;
}

// Деление в-ра на число
MVector<DataV>& operator /= (DataV f)
{
 x/=f;
 y/=f;
 z/=f;
 return *this;
}

// Почленное деление в-ра на в-р
MVector<DataV>& operator /= (const MVector<DataV> &v)
{
 x/=v.x;
 y/=v.y;
 z/=v.z;
 return *this;
}

};

template<class DataV>
MVector<DataV> Normalize(const MVector<DataV> &v)
{
 MVector<DataV> tmp;
 DataV len=sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
 if(len != 0) tmp=v/len;
 return MVector<DataV>(tmp);
}

// Сложение векторов.
template<class DataV>
MVector<DataV> operator + (const MVector<DataV> &u,const MVector<DataV> &v)
{

 return MVector<DataV>(u.x+v.x,u.y+v.y,u.z+v.z);
}

// Вычитание векторов.
template<class DataV>
MVector<DataV> operator - (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return MVector<DataV>(u.x-v.x,u.y-v.y,u.z-v.z);
}

// Векторное умножение векторов.
template<class DataV>
MVector<DataV> operator ^ (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return MVector<DataV>(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}

// Скалярное умножение векторов.
template<class DataV>
DataV operator * (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return u.x*v.x+u.y*v.y+u.z*v.z;
}


// Умножение вектора и числа справа.
template<class DataV>
MVector<DataV> operator * (const MVector<DataV> &v,DataV f)
{
 return MVector<DataV>(v.x*f,v.y*f,v.z*f);
}

// Умножение вектора и числа слева.
template<class DataV>
MVector<DataV> operator * (DataV f,const MVector<DataV> &v)
{
 return MVector<DataV>(f*v.x,f*v.y,f*v.z);
}

// Деление вектора и числа.
template<class DataV>
MVector<DataV> operator / (const MVector<DataV> &v,DataV f)
{
 return MVector<DataV>(v.x/f,v.y/f,v.z/f);
}

// Покомпонентное деление в-ров
template<class DataV>
MVector<DataV> operator / (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return MVector<DataV>(u.x/v.x,u.y/v.y,u.z/v.z);
}
// ##############################


// ## Потоковые операторы ввода-вывода ##
// Вывод в массив
template<class DataV>
unsigned char* operator >> (const MVector<DataV> &v,unsigned char* p)
{
 if(!p)
  return 0;

 std::memcpy(p,&v,sizeof(DataV)*3);
 return p+sizeof(DataV)*3;
}

// Ввод из массива
template<class DataV>
const unsigned char* operator << (MVector<DataV> &v, const unsigned char* p)
{
 if(!p)
  return 0;

 std::memcpy(&v,p,sizeof(DataV)*3);
 return p+sizeof(DataV)*3;
}
// ######################################
					  */
}
#endif

