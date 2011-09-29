/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef MDVECTOR_CPP
#define MDVECTOR_CPP

#include <math.h>
#include "MDVector.h"

namespace RDK {

#ifdef MVECTOR_H
#else

//class MDVector
// Методы
//---------------------------
// Конструкторы и деструкторы
//---------------------------
//  Создаёт в-р нулевой длины.
MDVector::MDVector(void)
{ x=y=z=0; };

// Создаёт в-р все коорд. которого = ksi.
MDVector::MDVector(double ksi)
{ x=y=z=ksi; };

// Создаёт в-р с коорд. xx,yy,zz.
MDVector::MDVector(double xx,double yy,double zz)
{ x=xx; y=yy; z=zz; };

// Создаёт в-р равный в-ру v
MDVector::MDVector(const MDVector &v)
{ x=v.x; y=v.y; z=v.z; }
//---------------------------

//---------------------------
// Перегружаемые операторы
//---------------------------
// ## Сервисные операторы ##
// Доступ к коорд. в-ра по их порядк. номеру.
double& MDVector::operator [](int n)
{
 return *(&x+n);
}

// Получение длины вектора
double MDVector::operator !(void) const
{
 return sqrt(x*x+y*y+z*z);
}
// Функция нормирования вектора.
MDVector& MDVector::Normalize(void)
{
 double len=sqrt(x*x+y*y+z*z);
 if(len != 0) *this/=len;
 return *this;
}

// #########################

// ## Операторы присваивания ##
// Присваивание в-ра в-ру.
MDVector& MDVector::operator = (const MDVector &v)
{
 x=v.x; y=v.y; z=v.z;
 return *this;
}

// Присваивание всем коорд. в-ра значения double
MDVector& MDVector::operator = (double f)
{
 x=y=z=f;
 return *this;
}
// ############################


// ## Логические операторы ##
// Проверка на равенство.
bool MDVector::operator == (const MDVector& v) const
{
 if(x==v.x && y==v.y && z==v.z)
  return true;
 else
  return false;
}

// Проверка на неравенство.
bool MDVector::operator != (const MDVector& v) const
{
 return !(*this == v);
}

// Проверка "все коорд. строго меньше".
bool MDVector::operator < (double v)
{
 return x<v && y<v && z<v;
}

// Проверка "все коорд. строго больше".
bool MDVector::operator > (double v)
{
 return x>v && y>v && z>v;
}
// ##########################

// ## Арифметическии операторы ##
// Смена знака у всех коорд. в-ра.
MDVector MDVector::operator - (void) const
{
 return MDVector (-x,-y,-z);
}

// Добавление к вектору вектора.
MDVector& MDVector::operator += (const MDVector &v)
{
 x+=v.x;
 y+=v.y;
 z+=v.z;
 return *this;
}

// Вычитание из вектора вектора.
MDVector& MDVector::operator -= (const MDVector &v)
{
 x-=v.x;
 y-=v.y;
 z-=v.z;
 return *this;
}

// Векторное домножение вектора на вектор.
MDVector& MDVector::operator ^= (const MDVector &v)
{
 MDVector temp(*this);

 x=temp.y*v.z-temp.z*v.y;
 y=temp.z*v.x-temp.x*v.z;
 z=temp.x*v.y-temp.y*v.x;
 return *this;
}

// Умножение в-ра на число
MDVector& MDVector::operator *= (double f)
{
 x*=f;
 y*=f;
 z*=f;
 return *this;
}

// Деление в-ра на число
MDVector& MDVector::operator /= (double f)
{
 x/=f;
 y/=f;
 z/=f;
 return *this;
}

// Почленное деление в-ра на в-р
MDVector& MDVector::operator /= (const MDVector &v)
{
 x/=v.x;
 y/=v.y;
 z/=v.z;
 return *this;
}

// Нормировка
MDVector Normalize(const MDVector &v)
{
 MDVector tmp;
 double len=sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
 if(len != 0) tmp=v/len;
 return MDVector(tmp);
}

// Сложение векторов.
MDVector operator + (const MDVector &u,const MDVector &v)
{
 return MDVector(u.x+v.x,u.y+v.y,u.z+v.z);
}

// Вычитание векторов.
MDVector operator - (const MDVector &u,const MDVector &v)
{
 return MDVector(u.x-v.x,u.y-v.y,u.z-v.z);
}

// Векторное умножение векторов.
MDVector operator ^ (const MDVector &u,const MDVector &v)
{
 return MDVector(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}

// Скалярное умножение векторов.
double operator * (const MDVector &u,const MDVector &v)
{
 return u.x*v.x+u.y*v.y+u.z*v.z;
}


// Умножение вектора и числа справа.
MDVector operator * (const MDVector &v,double f)
{
 return MDVector(v.x*f,v.y*f,v.z*f);
}

// Умножение вектора и числа слева.
MDVector operator * (double f,const MDVector &v)
{
 return MDVector(f*v.x,f*v.y,f*v.z);
}

// Деление вектора и числа.
MDVector operator / (const MDVector &v,double f)
{
 return MDVector(v.x/f,v.y/f,v.z/f);
}

// Покомпонентное деление в-ров
MDVector operator / (const MDVector &u,const MDVector &v)
{
 return MDVector(u.x/v.x,u.y/v.y,u.z/v.z);
}
// ##############################
#endif

}
#endif
