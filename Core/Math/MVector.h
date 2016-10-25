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
// ������������ � �����������
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
// ����� ������ ������� � ���������� �������
// -----------------------------------
/// ���������� ����������� �������
virtual int GetDimensions(void) const;

/// ���������� ����� ��������� �� ���� ������������
virtual MMatrixSize GetMatrixSize(void) const;

/// ������������� ����� ��������� �� ���� ������������
virtual bool Resize(const MMatrixSize &size);
// -----------------------------------

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������
MVector<T,Rows>& operator = (const MVector<T,Rows> &copy);
MVector<T,Rows>& operator = (const MMatrix<T,Rows,1> &copy);
//MVector<T,Rows>& operator = (const T* data);
MVector<T,Rows>& operator = (T value);

//void Assign(const T *data);
void Assign(T value);

// ������ � ��������
T& operator () (int i);
const T& operator () (int i) const;
// --------------------------

};

// --------------------------
// ������������ � �����������
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
// ����� ������ ������� � ���������� �������
// -----------------------------------
/// ���������� ����������� �������
template<class T, unsigned Rows>
int MVector<T,Rows>::GetDimensions(void) const
{
 return 1;
}

/// ���������� ����� ��������� �� ���� ������������
template<class T, unsigned Rows>
MMatrixSize MVector<T,Rows>::GetMatrixSize(void) const
{
 std::vector<int> dims;
 dims.assign(1,Rows);
 MMatrixSize size(dims);

 return size;
}

/// ������������� ����� ��������� �� ���� ������������
template<class T, unsigned Rows>
bool MVector<T,Rows>::Resize(const MMatrixSize &size)
{
 if(size.GetDimensions() != 1)
  return false;

 return (size[0] == Rows);
}
// -----------------------------------

// --------------------------
// ��������� ���������� �������
// --------------------------
// �������� ������������
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

// ������ � ��������
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
// �������������� ���������
// --------------------------
// ��������� ������������ ��������
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

// ��������� ������������ ��������
template<class T>
MVector<T,3> operator ^ (const MVector<T,3> &u, const MVector<T,3> &v)
{
 return MVector<T,3>(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}
// --------------------------




/*
  ����� - MVector. ���������� ����� ��� ������ - 3D-������.
 ������ ����� ������� ����. ���������:
  1. MVector<type> v; - �������� �-� ������� �����.
  2. MVector<type> v( (type)num ); - �������� �-� ��� �����.
                                               �������� = num.
  3. MVector<type> v( (type)x,(type)y,(type)z );
      - �������� �-� � �����. x,y,z.
  4. MVector<type> v( (MVector<type>)v1 );
      - �������� �-� ������ �-�� v1
 --------------------------------------

 � ������ ����������� ��������� ��������:
   ## ��������� ��������� ##
   MVector<type> []; - ������ � �����. �-�� �� �� ������. ������.
   ! MVector<type>; - ��������� ����� ������� ( �����. type )
   MVector<type>.Normalize(); - ������� ������������ �������.
   Normalize(MVector<type>); - ������� ������ �������������� �������.
   ## ��������� ������������ ##
   MVector<type> = MVector<type>; - ������������ �-�� �-��.
   MVector<type> = type; - ������������ ���� �����. �-�� �������� type

   ## ���������� ��������� ##
   MVector<type> == MVector<type>; - �������� �� ���������.
   MVector<type> != MVector<type>; - �������� �� �����������.
   MVector<type> < type; - �������� "��� �����. ������ ������".
   MVector<type> > type; - �������� "��� �����. ������ ������".

   ## �������������� ��������� ##
   - MVector<type>; - ����� ����� � ���� �����. �-��.
   MVector<type> += MVector<type>; - ���������� � ������� �������.
   MVector<type> -= MVector<type>; - ��������� �� ������� �������.
   MVector<type> ^= MVector<type>; - ��������� ���������� ������� �� ������.
   MVector<type> *= type; - ��������� �-�� �� �����
   MVector<type> /= type; - ������� �-�� �� �����
   MVector<type> /= MVector<type>; - ��������� ������� �-�� �� �-�
   MVector<type> + MVector<type>; - �������� ��������.
   MVector<type> - MVector<type>; - ��������� ��������.
   MVector<type> ^ MVector<type>; - ��������� ��������� ��������.
   MVector<type> * MVector<type>; - ��������� ��������� ��������.
   MVector<type> * type; - ��������� ������� � ����� ������.
   type * MVector<type>; - ��������� ������� � ����� �����.
   MVector<type> / type; - ������� ������� � �����.
   MVector<type> / MVector<type>; - ��������� ������� �-���

   ## ��������� ��������� �����-������ ##
   ostream << MVector<type>; - ����� � ����� ( ��� ������ cout ), � ������������� �����.
   istream >> MVector<type>; - ���� �� ������ ( ��� ������ cin ), � �������� ���� "v=".
   fstream << MVector<type>; - ����� � �������� �����.
   fstream >> MVector<type>; - ���� �� ��������� ������.
 ----------------------------------------
*/
    /*
template<class DataV>
class MVector
{
public: // ������
DataV x,y,z; // ���������� �-��.

public: // ������
//---------------------------
// ������������ � �����������
//---------------------------
//  ������ �-� ������� �����.
MVector(void)
{ x=y=z=0; };

// ������ �-� ��� �����. �������� = ksi.
MVector(DataV ksi)
{ x=y=z=ksi; };

// ������ �-� � �����. xx,yy,zz.
MVector(DataV xx,DataV yy,DataV zz)
{ x=xx; y=yy; z=zz; };

// ������ �-� ������ �-�� v
MVector(const MVector<DataV> &v)
{ x=v.x; y=v.y; z=v.z; }
//---------------------------

//---------------------------
// ������������� ���������
//---------------------------
// ## ��������� ��������� ##
// ������ � �����. �-�� �� �� ������. ������.
DataV& operator [](int n)
{
 return *(&x+n);
}

// ��������� ����� �������
DataV operator !(void) const
{
 return std::sqrt(x*x+y*y+z*z);
}
// ������� ������������ �������.
MVector<DataV>& Normalize(void)
{
 DataV len=sqrt(x*x+y*y+z*z);
 if(len != 0) *this/=len;
 return *this;
}

// ������� ������ �������������� �������.
MVector<DataV> Normalize(const MVector<DataV> &v);
// #########################

// ## ��������� ������������ ##
// ������������ �-�� �-��.
MVector<DataV>& operator = (const MVector<DataV> &v)
{
 x=v.x; y=v.y; z=v.z;
 return *this;
}

// ������������ ���� �����. �-�� �������� DataV
MVector<DataV>& operator = (DataV f)
{
 x=y=z=f;
 return *this;
}
// ############################


// ## ���������� ��������� ##
// �������� �� ���������.
bool operator == (const MVector<DataV>& v) const
{
 if(x==v.x && y==v.y && z==v.z)
  return true;
 else
  return false;
}

// �������� �� �����������.
bool operator != (const MVector<DataV>& v) const
{
 return !(*this == v);
}

// �������� "��� �����. ������ ������".
bool operator < (DataV v)
{
 return x<v && y<v && z<v;
}

// �������� "��� �����. ������ ������".
bool operator > (DataV v)
{
 return x>v && y>v && z>v;
}
// ##########################

// ## �������������� ��������� ##
// ����� ����� � ���� �����. �-��.
MVector<DataV> operator - (void) const
{
 return MVector<DataV> (-x,-y,-z);
}

// ���������� � ������� �������.
MVector<DataV>& operator += (const MVector<DataV> &v)
{
 x+=v.x;
 y+=v.y;
 z+=v.z;
 return *this;
}

// ��������� �� ������� �������.
MVector<DataV>& operator -= (const MVector<DataV> &v)
{
 x-=v.x;
 y-=v.y;
 z-=v.z;
 return *this;
}

// ��������� ���������� ������� �� ������.
MVector<DataV>& operator ^= (const MVector<DataV> &v)
{
 MVector<DataV> temp(*this);

 x=temp.y*v.z-temp.z*v.y;
 y=temp.z*v.x-temp.x*v.z;
 z=temp.x*v.y-temp.y*v.x;
 return *this;
}

// ��������� �-�� �� �����
MVector<DataV>& operator *= (DataV f)
{
 x*=f;
 y*=f;
 z*=f;
 return *this;
}

// ������� �-�� �� �����
MVector<DataV>& operator /= (DataV f)
{
 x/=f;
 y/=f;
 z/=f;
 return *this;
}

// ��������� ������� �-�� �� �-�
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

// �������� ��������.
template<class DataV>
MVector<DataV> operator + (const MVector<DataV> &u,const MVector<DataV> &v)
{

 return MVector<DataV>(u.x+v.x,u.y+v.y,u.z+v.z);
}

// ��������� ��������.
template<class DataV>
MVector<DataV> operator - (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return MVector<DataV>(u.x-v.x,u.y-v.y,u.z-v.z);
}

// ��������� ��������� ��������.
template<class DataV>
MVector<DataV> operator ^ (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return MVector<DataV>(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}

// ��������� ��������� ��������.
template<class DataV>
DataV operator * (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return u.x*v.x+u.y*v.y+u.z*v.z;
}


// ��������� ������� � ����� ������.
template<class DataV>
MVector<DataV> operator * (const MVector<DataV> &v,DataV f)
{
 return MVector<DataV>(v.x*f,v.y*f,v.z*f);
}

// ��������� ������� � ����� �����.
template<class DataV>
MVector<DataV> operator * (DataV f,const MVector<DataV> &v)
{
 return MVector<DataV>(f*v.x,f*v.y,f*v.z);
}

// ������� ������� � �����.
template<class DataV>
MVector<DataV> operator / (const MVector<DataV> &v,DataV f)
{
 return MVector<DataV>(v.x/f,v.y/f,v.z/f);
}

// �������������� ������� �-���
template<class DataV>
MVector<DataV> operator / (const MVector<DataV> &u,const MVector<DataV> &v)
{
 return MVector<DataV>(u.x/v.x,u.y/v.y,u.z/v.z);
}
// ##############################


// ## ��������� ��������� �����-������ ##
// ����� � ������
template<class DataV>
unsigned char* operator >> (const MVector<DataV> &v,unsigned char* p)
{
 if(!p)
  return 0;

 std::memcpy(p,&v,sizeof(DataV)*3);
 return p+sizeof(DataV)*3;
}

// ���� �� �������
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

