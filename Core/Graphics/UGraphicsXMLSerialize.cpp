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

namespace RDK {
namespace Serialize {

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

 return storage;
}

}
}

#endif


