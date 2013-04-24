#ifndef UBPtzCameraInfo_H
#define UBPtzCameraInfo_H

#include <string>

namespace RDK {

class UBPtzCameraInfo
{
public:
/// ����������� �������� ��������
static const int MinCValue=-10000;

/// ������������ �������� ��������
static const int MaxCValue=10000;

public:
/// ����������� �������� �������� �� ��� X � �������� ������
double MinPan;

/// ������������ �������� �������� �� ��� X � �������� ������
double MaxPan;

/// ����������� �������� �������� �� ��� X � ��������
double MinPanAngle;

/// ������������ �������� �������� �� ��� X � ��������
double MaxPanAngle;

/// ����������� �������� �������� �������� �� ��� X � �������� ������
double MinPanSpeed;

/// ������������ �������� �������� �������� �� ��� X � �������� ������
double MaxPanSpeed;

/// ����������� �������� �������� �� ��� Y � �������� ������
double MinTilt;

/// ������������ �������� �������� �� ��� Y � �������� ������
double MaxTilt;

/// ����������� �������� �������� �� ��� Y � ��������
double MinTiltAngle;

/// ������������ �������� �������� �� ��� Y � ��������
double MaxTiltAngle;

/// ����������� �������� �������� �������� �� ��� Y � �������� ������
double MinTiltSpeed;

/// ������������ �������� �������� �������� �� ��� Y � �������� ������
double MaxTiltSpeed;

/// ����������� �������� ���� � �������� ������
double MinZoom;

/// ������������ �������� ���� � �������� ������
double MaxZoom;

/// ����������� �������� ���� � ��������
double MinZoomAngle;

/// ������������ �������� ���� � ��������
double MaxZoomAngle;

/// ����������� �������� �������� ���� � �������� ������
double MinZoomSpeed;

/// ������������ �������� �������� ���� � �������� ������
double MaxZoomSpeed;

/// ����������� �������� �������� ����������� � �������� ������
double MinFocusSpeed;

/// ������������ �������� �������� ����������� � �������� ������
double MaxFocusSpeed;

/// ����������� �������� �������� ��������� ��������� � �������� ������
double MinIrisSpeed;

/// ������������ �������� �������� ��������� ��������� � �������� ������
double MaxIrisSpeed;

/// ����������� �������� �������� ��������� ������� � �������� ������
double MinBrightnessSpeed;

/// ������������ �������� �������� ��������� ������� � �������� ������
double MaxBrightnessSpeed;

/// ����������� �������� ������� ����� ��������� (��)
int MinCommandDelay;

public: // ������
UBPtzCameraInfo(void);

/// ���������� �������� �������� �� ������� �������� �� �������� �� ����������
double CalcExternalValue(double value, double min_internal, double max_internal, double min_external, double max_external) const;

/// ���������� �������� �������� �� ���������� �������� �� �������� �� �������
double CalcInternalValue(double value, double min_internal, double max_internal, double min_external, double max_external) const;

/// ���������� ���� ������ � �������� �� ���� � ��������
double CalcZoomAngle(double zoom) const;

/// ���������� ���� ������ � �������� �� ���� � ��������
double CalcZoom(double zoom) const;

/// �������������
void InitCanonVBM40(void);


bool operator == (const UBPtzCameraInfo &copy);
bool operator != (const UBPtzCameraInfo &copy);

};

class UBPtzCameraData: public UBPtzCameraInfo
{
protected:
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

/// ������� �������� ������
double Focus;

/// ������� �������� �����������
double FocusSpeed;

/// ������� �������� ����������
double Iris;

/// ������� �������� ���������
double IrisSpeed;

/// ������� �������� �������
double Brightness;

/// ������� �������� ��������� �������
double BrightnessSpeed;

public: // ������
UBPtzCameraData(void);
virtual ~UBPtzCameraData(void);

std::string& GenerateCanonVBM40Command(const std::string &addr, std::string &result);

// -----------------------------------
// ������ ������� � ������
// -----------------------------------
/// ������� �������� �������� �� ��� X
double GetPan(void) const;
double GetPanNative(void) const;
void SetPanNative(double value);

/// ������� �������� �������� �� ��� X
double GetPanSpeed(void) const;
double GetPanSpeedNative(void) const;
void SetPanSpeedNative(double value);

/// ������� �������� �������� �� ��� Y
double GetTilt(void) const;
double GetTiltNative(void) const;
void SetTiltNative(double value);

/// ������� �������� �������� �� ��� Y
double GetTiltSpeed(void) const;
double GetTiltSpeedNative(void) const;
void SetTiltSpeedNative(double value);

/// ������� �������� ����
double GetZoom(void) const;
double GetZoomNative(void) const;
void SetZoomNative(double value);

/// ������� �������� ������������
double GetZoomSpeed(void) const;
double GetZoomSpeedNative(void) const;
void SetZoomSpeedNative(double value);

/// ������� �������� ������
double GetFocus(void) const;
double GetFocusNative(void) const;
void SetFocusNative(double value);

/// ������� �������� �����������
double GetFocusSpeed(void) const;
double GetFocusSpeedNative(void) const;
void SetFocusSpeedNative(double value);

/// ������� �������� ����������
double GetIris(void) const;
double GetIrisNative(void) const;
void SetIrisNative(double value);

/// ������� �������� ���������
double GetIrisSpeed(void) const;
double GetIrisSpeedNative(void) const;
void SetIrisSpeedNative(double value);

/// ������� �������� �������
double GetBrightness(void) const;
double GetBrightnessNative(void) const;
void SetBrightnessNative(double value);

/// ������� �������� ��������� �������
double GetBrightnessSpeed(void) const;
double GetBrightnessSpeedNative(void) const;
void SetBrightnessSpeedNative(double value);
// -----------------------------------

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

virtual void ContinuesMove(double pan_speed, double tilt_speed);
virtual void ContinuesZoom(double speed);

virtual void ContinuesFocus(double speed);
virtual void ContinuesIris(double speed);
virtual void ContinuesBrightness(double speed);

bool operator == (const UBPtzCameraData &copy);
bool operator != (const UBPtzCameraData &copy);

UBPtzCameraData& operator = (const UBPtzCameraInfo &info);
};

}
#endif
