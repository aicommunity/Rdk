#ifndef WUBITMAP_H
#define WUBITMAP_H

#include "../../UGraphics/UBitmap.h"
#include <windows.h>

namespace RDK {

// ����������� UBitmap � HBITMAP
void* operator >> (const UBitmap &bmp, HBITMAP &bmphandle);

// ����������� HBITMAP � UBitmap 
UBitmap& operator << (UBitmap &bmp, const HBITMAP bmphandle);

// ������������� � UBitmap ���������� ����� �� ��� � HBITMAP
UBitmap& ULoadBitmapRes(UBitmap &bmp, const HBITMAP bmphandle);

}
#endif
