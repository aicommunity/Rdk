#ifndef UBPtzCameraInfo_CPP
#define UBPtzCameraInfo_CPP

#include <string>
#include "UBPtzCamera.h"
#include "../Utilities/USupport.h"

namespace RDK {

UBPtzCameraInfo::UBPtzCameraInfo(void)
{
 MinPanAngle=-360;
 MaxPanAngle=360;
 MinTiltAngle=-180;
 MaxTiltAngle=180;
}

/// Возвращает значение величины во внешних единицах по значению во внутренних
double UBPtzCameraInfo::CalcExternalValue(double value, double min_internal, double max_internal, double min_external, double max_external) const
{
 if(fabs(max_internal-min_internal)<1e-6)
  return 0;

 return ((value-min_internal)*(max_external-min_external))/(max_internal-min_internal)+min_external;
}

/// Возвращает значение величины во внутренних единицах по значению во внешних
double UBPtzCameraInfo::CalcInternalValue(double value, double min_internal, double max_internal, double min_external, double max_external) const
{
 if(fabs(max_external-min_external)<1e-6)
  return 0;

 return ((value-min_external)*(max_internal-min_internal))/(max_external-min_external)+min_internal;
}


/// Возвращает угол зрения в градусах по углу в попугаях
double UBPtzCameraInfo::CalcZoomAngle(double zoom) const
{
 if(fabs(MaxZoom-MinZoom)>0)
  return ((zoom-MinZoom)*(MinZoomAngle-MaxZoomAngle))/(MaxZoom-MinZoom)+MaxZoomAngle;
 return zoom;
}

/// Возвращает угол зрения в попугаях по углу в градусах
double UBPtzCameraInfo::CalcZoom(double zoom) const
{
 //
 double coeff=(MinZoom-MaxZoom)/(MinZoomAngle-MaxZoomAngle);
 double res=zoom-min(MinZoomAngle,MaxZoomAngle);
 res*=coeff;
 res+=min(MinZoom,MaxZoom);
 return res;
// return ((zoom-MaxZoomAngle)*(MaxZoom-MinZoom))/(MinZoomAngle-MaxZoomAngle)+MinZoom;
}

void UBPtzCameraInfo::InitCanonVBM40(void)
{
 MinPan=-17000;
 MaxPan=17000;
 MinPanSpeed=62;
 MaxPanSpeed=15000;
 MinTilt=-1000;
 MaxTilt=9000;
 MinTiltSpeed=62;
 MaxTiltSpeed=15000;
 MinZoom=280;
 MaxZoom=5540;
 MinZoomSpeed=0;
 MaxZoomSpeed=7;
}



bool UBPtzCameraInfo::operator == (const UBPtzCameraInfo &copy)
{
 return false;
}

bool UBPtzCameraInfo::operator != (const UBPtzCameraInfo &copy)
{
 return !((*this) == copy);
}

std::string& UBPtzCameraData::GenerateCanonVBM40Command(const std::string &addr, std::string &result)
{
 result=addr;
 result+=std::string("c.1.pan=")+RDK::sntoa(int(Pan));
 result+=std::string("&c.1.pan.speed=")+RDK::sntoa(int(PanSpeed));
 result+=std::string("&c.1.tilt=")+RDK::sntoa(int(Tilt));
 result+=std::string("&c.1.tilt.speed=")+RDK::sntoa(int(TiltSpeed));
 result+=std::string("&c.1.zoom=")+RDK::sntoa(int(Zoom));
 result+=std::string("&c.1.zoom.speed=")+RDK::sntoa(int(ZoomSpeed));
 return result;
}

UBPtzCameraData::UBPtzCameraData(void)
{
 ZeroPosition();
}

UBPtzCameraData::~UBPtzCameraData(void)
{

}

// -----------------------------------
// Методы доступа к данным
// -----------------------------------
/// Текущее значение поворота по оси X
double UBPtzCameraData::GetPan(void) const
{
 return CalcExternalValue(Pan,MinPan,MaxPan,MinPanAngle,MaxPanAngle);
}

double UBPtzCameraData::GetPanNative(void) const
{
 return Pan;
}

void UBPtzCameraData::SetPanNative(double value)
{
 Pan=value;
}

/// Текущая скорость поворота по оси X
double UBPtzCameraData::GetPanSpeed(void) const
{
 return CalcExternalValue(PanSpeed,MinPanSpeed,MaxPanSpeed,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetPanSpeedNative(void) const
{
 return PanSpeed;
}

void UBPtzCameraData::SetPanSpeedNative(double value)
{
 PanSpeed=value;
}

/// Текущее значение поворота по оси Y
double UBPtzCameraData::GetTilt(void) const
{
 return CalcExternalValue(Tilt,MinTilt,MaxTilt,MinTiltAngle,MaxTiltAngle);
}

double UBPtzCameraData::GetTiltNative(void) const
{
 return Tilt;
}

void UBPtzCameraData::SetTiltNative(double value)
{
 Tilt=value;
}

/// Текущая скорость поворота по оси Y
double UBPtzCameraData::GetTiltSpeed(void) const
{
 return CalcExternalValue(TiltSpeed,MinTiltSpeed,MaxTiltSpeed,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetTiltSpeedNative(void) const
{
 return TiltSpeed;
}

void UBPtzCameraData::SetTiltSpeedNative(double value)
{
 TiltSpeed=value;
}

/// Текущее значение зума
double UBPtzCameraData::GetZoom(void) const
{
 return CalcZoomAngle(Zoom);
}

double UBPtzCameraData::GetZoomNative(void) const
{
 return Zoom;
}

void UBPtzCameraData::SetZoomNative(double value)
{
 Zoom=value;
}

/// Текущая скорость зуммирования
double UBPtzCameraData::GetZoomSpeed(void) const
{
 return CalcExternalValue(ZoomSpeed,MinZoomSpeed,MaxZoomSpeed,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetZoomSpeedNative(void) const
{
 return ZoomSpeed;
}

void UBPtzCameraData::SetZoomSpeedNative(double value)
{
 ZoomSpeed=value;
}

/// Текущее значение фокуса
double UBPtzCameraData::GetFocus(void) const
{
 return 0;// return CalcExternalValue(Focus,MinFocus,MaxFocus,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetFocusNative(void) const
{
 return Focus;
}

void UBPtzCameraData::SetFocusNative(double value)
{
 Focus=value;
}

/// Текущая скорости фокусировки
double UBPtzCameraData::GetFocusSpeed(void) const
{
 return CalcExternalValue(FocusSpeed,MinFocusSpeed,MaxFocusSpeed,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetFocusSpeedNative(void) const
{
 return FocusSpeed;
}

void UBPtzCameraData::SetFocusSpeedNative(double value)
{
 FocusSpeed=value;
}

/// Текущее значение диафрагмиы
double UBPtzCameraData::GetIris(void) const
{
 return 0;// return CalcExternalValue(Iris,MinIris,MaxIris,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetIrisNative(void) const
{
 return Iris;
}

void UBPtzCameraData::SetIrisNative(double value)
{
 Iris=value;
}

/// Текущая скорости диафрагмы
double UBPtzCameraData::GetIrisSpeed(void) const
{
 return CalcExternalValue(IrisSpeed,MinIrisSpeed,MaxIrisSpeed,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetIrisSpeedNative(void) const
{
 return IrisSpeed;
}

void UBPtzCameraData::SetIrisSpeedNative(double value)
{
 IrisSpeed=value;
}

/// Текущее значение яркости
double UBPtzCameraData::GetBrightness(void) const
{
 return 0;// return CalcExternalValue(Brightness,MinBrightness,MaxBrightness,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetBrightnessNative(void) const
{
 return Brightness;
}

void UBPtzCameraData::SetBrightnessNative(double value)
{
 Brightness=value;
}

/// Текущая скорости изменения яркости
double UBPtzCameraData::GetBrightnessSpeed(void) const
{
 return CalcExternalValue(BrightnessSpeed,MinBrightnessSpeed,MaxBrightnessSpeed,MinCValue,MaxCValue);
}

double UBPtzCameraData::GetBrightnessSpeedNative(void) const
{
 return BrightnessSpeed;
}

void UBPtzCameraData::SetBrightnessSpeedNative(double value)
{
 BrightnessSpeed=value;
}
// -----------------------------------


bool UBPtzCameraData::operator == (const UBPtzCameraData &copy)
{
 return false;
}

bool UBPtzCameraData::operator != (const UBPtzCameraData &copy)
{
 return !((*this) == copy);
}


/// Возвращает текущий угол зрения в градусах
double UBPtzCameraData::CalcCurrentZoomAngle(void)
{
 return CalcZoomAngle(Zoom);
}

void UBPtzCameraData::ZeroPosition(void)
{
 Pan=0;
 Tilt=0;
 Zoom=MaxZoom;

 TiltSpeed=MaxTiltSpeed;//(MaxTiltSpeed-MinTiltSpeed)/2;
 ZoomSpeed=MaxZoomSpeed;//(MaxZoomSpeed-MinZoomSpeed)/2;
 PanSpeed=MaxPanSpeed;//(MaxPanSpeed-MinPanSpeed)/2;
}

/// Перемещает камеру по оси X в зависимости от знака shift
void UBPtzCameraData::MovePan(double shift, double speed)
{
 shift=CalcInternalValue(shift,MinPan,MaxPan,MinPanAngle,MaxPanAngle);
 speed=CalcInternalValue(speed,MinPanSpeed,MaxPanSpeed,MinCValue,MaxCValue);
 if(speed>=0)
 {
  PanSpeed=speed;
  PanSpeed=(PanSpeed<MinPanSpeed)?MinPanSpeed:PanSpeed;
  PanSpeed=(PanSpeed>MaxPanSpeed)?MaxPanSpeed:PanSpeed;
 }

 Pan+=shift;
 Pan=(Pan<MinPan)?MinPan:Pan;
 Pan=(Pan>MaxPan)?MaxPan:Pan;
}

/// Перемещает камеру по оси Y в зависимости от знака shift
void UBPtzCameraData::MoveTilt(double shift, double speed)
{
 shift=CalcInternalValue(shift,MinTilt,MaxTilt,MinTiltAngle,MaxTiltAngle);
 speed=CalcInternalValue(speed,MinTiltSpeed,MaxTiltSpeed,MinCValue,MaxCValue);
 if(speed>=0)
 {
  TiltSpeed=speed;
  TiltSpeed=(TiltSpeed<MinTiltSpeed)?MinTiltSpeed:TiltSpeed;
  TiltSpeed=(TiltSpeed>MaxTiltSpeed)?MaxTiltSpeed:TiltSpeed;
 }

 Tilt+=shift;
 Tilt=(Tilt<MinTilt)?MinTilt:Tilt;
 Tilt=(Tilt>MaxTilt)?MaxTilt:Tilt;
}

/// Зуммирует камеру в зависимости от знака shift
void UBPtzCameraData::MoveZoom(double shift, double speed)
{
 double zoom_sign=(shift<0)?-1:1;
 shift=CalcZoom(fabs(shift)+min(MinZoomAngle,MaxZoomAngle));
 speed=CalcInternalValue(speed,MinZoomSpeed,MaxZoomSpeed,MinCValue,MaxCValue);
 if(speed>=0)
 {
  ZoomSpeed=speed;
  ZoomSpeed=(ZoomSpeed<MinZoomSpeed)?MinZoomSpeed:ZoomSpeed;
  ZoomSpeed=(ZoomSpeed>MaxZoomSpeed)?MaxZoomSpeed:ZoomSpeed;
 }
 else
 {
  ZoomSpeed=speed;
  ZoomSpeed=(ZoomSpeed<MinZoomSpeed)?MinZoomSpeed:ZoomSpeed;
  ZoomSpeed=(ZoomSpeed>MaxZoomSpeed)?MaxZoomSpeed:ZoomSpeed;
 }

 Zoom+=shift*zoom_sign;
 if(MinZoom<MaxZoom)
 {
  Zoom=(Zoom<MinZoom)?MinZoom:Zoom;
  Zoom=(Zoom>MaxZoom)?MaxZoom:Zoom;
 }
 else
 {
  Zoom=(Zoom>MinZoom)?MinZoom:Zoom;
  Zoom=(Zoom<MaxZoom)?MaxZoom:Zoom;
 }
}

void UBPtzCameraData::ContinuesMove(double pan_speed, double tilt_speed)
{
 pan_speed=CalcInternalValue(pan_speed,MinPanSpeed,MaxPanSpeed,MinCValue,MaxCValue);
 tilt_speed=CalcInternalValue(tilt_speed,MinTiltSpeed,MaxTiltSpeed,MinCValue,MaxCValue);
 PanSpeed=pan_speed;
 TiltSpeed=tilt_speed;
}

void UBPtzCameraData::ContinuesZoom(double speed)
{
 speed=CalcInternalValue(speed,MinZoomSpeed,MaxZoomSpeed,MinCValue,MaxCValue);
 ZoomSpeed=speed;
}

void UBPtzCameraData::ContinuesFocus(double speed)
{
 speed=CalcInternalValue(speed,MinFocusSpeed,MaxFocusSpeed,MinCValue,MaxCValue);
 FocusSpeed=speed;
}

void UBPtzCameraData::ContinuesIris(double speed)
{
 speed=CalcInternalValue(speed,MinIrisSpeed,MaxIrisSpeed,MinCValue,MaxCValue);
 IrisSpeed=speed;
}

void UBPtzCameraData::ContinuesBrightness(double speed)
{
 speed=CalcInternalValue(speed,MinBrightnessSpeed,MaxBrightnessSpeed,MinCValue,MaxCValue);
 BrightnessSpeed=speed;
}

UBPtzCameraData& UBPtzCameraData::operator = (const UBPtzCameraInfo &info)
{
 MinPan=info.MinPan;
 MaxPan=info.MaxPan;
 MinPanSpeed=info.MinPanSpeed;
 MaxPanSpeed=info.MaxPanSpeed;
 MinTilt=info.MinTilt;
 MaxTilt=info.MaxTilt;
 MinTiltSpeed=info.MinTiltSpeed;
 MaxTiltSpeed=info.MaxTiltSpeed;
 MinZoom=info.MinZoom;
 MaxZoom=info.MaxZoom;
 MinZoomAngle=info.MinZoomAngle;
 MaxZoomAngle=info.MaxZoomAngle;
 MinZoomSpeed=info.MinZoomSpeed;
 MaxZoomSpeed=info.MaxZoomSpeed;
 MinCommandDelay=info.MinCommandDelay;

 MinFocusSpeed=info.MinFocusSpeed;
 MaxFocusSpeed=info.MaxFocusSpeed;
 MinIrisSpeed=info.MinIrisSpeed;
 MaxIrisSpeed=info.MaxIrisSpeed;
 MinBrightnessSpeed=info.MinBrightnessSpeed;
 MaxBrightnessSpeed=info.MaxBrightnessSpeed;

 return *this;
}

      
}
#endif
