/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MCAMERA_CPP
#define MCAMERA_CPP

#include <math.h>
#include "MCamera.h"

namespace RDK {

//class MCamera: public UAComponent
// ������
// --------------------------
// ������������ � �����������
// --------------------------
MCamera::MCamera(void)
{
}

MCamera::MCamera(const MCamera &copy)
{
}

MCamera::~MCamera(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� ����������������� ����������������
// --------------------------
// ��������� ������ ������� ������ � ������������ (�)
const MDVector& MCamera::GetPosition(void) const
{
 return Position;
}

bool MCamera::SetPosition(const MDVector& value)
{
 if(Position == value)
  return true;

 Ready=false;
 Position=value;
 return true;
}

// ��������� ������ ����������� ������
const MDVector& MCamera::GetDirection(void) const
{
 return Direction;
}

bool MCamera::SetDirection(const MDVector& value)
{
 if(Direction == value)
  return true;

 Ready=false;
 Direction=value;
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ����������� ������
// --------------------------
// ���������� ����������� � ��������
int MCamera::GetImageWidth(void) const
{
 return ImageWidth;
}

int MCamera::GetImageHeight(void) const
{
 return ImageHeight;
}

bool MCamera::SetImageRes(int width, int height)
{
 if(ImageWidth == width && ImageHeight == height)
  return true;

 Ready=false;
 ImageWidth=width;
 ImageHeight=height;
 return true;
}

// ������ �������
double MCamera::GetMatrixWidth(void) const
{
 return MatrixWidth;
}

double MCamera::GetMatrixHeight(void) const
{
 return MatrixHeight;
}

bool MCamera::SetMatrixRes(double width, double height)
{
 if(MatrixWidth == width && MatrixHeight == height)
  return true;

 Ready=false;
 MatrixWidth=width;
 MatrixHeight=height;
 return true;
}

// �������� ����������
double MCamera::GetFocalLength(void) const
{
 return FocalLength;
}

bool MCamera::SetFocalLength(double value)
{
 if(FocalLength == value)
  return true;

 Ready=false;
 FocalLength=value;
 return true;
}

// ���� ������ �� ����
double MCamera::GetHOpticalAngle(void) const
{
 return HOpticalAngle;
}

double MCamera::GetVOpticalAngle(void) const
{
 return VOpticalAngle;
}

bool MCamera::SetOpticalAngles(double hangle, double vangle)
{
 if(HOpticalAngle == hangle && VOpticalAngle == vangle)
  return true;

 Ready=false;
 HOpticalAngle=hangle;
 VOpticalAngle=vangle;
 return true;
}

// �������������� �����
double MCamera::GetPrincipleX(void) const
{
 return PrincipleX;
}

double MCamera::GetPrincipleY(void) const
{
 return PrincipleY;
}

bool MCamera::SetPriciplePoint(double x, double y)
{
 if(PrincipleX == x && PrincipleY == y)
  return true;

 Ready=false;
 PrincipleX=x;
 PrincipleY=y;
 return true;
}

// ���������
double MCamera::GetDistorionX(void) const
{
 return DistorionX;
}

double MCamera::GetDistorionY(void) const
{
 return DistorionY;
}

bool MCamera::SetDistorion(double x, double y)
{
 if(DistorionX == x && DistorionY == y)
  return true;

 Ready=false;
 DistorionX=x;
 DistorionY=y;
 return true;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ����������
bool MCamera::Calibration()
{
 return true;
}
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ��������� ��������� ����� �� ����� �� ��������� ����� � ������������
UBPoint MCamera::CalcScreenBySpacePoint(const MDVector &space_point)
{
 UBPoint point;

 if(!Build())
  return point;


 return point;
}

// ��������� ��������� ����� � ������������ �� ��������� ����� �� ����� � ��������� ����������
MDVector MCamera::CalcSpaceByScreenPoint(const UBPoint &screen_point, double distance)
{
 MDVector point;

 if(!Build())
  return point;

 return point;
}

// ��������� � ���������� ����������� ����� ������� �� ������� � �������� ��� �������� ����������
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
double MCamera::CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, double distance)
{
 if(!Build())
  return 0;

 double hx=0,hy=0;
 double divisor=1;
 if(screen_segment.Width != 0)
 {
  hx=screen_segment.Width*distance*MatrixWidth;
  divisor=FocalLength*ImageWidth;
  if(divisor>0)
   hx/=divisor;
 }
 else
 if(screen_segment.Height != 0)
 {
  hy=screen_segment.Height*distance*MatrixHeight;
  divisor=FocalLength*ImageHeight;
  if(divisor>0)
   hx/=divisor;
 }

 return sqrt(hx*hx+hy*hy);
}

// ��������� � ���������� ���������� �� ������� �� ������� � �������� � �������� ����������� ��������
double MCamera::CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, double segment_length)
{
 if(!Build())
  return 0;

 return 0;
}
// --------------------------

// --------------------------
// ��������������� ������ �����
// --------------------------
// ������ ��������������� ����������
bool MCamera::CalcAdditionalParameters(void)
{
 if(!MatrixWidth || !MatrixHeight || !ImageWidth || !ImageHeight)
  return false;

 // ����������� ������� �������
 PixelSizeX=ImageWidth/MatrixWidth;
 PixelSizeY=ImageHeight/MatrixHeight;

 // ���� ���������� ������ � ������������ (�)
// CosPhi,SinPhi,CosPsi,SinPsi,CosTheta,SinTheta;

 // ��������� ��������� ���������� � ������� �������
 FocalPixelX=FocalLength/PixelSizeX;
 FocalPixelY=FocalLength/PixelSizeY;

 // ����� ������ �� ����
// HOpticalAngle, VOpticalAngle;

// ����� � ������ Y ���������� ����� ��������� �� ����� � ��������
//int HorizontY1, HorizontY2;

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool MCamera::ADefault(void)
{
 // ������ ����������� ������
 Position=0;

 // ���������� ����������� � ��������
 ImageWidth=720;
 ImageHeight=576;

 // ������ �������
 MatrixWidth=3;
 MatrixHeight=2;

 // �������� ����������
 FocalLength=4;

 // �������������� �����
 PrincipleX=0;
 PrincipleY=0;

 // ���������
 DistorionX=0;
 DistorionY=0;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool MCamera::ABuild(void)
{
 return CalcAdditionalParameters();
}

// ����� �������� �����.
bool MCamera::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool MCamera::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif

