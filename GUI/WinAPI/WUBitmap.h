#ifndef WUBITMAP_H
#define WUBITMAP_H

#include "../../UGraphics/UBitmap.h"
#include <windows.h>

namespace RDK {

// Преобразует UBitmap в HBITMAP
void* operator >> (const UBitmap &bmp, HBITMAP &bmphandle);

// Преобразует HBITMAP в UBitmap 
UBitmap& operator << (UBitmap &bmp, const HBITMAP bmphandle);

// Устанавливает в UBitmap разрешение такое же как у HBITMAP
UBitmap& ULoadBitmapRes(UBitmap &bmp, const HBITMAP bmphandle);

}
#endif
