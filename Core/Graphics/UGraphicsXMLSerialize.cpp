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

namespace RDK {
namespace Serialize {

// UColorT
USerStorageXML& operator << (USerStorageXML& storage, const UColorT &data)
{
 storage.SetNodeAttribute("Type",typeid(UColorT).name());

 std::stringstream stream;
 stream.setf(ios::hex);
 stream<<data.c;
 std::string str=stream.str();
 storage.SetNodeText(str);

 return storage;
}

USerStorageXML& operator >> (USerStorageXML& storage, UColorT &data)
{
 if(storage.GetNodeAttribute("Type") != typeid(UColorT).name())
  return storage;

 std::istringstream stream(storage.GetNodeText().c_str());
 stream.setf(ios::hex);
 stream>>data.c;

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
 data.Normalize(atoi(storage.GetNodeAttribute("NormalizeFlag")));
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


}
}

#endif


