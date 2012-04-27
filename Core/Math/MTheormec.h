/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 1997.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef THEORMEC_H
#define THEORMEC_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MVector.h"
#include "MTensor.h"

namespace RDK {

// Тензор поворота
template<class DataT>
class MRotationTensor
{
public:
MVector<DataT,3> m;
DataT angle;

public:
MRotationTensor(void)
{ angle=0; };

MRotationTensor(const MVector<DataT,3> &v, DataT ang)
{ m=v; angle=ang; };

MRotationTensor(const MRotationTensor<DataT>& P)
{ m=P.m; angle=P.angle; };

// Перегружаемые операторы
MRotationTensor<DataT>& operator = (const MRotationTensor<DataT> &P )
{ m=P.m; angle=P.angle; return *this; };

MRotationTensor<DataT>& operator = (const MTensor<DataT> &P)
{
 m=0.0;
// for(size_t i=0;i<P.Dyads.size();i++) // Заглушка!! Пока не собирается оператор ^
//  m+=P.Dyads[i].d1^P.Dyads[i].d2;

 angle=0;
 for(size_t i=0;i<P.Dyads.size();i++)
  angle+=P.Dyads[i].d1*P.Dyads[i].d2;

 angle=((angle-1)/2);
 return *this;
};

MRotationTensor<DataT>& operator *= (const MRotationTensor<DataT> &P );

//MRotationTensor<DataT> operator * (const MRotationTensor<DataT> &P1
//                                        ,const MRotationTensor<DataT> &P2);
//MVector<DataT,3> operator *(const MRotationTensor<DataT> &P
//                                        ,const MVector<DataT,3> &v);

// Операторы сравнения
bool operator == (const MRotationTensor<DataT> &v) const;
bool operator != (const MRotationTensor<DataT> &v) const;

// powers
MRotationTensor<DataT>& Power2(void);
MRotationTensor<DataT>& Power(int pow);

// Вывод в массив
//friend unsigned char* operator >> (const MRotationTensor<DataT> &v, unsigned char* p);

// Ввод из массива
//friend const unsigned char* operator << (MRotationTensor<DataT> &v, const unsigned char* p);
};


template<class DataT>
MRotationTensor<DataT>& MRotationTensor<DataT>::operator *= (const MRotationTensor<DataT> &P )
{
 DataT sin_P=sin(P.angle),sin_this=sin(angle);

 if(sin_P==0) return *this;
 if(sin_this==0) m=P.m;
 else
  {
   DataT tang_P=sin_P/cos(P.angle);
   DataT ctang_this=cos(angle)/sin_this;
   m=m+ctang_this*tang_P*P.m+tang_P*(m^P.m);
   m=Normalize(m);
  }
 angle+=P.angle;
 if(angle>=M_PI)angle-=M_PI*2;
 else angle-=0;
 if(angle<=-M_PI)angle+=M_PI*2;
 else angle+=0;
 return *this;
}

template<class DataT>
MRotationTensor<DataT> operator * (const MRotationTensor<DataT> &P1
                              ,const MRotationTensor<DataT> &P2)
{
 DataT sin_P2=sin(P2.angle),sin_P1=sin(P1.angle);
 DataT angle;
 MVector<DataT,3> m;

 if(sin_P2==0) return P1;
 if(sin_P1==0) m=P2.m;
 else
  {
   DataT tang_P2=sin_P2/cos(P2.angle);
   DataT ctang_P1=cos(P1.angle)/sin_P1;
   m=P1.m+ctang_P1*tang_P2*P2.m+tang_P2*(P1.m^P2.m);
   m=Normalize(m);
  }
 angle=P1.angle+P2.angle;
 if(angle>=M_PI)angle-=M_PI*2;
 else angle-=0;
 if(angle<=-M_PI)angle+=M_PI*2;
 else angle+=0;
 return MRotationTensor<DataT>(m,angle);
}

template<class DataT>
MVector<DataT,3> operator *(const MRotationTensor<DataT> &P
                                ,const MVector<DataT,3> &v)
{
 MVector<DataT,3> ret,u=P.m;
 DataT cosa=cos(P.angle),sina=sin(P.angle);

 ret=(1-cosa)*P.m*(P.m*v)+cosa*v+sina*(P.m^v);
 return MVector<DataT,3>(ret);
}

// Операторы сравнения
template<class DataT>
bool MRotationTensor<DataT>::operator == (const MRotationTensor<DataT> &v) const
{
 return (m == v.m) & (angle == v.angle);
}

template<class DataT>
bool MRotationTensor<DataT>::operator != (const MRotationTensor<DataT> &v) const
{
 return (m != v.m) | (angle != v.angle);
}

// powers
template<class DataT>
MRotationTensor<DataT>& MRotationTensor<DataT>::Power2(void)
{
 angle+=angle;
 return *this;
}

template<class DataT>
MRotationTensor<DataT>& MRotationTensor<DataT>::Power(int pow)
{
 for(int i=1;i<pow;i++)
  angle+=angle;
 return *this;
}

// Вывод в массив
template<class DataT>
unsigned char* operator >> (const MRotationTensor<DataT> &v, unsigned char* p)
{
 if(!p)
  return 0;

 p=v.m>>p;
 memcpy(p,&v.angle,sizeof(DataT));
 return p+sizeof(DataT);
}

// Ввод из массива
template<class DataT>
const unsigned char* operator << (MRotationTensor<DataT> &v, const unsigned char* p)
{
 if(!p)
  return 0;

 p=v.m<<p;
 memcpy(&v.angle,p,sizeof(DataT));
 return p+sizeof(DataT);
}
// ######################################


/*
 Центральный тензор инерции. Его данные -
 главные оси и главные моменты инерции.
*/
template<class DataT>
class MInertiaTensor
{
public:
// Axis of inertia
MVector<DataT,3> d1,d2,d3;

// Moment of inertia
DataT m1,m2,m3;

public:
MInertiaTensor(void)
{ d1=d2=d3=0; m1=m2=m3=0; };

MInertiaTensor(const MInertiaTensor<DataT> &IT)
{
 d1=IT.d1; d2=IT.d2; d3=IT.d3;
 m1=IT.m1; m2=IT.m2; m3=IT.m3;
};

MInertiaTensor<DataT>& operator=(const MInertiaTensor<DataT> &IT)
{
 d1=IT.d1; d2=IT.d2; d3=IT.d3;
 m1=IT.m1; m2=IT.m2; m3=IT.m3;
 return *this;
};

template<class DataU> friend MVector<DataU,3> operator * (const MInertiaTensor<DataU> &,
                                  const MVector<DataU,3> &);

// Операторы сравнения
bool operator == (const MInertiaTensor<DataT> &v) const;
bool operator != (const MInertiaTensor<DataT> &v) const;

MInertiaTensor<DataT>& Rotate(MRotationTensor<DataT> &);
};

template<class DataT>
MVector<DataT,3> operator * (const MInertiaTensor<DataT> &I,
                             const MVector<DataT,3> &v)
{
 MVector<DataT,3> tmp;

 tmp=I.m1*(I.d1*v)*I.d1+I.m2*(I.d2*v)*I.d2+I.m3*(I.d3*v)*I.d3;
 return MVector<DataT,3>(tmp);
}


// Операторы сравнения
template<class DataT>
bool MInertiaTensor<DataT>::operator == (const MInertiaTensor<DataT> &v) const
{
 return (m1 == v.m1) & (m2 == v.m2) & (m3 == v.m3)
     & (d1 == v.d1) & (d2 == v.d2) & (d3 == v.d3);
}

template<class DataT>
bool MInertiaTensor<DataT>::operator != (const MInertiaTensor<DataT> &v) const
{
 return !operator == (v);
}

template<class DataT>
MInertiaTensor<DataT>& MInertiaTensor<DataT>::Rotate(MRotationTensor<DataT> &P)
{
 d1=P*d1; d2=P*d2; d3=P*d3;
 return *this;
}



/*
void main(void)
{
 MVector<DataT,3> v(0,23,0);
 MVector<DataT,3> u=MVector<DataT,3>(5);
 MVector<DataT,3> w(v);
 MRotationTensor<DataT> P=MRotationTensor<DataT>(MVector<DataT,3>(0,42,23),0.2);
 MInertiaTensor<DataT> I;

 w=v^u;
 u+=v;
 v+=v;
 v=P*w;
 P=P*P;
 u=I*v;
 I.Rotate(P);
 cout<<w*v;
 cout<<w<<v<<u;
}  */

}
#endif


