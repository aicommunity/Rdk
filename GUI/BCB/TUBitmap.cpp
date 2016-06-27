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
#include "../../Core/Graphics/UGraphicsIO.h"
#include <VCLTee.Series.hpp>

namespace RDK {


// Отправляет данные объекта UBitmap в TBitmap
void UBitmapToTBitmap(const UBitmap &source, Graphics::TBitmap *target, bool reflect)
{
 UBColor *source_data;
 UColorT c;
 struct {
  TLogPalette lpal;
  TPaletteEntry dummy[256];
 } SysPal;
 int linebytelength=0;

 if(!target)
  return;

 if(target->Height != source.GetHeight())
  target->Height=source.GetHeight();

 if(target->Width != source.GetWidth())
  target->Width=source.GetWidth();

 if(!source.GetWidth() || !source.GetHeight())
  return;

 switch(source.GetColorModel())
 {
 case ubmY32:
 {
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

   unsigned char *out=(unsigned char*)target->ScanLine[source.GetHeight()-1];
   unsigned int *in=(unsigned int*)source.GetData();
   int shift=source.GetShift();
   if(!reflect)
   {
	for(int i=0;i<source.GetLength();i++)
	 *out++=*in++>>shift;
   }
   else
   {
	for(int i=0;i<source.GetHeight();i++)
	{
	 out=(unsigned char*)target->ScanLine[i];
	 for(int j=0;j<source.GetWidth();j++)
	 {
	  *out++=*in++>>shift;
	 }
	}
   }
 }
 break;

 case ubmRGB96:
 {
  if(target->HandleType != bmDIB)
   target->HandleType=bmDIB;
  if(target->PixelFormat != pf24bit)
   target->PixelFormat=pf24bit;

   unsigned char *out=(unsigned char*)target->ScanLine[source.GetHeight()-1];
   unsigned int *in=(unsigned int*)source.GetData();
   int shift=source.GetShift();
   if(!reflect)
   {
	for(int i=0;i<source.GetLength()*3;i++)
	 *out++=*in++>>shift;
   }
   else
   {
	for(int i=0;i<source.GetHeight();i++)
	{
	 out=(unsigned char*)target->ScanLine[i];
	 for(int j=0;j<source.GetWidth();j++)
	 {
	  *out++=*in++>>shift;
	  *out++=*in++>>shift;
	  *out++=*in++>>shift;
	 }
	}
   }
 }
 break;

 case ubmRGB24:
  if(target->HandleType != bmDIB)
   target->HandleType=bmDIB;
  if(target->PixelFormat != pf24bit)
   target->PixelFormat=pf24bit;
//{
//  DIBSECTION dib_section;
// GetObject(target->Handle, sizeof(dib_section), &dib_section);
// BITMAPINFOHEADER &header=dib_section.dsBmih;
// bool scan_line_normal_order=(header.biHeight>=0)?true:false;
// }

  if(!reflect)
  {
   if(!(source.GetWidth() % 4))
	memcpy(target->ScanLine[source.GetHeight()-1],source.GetData(),
		  source.GetByteLength()*sizeof(UBColor));
   else
   {
	linebytelength=source.GetLineByteLength();
	source_data=source.GetData();
	for(int i=source.GetHeight()-1;i>=0;--i,source_data+=linebytelength)
	 memcpy(target->ScanLine[i],source_data,
		  linebytelength*sizeof(UBColor));
   }
  }
  else
  {
	linebytelength=source.GetLineByteLength();
	source_data=source.GetData();
	for(int i=0;i<source.GetHeight();++i,source_data+=linebytelength)
	 memcpy(target->ScanLine[i],source_data,
		  linebytelength*sizeof(UBColor));
  }
 break;

 case ubmRGB32:
   target->HandleType=bmDIB;
   target->PixelFormat=pf32bit;

   if(!reflect)
   {
	if(!(source.GetWidth() % 4))
	 memcpy(target->ScanLine[source.GetHeight()-1],source.GetData(),
		  source.GetByteLength()*sizeof(UBColor));
	else
	{
	 linebytelength=source.GetLineByteLength();
	 source_data=source.GetData();
	 for(int i=source.GetHeight()-1;i>=0;--i,source_data+=linebytelength)
	  memcpy(target->ScanLine[i],source_data,
		  linebytelength*sizeof(UBColor));
	}
   }
   else
   {
	 linebytelength=source.GetLineByteLength();
	 source_data=source.GetData();
	 for(int i=0;i<source.GetHeight();++i,source_data+=linebytelength)
	  memcpy(target->ScanLine[i],source_data,
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

   if(!reflect)
   {
	if(!(source.GetWidth() % 4))
	 memcpy(target->ScanLine[source.GetHeight()-1],source.GetData(),
		  source.GetByteLength()*sizeof(UBColor));
	else
	{
	 linebytelength=source.GetLineByteLength();
	 source_data=source.GetData();
	 for(int i=source.GetHeight()-1;i>=0;--i,source_data+=linebytelength)
	  memcpy(target->ScanLine[i],source_data,
		  linebytelength*sizeof(UBColor));
    }
   }
   else
   {
	 linebytelength=source.GetLineByteLength();
	 source_data=source.GetData();
	 for(int i=0;i<source.GetHeight();++i,source_data+=linebytelength)
	  memcpy(target->ScanLine[i],source_data,
		  linebytelength*sizeof(UBColor));
   }
 break;

 case ubmYCrCb444:
 break;
 }
}

void operator >> (const UBitmap &source, Graphics::TBitmap *target)
{
 UBitmapToTBitmap(source, target);
}

// Отправляет данные объекта TBitmap в UBitmap
void TBitmapToUBitmap(UBitmap &target, Graphics::TBitmap *source, bool reflect)
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

   if(!(target.GetWidth() % 4) && !reflect)
    memcpy(target.GetData(),source->ScanLine[target.GetHeight()-1],
          target.GetByteLength()*sizeof(UBColor));
   else
   if(!reflect)
   {
	linebytelength=target.GetLineByteLength();
	target=0;
	for(int i=target.GetHeight()-1;i>=0;--i,target+=linebytelength)
	 memcpy(&target(), source->ScanLine[i],
		  linebytelength*sizeof(UBColor));
   }
   else
   {
	linebytelength=target.GetLineByteLength();
	target=0;
	for(int i=0;i<target.GetHeight()-1;++i,target+=linebytelength)
	 memcpy(&target(), source->ScanLine[i],
		  linebytelength*sizeof(UBColor));
   }
 break;

 case pf32bit:
  target.SetRes(source->Width,source->Height,ubmRGB32);

  if(!source->Width || !source->Height)
   return;

   if(!(target.GetWidth() % 4) && !reflect)
	memcpy(target.GetData(),source->ScanLine[target.GetHeight()-1],
		  target.GetByteLength()*sizeof(UBColor));
   else
   if(!reflect)
   {
	linebytelength=target.GetLineByteLength();
	target=0;
	for(int i=target.GetHeight()-1;i>=0;--i,target+=linebytelength)
	 memcpy(&target(), source->ScanLine[i],
		  linebytelength*sizeof(UBColor));
   }
   else
   {
	linebytelength=target.GetLineByteLength();
	target=0;
	for(int i=0;i<target.GetHeight()-1;++i,target+=linebytelength)
	 memcpy(&target(), source->ScanLine[i],
		  linebytelength*sizeof(UBColor));
   }
 break;

 case pf8bit:
  target.SetRes(source->Width,source->Height,ubmY8);

  if(!source->Width || !source->Height)
   return;

  if(!(target.GetWidth() % 4) && !reflect)
   memcpy(target.GetData(),source->ScanLine[target.GetHeight()-1],
          target.GetByteLength());
   else
   if(!reflect)
   {
	linebytelength=target.GetLineByteLength();
	target=0;
	for(int i=target.GetHeight()-1;i>=0;--i,target+=linebytelength)
	 memcpy(&target(),source->ScanLine[i],
		  linebytelength);
   }
   else
   {
	linebytelength=target.GetLineByteLength();
	target=0;
	for(int i=0;i<target.GetHeight()-1;++i,target+=linebytelength)
	 memcpy(&target(), source->ScanLine[i],
		  linebytelength*sizeof(UBColor));
   }
 break;
 }
}

void operator << (UBitmap &target, Graphics::TBitmap *source)
{
 TBitmapToUBitmap(target,source);
}


// Загружает изображение из файла с именем FileName
bool LoadBitmapFromFile(String FileName, UBitmap *target)
{
 Graphics::TBitmap *source=new Graphics::TBitmap;

 UBmpHeader header;
 short type=0;

 ifstream stream(AnsiString(FileName).c_str(),ios::in);
 if(!stream)
 {
  delete source;
  return false;
 }
 stream.read(reinterpret_cast<char*>(&type),sizeof(type)/sizeof(char));
 stream.read(reinterpret_cast<char*>(&header),sizeof(header)/sizeof(char));
 stream.close();

 bool default_order=true;
 if(header.height<0)
 {
  header.height=-header.height;
  default_order=false;
 }
// RDK::LoadBitmapFromFile(AnsiString(FileName).c_str(),*target);
 source->LoadFromFile(FileName);

 TBitmapToUBitmap(*target, source, !default_order);
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
/*bool ShowHistogram(const UBHistogram& Histogram, TChart *chart, int series_index)
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
}     */

/// Вычисляет положение в TBitmap по координатам в TImage
TPoint CalcBitmapCoords(TImage *image, const TPoint &point)
{
 TPoint res(0,0);
 if(!image)
  return res;

 double xscale(0.0);
 double yscale(0.0);

 if(image->Proportional == false && image->Stretch == false)
 {
  res=point;
 }
 else
 if(image->Proportional == true)
 {
  if(image->Width>0)
   xscale=double(image->Picture->Bitmap->Width)/double(image->Width);

  if(image->Height>0)
   yscale=double(image->Picture->Bitmap->Height)/double(image->Height);

  double scale=(xscale>yscale)?xscale:yscale;
  if(scale<1e-3)
   return res;

  int real_width=image->Picture->Bitmap->Width/scale;
  int real_height=image->Picture->Bitmap->Height/scale;

  if(real_width>0)
   res.X=point.X*image->Picture->Bitmap->Width/real_width;

  if(real_height>0)
   res.Y=point.Y*image->Picture->Bitmap->Height/real_height;
 }
 else
 if(image->Proportional == false && image->Stretch == true)
 {
  if(image->Width>0)
   res.X=point.X*image->Picture->Bitmap->Width/image->Width;
  if(image->Height>0)
   res.Y=point.Y*image->Picture->Bitmap->Height/image->Height;
 }
 return res;
}


TPoint CalcBitmapCoords(TImage *image, int x, int y)
{
 return CalcBitmapCoords(image, TPoint(x,y));
}


}
#endif
