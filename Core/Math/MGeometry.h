/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MGeometryH
#define MGeometryH

#include <vector>
#include <string>
#include <stddef.h>
//#include "MTheormec.h"
#include "MVector.h"
#include "MDVector.h"


namespace RDK {

// -----------------------------------------------------------------
// ��������������� ������� ������������ � ��������� ������� ����������
// -----------------------------------------------------------------
// ��������� ������� ������� ���������� �� ������� �������� � ������ �����������
template<class T>
void SplitEcc(const MMatrix<T,4,4>& ecc, MMatrix<T,3,3>& rotation, MMatrix<T,3,1> &translation)
{
 GetSubMatrix(ecc,0,0,rotation);
 GetSubMatrix(ecc,0,3,translation);
}

// �������� ������� ������� ���������� �� ������� �������� � ������� �����������
template<class T>
void MergeEcc(const MMatrix<T,3,3>& rotation, const MMatrix<T,3,1> &translation, MMatrix<T,4,4>& ecc)
{
 ecc=T(0);
 SetSubMatrix(ecc,0, 0, rotation);
 SetSubMatrix(ecc,0, 3, translation);
 ecc.Data[3][3]=1;
}

// �������� ������� ������� ����������
template<class T>
void InverseEcc(const MMatrix<T,4,4>& source_ecc,MMatrix<T,4,4>& dest_ecc)
{
 MMatrix<T,3,3> rotation;
 MMatrix<T,3,1> translation;
 SplitEcc(source_ecc, rotation, translation);
 MergeEcc(rotation.Transpose(),-rotation.Transpose()*translation,dest_ecc);
}

// ����������� ������� ���������� ������� �� �� ������� � �� ������ ��� ����� �������
// ������� ���������� (��������������, ��� ������� ������� ���������� ���������)
// ���� ���������� � ��������, ���������� � ������
template<class T>
MMatrix<T, 4,4> CalcObjectPositionMatrix(const MVector<T,3> &angles, const MVector<T,3> &shifts)
{
	MMatrix<double, 4,4> res;

	double cos_gamma, sin_gamma, cos_beta, sin_beta, cos_alpha, sin_alpha;
	cos_gamma       = cos(angles(0));
	sin_gamma       = sin(angles(0));
	cos_beta       = cos(angles(1));
	sin_beta       = sin(angles(1));
	cos_alpha       = cos(angles(2));
	sin_alpha       = sin(angles(2));

	res(0,0)=cos_alpha*cos_beta;
	res(1,0)=-sin_gamma*sin_beta*cos_alpha+cos_gamma*sin_alpha;
	res(2,0)=cos_gamma*sin_beta*cos_alpha+sin_gamma*sin_alpha;
	res(0,1)=-cos_beta * sin_alpha;
	res(1,1)=sin_gamma*sin_beta*sin_alpha+cos_gamma*cos_alpha;
	res(2,1)=-cos_gamma*sin_beta*sin_alpha+sin_gamma*cos_alpha;
	res(0,2)=-sin_beta;
	res(1,2)=-sin_gamma*cos_beta;
	res(2,2)=cos_gamma*cos_beta;

	res=res.Transpose(); //����� �� ������ ����, ���� ��� �������� - ������ ������ � ������ �����

	res(0,3)=shifts(0);
	res(1,3)=shifts(1);
	res(2,3)=shifts(2);

	res(3,0)=0;
	res(3,1)=0;
	res(3,2)=0;
	res(3,3)=1;
 return res;
}

template<class T>
MDMatrix<T> CalcObjectPositionMatrixD(const MDVector<T> &angles, const MDVector<T> &shifts)
{
	MDMatrix<T> res(4,4);

	T cos_gamma, sin_gamma, cos_beta, sin_beta, cos_alpha, sin_alpha;
	cos_gamma       = cos(angles(0));
	sin_gamma       = sin(angles(0));
	cos_beta       = cos(angles(1));
	sin_beta       = sin(angles(1));
	cos_alpha       = cos(angles(2));
	sin_alpha       = sin(angles(2));

	res(0,0)=cos_alpha*cos_beta;
	res(1,0)=-sin_gamma*sin_beta*cos_alpha+cos_gamma*sin_alpha;
	res(2,0)=cos_gamma*sin_beta*cos_alpha+sin_gamma*sin_alpha;
	res(0,1)=-cos_beta * sin_alpha;
	res(1,1)=sin_gamma*sin_beta*sin_alpha+cos_gamma*cos_alpha;
	res(2,1)=-cos_gamma*sin_beta*sin_alpha+sin_gamma*cos_alpha;
	res(0,2)=-sin_beta;
	res(1,2)=-sin_gamma*cos_beta;
	res(2,2)=cos_gamma*cos_beta;

	res(0,3)=shifts(0);
	res(1,3)=shifts(1);
	res(2,3)=shifts(2);

	res(3,0)=0;
	res(3,1)=0;
	res(3,2)=0;
	res(3,3)=1;
 return res;
}

template<class T>
void CalcObjectAnglesAndShiftsD(const MDMatrix<T> &ExtMat, MDVector<T> &angles, MDVector<T> &shifts)
{
	T C, trX, trY;

	angles(1) = -asin( ExtMat(0,2));        /* ���������� ���� �������� ������ ��� Y */
    C           =  cos( angles(1) );

    if ( fabs( C ) > 0.005 )          /* "��������� �����" (Gimball lock)? */
      {
      trX      =  ExtMat(2,2) / C;        /* ���� ���, �� �������� ���� �������� ������ ��� X */
      trY      = -ExtMat(1,2) / C;

      angles(0)  = atan2( trY, trX );

      trX      =  ExtMat(0,0) / C;            /* �������� ���� �������� ������ ���  Z */
      trY      =  -ExtMat(0,1) / C;

      angles(2)  = atan2( trY, trX );
      }
    else                                 /* ����� ����� "��������� �����" (Gimball lock) */
      {
      angles(0)  = 0;                      /* ���� �������� ������ ��� X ������������ � ���� */

      trX      = ExtMat(1,1);                 /* � ��������� ���� �������� ������ ��� Z */
      trY      = ExtMat(1,0);

      angles(2)  = atan2( trY, trX );
      }

	shifts(0)=ExtMat(0,3);
	shifts(1)=ExtMat(1,3);
	shifts(2)=ExtMat(2,3);

	return;
}

// ����������� ������� ������� ����������
// ���� ���������� � ��������, ���������� � ������
template<class T>
MMatrix<T, 4,4> CalcCameraPositionMatrix(const MVector<T,3> &angles, const MVector<T,3> &shifts)
{
 MMatrix<double, 4,4> res;
 MMatrix<double, 3,3> res3;
 MVector<double, 3> r_shifts;

 double cos_gamma, sin_gamma, cos_beta, sin_beta, cos_alpha, sin_alpha;
 cos_gamma       = cos(angles(0));
 sin_gamma       = sin(angles(0));
 cos_beta       = cos(angles(1));
 sin_beta       = sin(angles(1));
 cos_alpha       = cos(angles(2));
 sin_alpha       = sin(angles(2));

 res3(0,0)=res(0,0)=cos_alpha*cos_beta;
 res3(1,0)=res(1,0)=sin_alpha*cos_gamma+cos_alpha*sin_beta*sin_gamma;
 res3(2,0)=res(2,0)=-cos_gamma*sin_beta*cos_alpha+sin_alpha*sin_gamma;
 res3(0,1)=res(0,1)=sin_alpha*cos_beta;
 res3(1,1)=res(1,1)=-sin_alpha*sin_beta*sin_gamma+cos_alpha*cos_gamma;
 res3(2,1)=res(2,1)=sin_alpha*sin_beta*cos_gamma+cos_alpha*sin_gamma;
 res3(0,2)=res(0,2)=sin_beta;
 res3(1,2)=res(1,2)=-cos_beta*sin_gamma;
 res3(2,2)=res(2,2)=cos_beta*cos_gamma;

 res=res.Transpose();
 res3=res3.Transpose();
 r_shifts=-res3*shifts;

 res(0,3)=r_shifts(0);
 res(1,3)=r_shifts(1);
 res(2,3)=r_shifts(2);

 res(3,0)=0;
 res(3,1)=0;
 res(3,2)=0;
 res(3,3)=1;
 return res;
}
// -----------------------------------------------------------------

template<class T, int Rows>
class MRay
{
public: // ������
// ������ ����
MVector<T,Rows> Origin;

// ����������� ����
MVector<T,Rows> Direction;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MRay(void)
{};
MRay(const MRay &copy)
{ *this=copy; };
MRay(MVector<T,Rows> &o, MVector<T,Rows> &d)
{ Origin=o; Direction=d; };
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
inline MRay& operator = (const MRay &copy)
{
 Origin=copy.Origin;
 Direction=copy.Direction;
 return *this;
}

// ��������� ���������
inline bool operator == (const MRay &v) const
{
 return (Origin == v.Origin) & (Direction == v.Direction);
}

inline bool operator != (const MRay &v) const
{
 return !operator == (v);
}
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ��������� ������ �����, ��������� ����� ���� �� ���������� t
inline MVector<T,Rows> CalcPoint(T t)
{ return Origin+Direction*t; };

// ��������� ����� ����������� �����
// ���������� true ���� ����������� ����������
bool CalcIntersection(const MRay<T,Rows> &r, MVector<T,Rows> &res)
{
 T t;
 MVector<T,Rows> v1,v2;
 int i;

 v1=r.Origin-Origin;
 v2=Direction-r.Direction;
 if(!v1 == 0)
  return false;

 for(i=0;i<3;i++)
  if(v1[i] != 0)
  {
   t=v1[i]/v2[i];
   break;
  }

 for(i=0;i<3;i++)
  if(v2[i]*t != v1[i])
   return false;

 res=CalcPoint(t);
 return true;
}
// --------------------------
};

// ����� ���������
template<class T, int Rows>
class MPlane
{
public: // ������
// ������� � ���������
MVector<T,Rows> Normal;

// �������� ����� ��������� ����� ������� �� ������ ���������
T Distance;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MPlane(void)
{ Normal=0; Distance=0; };
MPlane(const MPlane &copy)
{ *this=copy; };
MPlane(MVector<T,Rows> n, T dist)
{ Normal=n; Distance=dist; };
virtual ~MPlane(void){};
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
inline MPlane& operator = (const MPlane &copy)
{
 Normal=copy.Normal;
 Distance=copy.Distance;
 return *this;
}

// ��������� ���������
inline bool operator == (const MPlane &v) const
{
 return (Normal == v.Normal) & (Distance == v.Distance);
}

inline bool operator != (const MPlane &v) const
{
 return !operator == (v);
}
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ��������� ����� ����������� ���� � ����������
// ���������� true ���� ����������� ����������
bool CalcIntersection(MRay<T,Rows> ray, MVector<T,Rows> &p)
{
 T alfa,beta,t;

 if( (alfa=Normal&ray.dir)==0 ) return false;
 beta=-(Normal&ray.org)+Distance;
 if( (t=beta/alfa)<0 ) return false;

 p=ray.CalcPoint(t);
 return true;
};

// --------------------------
};

typedef std::vector<int> MBorder;

// ��������� �������
template<class T, int Rows>
class MGeometry
{
protected: // ������
// ������� ������
std::vector<MVector<T,Rows> > Vertices;

// ����� ������
std::vector<std::string> VerticesNames;

// ������� �������
std::vector<MBorder> Borders;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MGeometry(void);
MGeometry(const MGeometry<T,Rows> &copy);
MGeometry(size_t na, size_t nf);
virtual ~MGeometry(void);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
void Clear(void);

// ����� ������
size_t GetNumVertices(void) const;
bool SetNumVertices(size_t value);

// ����� ������
size_t GetNumBorders(void) const;
bool SetNumBorders(size_t value);

// ������� ������
const MVector<T,Rows>& Vertex(int index) const;
MVector<T,Rows>& Vertex(int index);

// ����� �������� ������
const std::string& VertexName(int index) const;
std::string& VertexName(int index);

// ������� �������
const MBorder& Border(int index) const;
MBorder& Border(int index);

// ������� ������
const std::vector<MVector<T,Rows> >& GetVertices(void) const;
bool SetVertices(const std::vector<MVector<T,Rows> >& value);

// ����� ������
const std::vector<std::string>& GetVerticesNames(void) const;
bool SetVerticesNames(const std::vector<std::string>& value);

// ������� �������
const std::vector<MBorder>& GetBorders(void) const;
bool SetBorders(const std::vector<MBorder>& value);
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
MGeometry<T,Rows>& operator = (const MGeometry<T,Rows> &copy);

// ��������� ���������
bool operator == (const MGeometry<T,Rows> &v) const;
bool operator != (const MGeometry<T,Rows> &v) const;
// --------------------------
};


// ****************************************************************************
// ****************************************************************************
// ���������� �������
// ****************************************************************************
// ****************************************************************************
// MGeometry
// --------------------------
// ������������ � �����������
// --------------------------
template<class T, int Rows>
MGeometry<T,Rows>::MGeometry(void)
{
}

template<class T, int Rows>
MGeometry<T,Rows>::MGeometry(const MGeometry &copy)
{
 *this=copy;
}

template<class T, int Rows>
MGeometry<T,Rows>::MGeometry(size_t na, size_t nf)
{
 SetNumVertices(na);
 SetNumBorders(nf);
}

template<class T, int Rows>
MGeometry<T,Rows>::~MGeometry(void)
{
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ������� ������
template<class T, int Rows>
void MGeometry<T,Rows>::Clear(void)
{
 Vertices.clear();
 Borders.clear();
 VerticesNames.clear();
}

// ����� ������
template<class T, int Rows>
size_t MGeometry<T,Rows>::GetNumVertices(void) const
{
 return Vertices.size();
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetNumVertices(size_t value)
{
 Vertices.resize(value);
 VerticesNames.resize(value);
 return true;
}

// ����� ������
template<class T, int Rows>
size_t MGeometry<T,Rows>::GetNumBorders(void) const
{
 return Borders.size();
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetNumBorders(size_t value)
{
 Borders.resize(value);
 return true;
}

// ������� ������
template<class T, int Rows>
const MVector<T,Rows>& MGeometry<T,Rows>::Vertex(int index) const
{
 return Vertices[index];
}

template<class T, int Rows>
MVector<T,Rows>& MGeometry<T,Rows>::Vertex(int index)
{
 return Vertices[index];
}

// ����� �������� ������
template<class T, int Rows>
const std::string& MGeometry<T,Rows>::VertexName(int index) const
{
 return VerticesNames[index];
}

template<class T, int Rows>
std::string& MGeometry<T,Rows>::VertexName(int index)
{
 return VerticesNames[index];
}

// ������� �������
template<class T, int Rows>
const MBorder& MGeometry<T,Rows>::Border(int index) const
{
 return Borders[index];
}

template<class T, int Rows>
MBorder& MGeometry<T,Rows>::Border(int index)
{
 return Borders[index];
}

// ������� ������
template<class T, int Rows>
const std::vector<MVector<T,Rows> >& MGeometry<T,Rows>::GetVertices(void) const
{
 return Vertices;
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetVertices(const std::vector<MVector<T,Rows> >& value)
{
 Vertices=value;
 return true;
}

// ����� ������
template<class T, int Rows>
const std::vector<std::string>& MGeometry<T,Rows>::GetVerticesNames(void) const
{
 return VerticesNames;
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetVerticesNames(const std::vector<std::string>& value)
{
 VerticesNames=value;
 VerticesNames.resize(Vertices.size());
 return true;
}

// ������� �������
template<class T, int Rows>
const std::vector<MBorder>& MGeometry<T,Rows>::GetBorders(void) const
{
 return Borders;
}

template<class T, int Rows>
bool MGeometry<T,Rows>::SetBorders(const std::vector<MBorder>& value)
{
 Borders=value;
 return true;
}
// --------------------------

// --------------------------
// ��������� ����������
// --------------------------
// �������� �����������
template<class T, int Rows>
MGeometry<T,Rows>& MGeometry<T,Rows>::operator = (const MGeometry<T,Rows> &copy)
{
 // ������� ������
 Vertices=copy.Vertices;

 VerticesNames=copy.VerticesNames;

 // ������� �������
 Borders=copy.Borders;

 return *this;
}

// ��������� ���������
template<class T, int Rows>
bool MGeometry<T,Rows>::operator == (const MGeometry<T,Rows> &v) const
{
 return (Vertices == v.Vertices) & (Borders == v.Borders) & (VerticesNames == v.VerticesNames);
}

template<class T, int Rows>
bool MGeometry<T,Rows>::operator != (const MGeometry<T,Rows> &v) const
{
 return (Vertex != v.Vertex) | (Borders != v.Borders) | (VerticesNames != v.VerticesNames);
}
// --------------------------

}
#endif
