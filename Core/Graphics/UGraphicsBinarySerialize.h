/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UGraphicsBinarySerializeH
#define UGraphicsBinarySerializeH

#include "UBitmap.h"
#include "UBitmapVector.h"
#include "../Serialize/USerStorageBinary.h"

namespace RDK {

struct UBVSObject;
struct UBVSObject3D;

// UBMColorModel
USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data);

// UColorT
USerStorageBinary& operator << (USerStorageBinary& storage, const UColorT &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UColorT &data);

//struct UBPoint;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBPoint &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBPoint &data);

//struct UBColorPoint;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBColorPoint &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBColorPoint &data);

//struct UBRect;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBRect &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBRect &data);

//struct UBHistogramElement;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBHistogramElement &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBHistogramElement &data);

//class UBHistogram;
USerStorageBinary& operator << (USerStorageBinary& storage, const UBHistogram &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBHistogram &data);

//class UBitmapParam
USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmapParam &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmapParam &data);

//class UBitmap
USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmap &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmap &data);

//class UBitmapVector
USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmapVector &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmapVector &data);

// struct UBVSObject
USerStorageBinary& operator << (USerStorageBinary& storage, const UBVSObject &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBVSObject &data);

// struct UBVSObject3D
USerStorageBinary& operator << (USerStorageBinary& storage, const UBVSObject3D &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBVSObject3D &data);

}
#endif
