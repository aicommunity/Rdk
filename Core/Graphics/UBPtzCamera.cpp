#ifndef UBPtzCameraInfo_CPP
#define UBPtzCameraInfo_CPP

#include <string>

namespace RDK {


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

bool UBPtzCameraData::operator == (const UBPtzCameraData &copy)
{
 return false;
}

bool UBPtzCameraData::operator != (const UBPtzCameraData &copy)
{
 return !((*this) == copy);
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
 if(speed>=0)
 {
  ZoomSpeed=speed;
  ZoomSpeed=(ZoomSpeed<MinZoomSpeed)?MinZoomSpeed:ZoomSpeed;
  ZoomSpeed=(ZoomSpeed>MaxZoomSpeed)?MaxZoomSpeed:ZoomSpeed;
 }

 Zoom+=shift;
 Zoom=(Zoom<MinZoom)?MinZoom:Zoom;
 Zoom=(Zoom>MaxZoom)?MaxZoom:Zoom;
}

void UBPtzCameraData::MoveUp(double shift, double speed)
{
 MoveTilt(shift,speed);
}

void UBPtzCameraData::MoveDown(double shift, double speed)
{
 MoveTilt(-shift,speed);
}

void UBPtzCameraData::MoveLeft(double shift, double speed)
{
 MovePan(-shift,speed);
}

void UBPtzCameraData::MoveRight(double shift, double speed)
{
 MovePan(shift,speed);
}

void UBPtzCameraData::ZoomIn(double shift, double speed)
{
 MoveZoom(-shift,speed);
}

void UBPtzCameraData::ZoomOut(double shift, double speed)
{
 MoveZoom(shift,speed);
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
 MinZoomSpeed=info.MinZoomSpeed;
 MaxZoomSpeed=info.MaxZoomSpeed;

 return *this;
}

      
}
#endif
