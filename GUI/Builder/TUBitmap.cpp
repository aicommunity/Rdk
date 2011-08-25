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

#ifndef TUBITMAP_CPP
#define TUBITMAP_CPP

#include "TUBitmap.h"
#include "Series.hpp"

namespace RDK {


// Отправляет данные объекта UBitmap в TBitmap
void operator >> (UBitmap &source, Graphics::TBitmap *target)
{
 UColorT c;
 struct {
  TLogPalette lpal;
  TPaletteEntry dummy[256];
 } SysPal;
 int linebytelength=0;

 if(!target)
  return;

 target->Height=source.GetHeight();
 target->Width=source.GetWidth();

 if(!source.GetWidth() || !source.GetHeight())
  return;

 switch(source.GetColorModel())
 {
 case ubmY32:
 break;

 case ubmRGB24:
  target->HandleType=bmDIB;
  target->PixelFormat=pf24bit;

   if(!(source.GetWidth() % 4))
    memcpy(target->ScanLine[source.GetHeight()-1],source.GetData(),
          source.GetByteLength()*sizeof(UBColor));
   else
   {
    linebytelength=source.GetLineByteLength();
    source=0;
    for(int i=source.GetHeight()-1;i>=0;--i,source+=linebytelength)
     memcpy(target->ScanLine[i],&source(),
          linebytelength*sizeof(UBColor));
   }
 break;

 case ubmRGB32:
   target->HandleType=bmDIB;
   target->PixelFormat=pf32bit;

   if(!(source.GetWidth() % 4))
    memcpy(target->ScanLine[source.GetHeight()-1],source.GetData(),
          source.GetByteLength()*sizeof(UBColor));
   else
   {
    linebytelength=source.GetLineByteLength();
    source=0;
    for(int i=source.GetHeight()-1;i>=0;--i,source+=linebytelength)
     memcpy(target->ScanLine[i],&source(),
          linebytelength*sizeof(UBColor));
   }
 break;

 case ubmYCrCb411:
 break;

 case ubmYCrCb422:
 break;

 case ubmY8:
   target->HandleType=bmDIB;
   target->PixelFormat=pf8bit;
   SysPal.lpal.palVersion = 0x300;
   SysPal.lpal.palNumEntries = 256;
   for(int i=0;i<256;i++)
   {
    SysPal.lpal.palPalEntry[i].peRed=SysPal.lpal.palPalEntry[i].peGreen=SysPal.lpal.palPalEntry[i].peBlue=i;
    SysPal.lpal.palPalEntry[i].peFlags = 0;
   }

   if(target->Palette)
    DeleteObject(target->Palette);
   target->Palette = CreatePalette((const tagLOGPALETTE *)&SysPal.lpal);

   if(!(source.GetWidth() % 4))
    memcpy(target->ScanLine[source.GetHeight()-1],source.GetData(),
          source.GetByteLength()*sizeof(UBColor));
   else
   {
    linebytelength=source.GetLineByteLength();
    source=0;
    for(int i=source.GetHeight()-1;i>=0;--i,source+=linebytelength)
     memcpy(target->ScanLine[i],&source(),
          linebytelength*sizeof(UBColor));
   }
 break;

 case ubmYCrCb444:
 break;
 }
}

// Отправляет данные объекта TBitmap в UBitmap
void operator << (UBitmap &target, Graphics::TBitmap *source)
{
 if(!source)
  return;

 int linebytelength;

 switch(source->PixelFormat)
 {
 case pf24bit:
  target.SetRes(source->Width,source->Height,ubmRGB24);

  if(!source->Width || !source->Height)
   return;

   if(!(target.GetWidth() % 4))
    memcpy(target.GetData(),source->ScanLine[target.GetHeight()-1],
          target.GetByteLength()*sizeof(UBColor));
   else
   {
    linebytelength=target.GetLineByteLength();
    target=0;
    for(int i=target.GetHeight()-1;i>=0;--i,target+=linebytelength)
     memcpy(&target(), source->ScanLine[i],
          linebytelength*sizeof(UBColor));
   }
 break;

 case pf32bit:
  target.SetRes(source->Width,source->Height,ubmRGB32);

  if(!source->Width || !source->Height)
   return;

   if(!(target.GetWidth() % 4))
    memcpy(target.GetData(),source->ScanLine[target.GetHeight()-1],
          target.GetByteLength()*sizeof(UBColor));
   else
   {
    linebytelength=target.GetLineByteLength();
    target=0;
    for(int i=target.GetHeight()-1;i>=0;--i,target+=linebytelength)
     memcpy(&target(), source->ScanLine[i],
          linebytelength*sizeof(UBColor));
   }
 break;

 case pf8bit:
  target.SetRes(source->Width,source->Height,ubmY8);

  if(!source->Width || !source->Height)
   return;

  if(!(target.GetWidth() % 4))
   memcpy(target.GetData(),source->ScanLine[target.GetHeight()-1],
          target.GetByteLength());
   else
   {
    linebytelength=target.GetLineByteLength();
    source=0;
    for(int i=target.GetHeight()-1;i>=0;--i,target+=linebytelength)
     memcpy(&target(),source->ScanLine[i],
          linebytelength);
   }
 break;
 }
}


// Загружает изображение из файла с именем FileName
bool LoadBitmapFromFile(String FileName, UBitmap *target)
{
 Graphics::TBitmap *source=new Graphics::TBitmap;

 source->LoadFromFile(FileName);
 *target<<source;
 delete source;

 return true;
}

// Сохраняет изображение в файл с именем FileName
bool SaveBitmapToFile(String FileName, UBitmap *target)
{
 Graphics::TBitmap *source=new Graphics::TBitmap;

 *target>>source;
 source->SaveToFile(FileName);
 delete source;

 return true;
}



// Отображает гистограмму на TChart на серию series_index
// Если серия не задана (series_index <0) то создает новую
bool ShowHistogram(const UBHistogram& Histogram, TChart *chart, int series_index)
{
 if(!Histogram.GetSize() || !chart)
  return false;

 TChartSeries* series=0;
 if(series_index >= chart->SeriesCount())
 {
  series=new TBarSeries(chart);
  chart->AddSeries(series);
 }
 else
  series=chart->Series[series_index];

 series->Clear();
 for(int i=0;i<Histogram.GetSize();i++)
 {
  series->AddXY(Histogram[i].Color.rgb.b,Histogram[i].Number.Int);
 }

 return true;
}


}
#endif
