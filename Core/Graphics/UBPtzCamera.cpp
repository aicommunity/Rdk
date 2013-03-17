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

void UBPtzCameraData::MoveUp(double shift)
{
 Tilt+=shift;
 if(Tilt>MaxTilt)
  Tilt=MaxTilt;
}

void UBPtzCameraData::MoveDown(double shift)
{
 Tilt-=shift;
 if(Tilt<MinTilt)
  Tilt=MinTilt;
}

void UBPtzCameraData::MoveLeft(double shift)
{
 Pan-=shift;
 if(Pan<MinPan)
  Pan=MinPan;
}

void UBPtzCameraData::MoveRight(double shift)
{
 Pan+=shift;
 if(Pan>MaxPan)
  Pan=MaxPan;
}

void UBPtzCameraData::ZoomIn(double shift)
{
 Zoom+=shift;
 if(Zoom>MaxZoom)
  Zoom=MaxZoom;
}

void UBPtzCameraData::ZoomOut(double shift)
{
 Zoom-=shift;
 if(Zoom<MinZoom)
  Zoom=MinZoom;
}

      
}
#endif
