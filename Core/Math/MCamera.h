#ifndef MCameraH
#define MCameraH

#include "MVector.h"
#include "MDVector.h"
#include "MGeometry.h"
#include "../Graphics/UBitmap.h"

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
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point)=0;
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)=0;

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

/// ������ ������
/// 0 - ������� (pinhole)
/// 1 - fisheye (opencv)
int CameraMode;

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

/// ������ ������
/// 0 - ������� (pinhole)
/// 1 - fisheye (opencv)
const int& GetCameraMode(void) const;
bool SetCameraMode(const int& value);

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
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point);
virtual MVector<T,3> CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point, int image_width, int image_height, bool &res);

// ��������� ��������� ����� � ������������ �� ��������� ����� �� ����� � ��������� ����������
virtual MVector<T,4> CalcSpaceByScreenPoint(const MVector<T,3> &screen_point, T distance);

// ��������� � ���������� ����������� ����� ������� �� ������� � �������� ��� �������� ����������
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
virtual T CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, T distance);

// ��������� � ���������� ���������� �� ������� �� ������� � �������� � �������� ����������� ��������
virtual T CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, T segment_length);

/// ��������� ������� �������� ������� � ������ ������
virtual T CalcAngleX(int pixel) const;
virtual T CalcAngleY(int pixel) const;

/// ��������� ���������� �������� ���� � ������ ������
virtual T CalcPixelXByAngle(T angle) const;
virtual T CalcPixelYByAngle(T angle) const;

/// ��������� ������� ���������� ���������� �� ��������� ����� ������
virtual bool CalcIccByVisualAngle(T angle_x, T angle_y, T principle_x, T principle_y, int image_width, int image_height, MMatrix<T,3,3> &icc);

/// ��������� ������� ���������� ���������� �� ��������� ����� ������
virtual bool CalcVisualAnglesByIcc(const MMatrix<T,3,3> &icc, T &angle_x, T &angle_y, T &principle_x, T &principle_y, int image_width, int image_height);

protected: // ������� ������

// ������� ���������� ��������� ��� fisheye thetaD=theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9  (theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9 - thetaD = 0)
T FuncTheta(T thetaD, T Xthet) const;

// ����� ���� ��� ���������� ���������� ������� ��������� ������������ theta
T ChordMethod(T Cx1, T Cx2, double epsilon, T thetaD) const;


MCameraStandard& operator = (const MCameraStandard& copy);
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
  MVector<T,4> v=geometry_3d.Vertex(int(i));
  geometry_2d.Vertex(int(i))=CalcScreenBySpacePoint(v);
 }
 for(size_t i=0;i<geometry_2d.GetNumBorders();i++)
  geometry_2d.Border(int(i))=geometry_3d.Border(int(i));
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
: MCamera<T>(), DistortionMode(0), CameraMode(0)
{
 SetIcc(MMatrix<T,3,3>::Eye());
}

template<class T>
MCameraStandard<T>::MCameraStandard(const MCameraStandard& copy)
: MCamera<T>(copy), DistortionMode(copy.DistortionMode), CameraMode(copy.CameraMode), DistortionCoeff(copy.DistortionCoeff)
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

/// ������ ������
/// 0 - ������� (pinhole)
/// 1 - fisheye (opencv)
template<class T>
const int& MCameraStandard<T>::GetCameraMode(void) const
{
 return CameraMode;
}

template<class T>
bool MCameraStandard<T>::SetCameraMode(const int& value)
{
 CameraMode=value;
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
 switch(CameraMode)
 {
 case 0:
 {
  if(DistortionMode == 0)
   return point;

  if(DistortionMode == 1)
  {
   if(DistortionCoeff.GetSize()<1)
	return point;

   MVector<T,3> res;
   res=point;
   if(DistortionCoeff.GetSize() == 1)
   {
	T r=point(0)*point(0)+point(1)*point(1);
	T m1=(1.0+DistortionCoeff[0]*r);
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;
   }
   else
   if(DistortionCoeff.GetSize() == 5)
   {
	T r=point(0)*point(0)+point(1)*point(1);
	T m1=(1.0+DistortionCoeff[0]*r+DistortionCoeff[1]*r*r+DistortionCoeff[4]*r*r*r);
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);
   }
   else
   if(DistortionCoeff.GetSize() == 8)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r4=r2*r2;
	T r6=r4*r2;
	T dividend=1.0+DistortionCoeff[0]*r2+DistortionCoeff[1]*r4+DistortionCoeff[4]*r6;
	T divider=1.0+DistortionCoeff[5]*r2+DistortionCoeff[6]*r4+DistortionCoeff[7]*r6;
	T m1=dividend/divider;
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r2+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r2+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);
   }
   else
   if(DistortionCoeff.GetSize() == 10)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r4=r2*r2;
	T r6=r4*r2;
	T dividend=1.0+DistortionCoeff[0]*r2+DistortionCoeff[1]*r4+DistortionCoeff[4]*r6;
	T divider=1.0+DistortionCoeff[5]*r2+DistortionCoeff[6]*r4+DistortionCoeff[7]*r6;
	T m1=dividend/divider;
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r2+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r2+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);

	res(0)+=DistortionCoeff[8]*r2+DistortionCoeff[9]*r4;
	res(1)+=DistortionCoeff[8]*r2+DistortionCoeff[9]*r4;
   }
   else
   if(DistortionCoeff.GetSize() == 12)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r4=r2*r2;
	T r6=r4*r2;
	T dividend=1.0+DistortionCoeff[0]*r2+DistortionCoeff[1]*r4+DistortionCoeff[4]*r6;
	T divider=1.0+DistortionCoeff[5]*r2+DistortionCoeff[6]*r4+DistortionCoeff[7]*r6;
	T m1=dividend/divider;
	res(0)=m1*point(0);
	res(1)=m1*point(1);
	res(2)=1;

	res(0)+=2*DistortionCoeff[2]*point(0)*point(1)+DistortionCoeff[3]*(r2+2*point(0)*point(0));
	res(1)+=DistortionCoeff[2]*(r2+2*point(1)*point(1))+2*DistortionCoeff[3]*point(0)*point(1);

	res(0)+=DistortionCoeff[8]*r2+DistortionCoeff[9]*r4;
	res(1)+=DistortionCoeff[10]*r2+DistortionCoeff[11]*r4;
   }
   return res;
  }

  if(DistortionMode == 2)
  {
   if(DistortionCoeff.GetSize()<4)
	return point;

   MVector<T,3> res;
   T x=DistortionCoeff[3]*(point(0)-DistortionCoeff[0]);
   T y=DistortionCoeff[3]*(point(1)-DistortionCoeff[1]);
   T d=x*x+y*y;
   T p=1.0-(DistortionCoeff[2]*d)/100000000.0;
   res(0)=p*x+DistortionCoeff[0];
   res(1)=p*y+DistortionCoeff[1];
   res(2)=1;
   return res;
  }
 }
 break;

 case 1:
 {
  if(DistortionMode == 1)
  {
   if(DistortionCoeff.GetSize()<1)
	return point;

   MVector<T,3> res;
   res=point;

   if(DistortionCoeff.GetSize() == 4)
   {
	T r2=point(0)*point(0)+point(1)*point(1);
	T r=sqrt(r2);
	T teta=atan(r);
	T teta2=teta*teta;
	T teta4=teta2*teta2;
	T teta6=teta2*teta;
	T teta8=teta4*teta4;
	T teta_d=teta*(1.0+DistortionCoeff[0]*teta2+DistortionCoeff[1]*teta4+DistortionCoeff[2]*teta6+DistortionCoeff[3]*teta8);
	res(0)=(teta_d*point(0))/r;
	res(1)=(teta_d*point(1))/r;
	res(2)=1;
   }
   return res;
  }
 }
 break;
 }

 return point;
}

// ��������� ��������� ������� �� ����� � ������ ���������, �� ��������� ������� ��� ����� ���������
template<class T>
MVector<T,3> MCameraStandard<T>::CalcDistortPixelPosition(const MVector<T,3> &undistort_pixel)
{
 MVector<T,3> normal_point(InvIcc*undistort_pixel);
 MVector<T,3> dist_point(CalcPixelPositionFromNormalPosition(normal_point));
 MVector<T,3> result(Icc*dist_point);

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
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
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
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;
 if(screenpoint(0)<0 || screenpoint(0)>=image_width || screenpoint(1)<0 || screenpoint(1)>=image_height)
 {
  return screenpoint;
 }
 distpoint=CalcPixelPositionFromNormalPosition(normalpoint);
 screenpoint=Icc*distpoint;
 res=true;

 return screenpoint;
}

template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point)
{
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;

 return screenpoint;
}

template<class T>
MVector<T,3> MCameraStandard<T>::CalcScreenBySpacePointWODistortions(const MVector<T,4> &space_point, int image_width, int image_height, bool &res)
{
 res=false;
 MVector<T,3> normalpoint, distpoint, screenpoint;
 MVector<T,4> temp4;
 MVector<T,3> cameraspacepoint;
 MMatrix<T,3,4> E=MMatrix<T,3,4>::Eye();

 cameraspacepoint=E*(MCamera<T>::GetEcc()*space_point);
 if(fabs(cameraspacepoint(2))>10e-7)
 {
  normalpoint(0)=cameraspacepoint(0)/cameraspacepoint(2);
  normalpoint(1)=cameraspacepoint(1)/cameraspacepoint(2);
  normalpoint(2)=1;
 }
 else
  return screenpoint;

 screenpoint=Icc*normalpoint;
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


/// ��������� ������� �������� ������� � ������ ������
template<class T>
T MCameraStandard<T>::CalcAngleX(int pixel) const
{
 if(GetIcc()(0,0) < 1e-5 || GetIcc()(1,1)< 1e-5)
  return 0; // TODO: ��� ����������
 switch(CameraMode)
 {
 case 0:
 {
  T center=pixel-GetIcc()(0,2);
  T tg_res=center/GetIcc()(0,0);
  T res=atan(tg_res);
  return res;
 }
 break;

 case 1:
 {
  T x_diff=(pixel-GetIcc()(0,2))/GetIcc()(0,0);
  T y_diff=(0-GetIcc()(0,1))/GetIcc()(1,1);
  T teta=sqrt(x_diff*x_diff+y_diff*y_diff);
  teta=ChordMethod(0,M_PI,1e-5,teta); // ���� ���������
  T beta=atan2(y_diff,x_diff);
  T res=atan(tan(teta)*cos(beta));
  return res;
 }
 break;
 }
 return 0.0;
}

template<class T>
T MCameraStandard<T>::CalcAngleY(int pixel) const
{
 if(GetIcc()(0,0) < 1e-5 || GetIcc()(1,1)< 1e-5)
  return 0; // TODO: ��� ����������

 switch(CameraMode)
 {
 case 0:
 {
  T center=pixel-GetIcc()(1,2);
  T tg_res=center/GetIcc()(1,1);
  T res=atan(tg_res);
  return res;
 }
 break;

 case 1:
 {
  T x_diff=(0-GetIcc()(0,2))/GetIcc()(0,0);
  T y_diff=(pixel-GetIcc()(0,1))/GetIcc()(1,1);
  T teta=sqrt(x_diff*x_diff+y_diff*y_diff);
  teta=ChordMethod(0,M_PI,1e-5,teta); // ���� ���������
  T beta=atan2(x_diff,y_diff);
  T res=atan(tan(teta)*cos(beta));
  return res;
 }
 break;
 }
 return 0.0;
}

/// ��������� ���������� �������� ���� � ������ ������
template<class T>
T MCameraStandard<T>::CalcPixelXByAngle(T angle) const
{
 switch(CameraMode)
 {
 case 0:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(0,0)+GetIcc()(0,2);
  return res;
 }
 break;

 case 1:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(0,0)+GetIcc()(0,2);
  return res;
 }
 break;
 }
 return 0.0;
}

template<class T>
T MCameraStandard<T>::CalcPixelYByAngle(T angle) const
{
 switch(CameraMode)
 {
 case 0:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(1,1)+GetIcc()(1,2);
  return res;
 }
 break;

 case 1:
 {
  T tg_angle=tan(angle);
  T res=tg_angle*GetIcc()(1,1)+GetIcc()(1,2);
  return res;
 }
 break;
 }
 return 0.0;
}


/// ��������� ������� ���������� ���������� �� ��������� ����� ������
template<class T>
bool MCameraStandard<T>::CalcIccByVisualAngle(T angle_x, T angle_y, T principle_x, T principle_y, int image_width, int image_height, MMatrix<T,3,3> &icc)
{
 switch(CameraMode)
 {
 case 0:
 {
  T h_angle((angle_x*M_PI/180.0)/2.0);
  T w_angle((angle_y*M_PI/180.0)/2.0);

  icc=icc.Zero();

  if(image_width && h_angle)
   icc(0,0)=(image_width/2.0)/tan(h_angle);
  else
   return false;

  if(image_height && w_angle)
   icc(1,1)=(image_height/2.0)/tan(w_angle);
  else
   return false;

  icc(0,2)=principle_x*image_width;
  icc(1,2)=principle_y*image_height;
  icc(2,2)=1.0;
 }
 break;

 case 1:
 {
  icc=icc.Zero();
  icc(0,0)=image_width/angle_x;
  icc(1,1)=image_height/angle_y;
  icc(0,2)=principle_x*image_width;
  icc(1,2)=principle_y*image_height;
  icc(2,2)=1.0;
 }
 break;
 }
 return true;
}

/// ��������� ������� ���������� ���������� �� ��������� ����� ������
template<class T>
bool MCameraStandard<T>::CalcVisualAnglesByIcc(const MMatrix<T,3,3> &icc, T &angle_x, T &angle_y, T &principle_x, T &principle_y, int image_width, int image_height)
{
 switch(CameraMode)
 {
 case 0:
 {
  if(icc(0,0) > 1e-5)
   angle_x=(atan(icc(0,2)/icc(0,0))+atan((image_width-icc(0,2))/icc(0,0)))*180.0/M_PI;
  else
   return false;

  if(icc(1,1) > 1e-5)
   angle_y=(atan(icc(1,2)/icc(1,1))+atan((image_height-icc(1,2))/icc(1,1)))*180.0/M_PI;
  else
   return false;

  if(image_width)
   principle_x=icc(0,2)/image_width;
  else
   principle_x=0;

  if(image_height)
   principle_y=icc(1,2)/image_height;
  else
   principle_y=0;
 }
 break;

 case 1:
 {
  if(icc(0,0) > 1e-5)
   angle_x=image_width/icc(0,0);
  else
   return false;

  if(icc(1,1) > 1e-5)
   angle_y=image_height/icc(1,1);
  else
   return false;

  if(image_width)
   principle_x=icc(0,2)/image_width;
  else
   principle_x=0;

  if(image_height)
   principle_y=icc(1,2)/image_height;
  else
   principle_y=0;
 }
 break;
 }
 return true;
}

// ������� ���������� ��������� ��� fisheye thetaD=theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9  (theta+k1*theta^3+k2*theta^5+k3*theta^7+k4*theta^9 - thetaD = 0)
template<class T>
T MCameraStandard<T>::FuncTheta(T thetaD, T Xthet) const
{
	if(DistortionCoeff.GetSize()<4) return thetaD;

	T Xthet3=Xthet*Xthet*Xthet;
	T Xthet5=Xthet3*Xthet*Xthet;
	T Xthet7=Xthet5*Xthet*Xthet;
	T Xthet9=Xthet7*Xthet*Xthet;
	T res= Xthet+DistortionCoeff(0)*Xthet3+DistortionCoeff(1)*Xthet5+DistortionCoeff(2)*Xthet7+DistortionCoeff(3)*Xthet9-thetaD;

	return res;
}

// ����� ���� ��� ���������� ���������� ������� ��������� ������������ theta
template<class T>
T MCameraStandard<T>::ChordMethod(T Cx1, T Cx2, double epsilon, T thetaD) const
{
	if( fabs(Cx2-Cx1)< epsilon) return (Cx1+Cx1)/2;

	T Cy1 = FuncTheta(thetaD,Cx1);
	T Cy2 = FuncTheta(thetaD,Cx2);

	T Cx3 = Cx1-(Cx2-Cx1)*Cy1/(Cy2-Cy1);

	return ChordMethod(Cx2, Cx3, epsilon, thetaD);
}

template<class T>
MCameraStandard<T>& MCameraStandard<T>::operator = (const MCameraStandard<T>& copy)
{
 *static_cast<MCamera<T>*>(this)=copy;
 DistortionMode=copy.DistortionMode;
 CameraMode=copy.CameraMode; 
 DistortionCoeff=copy.DistortionCoeff;
 SetIcc(copy.GetIcc());
	
 return *this;
}

}
#endif

