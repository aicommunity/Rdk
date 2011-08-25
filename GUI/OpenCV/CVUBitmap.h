/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2008.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru
Version:        3.0.0

This file - part of the project: RDK

File License:        BSD License
Project License:    BSD License
See file license.txt for more information
*********************************************************** */

#ifndef CVUBitmapH
#define CVUBitmapH

#include "cxcore.h"
#include "../../UGraphics/UBitmap.h"

namespace RDK {

// Отправляет данные объекта UBitmap в IplImage
void operator >> (UBitmap &source, IplImage* &target);

// Отправляет данные объекта IplImage в UBitmap
void operator << (UBitmap &target, const IplImage *source);
}
//---------------------------------------------------------------------------
#endif
