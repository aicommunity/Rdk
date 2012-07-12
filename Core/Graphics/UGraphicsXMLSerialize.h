/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UGraphicsXMLSerializeH
#define UGraphicsXMLSerializeH

#include "UBitmap.h"
#include "UBitmapVector.h"
#include "../Serialize/USerStorageXML.h"

namespace RDK {

struct UBVSObject;
struct UBVSObject3D;

namespace Serialize {

// UBMColorModel
USerStorageXML& operator << (USerStorageXML& storage, const UBMColorModel &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBMColorModel &data);

// UColorT
USerStorageXML& operator << (USerStorageXML& storage, const UColorT &data);
USerStorageXML& operator >> (USerStorageXML& storage, UColorT &data);

//struct UBPoint;
USerStorageXML& operator << (USerStorageXML& storage, const UBPoint &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBPoint &data);

//struct UBColorPoint;
USerStorageXML& operator << (USerStorageXML& storage, const UBColorPoint &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBColorPoint &data);

//struct UBRect;
USerStorageXML& operator << (USerStorageXML& storage, const UBRect &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBRect &data);

//struct UBHistogramElement;
USerStorageXML& operator << (USerStorageXML& storage, const UBHistogramElement &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBHistogramElement &data);

//class UBHistogram;
USerStorageXML& operator << (USerStorageXML& storage, const UBHistogram &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBHistogram &data);

//class UBitmapParam
USerStorageXML& operator << (USerStorageXML& storage, const UBitmapParam &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBitmapParam &data);

//class UBitmap
USerStorageXML& operator << (USerStorageXML& storage, const UBitmap &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBitmap &data);

//class UBitmapVector
USerStorageXML& operator << (USerStorageXML& storage, const UBitmapVector &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBitmapVector &data);

// struct UBVSObject
USerStorageXML& operator << (USerStorageXML& storage, const UBVSObject &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBVSObject &data);

// struct UBVSObject
USerStorageXML& operator << (USerStorageXML& storage, const UBVSObject3D &data);
USerStorageXML& operator >> (USerStorageXML& storage, UBVSObject3D &data);
}
}
#endif
