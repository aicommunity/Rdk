/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UGRAPHICS_BINARY_SERIALIZE_CPP
#define UGRAPHICS_BINARY_SERIALIZE_CPP

#include "UGraphicsBinarySerialize.h"
#include "Libraries/Simulator/UBAVideoSimulator.h"
#include "Libraries/Simulator/UBAVideo3DSimulator.h"

namespace RDK {
namespace Serialize {

// UBMColorModel
USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data)
{
 return USimpleFromStorage(storage,data);
}

// UColorT
USerStorageBinary& operator << (USerStorageBinary& storage, const UColorT &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UColorT &data)
{
 return USimpleFromStorage(storage,data);
}

//struct UBPoint;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBPoint &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBPoint &data)
{
 return USimpleFromStorage(storage,data);
}

//struct UBColorPoint;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBColorPoint &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBColorPoint &data)
{
 return USimpleFromStorage(storage,data);
}

//struct UBRect;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBRect &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBRect &data)
{
 return USimpleFromStorage(storage,data);
}

//struct UBHistogramElement;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBHistogramElement &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBHistogramElement &data)
{
 return USimpleFromStorage(storage,data);
}

//class UBHistogram;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBHistogram &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBHistogram &data)
{
 return USimpleFromStorage(storage,data);
}

//class UBitmapParam
USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmapParam &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmapParam &data)
{
 return USimpleFromStorage(storage,data);
}


//class UBitmap
USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmap &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmap &data)
{
 return USimpleFromStorage(storage,data);
}

//class UBitmapVector
USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmapVector &data)
{
 return USimpleToStorage(storage,data);
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmapVector &data)
{
 return USimpleFromStorage(storage,data);
}

// struct UBVSObject
USerStorageBinary& operator << (USerStorageBinary& storage, const UBVSObject &data)
{
 operator << (storage,data.ObjectX);
 operator << (storage,data.ObjectY);
 operator << (storage,data.ObjectWidth);
 operator << (storage,data.ObjectHeight);
 operator << (storage,data.XShift);
 operator << (storage,data.YShift);
 operator << (storage,data.ObjectColor);
 operator << (storage,data.ObjectColor2);
 operator << (storage,data.Visible);
 operator << (storage,data.MovingDirection);

 return storage;
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBVSObject &data)
{
 operator >> (storage,data.ObjectX);
 operator >> (storage,data.ObjectY);
 operator >> (storage,data.ObjectWidth);
 operator >> (storage,data.ObjectHeight);
 operator >> (storage,data.XShift);
 operator >> (storage,data.YShift);
 operator >> (storage,data.ObjectColor);
 operator >> (storage,data.ObjectColor2);
 operator >> (storage,data.Visible);
 operator >> (storage,data.MovingDirection);

 return storage;
}


// struct UBVSObject3D
USerStorageBinary& operator << (USerStorageBinary& storage, const UBVSObject3D &data)
{
 operator << (storage,data.Speed);
 operator << (storage,data.ObjectColor);
 operator << (storage,data.ObjectColor2);
 operator << (storage,data.Visible);
 operator << (storage,data.MovingDirection);

 return storage;
}

USerStorageBinary& operator >> (USerStorageBinary& storage, UBVSObject3D &data)
{
 operator >> (storage,data.Speed);
 operator >> (storage,data.ObjectColor);
 operator >> (storage,data.ObjectColor2);
 operator >> (storage,data.Visible);
 operator >> (storage,data.MovingDirection);

 return storage;
}

}
}

#endif
