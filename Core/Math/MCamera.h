#ifndef MCameraH
#define MCameraH

#include "MVector.h"
#include "MDVector.h"
#include "MGeometry.h"

namespace RDK {

template<class T>
class MCamera
{
private: // ������ ������
// ������� ������� ����������
MMatrix<T,4,4> Ecc;

// �������� ������� ������� ����������
MMatrix<T,4,4> InvEcc;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MCamera(void);
MCamera(const MCamera& copy);
virtual ~MCamera(void);
// --------------------------

// ���������� �������� ������� ���������� ������
const MMatrix<T,4,4>& GetEcc(void) const;
const MMatrix<T,4,4>& GetInvEcc(void) const;
bool SetEcc(const MMatrix<T,4,4>& value);
bool SetInvEcc(const MMatrix<T,4,4>& value);

// �������������� ����� � ���������� ��������� � �������� ����� (���� ���������)
virtual MVector<T,3> CalcPixelPositionFromNormalPosition(const MVector<T,3> &point)=0;

// ��������� ��������� ������� �� ����� � ������ ���������, �� ��������� ������� ��� ����� ���������
virtual MVector<T,3> CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel)=0;

// ��������� ��������� ������� �� ������������ �����, �� ��������� ������� �� ����� � ����������
virtual MVector<T,3> CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel)=0;

// ��������� ��������� ����� �� ����� �� ��������� ����� � ������������
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point)=0;

virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)=0;

// ��������� ��������� ����� � ������������ �� ��������� ����� �� ����� � ��������� ����������
virtual MVector<T,4> CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance)=0;

// ��������� � ���������� ����������� ����� ������� �� ������� � �������� ��� �������� ����������
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
virtual T CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance)=0;

// ��������� � ���������� ���������� �� ������� �� ������� � �������� � �������� ����������� ��������
virtual T CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length)=0;

// ����������� 3D ��������� � 2D
virtual void Convert3Dto2DGeometry(const MGeometry<T,4> &geometry_3d, MGeometry<T,3> &geometry_2d);

/// ����������� 3D ��������� � 2D
/// geometry_3d - N ����� �� 3 ���������� �����
/// geometry_2d - N ����� �� 2 ���������� �����
virtual void Convert3Dto2DGeometry(const MDMatrix<T> &geometry_3d, MDMatrix<T> &geometry_2d);

protected: // ������� ������

};

// ����������� ������ ������ � ������������ ����������
template<class T>
class MCameraStandard: public MCamera<T>
{
private: // ������
// ������� ���������� ����������
MMatrix<T,3,3> Icc;

// �������� ������� ���������� ����������
MMatrix<T,3,3> InvIcc;

// ������ ���������
// 0 - ����� ��������� ���
// 1 - ������ OpenCV
// 2 - ������ Artoolkit
int DistortionMode;

// �-�� ���������
MDVector<double> DistortionCoeff;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MCameraStandard(void);
MCameraStandard(const MCameraStandard& copy);
virtual ~MCameraStandard(void);
// --------------------------

// ���������� �������� ���������� ���������� ������
const MMatrix<T,3,3>& GetIcc(void) const;
const MMatrix<T,3,3>& GetInvIcc(void) const;
bool SetIcc(const MMatrix<T,3,3>& value);
bool SetInvIcc(const MMatrix<T,3,3>& value);

// ������ ���������
const int& GetDistortionMode(void) const;
bool SetDistortionMode(const int &value);

// �-�� ���������
const MDVector<T>& GetDistortionCoeff(void) const;
bool SetDistortionCoeff(const MDVector<T>& value);

// �������������� ����� � ���������� ��������� � �������� ����� (���� ���������)
virtual MVector<T,3> CalcPixelPositionFromNormalPosition(const MVector<T,3> &point);

// ��������� ��������� ������� �� ����� � ������ ���������, �� ��������� ������� ��� ����� ���������
virtual MVector<T,3> CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel);

// ��������� ��������� ������� �� ������������ �����, �� ��������� ������� �� ����� � ����������
virtual MVector<T,3> CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel);

// ��������� ��������� ����� �� ����� �� ��������� ����� � ������������
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point);
virtual MVector<T,3> CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res);

// ��������� ��������� ����� � ������������ �� ��������� ����� �� ����� � ��������� ����������
virtual MVector<T,4> CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance);

// ��������� � ���������� ����������� ����� ������� �� ������� � �������� ��� �������� ����������
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
virtual T CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance);

// ��������� � ���������� ���������� �� ������� �� ������� � �������� � �������� ����������� ��������
virtual T CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length);

};

// -----------------------------------------------------------------------------
// ����������
// -----------------------------------------------------------------------------
// MCamera
// --------------------------
// ������������ � �����������
// --------------------------
template<class T>
MCamera<T>::MCamera(void)
{
 SetEcc(MMatrix<T,4,4>::Eye());
}

template<class T>
MCamera<T>::MCamera(const MCamera<T>& copy)
{
 SetEcc(copy.GetEcc());
}

template<class T>
MCamera<T>::~MCamera(void)
{

}
// --------------------------


// ���������� �������� ������� ���������� ������
template<class T>
const MMatrix<T,4,4>& MCamera<T>::GetEcc(void) const
{
 return Ecc;
}

template<class T>
const MMatrix<T,4,4>& MCamera<T>::GetInvEcc(void) const
{
 return InvEcc;
}

template<class T>
bool MCamera<T>::SetEcc(const MMatrix<T,4,4>& value)
{
 Ecc=value;
 InverseEcc(Ecc,InvEcc);
 InvEcc=Ecc.Inverse();
 return true;
}

template<class T>
bool MCamera<T>::SetInvEcc(const MMatrix<T,4,4>& value)
{
 InvEcc=value;
 InverseEcc(InvEcc,Ecc);
 return true;
}


// ����������� 3D ��������� � 2D
template<class T>
void MCamera<T>::Convert3Dto2DGeometry(const MGeometry<T,4> &geometry_3d, MGeometry<T,3> &geometry_2d)
{
 geometry_2d.Clear();
 geometry_2d.SetNumVertices(geometry_3d.GetNumVertices());
 geometry_2d.SetNumBorders(geometry_3d.GetNumBorders());
 for(size_t i=0;i<geometry_2d.GetNumVertices();i++)
 {
  MVector<T,4> v=geometry_3d.Vertex(i);
  geometry_2d.Vertex(i)=CalcScreenBySpacePoint(v);
 }
 for(size_t i=0;i<geometry_2d.GetNumBorders();i++)
  geometry_2d.Border(i)=geometry_3d.Border(i);
}


/// ����������� 3D ��������� � 2D
/// geometry_3d - N ����� �� 3 ���������� �����
/// geometry_2d - N ����� �� 2 ���������� �����
template<class T>
void MCamera<T>::Convert3Dto2DGeometry(const MDMatrix<T> &geometry_3d, MDMatrix<T> &geometry_2d)
{
 geometry_2d.Resize(geometry_3d.GetRows(),2);
 for(int i=0;i<geometry_3d.GetRows();i++)
 {
  MVector<T,4> v;
  for(int j=0;j<3;j++)
   v(j)=geometry_3d(i,j);
  v(3)=1;
  MVector<T,3> res;
  res=CalcScreenBySpacePoint(v);
  for(int j=0;j<2;j++)
   geometry_2d(i,j)=res(j);
 }
}


// ****************************************************************************
// MCameraStandard
// ****************************************************************************
// --------------------------
// ������������ � �����������
// --------------------------
template<class T>
MCameraStandard<T>::MCameraStandard(void)
: MCamera<T>(), DistortionMode(0)
{
 SetIcc(MMatrix<T,3,3>::Eye());
}

template<class T>
MCameraStandard<T>::MCameraStandard(const MCameraStandard& copy)
: MCamera<T>(copy), DistortionMode(0)
{
 SetIcc(copy.GetIcc());
}

template<class T>
MCameraStandard<T>::~MCameraStandard(void)
{

}
// --------------------------

// ���������� �������� ���������� ���������� ������
template<class T>
const MMatrix<T,3,3>& MCameraStandard<T>::GetIcc(void) const
{
 return Icc;
}

template<class T>
const MMatrix<T,3,3>& MCameraStandard<T>::GetInvIcc(void) const
{
 return InvIcc;
}

template<class T>
bool MCameraStandard<T>::SetIcc(const MMatrix<T,3,3>& value)
{
 Icc=value;
 InvIcc=Icc.Inverse();
 return true;
}

template<class T>
bool MCameraStandard<T>::SetInvIcc(const MMatrix<T,3,3>& value)
{
 InvIcc=value;
 Icc=InvIcc.Inverse();
 return true;
}

// ������ ���������
template<class T>
const int& MCameraStandard<T>::GetDistortionMode(void) const
{
 return DistortionMode;
}

template<class T>
bool MCameraStandard<T>::SetDistortionMode(const int &value)
{
 DistortionMode=value;
 return true;
}

// �-�� ���������
template<class T>
const MDVector<T>& MCameraStandard<T>::GetDistortionCoeff(void) const
{
 return DistortionCoeff;
}

template<class T>
bool MCameraStandard<T>::SetDistortionCoeff(const MDVector<T>& value)
{
 DistortionCoeff.Resize(value.GetSize());
 DistortionCoeff=value;
 return true;
}

// �������������� ����� � ���������� ��������� � �������� ����� (���� ���������)
template<class T>
MVector<T,3> MCameraStandard<T>::CalcPixelPositionFromNormalPosition(const MVector<T,3> &point)
{
 if(DistortionMode == 0)
  return point;

 if(DistortionMode == 1)
 {
  if(DistortionCoeff.GetSize()<5)
   return point;

  MVector<T,3> res;
  T r=point.x*point.x+point.y*point.y;
  T m1=(1.0+DistortionCoeff[0]*r+DistortionCoeff[1]*r*r+DistortionCoeff[4]*r*r*r);
  res.x=m1*point.x;
  res.y=m1*point.y;
  res.z=1;

  res.x+=2*DistortionCoeff[2]*point.x*point.y+DistortionCoeff[3]*(r+2*point.x*point.x);
  res.y+=DistortionCoeff[2]*(r+2*point.y*point.y)+2*DistortionCoeff[3]*point.x*point.y;
  return res;
 }

 if(DistortionMode == 2)
 {
  if(DistortionCoeff.GetSize()<4)
   return point;

  MVector<T,3> res;
  T x=DistortionCoeff[3]*(point.x-DistortionCoeff[0]);
  T y=DistortionCoeff[3]*(point.y-DistortionCoeff[1]);
  T d=x*x+y*y;
  T p=1.0-(DistortionCoeff[2]*d)/100000000.0;
  res.x=p*x+DistortionCoeff[0];
  res.y=p*y+DistortionCoeff[1];
  res.z=1;
  return res;
 }

 return point;
}

// ��������� ��������� ������� �� ����� � ������ ���������, �� ��������� ������� ��� ����� ���������
template<class T>
MVector<T,3> MCameraStandard<T>::CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel)
{
 MVector<T,3> normal_point=InvIcc*undistort_pixel;
 MVector<T,3> dist_point=CalcPixelPositionFromNormalPosition(normal_point);
 MVector<T,3> result=Icc*dist_point;

 return result;
}

// ��������� ��������� ������� �� ������������ �����, �� ��������� ������� �� ����� � ����������
template<class T>
MVector<T,3> MCameraStandard<T>::CalcUndistortPixelPosition(const MVector<T,3> &distort_pixel)
{
 MVector<T,3> result;

 MVector<T,3> dist_iteration=CalcPixelPositionFromNormalPosition(distort_pixel);

 return result;
}

// ��������� ��������� ����� �� ����� �� ��������� ����� � ������������
template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePoint(const MVector<T,4> &space_point)
{
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint.z)>10e-7)
 {
  normalpoint.x=cameraspacepoint.x/cameraspacepoint.z;
  normalpoint.y=cameraspacepoint.y/cameraspacepoint.z;
  normalpoint.z=1;
 }
 else
  return screenpoint;

 distpoint=CalcPixelPositionFromNormalPosition(normalpoint);
 screenpoint=Icc*distpoint;

 return screenpoint;
}

// ��������� ��������� ����� �� ����� �� ��������� ����� � ������������
template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePoint(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)
{
 res=false;
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint.z)>10e-7)
 {
  normalpoint.x=cameraspacepoint.x/cameraspacepoint.z;
  normalpoint.y=cameraspacepoint.y/cameraspacepoint.z;
  normalpoint.z=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;
 if(screenpoint.x<0 || screenpoint.x>=image_width || screenpoint.y<0 || screenpoint.y>=image_height)
 {
  return screenpoint;
 }
 distpoint=CalcPixelPositionFromNormalPosition(normalpoint);
 screenpoint=Icc*distpoint;
 res=true;

 return screenpoint;
}

// ��������� ��������� ����� � ������������ �� ��������� ����� �� ����� � ��������� ����������
template<class T>
MVector<T,4> MCameraStandard<T>::CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance)
{
 MVector<T,4> res;

 return res;
}

// ��������� � ���������� ����������� ����� ������� �� ������� � �������� ��� �������� ����������
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
template<class T>
T MCameraStandard<T>::CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance)
{
 return 0;
}

// ��������� � ���������� ���������� �� ������� �� ������� � �������� � �������� ����������� ��������
template<class T>
T MCameraStandard<T>::CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length)
{
 return 0;
}

}
#endif

