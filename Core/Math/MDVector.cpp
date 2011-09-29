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
// ������
//---------------------------
// ������������ � �����������
//---------------------------
//  ������ �-� ������� �����.
MDVector::MDVector(void)
{ x=y=z=0; };

// ������ �-� ��� �����. �������� = ksi.
MDVector::MDVector(double ksi)
{ x=y=z=ksi; };

// ������ �-� � �����. xx,yy,zz.
MDVector::MDVector(double xx,double yy,double zz)
{ x=xx; y=yy; z=zz; };

// ������ �-� ������ �-�� v
MDVector::MDVector(const MDVector &v)
{ x=v.x; y=v.y; z=v.z; }
//---------------------------

//---------------------------
// ������������� ���������
//---------------------------
// ## ��������� ��������� ##
// ������ � �����. �-�� �� �� ������. ������.
double& MDVector::operator [](int n)
{
 return *(&x+n);
}

// ��������� ����� �������
double MDVector::operator !(void) const
{
 return sqrt(x*x+y*y+z*z);
}
// ������� ������������ �������.
MDVector& MDVector::Normalize(void)
{
 double len=sqrt(x*x+y*y+z*z);
 if(len != 0) *this/=len;
 return *this;
}

// #########################

// ## ��������� ������������ ##
// ������������ �-�� �-��.
MDVector& MDVector::operator = (const MDVector &v)
{
 x=v.x; y=v.y; z=v.z;
 return *this;
}

// ������������ ���� �����. �-�� �������� double
MDVector& MDVector::operator = (double f)
{
 x=y=z=f;
 return *this;
}
// ############################


// ## ���������� ��������� ##
// �������� �� ���������.
bool MDVector::operator == (const MDVector& v) const
{
 if(x==v.x && y==v.y && z==v.z)
  return true;
 else
  return false;
}

// �������� �� �����������.
bool MDVector::operator != (const MDVector& v) const
{
 return !(*this == v);
}

// �������� "��� �����. ������ ������".
bool MDVector::operator < (double v)
{
 return x<v && y<v && z<v;
}

// �������� "��� �����. ������ ������".
bool MDVector::operator > (double v)
{
 return x>v && y>v && z>v;
}
// ##########################

// ## �������������� ��������� ##
// ����� ����� � ���� �����. �-��.
MDVector MDVector::operator - (void) const
{
 return MDVector (-x,-y,-z);
}

// ���������� � ������� �������.
MDVector& MDVector::operator += (const MDVector &v)
{
 x+=v.x;
 y+=v.y;
 z+=v.z;
 return *this;
}

// ��������� �� ������� �������.
MDVector& MDVector::operator -= (const MDVector &v)
{
 x-=v.x;
 y-=v.y;
 z-=v.z;
 return *this;
}

// ��������� ���������� ������� �� ������.
MDVector& MDVector::operator ^= (const MDVector &v)
{
 MDVector temp(*this);

 x=temp.y*v.z-temp.z*v.y;
 y=temp.z*v.x-temp.x*v.z;
 z=temp.x*v.y-temp.y*v.x;
 return *this;
}

// ��������� �-�� �� �����
MDVector& MDVector::operator *= (double f)
{
 x*=f;
 y*=f;
 z*=f;
 return *this;
}

// ������� �-�� �� �����
MDVector& MDVector::operator /= (double f)
{
 x/=f;
 y/=f;
 z/=f;
 return *this;
}

// ��������� ������� �-�� �� �-�
MDVector& MDVector::operator /= (const MDVector &v)
{
 x/=v.x;
 y/=v.y;
 z/=v.z;
 return *this;
}

// ����������
MDVector Normalize(const MDVector &v)
{
 MDVector tmp;
 double len=sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
 if(len != 0) tmp=v/len;
 return MDVector(tmp);
}

// �������� ��������.
MDVector operator + (const MDVector &u,const MDVector &v)
{
 return MDVector(u.x+v.x,u.y+v.y,u.z+v.z);
}

// ��������� ��������.
MDVector operator - (const MDVector &u,const MDVector &v)
{
 return MDVector(u.x-v.x,u.y-v.y,u.z-v.z);
}

// ��������� ��������� ��������.
MDVector operator ^ (const MDVector &u,const MDVector &v)
{
 return MDVector(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}

// ��������� ��������� ��������.
double operator * (const MDVector &u,const MDVector &v)
{
 return u.x*v.x+u.y*v.y+u.z*v.z;
}


// ��������� ������� � ����� ������.
MDVector operator * (const MDVector &v,double f)
{
 return MDVector(v.x*f,v.y*f,v.z*f);
}

// ��������� ������� � ����� �����.
MDVector operator * (double f,const MDVector &v)
{
 return MDVector(f*v.x,f*v.y,f*v.z);
}

// ������� ������� � �����.
MDVector operator / (const MDVector &v,double f)
{
 return MDVector(v.x/f,v.y/f,v.z/f);
}

// �������������� ������� �-���
MDVector operator / (const MDVector &u,const MDVector &v)
{
 return MDVector(u.x/v.x,u.y/v.y,u.z/v.z);
}
// ##############################
#endif

}
#endif
