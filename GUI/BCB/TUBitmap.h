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

// Отправляет данные объекта UBitmap в TBitmap
void UBitmapToTBitmap(const UBitmap &source, Graphics::TBitmap *target, bool reflect=false);
void operator >> (const UBitmap &source, Graphics::TBitmap *target);

// Отправляет данные объекта TBitmap в UBitmap
void TBitmapToUBitmap(UBitmap &target, Graphics::TBitmap *source, bool reflect=false);
void operator << (UBitmap &target, Graphics::TBitmap *source);

// Загружает изображение из файла с именем FileName
bool LoadBitmapFromFile(String FileName, UBitmap *target);

// Сохраняет изображение в файл с именем FileName
bool SaveBitmapToFile(String FileName, UBitmap *target);

// Отображает гистограмму на TChart на серию series_index
// Если серия не задана (series_index <0) то создает новую
bool ShowHistogram(const UBHistogram& Histogram, TChart *chart, int series_index=-1);

/// Вычисляет положение в TBitmap по координатам в TImage
TPoint CalcBitmapCoords(TImage *image, int x, int y);
TPoint CalcBitmapCoords(TImage *image, const TPoint &point);

}
#endif

