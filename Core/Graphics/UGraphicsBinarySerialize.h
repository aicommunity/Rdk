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
//#include "UBPtzCamera.h"
#include "../Serialize/USerStorageBinary.h"

namespace RDK {

//struct UBVSObject;

// UBMColorModel
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, UBMColorModel data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBMColorModel &data);

// UColorT
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UColorT &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UColorT &data);

//struct UBPoint;
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBPoint &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBPoint &data);

//struct UBColorPoint;
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBColorPoint &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBColorPoint &data);

//struct UBRect;
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBRect &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBRect &data);

//struct UBHistogramElement;
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBHistogramElement &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBHistogramElement &data);

//class UBHistogram;
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBHistogram &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBHistogram &data);

//class UBitmapParam
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmapParam &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmapParam &data);

//class UBitmap
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmap &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmap &data);

//class UBitmapVector
RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBitmapVector &data);
RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBitmapVector &data);

// struct UBVSObject
//RDK_LIB_TYPE USerStorageBinary& operator << (USerStorageBinary& storage, const UBVSObject &data);
//RDK_LIB_TYPE USerStorageBinary& operator >> (USerStorageBinary& storage, UBVSObject &data);

// struct UBVSObject3D
//USerStorageBinary& operator << (USerStorageBinary& storage, const UBVSObject3D &data);
//USerStorageBinary& operator >> (USerStorageBinary& storage, UBVSObject3D &data);
	  /*
//class UBPtzCameraInfo
USerStorageBinary& operator << (USerStorageBinary& storage, const UBPtzCameraInfo &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBPtzCameraInfo &data);

//class UBPtzCameraData
USerStorageBinary& operator << (USerStorageBinary& storage, const UBPtzCameraData &data);
USerStorageBinary& operator >> (USerStorageBinary& storage, UBPtzCameraData &data);
        */
}
#endif
