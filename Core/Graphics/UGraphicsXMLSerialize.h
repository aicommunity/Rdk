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
//#include "UBPtzCamera.h"
#include "../Serialize/USerStorageXML.h"

namespace RDK {

struct UBVSObject;
//struct UBVSObject3D;

// UBMColorModel
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBMColorModel &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBMColorModel &data);

// UColorT
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UColorT &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UColorT &data);

//struct UBPoint;
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBPoint &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBPoint &data);

//struct UBColorPoint;
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBColorPoint &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBColorPoint &data);

//struct UBRect;
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBRect &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBRect &data);

//struct UBHistogramElement;
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBHistogramElement &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBHistogramElement &data);

//class UBHistogram;
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBHistogram &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBHistogram &data);

//class UBitmapParam
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBitmapParam &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBitmapParam &data);

//class UBitmap
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBitmap &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBitmap &data);

//class UBitmapVector
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBitmapVector &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBitmapVector &data);

// struct UBVSObject
RDK_LIB_TYPE USerStorageXML& operator << (USerStorageXML& storage, const UBVSObject &data);
RDK_LIB_TYPE USerStorageXML& operator >> (USerStorageXML& storage, UBVSObject &data);

//class UBPtzCameraInfo
//USerStorageXML& operator << (USerStorageXML& storage, const UBPtzCameraInfo &data);
//USerStorageXML& operator >> (USerStorageXML& storage, UBPtzCameraInfo &data);

//class UBPtzCameraData
//USerStorageXML& operator << (USerStorageXML& storage, const UBPtzCameraData &data);
//USerStorageXML& operator >> (USerStorageXML& storage, UBPtzCameraData &data);

}
#endif
