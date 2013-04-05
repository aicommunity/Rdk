#ifndef UBPtzCameraInfo_H
#define UBPtzCameraInfo_H

#include <string>

namespace RDK {

class UBPtzCameraInfo
{
public:
/// ����������� �������� �������� �� ��� X
double MinPan;

/// ������������ �������� �������� �� ��� X
double MaxPan;

/// ����������� �������� �������� �������� �� ��� X
double MinPanSpeed;

/// ������������ �������� �������� �������� �� ��� X
double MaxPanSpeed;

/// ����������� �������� �������� �� ��� Y
double MinTilt;

/// ������������ �������� �������� �� ��� Y
double MaxTilt;

/// ����������� �������� �������� �������� �� ��� Y
double MinTiltSpeed;

/// ������������ �������� �������� �������� �� ��� Y
double MaxTiltSpeed;

/// ����������� �������� ����
double MinZoom;

/// ������������ �������� ����
double MaxZoom;

/// ����������� �������� ���� � ��������
double MinZoomAngle;

/// ������������ �������� ���� � ��������
double MaxZoomAngle;

/// ����������� �������� �������� ����
double MinZoomSpeed;

/// ������������ �������� �������� ����
double MaxZoomSpeed;

public: // ������
/// �������������
void InitCanonVBM40(void);


bool operator == (const UBPtzCameraInfo &copy);
bool operator != (const UBPtzCameraInfo &copy);

};

class UBPtzCameraData: public UBPtzCameraInfo
{
public:
/// ������� �������� �������� �� ��� X
double Pan;

/// ������� �������� �������� �� ��� X
double PanSpeed;

/// ������� �������� �������� �� ��� Y
double Tilt;

/// ������� �������� �������� �� ��� Y
double TiltSpeed;

/// ������� �������� ����
double Zoom;

/// ������� �������� ������������
double ZoomSpeed;

public: // ������
UBPtzCameraData(void);
virtual ~UBPtzCameraData(void);

std::string& GenerateCanonVBM40Command(const std::string &addr, std::string &result);

/// ���������� ������� ���� ������ � ��������
double CalcCurrentZoomAngle(void);

/// ������������� ������ � ������� ������� � ����������� �����
/// ������������� ������������ �������� �����������
virtual void ZeroPosition(void);

/// ���������� ������ �� ��� X � ����������� �� ����� shift
virtual void MovePan(double shift, double speed=-1);

/// ���������� ������ �� ��� Y � ����������� �� ����� shift
virtual void MoveTilt(double shift, double speed=-1);

/// ��������� ������ � ����������� �� ����� shift
virtual void MoveZoom(double shift, double speed=-1);

virtual void MoveUp(double shift, double speed=-1);

virtual void MoveDown(double shift, double speed=-1);

virtual void MoveLeft(double shift, double speed=-1);

virtual void MoveRight(double shift, double speed=-1);

virtual void ZoomIn(double shift, double speed=-1);

virtual void ZoomOut(double shift, double speed=-1);

bool operator == (const UBPtzCameraData &copy);
bool operator != (const UBPtzCameraData &copy);

UBPtzCameraData& operator = (const UBPtzCameraInfo &info);




};

}
#endif
