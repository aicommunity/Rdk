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
//1.1
template<class T>
MMatrix<T,4,4> CalcObjectPositionMatrix(const MVector<T,3> &angles, const MVector<T,3> &shifts, int seqmat=1)
{
	MMatrix<T,4,4> res;
	MVector<T,6> anglesANDshifts;
	for(int i=0;i<3;i++)
		anglesANDshifts(i)=shifts(i);
	for(int i=3;i<6;i++)
		anglesANDshifts(i)=angles(i-3);
	res=CalcObjectPositionMatrix(anglesANDshifts, seqmat);
	return res;
}

//1.2!!!
template<class T>
MMatrix<T,4,4> CalcObjectPositionMatrix(const MVector<T,6> &anglesANDshifts, int seqmat=1)
{
 MMatrix<T,4,4> res;

	T cos_gamma, sin_gamma, cos_beta, sin_beta, cos_alpha, sin_alpha;
	cos_gamma       = cos(anglesANDshifts(3));
	sin_gamma       = sin(anglesANDshifts(3));
	cos_beta       = cos(anglesANDshifts(4));
	sin_beta       = sin(anglesANDshifts(4));
	cos_alpha       = cos(anglesANDshifts(5));
	sin_alpha       = sin(anglesANDshifts(5));

	MMatrix<T,3,3> Mx, My, Mz, M;
	
	Mx(0,0)=1; Mx(0,1)=0;		  Mx(0,2)=0;
	Mx(1,0)=0; Mx(1,1)=cos_gamma; Mx(1,2)=-sin_gamma;
	Mx(2,0)=0; Mx(2,1)=sin_gamma; Mx(2,2)=cos_gamma;

	My(0,0)=cos_beta;   My(0,1)=0; My(0,2)=sin_beta;
	My(1,0)=0;			My(1,1)=1; My(1,2)=0;
	My(2,0)=-sin_beta;  My(2,1)=0; My(2,2)=cos_beta;

	Mz(0,0)=cos_alpha; Mz(0,1)=-sin_alpha;	Mz(0,2)=0;
	Mz(1,0)=sin_alpha; Mz(1,1)=cos_alpha;	Mz(1,2)=0;
	Mz(2,0)=0;		   Mz(2,1)=0;			Mz(2,2)=1;

	//!!!������� ������������!!!
	switch (seqmat){
	case 2:
	M=Mx*Mz*My; break;
	case 3:
	M=My*Mx*Mz; break;
	case 4:
	M=My*Mz*Mx; break;
	case 5:
	M=Mz*Mx*My; break;
	case 6:
	M=Mz*My*Mx; break;
	default: M=Mx*My*Mz;
	}
	//!!!!!!

	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			res(i,j)=M(i,j);

	res(0,3)=anglesANDshifts(0);
	res(1,3)=anglesANDshifts(1);
	res(2,3)=anglesANDshifts(2);

	res(3,0)=0;
	res(3,1)=0;
	res(3,2)=0;
	res(3,3)=1;
 return res;
}

//2.1
template<class T>
void CalcObjectAnglesAndShifts(const MMatrix<T,4,4> &ExtMat, MVector<T,3> &angles, MVector<T,3> &shifts, int seqmat=1)
{
    MVector<T,6> anglesANDshifts;
	CalcObjectAnglesAndShifts(ExtMat, anglesANDshifts, seqmat)
	for(int i=0;i<3;i++)
		shifts(i)=anglesANDshifts(i);
	for(int i=3;i<6;i++)
		angles(i-3)=anglesANDshifts(i);
	return;
}

//2.2!!!
template<class T>
void CalcObjectAnglesAndShifts(const MMatrix<T,4,4> &ExtMat, MVector<T,6> &anglesANDshifts, int seqmat=1)
{
	T C, trX, trY;

		//!!!������� ������������!!!
	switch (seqmat)
	{
	case 2:
	{
		anglesANDshifts(5) = -asin( ExtMat(0,1));        // ���������� ���� �������� ������ ��� Z 
		C = cos( anglesANDshifts(5) );

		if ( fabs( C ) > 0.005 )          // "��������� �����" (Gimball lock)? 
		{
			trX      =  ExtMat(0,0) / C;        // ���� ���, �� �������� ���� �������� ������ ��� Y
			trY      = ExtMat(0,2) / C;

		anglesANDshifts(4)  = atan2( trY, trX );

		trX      =  ExtMat(1,1) / C;            // �������� ���� �������� ������ ���  X
		trY      =  ExtMat(2,1) / C;

		anglesANDshifts(3)  = atan2( trY, trX );
		}
	else                                 // ����� ����� "��������� �����" (Gimball lock) 
    {
      anglesANDshifts(3)  = 0;                      // ���� �������� ������ ��� X ������������ � ���� 

      trX      = ExtMat(2,2);                 // � ��������� ���� �������� ������ ��� Y 
      trY      = -ExtMat(2,0);

      anglesANDshifts(4)  = atan2( trY, trX );
    }
	break;
	} 
	case 3:
	{
		anglesANDshifts(3) = -asin( ExtMat(1,2));
		C = cos( anglesANDshifts(3) );

		if ( fabs( C ) > 0.005 )
		{
			trX      =  ExtMat(1,1) / C;
			trY      = ExtMat(1,0) / C;

		anglesANDshifts(5)  = atan2( trY, trX );

		trX      =  ExtMat(2,2) / C;
		trY      =  ExtMat(0,2) / C;

		anglesANDshifts(4)  = atan2( trY, trX );
		}
    else
      {
      anglesANDshifts(4)  = 0;

      trX      = ExtMat(0,0);
      trY      = -ExtMat(0,1);

      anglesANDshifts(5)  = atan2( trY, trX );
      }
	break;
	} 
	case 4:
	{
		anglesANDshifts(5) = asin( ExtMat(1,0));
		C = cos( anglesANDshifts(5) );

		if ( fabs( C ) > 0.005 )
		{
			trX      =  ExtMat(1,1) / C;
			trY      = -ExtMat(1,2) / C;

		anglesANDshifts(3)  = atan2( trY, trX );

		trX      =  ExtMat(0,0) / C;
		trY      =  -ExtMat(2,0) / C;

		anglesANDshifts(4)  = atan2( trY, trX );
		}
    else
      {
      anglesANDshifts(4)  = 0;

      trX      = ExtMat(2,2);
      trY      = ExtMat(2,1);

      anglesANDshifts(3)  = atan2( trY, trX );
      }
	break;
	} 
	case 5:
	{
		anglesANDshifts(3) = asin( ExtMat(2,1));
		C = cos( anglesANDshifts(3) );

		if ( fabs( C ) > 0.005 )
		{
			trX      =  ExtMat(2,2) / C;
			trY      = -ExtMat(2,0) / C;

		anglesANDshifts(4)  = atan2( trY, trX );

		trX      =  ExtMat(1,1) / C;
		trY      =  -ExtMat(0,1) / C;

		anglesANDshifts(5)  = atan2( trY, trX );
		}
    else
      {
      anglesANDshifts(5)  = 0;

      trX      = ExtMat(0,0);
      trY      = ExtMat(0,2);

      anglesANDshifts(4)  = atan2( trY, trX );
      }
	break;
	} 
	case 6:
	{
		anglesANDshifts(4) = -asin( ExtMat(2,0));
		C = cos( anglesANDshifts(4) );

		if ( fabs( C ) > 0.005 )
		{
			trX      =  ExtMat(2,2) / C;
			trY      = ExtMat(2,1) / C;

		anglesANDshifts(3)  = atan2( trY, trX );

		trX      =  ExtMat(0,0) / C;
		trY      =  ExtMat(1,0) / C;

		anglesANDshifts(5)  = atan2( trY, trX );
		}
    else
      {
      anglesANDshifts(5)  = 0;

      trX      = ExtMat(1,1);
      trY      = -ExtMat(1,2);

      anglesANDshifts(3)  = atan2( trY, trX );
      }
	break;
	} 
	default:
	{
		anglesANDshifts(4) = asin( ExtMat(0,2));        // ���������� ���� �������� ������ ��� Y 
		C = cos( anglesANDshifts(4) );

		if ( fabs( C ) > 0.005 )          // "��������� �����" (Gimball lock)? 
		{
			trX      =  ExtMat(2,2) / C;        // ���� ���, �� �������� ���� �������� ������ ��� X 
			trY      = -ExtMat(1,2) / C;

		anglesANDshifts(3)  = atan2( trY, trX );

		trX      =  ExtMat(0,0) / C;            // �������� ���� �������� ������ ���  Z 
		trY      =  -ExtMat(0,1) / C;

		anglesANDshifts(5)  = atan2( trY, trX );
		}
    else                                 // ����� ����� "��������� �����" (Gimball lock) 
      {
      anglesANDshifts(3)  = 0;                      // ���� �������� ������ ��� X ������������ � ���� 

      trX      = ExtMat(1,1);                 // � ��������� ���� �������� ������ ��� Z 
      trY      = ExtMat(1,0);

      anglesANDshifts(5)  = atan2( trY, trX );
      }
	} 
	}
	//!!!!!!

	anglesANDshifts(0)=ExtMat(0,3);
	anglesANDshifts(1)=ExtMat(1,3);
	anglesANDshifts(2)=ExtMat(2,3);

	return;
}

//2.2.1!!! //MMatrix-->MDVector
template<class T>
void CalcObjectAnglesAndShiftsM(const MMatrix<T,4,4> &ExtMat, MDMatrix<T> &anglesANDshifts, int seqmat=1)
{
	MDVector<T> AnS(6);

	MVector<T,6> M_anglesANDshifts;
	CalcObjectAnglesAndShifts(ExtMat, M_anglesANDshifts, seqmat);
	for(int i=0;i<6;i++)
		AnS(i)=M_anglesANDshifts(i);

	if(anglesANDshifts.GetCols()==1) anglesANDshifts=AnS;
    else anglesANDshifts=AnS.Transpose();

	return;
}

//3.1
template<class T>
MDMatrix<T> CalcObjectPositionMatrixD(const MDVector<T> &angles, const MDVector<T> &shifts, int seqmat=1)
{
	MDMatrix<T> res(4,4);
	MMatrix<T,4,4> M_res;
	MVector<T,6> M_anglesANDshifts;
	for(int i=0;i<3;i++)
		M_anglesANDshifts(i)=shifts(i);
	for(int i=3;i<6;i++)
		M_anglesANDshifts(i)=angles(i-3);

	M_res=CalcObjectPositionMatrix(M_anglesANDshifts, seqmat);

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			res(i,j)=M_res(i,j);

	return res;
}

//3.2!!!
template<class T>
MDMatrix<T> CalcObjectPositionMatrixD(const MDMatrix<T> &anglesANDshifts, int seqmat=1)
{
	MDMatrix<T> res(4,4);
	MMatrix<T,4,4> M_res;
	MVector<T,6> M_anglesANDshifts;
	MDVector<T> AnS(6);
    if(anglesANDshifts.GetCols()==1) AnS=anglesANDshifts;
    else AnS=anglesANDshifts.Transpose();

	for(int i=0;i<6;i++)
		M_anglesANDshifts(i)=AnS(i);

	M_res=CalcObjectPositionMatrix(M_anglesANDshifts, seqmat);

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			res(i,j)=M_res(i,j);

	return res;
}


//3.2.1!!! //for SetEcc //MMatrix<--MDVector
template<class T>
MMatrix<T,4,4> CalcObjectPositionMatrixM(const MDMatrix<T> &anglesANDshifts, int seqmat=1)
{
	MMatrix<T,4,4> res;
	MVector<T,6> M_anglesANDshifts;
	MDVector<T> AnS(6);
    if(anglesANDshifts.GetCols()==1) AnS=anglesANDshifts;
    else AnS=anglesANDshifts.Transpose();
	for(int i=0;i<6;i++)
		M_anglesANDshifts(i)=AnS(i);

	res=CalcObjectPositionMatrix(M_anglesANDshifts, seqmat);

	return res;
}

//4.1
template<class T>
void CalcObjectAnglesAndShiftsD(const MDMatrix<T> &ExtMat, MDVector<T> &angles, MDVector<T> &shifts, int seqmat=1)
{
	MVector<T,6> M_anglesANDshifts;
	MMatrix<T,4,4> M_ExtMat;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			M_ExtMat(i,j)=ExtMat(i,j);

	CalcObjectAnglesAndShifts(M_ExtMat, M_anglesANDshifts, seqmat);
	for(int i=0;i<3;i++)
		shifts(i)=M_anglesANDshifts(i);
	for(int i=3;i<6;i++)
		angles(i-3)=M_anglesANDshifts(i);

	return;
}

//4.2!!!
template<class T>
void CalcObjectAnglesAndShiftsD(const MDMatrix<T> &ExtMat, MDMatrix<T> &anglesANDshifts, int seqmat=1)
{
    MDVector<T> AnS(6);
	MVector<T,6> M_anglesANDshifts;
	MMatrix<T,4,4> M_ExtMat;

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			M_ExtMat(i,j)=ExtMat(i,j);

	CalcObjectAnglesAndShifts(M_ExtMat, M_anglesANDshifts, seqmat);

	for(int i=0;i<6;i++)
		AnS(i)=M_anglesANDshifts(i);

    if(anglesANDshifts.GetCols()==1) anglesANDshifts=AnS;
    else anglesANDshifts=AnS.Transpose();

	return;
}


// ����������� ������� ������� ����������
// ���� ���������� � ��������, ���������� � ������
// ����� �� ������������! ����������� y ������, � ����� ����������������
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

// ������� �������
void DelVertex(int index);
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

// ������� �������
template<class T, int Rows>
void MGeometry<T,Rows>::DelVertex(int index)
{
 if(index<0 || index>=Vertices.size())
  return;

 Vertices.erase(Vertices.begin()+index);
 VerticesNames.erase(VerticesNames.begin()+index);
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
