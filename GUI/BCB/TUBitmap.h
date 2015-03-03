/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru
Version:        4.0.0

This file - part of the project: RDK

File License:        BSD License
Project License:    BSD License
See file license.txt for more information
*********************************************************** */

#ifndef TUBITMAP_H
#define TUBITMAP_H

#include <vcl.h>
#include <VCLTee.Chart.hpp>
#include "../../Core/Graphics/UBitmap.h"
#pragma hdrstop

namespace RDK {


// ���������� ������ ������� UBitmap � TBitmap
RDK_LIB_TYPE void UBitmapToTBitmap(const UBitmap &source, Graphics::TBitmap *target, bool reflect=false);
RDK_LIB_TYPE void operator >> (const UBitmap &source, Graphics::TBitmap *target);

// ���������� ������ ������� TBitmap � UBitmap
RDK_LIB_TYPE void TBitmapToUBitmap(UBitmap &target, Graphics::TBitmap *source, bool reflect=false);
RDK_LIB_TYPE void operator << (UBitmap &target, Graphics::TBitmap *source);

// ��������� ����������� �� ����� � ������ FileName
RDK_LIB_TYPE bool LoadBitmapFromFile(String FileName, UBitmap *target);

// ��������� ����������� � ���� � ������ FileName
RDK_LIB_TYPE bool SaveBitmapToFile(String FileName, UBitmap *target);

// ���������� ����������� �� TChart �� ����� series_index
// ���� ����� �� ������ (series_index <0) �� ������� �����
//RDK_LIB_TYPE bool ShowHistogram(const UBHistogram& Histogram, TChart *chart, int series_index=-1);

/// ��������� ��������� � TBitmap �� ����������� � TImage
RDK_LIB_TYPE TPoint CalcBitmapCoords(TImage *image, int x, int y);
RDK_LIB_TYPE TPoint CalcBitmapCoords(TImage *image, const TPoint &point);

}
#endif

