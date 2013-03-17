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

virtual void ZeroPosition(void);

virtual void MoveUp(double shift);

virtual void MoveDown(double shift);

virtual void MoveLeft(double shift);

virtual void MoveRight(double shift);

virtual void ZoomIn(double shift);

virtual void ZoomOut(double shift);

bool operator == (const UBPtzCameraData &copy);
bool operator != (const UBPtzCameraData &copy);



};

}
#endif
