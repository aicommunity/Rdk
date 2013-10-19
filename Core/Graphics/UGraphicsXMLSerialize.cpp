/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UGraphics_XML_SERIALIZE_CPP
#define UGraphics_XML_SERIALIZE_CPP

//#include <strstream>
#include <sstream>
#include <iomanip>
#include "UGraphicsXMLSerialize.h"
#include "../Serialize/UXMLStdSerialize.h"
#include "Libraries/Simulator/UBAVideoSimulator.h"
#include "Libraries/Simulator/UBAVideo3DSimulator.h"

namespace RDK {

// UBMColorModel
USerStorageXML& operator << (USerStorageXML& storage, const UBMColorModel &data)
{
 storage.SetNodeAttribute("Type",typeid(UBMColorModel).name());

 std::stringstream stream;
 stream<<int(data);
 std::string str=stream.str();
 storage.SetNodeText(str);

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBMColorModel &data)
{
// if(storage.GetNodeAttribute("Type") != typeid(UBMColorModel).name())
//  return storage;

 std::istringstream stream(storage.GetNodeText().c_str());
 int temp;
 stream>>temp;
 data=UBMColorModel(temp);

 return storage;
}

// UColorT
USerStorageXML& operator << (USerStorageXML& storage, const UColorT &data)
{
 storage.SetNodeAttribute("Type",typeid(UColorT).name());

 std::stringstream stream;
// stream.setf(ios::hex);
 stream<<uppercase<<std::hex<<data.c;
 std::string str=stream.str();
 storage.SetNodeText(str);

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UColorT &data)
{
// if(storage.GetNodeAttribute("Type") != typeid(UColorT).name())
//  return storage;

 std::istringstream stream(storage.GetNodeText().c_str());
// stream.setf(ios::hex);
 stream>>std::hex>>data.c;

 return storage;
}

//struct UBPoint;
USerStorageXML& operator << (USerStorageXML& storage, const UBPoint &data)
{
  storage.AddNode("X");
  storage<<data.X;
  storage.SelectUp();

  storage.AddNode("Y");
  storage<<data.Y;
  storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBPoint &data)
{
// if(storage.GetNodeAttribute("Type") != typeid(UBPoint).name())
//  return storage;

  if(!storage.SelectNode("X"))
   return storage;
  storage>>data.X;
  storage.SelectUp();

  if(!storage.SelectNode("Y"))
   return storage;
  storage>>data.Y;
  storage.SelectUp();

 return storage;
}

//struct UBColorPoint;
USerStorageXML& operator << (USerStorageXML& storage, const UBColorPoint &data)
{
// storage.SetNodeAttribute("Type",typeid(UBColorPoint).name());

 operator << (storage,static_cast<const UBPoint&>(data));
 storage.AddNode("Color");
 operator << (storage,data.Color);
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBColorPoint &data)
{
// if(storage.SetNodeAttribute("Type",typeid(UBColorPoint).name());

 operator >> (storage,static_cast<UBPoint&>(data));
 if(!storage.SelectNode("Color"))
  return storage;
 operator >> (storage,data.Color);
 storage.SelectUp();

 return storage;
}

//struct UBRect;
USerStorageXML& operator << (USerStorageXML& storage, const UBRect &data)
{
 operator << (storage,static_cast<const UBPoint&>(data));

  storage.AddNode("Width");
  storage<<data.Width;
  storage.SelectUp();

  storage.AddNode("Height");
  storage<<data.Height;
  storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBRect &data)
{
 operator >> (storage,static_cast<UBPoint&>(data));

  if(!storage.SelectNode("Width"))
   return storage;
  storage>>data.Width;
  storage.SelectUp();

  if(!storage.SelectNode("Height"))
   return storage;
  storage>>data.Height;
  storage.SelectUp();

 return storage;
}

//struct UBHistogramElement;
USerStorageXML& operator << (USerStorageXML& storage, const UBHistogramElement &data)
{
 storage.AddNode("Color");
 operator << (storage,data.Color);
 storage.SelectUp();
 storage.AddNode("Int");
 operator << (storage,data.Number.Int);
 storage.SelectUp();
 storage.AddNode("Float");
 operator << (storage,data.Number.Float);
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBHistogramElement &data)
{
 if(!storage.SelectNode("Color"))
  return storage;
 operator >> (storage,data.Color);
 storage.SelectUp();
 if(!storage.SelectNode("Int"))
  return storage;
 operator >> (storage,data.Number.Int);
 storage.SelectUp();
 if(!storage.SelectNode("Float"))
  return storage;
 operator >> (storage,data.Number.Float);
 storage.SelectUp();

 return storage;
}

//class UBHistogram;
USerStorageXML& operator << (USerStorageXML& storage, const UBHistogram &data)
{
 storage.SetNodeAttribute("Size",sntoa(data.GetSize()));
 storage.SetNodeAttribute("NormalizeFlag",sntoa(data.IsNormalized()));
 storage.SetNodeAttribute("NumPixels",sntoa(data.GetNumPixels()));

 storage.AddNode("Min");
 operator << (storage,data.GetMin());
 storage.SelectUp();

 storage.AddNode("Max");
 operator << (storage,data.GetMax());
 storage.SelectUp();

 for(int i=0;i<data.GetSize();i++)
 {
  storage.AddNode("Elem");
  operator << (storage,data[i]);
  storage.SelectUp();
 }
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBHistogram &data)
{
 int size=atoi(storage.GetNodeAttribute("Size"));

 data.Resize(size);
 data.Normalize((atoi(storage.GetNodeAttribute("NormalizeFlag"))!=0)?true:false);
 data.SetNumPixels(atoi(storage.GetNodeAttribute("NumPixels")));

 UBHistogramElement elem;
 if(!storage.SelectNode("Min"))
  return storage;
 operator >> (storage,elem);
 storage.SelectUp();
 data.SetMin(elem);

 if(!storage.SelectNode("Max"))
  return storage;
 operator >> (storage,elem);
 storage.SelectUp();
 data.SetMax(elem);

 for(int i=0;i<size;i++)
 {
  if(!storage.SelectNode("Elem",i))
   return storage;
  operator >> (storage,data[i]);
  storage.SelectUp();
 }
 return storage;
}

//class UBitmapParam
USerStorageXML& operator << (USerStorageXML& storage, const UBitmapParam &data)
{
 storage.AddNode("Width");
 operator << (storage,data.Width);
 storage.SelectUp();

 storage.AddNode("Height");
 operator << (storage,data.Height);
 storage.SelectUp();

 storage.AddNode("ColorModel");
 operator << (storage,data.ColorModel);
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBitmapParam &data)
{
 if(!storage.SelectNode("Width"))
  return storage;
 operator >> (storage,data.Width);
 storage.SelectUp();

 if(!storage.SelectNode("Height"))
  return storage;
 operator >> (storage,data.Height);
 storage.SelectUp();

 if(!storage.SelectNode("ColorModel"))
  return storage;
 operator >> (storage,data.ColorModel);
 storage.SelectUp();

 return storage;
}

//class UBitmap
USerStorageXML& operator << (USerStorageXML& storage, const UBitmap &data)
{
 storage.AddNode("Width");
 operator << (storage,data.GetWidth());
 storage.SelectUp();

 storage.AddNode("Height");
 operator << (storage,data.GetHeight());
 storage.SelectUp();

 storage.AddNode("ColorModel");
 operator << (storage,data.GetColorModel());
 storage.SelectUp();

 storage.SetNodeAttribute("Size",sntoa(data.GetByteLength()));
 storage.AddNode("Data");
 std::stringstream stream;
 UBColor *pdata=data.GetData();
 stream.setf(ios::hex);
 for(int i=0;i<data.GetByteLength();i++)
  stream<<*pdata++;

 std::string str=stream.str();
 storage.SetNodeText(str);
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBitmap &data)
{
 if(!storage.SelectNode("Width"))
  return storage;
 int width,height, colormodel;
 operator >> (storage,width);
 storage.SelectUp();

 if(!storage.SelectNode("Height"))
  return storage;
 operator >> (storage,height);
 storage.SelectUp();

 if(!storage.SelectNode("ColorModel"))
  return storage;
 operator >> (storage,colormodel);
 storage.SelectUp();

 data.SetRes(width,height,(RDK::UBMColorModel)colormodel);

 if(atoi(storage.GetNodeAttribute("Size")) <= data.GetByteLength())
 {
  if(!storage.SelectNode("Data"))
   return storage;
  std::istringstream stream(storage.GetNodeText().c_str());
  UBColor *pdata=data.GetData();
  stream.setf(ios::hex);
  for(int i=0;i<data.GetByteLength();i++)
   stream>>*pdata++;
  storage.SelectUp();
 }

 return storage;
}

//class UBitmapVector
USerStorageXML& operator << (USerStorageXML& storage, const UBitmapVector &data)
{
 storage.SetNodeAttribute("Size",sntoa(data.GetSize()));

 for(int i=0;i<data.GetSize();i++)
 {
  storage.AddNode("Bitmap");
  storage<<*data[i];
  storage.SelectUp();
 }
 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBitmapVector &data)
{
 int size=atoi(storage.GetNodeAttribute("Size"));

 data.Resize(size);
 for(int i=0;i<data.GetSize();i++)
 {
  if(!storage.SelectNode("Bitmap",i))
   return storage;
  storage>>*data[i];
  storage.SelectUp();
 }
 return storage;
}

// struct UBVSObject
USerStorageXML& operator << (USerStorageXML& storage, const UBVSObject &data)
{
 storage.AddNode("ObjectX");
 operator << (storage,data.ObjectX);
 storage.SelectUp();

 storage.AddNode("ObjectY");
 operator << (storage,data.ObjectY);
 storage.SelectUp();

 storage.AddNode("ObjectWidth");
 operator << (storage,data.ObjectWidth);
 storage.SelectUp();

 storage.AddNode("ObjectHeight");
 operator << (storage,data.ObjectHeight);
 storage.SelectUp();

 storage.AddNode("XShift");
 operator << (storage,data.XShift);
 storage.SelectUp();

 storage.AddNode("YShift");
 operator << (storage,data.YShift);
 storage.SelectUp();

 storage.AddNode("Color");
 operator << (storage,data.Color);
 storage.SelectUp();

 storage.AddNode("Color2");
 operator << (storage,data.Color2);
 storage.SelectUp();

 storage.AddNode("Color3");
 operator << (storage,data.Color2);
 storage.SelectUp();

 storage.AddNode("Color4");
 operator << (storage,data.Color2);
 storage.SelectUp();

 storage.AddNode("Mode");
 operator << (storage,data.Mode);
 storage.SelectUp();

 storage.AddNode("Visible");
 operator << (storage,data.Visible);
 storage.SelectUp();

 storage.AddNode("MovingDirection");
 operator << (storage,data.MovingDirection);
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBVSObject &data)
{
 if(!storage.SelectNode("ObjectX"))
  return storage;
 operator >> (storage,data.ObjectX);
 storage.SelectUp();

 if(!storage.SelectNode("ObjectY"))
  return storage;
 operator >> (storage,data.ObjectY);
 storage.SelectUp();

 if(!storage.SelectNode("ObjectWidth"))
  return storage;
 operator >> (storage,data.ObjectWidth);
 storage.SelectUp();

 if(!storage.SelectNode("ObjectHeight"))
  return storage;
 operator >> (storage,data.ObjectHeight);
 storage.SelectUp();

 if(!storage.SelectNode("XShift"))
  return storage;
 operator >> (storage,data.XShift);
 storage.SelectUp();

 if(!storage.SelectNode("YShift"))
  return storage;
 operator >> (storage,data.YShift);
 storage.SelectUp();

 if(!storage.SelectNode("Color"))
  return storage;
 operator >> (storage,data.Color);
 storage.SelectUp();

 if(!storage.SelectNode("Color2"))
  return storage;
 operator >> (storage,data.Color2);
 storage.SelectUp();

 if(!storage.SelectNode("Color3"))
  return storage;
 operator >> (storage,data.Color3);
 storage.SelectUp();

 if(!storage.SelectNode("Color4"))
  return storage;
 operator >> (storage,data.Color4);
 storage.SelectUp();

 if(!storage.SelectNode("Mode"))
  return storage;
 operator >> (storage,data.Mode);
 storage.SelectUp();

 if(!storage.SelectNode("Visible"))
  return storage;
 operator >> (storage,data.Visible);
 storage.SelectUp();

 if(!storage.SelectNode("MovingDirection"))
  return storage;
 operator >> (storage,data.MovingDirection);
 storage.SelectUp();

 return storage;
}

// struct UBVSObject3D
USerStorageXML& operator << (USerStorageXML& storage, const UBVSObject3D &data)
{
 storage.AddNode("Speed");
 operator << (storage,data.Speed);
 storage.SelectUp();

 storage.AddNode("ObjectColor");
 operator << (storage,data.ObjectColor);
 storage.SelectUp();

 storage.AddNode("ObjectColor2");
 operator << (storage,data.ObjectColor2);
 storage.SelectUp();

 storage.AddNode("Mode");
 operator << (storage,data.Mode);
 storage.SelectUp();

 storage.AddNode("Visible");
 operator << (storage,data.Visible);
 storage.SelectUp();

 storage.AddNode("MovingDirection");
 operator << (storage,data.MovingDirection);
 storage.SelectUp();

 storage.AddNode("Geometry");
 operator << (storage,data.Geometry);
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBVSObject3D &data)
{
 if(!storage.SelectNode("Speed"))
  return storage;
 operator >> (storage,data.Speed);
 storage.SelectUp();

 if(!storage.SelectNode("ObjectColor"))
  return storage;
 operator >> (storage,data.ObjectColor);
 storage.SelectUp();

 if(!storage.SelectNode("ObjectColor2"))
  return storage;
 operator >> (storage,data.ObjectColor2);
 storage.SelectUp();

 if(!storage.SelectNode("Mode"))
  return storage;
 operator >> (storage,data.Mode);
 storage.SelectUp();

 if(!storage.SelectNode("Visible"))
  return storage;
 operator >> (storage,data.Visible);
 storage.SelectUp();

 if(!storage.SelectNode("MovingDirection"))
  return storage;
 operator >> (storage,data.MovingDirection);
 storage.SelectUp();

 if(!storage.SelectNode("Geometry"))
  return storage;
 operator >> (storage,data.Geometry);
 storage.SelectUp();

 return storage;
}

   /*
//class UBPtzCameraInfo
USerStorageXML& operator << (USerStorageXML& storage, const UBPtzCameraInfo &data)
{
 storage.AddNode("MinPan");
 operator << (storage,data.MinPan);
 storage.SelectUp();

 storage.AddNode("MaxPan");
 operator << (storage,data.MaxPan);
 storage.SelectUp();

 storage.AddNode("MinPanAngle");
 operator << (storage,data.MinPanAngle);
 storage.SelectUp();

 storage.AddNode("MaxPanAngle");
 operator << (storage,data.MaxPanAngle);
 storage.SelectUp();

 storage.AddNode("MinPanSpeed");
 operator << (storage,data.MinPanSpeed);
 storage.SelectUp();

 storage.AddNode("MaxPanSpeed");
 operator << (storage,data.MaxPanSpeed);
 storage.SelectUp();

 storage.AddNode("MinTilt");
 operator << (storage,data.MinTilt);
 storage.SelectUp();

 storage.AddNode("MaxTilt");
 operator << (storage,data.MaxTilt);
 storage.SelectUp();

 storage.AddNode("MinTiltAngle");
 operator << (storage,data.MinTiltAngle);
 storage.SelectUp();

 storage.AddNode("MaxTiltAngle");
 operator << (storage,data.MaxTiltAngle);
 storage.SelectUp();

 storage.AddNode("MinTiltSpeed");
 operator << (storage,data.MinTiltSpeed);
 storage.SelectUp();

 storage.AddNode("MaxTiltSpeed");
 operator << (storage,data.MaxTiltSpeed);
 storage.SelectUp();

 storage.AddNode("MinZoom");
 operator << (storage,data.MinZoom);
 storage.SelectUp();

 storage.AddNode("MaxZoom");
 operator << (storage,data.MaxZoom);
 storage.SelectUp();

 storage.AddNode("MinZoomAngle");
 operator << (storage,data.MinZoomAngle);
 storage.SelectUp();

 storage.AddNode("MaxZoomAngle");
 operator << (storage,data.MaxZoomAngle);
 storage.SelectUp();

 storage.AddNode("MinZoomSpeed");
 operator << (storage,data.MinZoomSpeed);
 storage.SelectUp();

 storage.AddNode("MaxZoomSpeed");
 operator << (storage,data.MaxZoomSpeed);
 storage.SelectUp();

 storage.AddNode("MinFocusSpeed");
 operator << (storage,data.MinFocusSpeed);
 storage.SelectUp();

 storage.AddNode("MaxFocusSpeed");
 operator << (storage,data.MaxFocusSpeed);
 storage.SelectUp();

 storage.AddNode("MinIrisSpeed");
 operator << (storage,data.MinIrisSpeed);
 storage.SelectUp();

 storage.AddNode("MaxIrisSpeed");
 operator << (storage,data.MaxIrisSpeed);
 storage.SelectUp();

 storage.AddNode("MinBrightnessSpeed");
 operator << (storage,data.MinBrightnessSpeed);
 storage.SelectUp();

 storage.AddNode("MaxBrightnessSpeed");
 operator << (storage,data.MaxBrightnessSpeed);
 storage.SelectUp();

 storage.AddNode("MinCommandDelay");
 operator << (storage,data.MinCommandDelay);
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBPtzCameraInfo &data)
{
 if(!storage.SelectNode("MinPan"))
  return storage;
 operator >> (storage,data.MinPan);
 storage.SelectUp();

 if(!storage.SelectNode("MaxPan"))
  return storage;
 operator >> (storage,data.MaxPan);
 storage.SelectUp();

 if(!storage.SelectNode("MinPanAngle"))
  return storage;
 operator >> (storage,data.MinPanAngle);
 storage.SelectUp();

 if(!storage.SelectNode("MaxPanAngle"))
  return storage;
 operator >> (storage,data.MaxPanAngle);
 storage.SelectUp();

 if(!storage.SelectNode("MinPanSpeed"))
  return storage;
 operator >> (storage,data.MinPanSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MaxPanSpeed"))
  return storage;
 operator >> (storage,data.MaxPanSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MinTilt"))
  return storage;
 operator >> (storage,data.MinTilt);
 storage.SelectUp();

 if(!storage.SelectNode("MaxTilt"))
  return storage;
 operator >> (storage,data.MaxTilt);
 storage.SelectUp();

 if(!storage.SelectNode("MinTiltAngle"))
  return storage;
 operator >> (storage,data.MinTiltAngle);
 storage.SelectUp();

 if(!storage.SelectNode("MaxTiltAngle"))
  return storage;
 operator >> (storage,data.MaxTiltAngle);
 storage.SelectUp();

 if(!storage.SelectNode("MinTiltSpeed"))
  return storage;
 operator >> (storage,data.MinTiltSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MaxTiltSpeed"))
  return storage;
 operator >> (storage,data.MaxTiltSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MinZoom"))
  return storage;
 operator >> (storage,data.MinZoom);
 storage.SelectUp();

 if(!storage.SelectNode("MaxZoom"))
  return storage;
 operator >> (storage,data.MaxZoom);
 storage.SelectUp();

 if(!storage.SelectNode("MinZoomAngle"))
  return storage;
 operator >> (storage,data.MinZoomAngle);
 storage.SelectUp();

 if(!storage.SelectNode("MaxZoomAngle"))
  return storage;
 operator >> (storage,data.MaxZoomAngle);
 storage.SelectUp();

 if(!storage.SelectNode("MinZoomSpeed"))
  return storage;
 operator >> (storage,data.MinZoomSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MaxZoomSpeed"))
  return storage;
 operator >> (storage,data.MaxZoomSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MinCommandDelay"))
  return storage;
 operator >> (storage,data.MinCommandDelay);
 storage.SelectUp();

 if(!storage.SelectNode("MinFocusSpeed"))
  return storage;
 operator >> (storage,data.MinFocusSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MaxFocusSpeed"))
  return storage;
 operator >> (storage,data.MaxFocusSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MinIrisSpeed"))
  return storage;
 operator >> (storage,data.MinIrisSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MaxIrisSpeed"))
  return storage;
 operator >> (storage,data.MaxIrisSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MinBrightnessSpeed"))
  return storage;
 operator >> (storage,data.MinBrightnessSpeed);
 storage.SelectUp();

 if(!storage.SelectNode("MaxBrightnessSpeed"))
  return storage;
 operator >> (storage,data.MaxBrightnessSpeed);
 storage.SelectUp();

 return storage;
}

//class UBPtzCameraData
USerStorageXML& operator << (USerStorageXML& storage, const UBPtzCameraData &data)
{
 storage<<static_cast<const UBPtzCameraInfo&>(data);

 storage.AddNode("Pan");
 operator << (storage,data.GetPanNative());
 storage.SelectUp();

 storage.AddNode("PanSpeed");
 operator << (storage,data.GetPanSpeedNative());
 storage.SelectUp();

 storage.AddNode("Tilt");
 operator << (storage,data.GetTiltNative());
 storage.SelectUp();

 storage.AddNode("TiltSpeed");
 operator << (storage,data.GetTiltSpeedNative());
 storage.SelectUp();

 storage.AddNode("Zoom");
 operator << (storage,data.GetZoomNative());
 storage.SelectUp();

 storage.AddNode("ZoomSpeed");
 operator << (storage,data.GetZoomSpeedNative());
 storage.SelectUp();

 storage.AddNode("Focus");
 operator << (storage,data.GetFocusNative());
 storage.SelectUp();

 storage.AddNode("FocusSpeed");
 operator << (storage,data.GetFocusSpeedNative());
 storage.SelectUp();

 storage.AddNode("Iris");
 operator << (storage,data.GetIrisNative());
 storage.SelectUp();

 storage.AddNode("IrisSpeed");
 operator << (storage,data.GetIrisSpeedNative());
 storage.SelectUp();

 storage.AddNode("Brightness");
 operator << (storage,data.GetBrightnessNative());
 storage.SelectUp();

 storage.AddNode("BrightnessSpeed");
 operator << (storage,data.GetBrightnessSpeedNative());
 storage.SelectUp();

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UBPtzCameraData &data)
{
 storage>>static_cast<UBPtzCameraInfo&>(data);

 double value;

 if(!storage.SelectNode("Pan"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetPanNative(value);

 if(!storage.SelectNode("PanSpeed"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetPanSpeedNative(value);

 if(!storage.SelectNode("Tilt"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetTiltNative(value);

 if(!storage.SelectNode("TiltSpeed"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetTiltSpeedNative(value);

 if(!storage.SelectNode("Zoom"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetZoomNative(value);

 if(!storage.SelectNode("ZoomSpeed"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetZoomSpeedNative(value);

 if(!storage.SelectNode("Focus"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetFocusNative(value);

 if(!storage.SelectNode("FocusSpeed"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetFocusSpeedNative(value);

 if(!storage.SelectNode("Iris"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetIrisNative(value);

 if(!storage.SelectNode("IrisSpeed"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetIrisSpeedNative(value);

 if(!storage.SelectNode("Brightness"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetBrightnessNative(value);

 if(!storage.SelectNode("BrightnessSpeed"))
  return storage;
 operator >> (storage,value);
 storage.SelectUp();
 data.SetBrightnessSpeedNative(value);

 return storage;
}       */

}

#endif


