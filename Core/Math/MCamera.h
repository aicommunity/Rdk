/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef MCameraH
#define MCameraH

#include "../Engine/UAComponent.h"
#include "../Graphics/UBitmap.h"
#include "MDVector.h"

namespace RDK {

class MCamera: public UAComponent
{
protected: // ���������������� ��������������
// ��������� ������ ������� ������ � ������������ (�)
MDVector Position;

// ��������� ������ ����������� ������
MDVector Direction;

protected: // ��������� ������
// ���������� ����������� � ��������
int ImageWidth, ImageHeight;

// ������ �������
double MatrixWidth, MatrixHeight;

// �������� ����������
double FocalLength;

// ���� ������ �� ����
double HOpticalAngle, VOpticalAngle;

// �������������� �����
double PrincipleX, PrincipleY;

// ���������
double DistorionX, DistorionY;

protected: //... ����� ��� ��������� ������� ������ ���� ��� ����� ��������� �� ����?

protected: // ��������������� ���������
// ����������� ������� �������
double PixelSizeX, PixelSizeY;

// ��������� ��������� ���������� � ������������ ������� �������
double FocalPixelX, FocalPixelY;

// ���� ���������� ������ � ������������
double CosPhi,SinPhi,CosPsi,SinPsi,CosTheta,SinTheta;

// ����� � ������ Y ���������� ����� ��������� �� ����� � ��������
int HorizontY1, HorizontY2;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
MCamera(void);
MCamera(const MCamera &copy);
virtual ~MCamera(void);
// --------------------------

// --------------------------
// ������ ���������� ����������������� ����������������
// --------------------------
// ��������� ������ ������� ������ � ������������ (�)
const MDVector& GetPosition(void) const;
bool SetPosition(const MDVector& value);

// ��������� ������ ����������� ������
const MDVector& GetDirection(void) const;
bool SetDirection(const MDVector& value);
// --------------------------

// --------------------------
// ������ ���������� ����������� ������
// --------------------------
// ���������� ����������� � ��������
int GetImageWidth(void) const;
int GetImageHeight(void) const;
bool SetImageRes(int width, int height);

// ������ �������
double GetMatrixWidth(void) const;
double GetMatrixHeight(void) const;
bool SetMatrixRes(double width, double height);

// �������� ����������
double GetFocalLength(void) const;
bool SetFocalLength(double value);

// ���� ������ �� ����
double GetHOpticalAngle(void) const;
double GetVOpticalAngle(void) const;
bool SetOpticalAngles(double hangle, double vangle);

// �������������� �����
double GetPrincipleX(void) const;
double GetPrincipleY(void) const;
bool SetPriciplePoint(double x, double y);

// ���������
double GetDistorionX(void) const;
double GetDistorionY(void) const;
bool SetDistorion(double x, double y);
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ����������
bool Calibration();
// --------------------------

// --------------------------
// ������ �����
// --------------------------
// ��������� ��������� ����� �� ����� �� ��������� ����� � ������������
UBPoint CalcScreenBySpacePoint(const MDVector &space_point);

// ��������� ��������� ����� � ������������ �� ��������� ����� �� ����� � ��������� ����������
MDVector CalcSpaceByScreenPoint(const UBPoint &screen_point, double distance);

// ��������� � ���������� ����������� ����� ������� �� ������� � �������� ��� �������� ����������
// h=(y*l*d)/(f*ymax) || h=(x*l*d)/(f*xmax)
double CalcSpaceByScreenSegmentLength(const UBRect &screen_segment, double distance);

// ��������� � ���������� ���������� �� ������� �� ������� � �������� � �������� ����������� ��������
double CalcSpaceByScreenSegmentDistance(const UBRect &screen_segment, double segment_length);
// --------------------------

// --------------------------
// ��������������� ������ �����
// --------------------------
protected:
// ������ ��������������� ����������
bool CalcAdditionalParameters(void);
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

}
#endif

