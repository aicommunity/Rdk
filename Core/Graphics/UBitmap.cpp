/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UBITMAP_CPP
#define UBITMAP_CPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "UBitmap.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBitmapParam::UBitmapParam(void)
{
 Width=Height=0;
 ColorModel=ubmRGB24;
}

UBitmapParam::UBitmapParam(const UBitmapParam &bitmap)
{
 Width=bitmap.Width;
 Height=bitmap.Height;
 ColorModel=bitmap.ColorModel;
}

UBitmapParam::UBitmapParam(UBMColorModel cmodel)
{
 Width=Height=0;
 ColorModel=cmodel;
}

UBitmapParam::~UBitmapParam(void)
{

}
// --------------------------


// Методы UBitmap
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBitmap::UBitmap(void)
 : UBitmapParam()
{
 Data=PData=0;
 Clear();
}

UBitmap::UBitmap(const UBitmap &bitmap)
 : UBitmapParam(bitmap)
{
 Data=PData=0;
 Clear();
 *this=bitmap;
}

UBitmap::UBitmap(UBMColorModel cmodel)
 : UBitmapParam(cmodel)
{
 Data=PData=0;
 Clear();
}

UBitmap::UBitmap(int width, int height, UColorT color,
				UBMColorModel cmodel)
{
 Data=PData=0;
 Clear();
 ColorModel=ubmRGB24;

 if(width >= 0 && height >=0)
  {
   SetRes(width,height);
   Fill(color);
  }
 else
  UBitmap(cmodel);
}

UBitmap::UBitmap(int width, int height, const UBColor* data,
				UBMColorModel cmodel)
{
 Data=PData=0;
 Clear();
 ColorModel=ubmRGB24;

 if(width >= 0 && height >=0)
  SetImage(width, height, data, cmodel);
 else
  UBitmap(cmodel);
}

UBitmap::~UBitmap(void)
{
 Clear();
}
// --------------------------

// -------------------------
// Методы управления данными
// -------------------------
// Длина линии изображения в байтах
int UBitmap::GetLineByteLength(void) const
{
 return CalcLineByteLength(Width,ColorModel);
}

// Длина пикселя в байтах
int UBitmap::GetPixelByteLength(void) const
{
 return CalcLineByteLength(1,ColorModel);
}

// Задает формат изображения
// Если isupdate == true то существующее изображение преобразуется
void UBitmap::SetColorModel(UBMColorModel cmodel, bool isupdate)
{
 if(ColorModel == cmodel)
  return;

 if(!Width && !Height)
 {
  ColorModel=cmodel;
  return;
 }

 UBColor* target=new UBColor[CalcByteLength(Width, Height, cmodel)];

 if(isupdate)
  {
   ColorConvert(Data, ColorModel, target, cmodel);
   // Устанавливаем новый буфер
   AttachBuffer(Width, Height, target, cmodel);
  }
  else
  {
   if(ByteLength != CalcByteLength(Width,Height,cmodel))
   {
    CreateData(Width,Height,cmodel);
   }
  }
}

// Создает внутренний буфер
void UBitmap::SetRes(int width, int height, UBMColorModel cmodel)
{
// if(Width == width && Height == height && (cmodel == ColorModel || cmodel == ubmUnknown))
 if(Width == width && Height == height && cmodel == ColorModel)
  return;

 if(width < 0 || height < 0)
  return;

 if(cmodel == ubmUnknown)
  CreateData(width,height,ColorModel);
 else
  CreateData(width,height,cmodel);
}

// Копирует новое изображение из буфера data
// с прежними размерами
void UBitmap::SetImage(const UBColor* data)
{
 memcpy(Data,data,ByteLength*sizeof(UBColor));
}

// Копирует новое изображение из буфера data
// с новыми размерами
void UBitmap::SetImage(int width, int height,
                        const UBColor* data, UBMColorModel cmodel)
{
 if(!data)
  return;

 SetRes(width,height,cmodel);
 SetImage(data);
}

// Устанавливает внутренний указатель на буфер data
// сохраняя прежнюю информацию о размерах
void UBitmap::AttachBuffer(UBColor* data)
{
 if(Data == data)
  return;

 delete[] Data;
 Data=data;
 PData=Data;
 MemoryLength=ByteLength;
 if(!Data)
 {
  Clear();
 }
}

// Устанавливает внутренний указатель на буфер data
// обновляя информацию о размерах
void UBitmap::AttachBuffer(int width, int height, UBColor* data, UBMColorModel cmodel)
{
 if(Data == data)
  return;

 Width=width;
 Height=height;
 Length=Width*Height;
 ColorModel=cmodel;
 ByteLength=MemoryLength=CalcByteLength(Width,Height,ColorModel);
 delete[] Data;
 Data=data;
 PData=Data;
 CalcChannelOffset(Width, Height, ColorModel, ChannelOffset);

 LineByteLength=CalcLineByteLength(Width,ColorModel);
 PixelByteLength=CalcLineByteLength(1,ColorModel);


 if(!Data)
  Clear();
}


// Отдает буфер изображения на внешнее управление
// После выполнения метода класс становится пустым
// Возвращает освобожденный буфер
UBColor* UBitmap::DetachBuffer(void)
{
 if(!Data)
  return 0;

 UBColor *pData=Data;
 Data=0;
 Clear();
 return pData;
}

// Очищает память изображения и обнуляет его размеры
void UBitmap::Clear(void)
{
 if(Data)
  delete[] Data;
 Data=0;
 PData=Data;

 Width=Height=Length=ByteLength=MemoryLength=LineByteLength=PixelByteLength=0;
 memset(ChannelOffset,0,sizeof(ChannelOffset));
}

// Заполняет изображение цветом color
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::Fill(UColorT color)
{
 if(!Data || Width == 0 || Height == 0)
  return;

 UBColor *pdata=0;
 switch(ColorModel)
 {
 case ubmY8:
  memset(Data,color.ycrcb.y,ByteLength*sizeof(UBColor));
 break;

 case ubmRGB24:
  if(color.c == 0 || (color.rgb.r == color.rgb.g && color.rgb.r == color.rgb.b))
   memset(Data,color.rgb.b,ByteLength*sizeof(UBColor));
  else
  {
   pdata=Data;
   for(int i=0;i<Width;++i)
   {
    *pdata++=color.rgb.b;
    *pdata++=color.rgb.g;
    *pdata++=color.rgb.r;
   }

   pdata=Data+Width*3;
   for(int i=1;i<Height;i++,pdata+=Width*3)
    memcpy(pdata,Data,Width*3*sizeof(UBColor));
  }
 break;

 case ubmY32:
  if(color.c == 0 || (color.rgb.r == color.rgb.g && color.rgb.r == color.rgb.b && color.rgb.r == color.rgb.d))
   memset(Data,color.rgb.b,ByteLength*sizeof(UBColor));
  else
  {
   pdata=Data;
   for(int i=0;i<Width;++i,pdata+=4)
    *reinterpret_cast<unsigned int*>(pdata)=color.c;

   pdata=Data+Width*4;
   for(int i=1;i<Height;i++,pdata+=Width*4)
    memcpy(pdata,Data,Width*4*sizeof(UBColor));
  }
 break;
 }
}

void UBitmap::Fill(UColorT color, const UBRect &rect)
{
 if(!Data || Width == 0 || Height == 0)
  return;

 UBRect realrect=rect;
 if(realrect.X<0)
  realrect.X=0;

 if(realrect.Y<0)
  realrect.Y=0;

 if(realrect.X2()>=Width)
  realrect.X2(Width-1);

 if(realrect.Y2()>=Height)
  realrect.Y2(Height-1);

 UBColor *pdata=0;
 switch(ColorModel)
 {
 case ubmY8:
  pdata=Data+realrect.Y*LineByteLength+realrect.X*PixelByteLength;
  for(int i=0;i<realrect.Height;i++,pdata+=LineByteLength)
   memset(pdata,color.ycrcb.y,LineByteLength);
 break;

 case ubmRGB24:
  pdata=Data+realrect.Y*LineByteLength+realrect.X*PixelByteLength;
  for(int i=0;i<realrect.Height;i++,pdata+=LineByteLength-realrect.Width)
   for(int i=0;i<realrect.Width;++i)
   {
	*pdata++=color.rgb.b;
	*pdata++=color.rgb.g;
	*pdata++=color.rgb.r;
   }
 break;

 case ubmY32:
  pdata=Data+realrect.Y*LineByteLength+realrect.X*PixelByteLength;
  for(int i=0;i<realrect.Height;i++,pdata+=LineByteLength)
   *reinterpret_cast<unsigned int*>(pdata)=color.c;
 break;
 }
}



// Преобразует это изображение в формат приемника 'target'
// и записывает результат в приемник.
void UBitmap::ConvertTo(UBitmap &target) const
{
 if(ColorModel == target.ColorModel)
 {
  target=*this;
  return;
 }

 if(!Width && !Height)
 {
  target=*this;
  return;
 }

 target.SetRes(Width,Height);

 ColorConvert(Data, ColorModel, target.Data, target.ColorModel);
}

// Копирует изображение в 'target' в позицию,
// начинающуюся как x,y
// Если изображение не вмещается целиком, то оно усекается
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::CopyTo(int x, int y, UBitmap &target)
{
 int xsize,ysize;

 if(x < 0 || y < 0 || x >= target.Width || y >= target.Height)
  return;

 xsize=(Width<target.Width-x)?Width:target.Width-x;
 ysize=(Height<target.Height-y)?Height:target.Height-y;

 target.SetColorModel(ColorModel,false);

 switch(ColorModel)
 {
 case ubmY8:
  for(int j=0;j<ysize;j++)
   memcpy(target.Data+(j+y)*target.Width+x,Data+j*Width,xsize*sizeof(UBColor));
 break;

 case ubmRGB24:
  for(int j=0;j<ysize;j++)
   memcpy(target.Data+(j+y)*target.Width*3+x*3,Data+j*Width*3,xsize*3);
 break;

 case ubmY32:
  for(int j=0;j<ysize;j++)
   memcpy(target.Data+(j+y)*target.Width*4+x*4,Data+j*Width*4,xsize*4);
 break;
 }
}

// Копирует изображение в 'target' в позицию,
// начинающуюся как x,y
// Если изображение не вмещается целиком, то оно усекается
// Изображение всегда преобразуется в цветовую модель цели 'target'
// Элементы изображения источника с цветом 'transp' не переносятся
// (эффект прозрачности)
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::CopyTo(int x, int y, UBitmap &target, UColorT transp)
{
 int xsize,ysize;
 UBColor *data,*tardata;

 if(x < 0 || y < 0 || x >= target.Width || y >= target.Height)
  return;

 xsize=(Width<target.Width-x)?Width:target.Width-x;
 ysize=(Height<target.Height-y)?Height:target.Height-y;

 target.SetColorModel(ColorModel);

 switch(ColorModel)
 {
 case ubmY8:
  for(int j=0;j<ysize;j++)
   {
    tardata=target.Data+(j+y)*target.Width+x;
    data=Data+j*Width;
    for(int k=0;k<xsize;++k,++tardata,++data)
     if(*data != transp.ycrcb.y)
      *tardata=*data;
   }
 break;

 case ubmRGB24:
  for(int j=0;j<ysize;j++)
   {
    tardata=target.Data+(j+y)*target.Width*3+x*3;
    data=Data+j*Width*3;
    for(int k=0;k<xsize;++k,tardata+=3,data+=3)
    {
     if(memcmp(data,&transp,3))
      memcpy(tardata,data,3);
    }
   }
 break;

 case ubmY32:
  for(int j=0;j<ysize;j++)
   {
    tardata=target.Data+(j+y)*target.Width*4+x*4;
    data=Data+j*Width*4;
    for(int k=0;k<xsize;++k,tardata+=4,data+=4)
     if(memcmp(data,&transp,4))
      memcpy(tardata,data,4);
   }
 break;
 }
}

// Копирует изображение в 'target' в позицию,
// начинающуюся как x,y
// Если изображение не вмещается целиком, то оно усекается
// Изображение всегда преобразуется в цветовую модель цели 'target'
// Элементы изображения источника с цветом 'transp' не переносятся
// (эффект прозрачности)
// Все остальные элементы заменяются в итоговом изображении на цвет 'color'
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::MaskTo(int x, int y, UBitmap &target, UColorT transp, UColorT color)
{
 int xsize,ysize;
 UBColor *data,*tardata;

 if(x < 0 || y < 0 || x >= target.Width || y >= target.Height)
  return;

 xsize=(Width<target.Width-x)?Width:target.Width-x;
 ysize=(Height<target.Height-y)?Height:target.Height-y;

 target.SetColorModel(ColorModel);

 switch(ColorModel)
 {
 case ubmY8:
  for(int j=0;j<ysize;j++)
   {
    tardata=target.Data+(j+y)*target.Width+x;
    data=Data+j*Width;
    for(int k=0;k<xsize;++k,++tardata,++data)
     if(*data != transp.ycrcb.y)
      *tardata=color.rgb.b;
   }
 break;

 case ubmRGB24:
  for(int j=0;j<ysize;j++)
   {
    tardata=target.Data+(j+y)*target.Width*3+x*3;
    data=Data+j*Width*3;
    for(int k=0;k<xsize;++k,tardata+=3,data+=3)
    {
     if(memcmp(data,&transp,3))
      memcpy(tardata,&color,3);
    }
   }
 break;

 case ubmY32:
  for(int j=0;j<ysize;j++)
   {
    tardata=target.Data+(j+y)*target.Width*4+x*4;
    data=Data+j*Width*4;
    for(int k=0;k<xsize;++k,tardata+=4,data+=4)
     if(memcmp(data,&transp,4))
      memcpy(tardata,&color,4);
   }
 break;
 }
}

// Возвращает участок изображения с координатами
// левого верхнего угла x,y и шириной и длиной
// соответствующей размерам 'target'
// Возвращает false если при копировании произошло усечение
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
bool UBitmap::GetRect(int x, int y, UBitmap &target) const
{
 int width,height;
 bool res=true;

 width=(target.Width > Width)?Width:target.Width;
 height=(target.Height > Height)?Height:target.Height;
 target.SetRes(width,height,ColorModel);

 if(x < 0)
  { x=0; res=false; }

 if(y < 0)
  { y=0; res=false; }

 if(target.Width > Width-x)
  { x=Width-target.Width; res=false; }

 if(target.Height > Height-y)
  { y=Height-target.Height; res=false; }


 switch(ColorModel)
 {
 case ubmY8:
  for(int i=0;i<target.Height;i++)
   memcpy(target.Data+i*target.Width, Data+(y+i)*Width+x,
         target.Width);
 break;

 case ubmRGB24:
  for(int i=0;i<target.Height;i++)
   memcpy(target.Data+i*target.Width*3, Data+(y+i)*Width*3+x*3,
         target.Width*3);
 break;

 case ubmY32:
  for(int i=0;i<target.Height;i++)
   memcpy(target.Data+i*target.Width*4, Data+(y+i)*Width*4+x*4,
         target.Width*4);
 break;
 }

 return res;
}


// Разделяет текущее изображение на RGB цветовые каналы
// Каналы формируются в цветовой модели 'Y8'
// Поддерживает режимы ubmRGB24
void UBitmap::Separate(UBitmap* channels)
{
 UBColor* *pdata;

 switch(ColorModel)
 {
 case ubmRGB24:
  pdata=new UBColor*[3];
  for(int i=0;i<3;i++)
  {
   channels[i].SetRes(Width,Height,ubmY8);
   pdata[i]=channels[i].Data;
  }

  for(int i=0;i<ByteLength;++i)
  {
   *(pdata[0]++)=*Data++;
   *(pdata[1]++)=*Data++;
   *(pdata[2]++)=*Data++;
  }
  delete []pdata;
 break;
 }
}
// -------------------------


// -------------------------
// Методы сбора статистики
// -------------------------
// Возвращает минимальное и максимальное значение яркостей
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::FindColorRange(UColorT &minval, UColorT &maxval) const
{
 minval.c=0xffffffff;
 maxval.c=0;
 UBColor* pdata=Data;

 switch(ColorModel)
 {
  case ubmRGB24:
   for(int i=0;i<Length;++i,pdata+=3)
    {
     if(minval.rgb.b > *pdata)
      minval.rgb.b=*pdata;

     if(maxval.rgb.b < *pdata)
      maxval.rgb.b=*pdata;

     if(minval.rgb.g > *pdata)
      minval.rgb.g=*pdata;

     if(maxval.rgb.g < *pdata)
      maxval.rgb.g=*pdata;

     if(minval.rgb.r > *pdata)
      minval.rgb.r=*pdata;

     if(maxval.rgb.r < *pdata)
      maxval.rgb.r=*pdata;
    }
  break;

  case ubmY8:
   for(int i=0;i<Length;++i,++pdata)
    {
     if(minval.ycrcb.y > *pdata)
      minval.ycrcb.y=*pdata;

     if(maxval.ycrcb.y < *pdata)
      maxval.ycrcb.y=*pdata;
    }
  break;

  case ubmY32:
   for(int i=0;i<Length;i++, pdata+=4)
    {
     if(minval.c > *reinterpret_cast<unsigned int*>(pdata))
      minval.c=*reinterpret_cast<unsigned int*>(pdata);

     if(maxval.c < *reinterpret_cast<unsigned int*>(pdata))
      maxval.c=*reinterpret_cast<unsigned int*>(pdata);
    }
  break;
 }
}

// Возвращает минимальное и максимальное значение яркостей и их координаты
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::FindColorRange(UBColorPoint &minval, UBColorPoint &maxval) const
{
 minval.Color.c=0xffffffff;
 maxval.Color.c=0;
 UBColor* pdata=Data;
 int minl=0,maxl=0;

 switch(ColorModel)
 {
  case ubmRGB24:
   for(int i=0;i<Length;++i,pdata+=3)
    {
     if(minval.Color.rgb.b > *pdata)
     {
      minval.Color.rgb.b=*pdata;
      minl=i;
     }

     if(maxval.Color.rgb.b < *pdata)
     {
      maxval.Color.rgb.b=*pdata;
      minl=i;
     }

     if(minval.Color.rgb.g > *pdata)
     {
      minval.Color.rgb.g=*pdata;
      minl=i;
     }

     if(maxval.Color.rgb.g < *pdata)
     {
      maxval.Color.rgb.g=*pdata;
      maxl=i;
     }

     if(minval.Color.rgb.r > *pdata)
     {
      minval.Color.rgb.r=*pdata;
      maxl=i;
     }

     if(maxval.Color.rgb.r < *pdata)
     {
      maxval.Color.rgb.r=*pdata;
      maxl=i;
     }
    }
  break;

  case ubmY8:
   for(int i=0;i<Length;++i,++pdata)
    {
     if(minval.Color.ycrcb.y > *pdata)
     {
      minval.Color.ycrcb.y=*pdata;
      minl=i;
     }

     if(maxval.Color.ycrcb.y < *pdata)
     {
      maxval.Color.ycrcb.y=*pdata;
      maxl=i;
     }
    }
  break;

  case ubmY32:
   for(int i=0;i<Length;i++, pdata+=4)
    {
     if(minval.Color.c > *reinterpret_cast<unsigned int*>(pdata))
     {
      minval.Color.c=*reinterpret_cast<unsigned int*>(pdata);
      minl=i;
     }

     if(maxval.Color.c < *reinterpret_cast<unsigned int*>(pdata))
     {
      maxval.Color.c=*reinterpret_cast<unsigned int*>(pdata);
      maxl=i;
     }
    }
  break;
 }

 minval.Y=minl/Width;
 minval.X=minl-minval.Y;

 maxval.Y=maxl/Width;
 maxval.X=maxl-maxval.Y;
}

// Возвращает минимальное и максимальное значение яркостей участка изображения
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::FindColorRange(UBRect rect, UColorT &minval, UColorT &maxval) const
{
 minval.c=0xffffffff;
 maxval.c=0;
 int x,y,width,height;

 x=(rect.X<0)?0:rect.X;

 y=(rect.Y<0)?0:rect.Y;

 if(rect.Width>Width-x)
  width=Width-x;
 else
  width=rect.Width;

 if(rect.Height>Height-y)
  height=Height-y;
 else
  height=rect.Height;

 UBColor* pdata=Data+y*LineByteLength+x*PixelByteLength;

 switch(ColorModel)
 {
  case ubmRGB24:
   for(int i=0;i<height;++i)
   {
    for(int j=0;j<width;++j,pdata+=3)
    {
     if(minval.rgb.b > *pdata)
      minval.rgb.b=*pdata;

     if(maxval.rgb.b < *pdata)
      maxval.rgb.b=*pdata;

     if(minval.rgb.g > *pdata)
      minval.rgb.g=*pdata;

     if(maxval.rgb.g < *pdata)
      maxval.rgb.g=*pdata;

     if(minval.rgb.r > *pdata)
      minval.rgb.r=*pdata;

     if(maxval.rgb.r < *pdata)
      maxval.rgb.r=*pdata;
    }
    pdata+=LineByteLength-width*PixelByteLength;
   }
  break;

  case ubmY8:
   for(int i=0;i<height;++i)
   {
    for(int j=0;j<width;++j,++pdata)
    {
     if(minval.ycrcb.y > *pdata)
      minval.ycrcb.y=*pdata;

     if(maxval.ycrcb.y < *pdata)
      maxval.ycrcb.y=*pdata;
    }
    pdata+=LineByteLength-width*PixelByteLength;
   }
  break;

  case ubmY32:
   for(int i=0;i<height;++i)
   {
    for(int j=0;j<width;++j,pdata+=4)
    {
     if(minval.c > *reinterpret_cast<unsigned int*>(pdata))
      minval.c=*reinterpret_cast<unsigned int*>(pdata);

     if(maxval.c < *reinterpret_cast<unsigned int*>(pdata))
      maxval.c=*reinterpret_cast<unsigned int*>(pdata);
    }
    pdata+=LineByteLength-width*PixelByteLength;
   }
  break;
 }
}

// Вычисляет средневзвешенную сумму яркостей всех пикслей участка изображения
// Если 'width' или 'height' < 0 или превышает максимум
// то размеры участка вычисляются до конца изображения
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
UColorT UBitmap::FindAverageColor(int x, int y, int width,int height) const
{
 UColorT result=0;
 unsigned r=0,g=0,b=0;
 UBColor *data=0;
 int length;

 if(x < 0) x=0;
 if(y < 0) y=0;
 if(width < 0)
  width=Width;
 if(height < 0)
  height=Height;
 if(x+width >= Width)
  width=Width-x;
 if(y+height >= Height)
  height=Height-y;
            
 length=width*height;
 switch(ColorModel)
 {
  case ubmRGB24:
   for(int j=y;j<height;j++)
    {
     data=Data+j*Width*3+x*3;
     for(int i=0;i<width;i++)
      {
       r+=*data++;
       g+=*data++;
       b+=*data++;
      }
    }

   result.rgb.r=r/length;
   result.rgb.g=g/length;
   result.rgb.b=b/length;
  break;

  case ubmY8:
   for(int j=y;j<height;j++)
    {
     data=Data+j*Width+x;
     for(int i=0;i<width;i++)
      r+=*data++;
    }

   result.ycrcb.y=r/length;
  break;

  case ubmY32:
   for(int j=y;j<height;++j)
    {
     data=Data+j*Width*4+x*4;
     for(int i=0;i<width;++i,++data)
      result.c+=*reinterpret_cast<unsigned int*>(data);
    }

   result.c/=length;
  break;
 }

 return result;
}

// Вычисляет суммарную яркость по строке изображения с номером y
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
unsigned int UBitmap::CalcBrightnessByRow(int y) const
{
 unsigned int res=0;
 UBColor* pdata;

 if(!Length || !Data || y<0 || y>=Height)
  return 0;

 switch(ColorModel)
 {
 case ubmRGB24:
  pdata=Data+y*Width*3;
  for(int i=0;i<Width*3;++i)
   res+=*pdata++;
  res/=3;
 break;

 case ubmY8:
  pdata=Data+y*Width;
  for(int i=0;i<Width;++i)
   res+=*pdata++;
 break;

 case ubmY32:
  pdata=Data+y*Width*4;
  for(int i=0;i<Width;++i,pdata+=4)
   res+=*reinterpret_cast<unsigned int*>(pdata);
 break;
 }

 return res;
}

// Вычисляет суммарную яркость по столбцу изображения с номером x
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
unsigned int UBitmap::CalcBrightnessByCol(int x) const
{
 unsigned int res=0;
 UBColor* pdata;

 if(!Length || !Data || x<0 || x>=Width)
  return 0;

 switch(ColorModel)
 {
 case ubmRGB24:
  pdata=Data+x*3;
  for(int i=0;i<Height;++i,pdata+=Width*3-3)
  {
   res+=*pdata++;
   res+=*pdata++;
   res+=*pdata++;
  }
  res/=3;
 break;

 case ubmY8:
  pdata=Data+x;
  for(int i=0;i<Height;++i,pdata+=Width)
   res+=*pdata;
 break;

 case ubmY32:
  pdata=Data+x*4;
  for(int i=0;i<Height;++i,pdata+=Width*4)
   res+=*reinterpret_cast<unsigned int*>(pdata);
 break;
 }

 return res;
}

// Вычисляет суммарную яркость раздельно по столбцам и строками изображения от 
// столбца x1 до x2, и от строки y1 до y2. 
// В x_result значения столбцов, в y_result - строк, память должна быть выделена
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::CalcBrightness(unsigned *x_result, unsigned *y_result,
                                   int x1, int x2, int y1, int y2) const
{                                                            
 if(!Length || !Data || !x_result || !y_result)
  return;
 
 if(x1<0 || x1>=Width)
  x1=0;

 if(x2<0 || x2>=Width)
  x2=Width-1;

 if(y1<0 || y1>=Height)
  y1=0;

 if(y2<0 || y2>=Height)
  y2=Height-1;
  
 int temp;
 if(x1>x2)
 {
  temp=x1;
  x1=x2;
  x2=temp;
 }
 if(y1>y2)
 {
  temp=y1;
  y1=y2;
  y2=temp;
 }


 memset(x_result,0,(x2-x1+1)*sizeof(unsigned));
 memset(y_result,0,(y2-y1+1)*sizeof(unsigned));
 UBColor *p;

 switch(ColorModel)
 {
 case ubmRGB24:
  p=Data+y1*Width*3+x1*3;
  for(int j=y1,k=0;j<=y2;j++,k++)
  {
   for(int i=x1,l=0;i<=x2;i++,l++)
   {
    y_result[k]+=*p + *(p+1) + *(p+2);
    x_result[l]+=((*p) + (*p) + (*p++))/3;
   }
   p+=(Width-x2+x1-1)*3;
   y_result[k]/=3;
  }

 break;

 case ubmY8:
  p=Data+y1*Width+x1;
  for(int j=y1,k=0;j<=y2;j++,k++)
  {
   for(int i=x1,l=0;i<=x2;i++,l++)
   {
    y_result[k]+=*p;
    x_result[l]+=*p++;
   }
   p+=Width-x2+x1-1;
  }
 break;

 case ubmY32:
  p=Data+y1*Width*4+x1*4;
  for(int j=y1,k=0;j<=y2;j++,k++)
  {
   for(int i=x1,l=0;i<=x2;i++,l++,p+=4)
   {
    y_result[k]+=*reinterpret_cast<unsigned int*>(p);
    x_result[l]+=*reinterpret_cast<unsigned int*>(p);
   }
   p+=(Width-x2+x1-1)*4;
  }
 break;
 }
}                     

                                   
// Вычисляет относительную суммарную интенсивность раздельно по столбцам и 
// строками изображения от столбца x1 до x2, и от строки y1 до y2. 
// Интенсивность считается раздельно по каналам в зависимости от цветовой модели
// В x_result значения столбцов, в y_result - строк, память должна быть выделена
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::CalcBrightnessAverage(UColorT *x_result, UColorT *y_result,
                                   int x1, int x2, int y1, int y2) const
{
 if(!Length || !Data || !x_result || !y_result)
  return;
 
 if(x1<0 || x1>=Width)
  x1=0;

 if(x2<0 || x2>=Width)
  x2=Width-1;

 if(y1<0 || y1>=Height)
  y1=0;

 if(y2<0 || y2>=Height)
  y2=Height-1;
  
 int temp;
 if(x1>x2)
 {
  temp=x1;
  x1=x2;
  x2=temp;
 }
 if(y1>y2)
 {
  temp=y1;
  y1=y2;
  y2=temp;
 }
        
 UBColor *p;

 // Временные буферы для цветного изображения
 unsigned* x_buf[3]={0,0,0};
 unsigned* y_buf[3]={0,0,0};


 switch(ColorModel)
 {
 case ubmRGB24:
  p=Data+y1*Width*3+x1*3;
  for(int i=0;i<3;i++)
  {
   x_buf[i]=new unsigned[x2-x1+1];
   memset(x_buf,0,(x2-x1+1)*sizeof(unsigned));
  }

  for(int i=0;i<3;i++)
  {
   y_buf[i]=new unsigned[y2-y1+1];
   memset(y_buf,0,(y2-y1+1)*sizeof(unsigned));
  }

  for(int j=y1,k=0;j<=y2;j++,k++)
  {         
   for(int i=x1,l=0;i<=x2;i++,l++)
   {
    y_buf[0][k]+=*p;
    y_buf[1][k]+=*(p+1);
    y_buf[2][k]+=*(p+2);
    x_buf[0][l]+=*p++;
    x_buf[1][l]+=*p++;
    x_buf[2][l]+=*p++;
   }
   p+=(Width-x2+x1)*3;
  }

  for(int i=0;i<=x2-x1;i++)
  {
   x_result[i].rgb.b=x_buf[0][i]/(y2-y1+1);
   x_result[i].rgb.g=x_buf[1][i]/(y2-y1+1);
   x_result[i].rgb.r=x_buf[2][i]/(y2-y1+1);
  }

  for(int i=0;i<=y2-y1;i++)
  {
   y_result[i].rgb.b=y_buf[0][i]/(x2-x1+1);
   y_result[i].rgb.g=y_buf[1][i]/(x2-x1+1);
   y_result[i].rgb.r=y_buf[2][i]/(x2-x1+1);
  }

  for(int i=0;i<3;i++)
   delete []x_buf[i];

  for(int i=0;i<3;i++)
   delete []y_buf[i];
 break;

 case ubmY8:
  p=Data+y1*Width+x1;
  memset(x_result,0,(x2-x1+1)*sizeof(unsigned));
  memset(y_result,0,(y2-y1+1)*sizeof(unsigned));
  for(int j=y1,k=0;j<=y2;j++,k++)
  {
   for(int i=x1,l=0;i<=x2;i++,l++)
   {
    y_result[k].c+=*p;
    x_result[l].c+=*p++;
   }
   p+=Width-x2+x1;
  }

  for(int i=0;i<=x2-x1;i++)
   x_result[i].ycrcb.y=x_result[i].c/(y2-y1+1);

  for(int i=0;i<=y2-y1;i++)
   y_result[i].ycrcb.y=y_result[i].c/(x2-x1+1);
 break;

 case ubmY32:
  p=Data+y1*Width*4+x1*4;
  memset(x_result,0,(x2-x1+1)*sizeof(unsigned));
  memset(y_result,0,(y2-y1+1)*sizeof(unsigned));
  for(int j=y1,k=0;j<=y2;j++,k++)
  {
   for(int i=x1,l=0;i<=x2;i++,l++)
   {
    y_result[k].c+=*reinterpret_cast<unsigned int*>(p);
    x_result[l].c+=*reinterpret_cast<unsigned int*>(p);
    p+=4;
   }
   p+=(Width-x2+x1)*4;
  }                   

  for(int i=0;i<=x2-x1;i++)
   x_result[i].c/=(y2-y1+1);

  for(int i=0;i<=y2-y1;i++)
   y_result[i].c/=(x2-x1+1);
 break;
 }
}

// Вычисляет ожидаемый размер гистограммы, если это возможно
// и выделяет память на необходимое число элементов
// result - указатель на возвращаемый массив данных гистограммы
// Поддерживает режимы ubmY8
/*void UBitmap::PrepareHistogram(UBHistogram &result,
                    int x, int y, int width, int height) const
{
 if(!Length || !Data)
  return;

 switch(ColorModel)
 {
 case ubmY8:
 case ubmRGB24:
  result.Resize(256);
 break;
 }
}   */

// Вычисляет гистограмму яркостей по изображению в заданной области
// result - указатель на возвращаемый массив данных гистограммы
// Память должна быть выделена на заданное число элементов 'ressize'
// Гистограмма вычисляется для первых N заданных яркостей пикселей в массиве result
// для случая ubmY8, для заданных яркостей для случая ubmY32
// Поддерживает режимы ubmY8, ubmY32
/*void UBitmap::CalcHistogram(UBHistogram &result,
                                   int x, int y, int width, int height) const
{
 if(!Length || !Data || !result.GetSize())
  return;

 if(x<0 || x>=Width)
  x=0;

 if(width<0 || width>=Width)
  width=Width;

 if(y<0 || y>=Height)
  y=0;

 if(height<0 || height>=Height)
  height=Height;

 int x2,y2;

 x2=x+width;
 y2=y+height;
 if(x2>=Width)
 {
  x2=Width-1;
 }
 if(y2>=Height)
 {
  y2=Height-1;
 }

 if(x==x2 || y == y2)
 {
  result.NumPixels=0;
  return;
 }

 UBColor *p;

 switch(ColorModel)
 {
 case ubmY8:
  for(int i=0;i<result.GetSize();i++)
  {
   result[i].Color=i;
   result[i].Number.Int=0;
  }

  p=Data+y*Width+x;
  for(int j=y,k=0;j<=y2;j++,k++)
  {
   for(int i=x,l=0;i<=x2;i++,l++)
   {
    if(*p<result.GetSize())
     ++result[*p++].Number.Int;
    else
     ++p;
   }
   p+=Width-x2+x-1;
  }
 break;

 case ubmY32:

 break;
 }

 result.NumPixels=width*height;
}    */

// Поддерживает режимы ubmRGB24
/*void UBitmap::CalcHistogram(UBHistogram &red, UBHistogram &green, UBHistogram &blue,
                    int x, int y, int width, int height) const
{
 if(!Length || !Data || !red.GetSize() || !green.GetSize() || !blue.GetSize())
  return;

 if(x<0 || x>=Width)
  x=0;

 if(width<0 || width>=Width)
  width=Width;

 if(y<0 || y>=Height)
  y=0;

 if(height<0 || height>=Height)
  height=Height;

 int x2,y2;

 x2=x+width;
 y2=y+height;
 if(x2>=Width)
 {
  x2=Width-1;
 }
 if(y2>=Height)
 {
  y2=Height-1;
 }

 if(x==x2 || y == y2)
 {
  red.NumPixels=0;
  green.NumPixels=0;
  blue.NumPixels=0;
  return;
 }

 UBColor *p;

 switch(ColorModel)
 {
 case ubmRGB24:
  for(int i=0;i<red.GetSize();i++)
  {
   red[i].Color=i;
   red[i].Number.Int=0;
  }

  for(int i=0;i<green.GetSize();i++)
  {
   green[i].Color=i;
   green[i].Number.Int=0;
  }

  for(int i=0;i<blue.GetSize();i++)
  {
   blue[i].Color=i;
   blue[i].Number.Int=0;
  }

  p=Data+y*LineByteLength+x*PixelByteLength;
  for(int j=y,k=0;j<=y2;j++,k++)
  {
   for(int i=x,l=0;i<=x2;i++,l++)
   {
    if(*p<blue.GetSize())
     ++blue[*p++].Number.Int;
    else
     ++p;
    if(*p<green.GetSize())
     ++green[*p++].Number.Int;
    else
     ++p;
    if(*p<red.GetSize())
     ++red[*p++].Number.Int;
    else
     ++p;
   }
   p+=LineByteLength-(x2-x+1)*PixelByteLength;
  }
 break;
 }
 red.NumPixels=width*height;
 green.NumPixels=width*height;
 blue.NumPixels=width*height;
}         */
// -------------------------

// -------------------------
// Методы обработки изображения
// -------------------------
// Отражение по вертикали
// Если 'target' != 0 то результат операции сохраняется в него
// и цветовая модель 'target' замещается моделью источника
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::ReflectionX(UBitmap *target)
{
 int i;
 UBColor *buf; // буферная строка
 UBColor *beg, *fin; // указатели на начало и конец изображения
 int LineWidth=CalcLineByteLength(Width,ColorModel);

 if(target && target != this)
 {
  target->SetRes(Width,Height,ColorModel);
  beg=Data;
  fin=target->GetData()+ByteLength-LineWidth;
  for(i=0; i<Height; i++)
  {
   memcpy(fin,beg,LineWidth);
   beg+=LineWidth;
   fin-=LineWidth;
  }
 }
 else
 {
  buf=new UBColor[LineWidth];
  beg=Data;
  fin=Data+ByteLength-LineWidth;
  for(i=0; i<(Height>>1); i++)
  {
   memcpy(buf,beg,LineWidth);
   memcpy(beg,fin,LineWidth);
   memcpy(fin,buf,LineWidth);
   beg+=LineWidth;
   fin-=LineWidth;
  }
  delete []buf;
 }
}

// Отражение по горизонтали
// Если 'target' != 0 то результат операции сохраняется в него
// и цветовая модель 'target' замещается моделью источника
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::ReflectionY(UBitmap *target)
{
 int i,j;
 UBColor *buf; // буферная строка
 UBColor *beg, *fin; // указатели на начало и конец изображения
 int LineWidth=CalcLineByteLength(Width,ColorModel);

 if(target && target != this)
 {
  target->SetRes(Width,Height,ColorModel);
  for(i=0; i<Height; i++)
  {
   beg=Data+LineWidth*i;
   fin=target->GetData()+LineWidth*(i+1)-PixelByteLength;
   for(j=0;j<Width;j++)
   {
    memcpy(fin,beg,PixelByteLength);
    beg+=PixelByteLength;
    fin-=PixelByteLength;
   }
  }
 }
 else
 {
  buf=new UBColor[PixelByteLength];
  for(i=0; i<Height; i++)
  {
   beg=Data+LineWidth*i;
   fin=Data+LineWidth*(i+1)-PixelByteLength;
   for(j=0;j<(Width>>1);j++)
   {
    memcpy(buf,beg,PixelByteLength);
    memcpy(beg,fin,PixelByteLength);
    memcpy(fin,buf,PixelByteLength);
    beg+=PixelByteLength;
    fin-=PixelByteLength;
   }
  }
  delete []buf;
 }
}

// Сдвигает изображение в требуемую сторону на 'pixels' пикселей
// Направление определяется 'direction'
// 1 - Up
// 2 - Down
// 3 - Left
// 4 - Right
// При иных значениях 'direction' или отрицательных значениях pixels
// метод не делает ничего
// Пустое место заполняется цветом 'color'
// Если 'target' != 0 то результат операции сохраняется в него
void UBitmap::Move(int pixels, int direction, UBMFillType filltype, UColorT color, UBitmap *target)
{
 UBColor *pout;
 int bytelinelength=GetLineByteLength();
 int bytepixellength=0;

 if(direction < 1 || direction > 4 || pixels <= 0)
  return;

 if(!Length)
  return;

 UBColor *fillbuffer=0;

 if(!target)
  pout=Data;
 else
  {
   target->SetRes(Width,Height,ColorModel);
   pout=target->Data;
  }

 bytepixellength=GetPixelByteLength();
 switch(direction)
  {
  case ubmUp:
   if(pixels > Height)
    {
     if(!target)
      Fill(color);
     else
      target->Fill(color);

     return;
    }

   if(filltype == ubmFillImage)
   {
    fillbuffer=new UBColor[bytelinelength*pixels];
    memcpy(fillbuffer,Data,bytelinelength*pixels);
   }

   if(!target)
    memmove(pout,Data+bytelinelength*pixels,(ByteLength-bytelinelength*pixels));
   else
    memcpy(pout,Data+bytelinelength*pixels,(ByteLength-bytelinelength*pixels));

   switch(filltype)
   {
   case ubmFillColor:
    for(int i=0;i<Width;i++)
    {
     for(int j=0;j<bytepixellength;j++)
      *(pout+ByteLength-bytelinelength*pixels+i*bytepixellength+j)=*(reinterpret_cast<UBColor*>(&color)+j);
    }
    for(int i=1;i<pixels;i++)
     memcpy(pout+ByteLength-bytelinelength*(pixels-i),pout+ByteLength-bytelinelength*pixels,bytelinelength);
   break;

   case ubmFillImage:
    memcpy(pout+(ByteLength-bytelinelength*pixels),fillbuffer,bytelinelength*pixels);
    delete []fillbuffer;
   break;
   }
  break;

  case ubmDown:
   if(pixels > Height)
    {
     if(!target)
      Fill(color);
     else
      target->Fill(color);

     return;
    }

   if(filltype == ubmFillImage)
   {
    fillbuffer=new UBColor[bytelinelength*pixels];
    memcpy(fillbuffer,Data+ByteLength-bytelinelength*pixels,bytelinelength*pixels);
   }

   if(!target)
    memmove(pout+bytelinelength*pixels,Data,(ByteLength-bytelinelength*pixels));
   else
    memcpy(pout+bytelinelength*pixels,Data,(ByteLength-bytelinelength*pixels));

   switch(filltype)
   {
   case ubmFillColor:
    for(int i=0;i<Width;i++)
    {
     for(int j=0;j<bytepixellength;j++)
      *(pout+i*bytepixellength+j)=*(reinterpret_cast<UBColor*>(&color)+j);
    }
    for(int i=1;i<pixels;i++)
     memcpy(pout+bytelinelength*i,pout,bytelinelength);
   break;

   case ubmFillImage:
    memcpy(pout,fillbuffer,bytelinelength*pixels);
    delete []fillbuffer;
   break;
   }
  break;

  case ubmLeft:
   if(pixels > Width)
    {
     if(!target)
      Fill(color);
     else
      target->Fill(color);

     return;
    }

   if(filltype == ubmFillImage)
   {
    fillbuffer=new UBColor[Height*pixels*bytepixellength];
    for(int i=0;i<Height;i++)
     memcpy(fillbuffer+i*pixels*bytepixellength,
            pout+i*bytelinelength,pixels*bytepixellength);
   }

   for(int i=0;i<Height;i++)
    if(!target)
     memmove(pout+i*bytelinelength,Data+i*bytelinelength+pixels*bytepixellength,(bytelinelength-pixels*bytepixellength));
    else
     memcpy(pout+i*bytelinelength,Data+i*bytelinelength+pixels*bytepixellength,(bytelinelength-pixels*bytepixellength));

   switch(filltype)
   {
   case ubmFillColor:
    for(int i=0;i<pixels;i++)
    {
     for(int j=0;j<bytepixellength;j++)
      *(pout+bytelinelength-(pixels-i)*bytepixellength+j)=*(reinterpret_cast<UBColor*>(&color)+j);
    }
    for(int i=1;i<Height;i++)
     memcpy(pout+(i+1)*bytelinelength-pixels*bytepixellength,
            pout+bytelinelength-pixels*bytepixellength,pixels*bytepixellength);
   break;

   case ubmFillImage:
    for(int i=0;i<Height;i++)
     memcpy(pout+(i+1)*bytelinelength-pixels*bytepixellength, fillbuffer+i*pixels*bytepixellength,
            pixels*bytepixellength);
    delete []fillbuffer;
   break;
   }
  break;

  case ubmRight:
   if(pixels > Width)
    {
     if(!target)
      Fill(color);
     else
      target->Fill(color);
     return;
    }

   if(filltype == ubmFillImage)
   {
    fillbuffer=new UBColor[Height*pixels*bytepixellength];
    for(int i=0;i<Height;i++)
     memcpy(fillbuffer+i*pixels*bytepixellength,
            pout+(i+1)*bytelinelength-pixels*bytepixellength,pixels*bytepixellength);
   }

   for(int i=0;i<Height;i++)
    if(!target)
     memmove(pout+i*bytelinelength+pixels*bytepixellength,Data+i*bytelinelength,(bytelinelength-pixels*bytepixellength));
    else
     memcpy(pout+i*bytelinelength+pixels*bytepixellength,Data+i*bytelinelength,(bytelinelength-pixels*bytepixellength));

   switch(filltype)
   {
   case ubmFillColor:
    for(int i=0;i<pixels;i++)
    {
     for(int j=0;j<bytepixellength;j++)
      *(pout+i*bytepixellength+j)=*(reinterpret_cast<UBColor*>(&color)+j);
    }
    for(int i=1;i<Height;i++)
     memcpy(pout+i*bytelinelength,pout,pixels*bytepixellength);
   break;

   case ubmFillImage:
    for(int i=0;i<Height;i++)
     memcpy(pout+i*bytelinelength, fillbuffer+i*pixels*bytepixellength,
            pixels*bytepixellength);
    delete []fillbuffer;
   break;
   }
  break;
  }
}

// Сдвигает изображение на 'x' пикселей по оси абсцисс и на 'y'
// пикселей по оси ординат
// положительные значения вызывают сдвиг вправо и вниз
// отрицательные - влево и вверх
// Пустое место заполняется цветом 'color'
// Если 'target' != 0 то результат операции сохраняется в него
void UBitmap::MoveXY(int x, int y, UBMFillType filltype, UColorT color, UBitmap *target)
{
 if(x > 0)
  Move(x,ubmRight,filltype,color,target);
 else
  Move(-x,ubmLeft,filltype,color,target);

 if(y > 0)
  Move(y,ubmDown,filltype,color,target);
 else
  Move(-y,ubmUp,filltype,color,target);
}

// Изменяет размер канвы, сохраняя нетронутым изображение
// Если изображение не помещается целиком в новую канву,
// то оно усекается
// Свободное место заполняется цветом 'color'
// Если 'target' != 0 то результат операции сохраняется в него
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::ResizeCanvas(int top, int left, int right, int bottom,
                            UColorT color, UBitmap *target)
{
 UBColor *out=0,*pout;
 UBColor *line;
 int i,j;

 // Координаты остающейся части исходного изображения
 int live_left, live_top, live_width, live_height;

 // Координаты начала исходного изображения в новой канве
 int start_left, start_top;

 // Новое разрешение изображения
 int nWidth,nHeight;

 if(!top && !left && !right && !bottom)
  {
   if(target)
   {
    *target=*this;
   }
   else
    return;
  }

 nWidth=Width+left+right;
 nHeight=Height+top+bottom;

 if(nHeight <= 0)
  nHeight=0;

 if(nWidth <= 0)
  nWidth=0;

 if(!target)
  {
   if(!nWidth || !nHeight)
    {
     SetRes(nWidth,nHeight);
     return;
    }
  }
 else
  {
   target->SetColorModel(ColorModel,false);
   target->SetRes(nWidth,nHeight);

   if(!nWidth || !nHeight)
     return;
  }

  // Вычисляем координаты остатка исходного изображения
 live_left=(left>0)?0:-left;
 live_top=(top>0)?0:-top;
 live_width=Width-live_left-((right>0)?0:-right);
 live_height=Height-live_top-((bottom>0)?0:-bottom);

 // Вычисляем координаты начала исходного изображения в новой канве
 start_left=(left>0)?left:0;
 start_top=(top>0)?top:0;


 // Определяем выходной буфер
 if(target)
  {
   target->SetRes(nWidth,nHeight);
   out=target->Data;
  }
 else
  out=new UBColor[CalcByteLength(nWidth, nHeight, ColorModel)];
 pout=out;

 int nByteLineLength=CalcLineByteLength(nWidth,ColorModel);
 int ByteLineLength=CalcLineByteLength(Width,ColorModel);
 int BytePixelLength=nByteLineLength/nWidth;

 // Источник заполнения пустот
 line=new UBColor[nByteLineLength];

 UBColor *pline=line;

 switch(ColorModel)
 {
 case ubmRGB24:
 for(i=0;i<nByteLineLength;++i)
 {
  *pline++=color.rgb.b;
  *pline++=color.rgb.g;
  *pline++=color.rgb.r;
 }
 break;

 case ubmY8:
 for(i=0;i<nByteLineLength;++i)
  *pline++=color.ycrcb.y;
 break;

 case ubmY32:
 for(i=0;i<nByteLineLength;i+=4,pline+=4)
  *reinterpret_cast<unsigned int*>(pline)=color.c;
 break;
 }
 // Заполнение верхней пустоты
 for(j=0;j<start_top;j++)
  {
   memcpy(pout,line,nByteLineLength);
   pout+=nByteLineLength;
  }

 // Перенос остатка исходного изображения в новый буфер
 j=start_top;
 for(i=live_top;i<live_top+live_height;i++,j++)
 {
  memcpy(pout,line,start_left*BytePixelLength);// Заполнение пустот
  memcpy(pout+start_left*BytePixelLength,Data+i*ByteLineLength+live_left*BytePixelLength,live_width*BytePixelLength);
  memcpy(pout+(start_left+live_width)*BytePixelLength, // Заполнение пустот
                line,(nWidth-live_width-start_left)*BytePixelLength);
  pout+=nByteLineLength;
 }

 for(j=start_top+live_height;j<nHeight;j++)
  {
   memcpy(pout,line,nByteLineLength);
   pout+=nByteLineLength;
  }

 delete []line;
 if(!target)
  AttachBuffer(nWidth,nHeight,out,ColorModel);
}

// Вставляет горизонтальную полосу толщиной 'thickness'
// начиная с позиции с верхней y-координатой
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::InsertHorLine(int y, int thickness, UColorT color, UBitmap *target)
{
 if(thickness < 0 || y < 0 || y >= Height)
  return;

 int LineByteLength=CalcLineByteLength(Width,ColorModel);
 int thicksize=thickness*LineByteLength;
 UBColor *p=0;
 unsigned *p2=0;

 if(target)
  {
   target->SetRes(Width,Height+thickness,ColorModel);
   memcpy(target->Data,Data,y*LineByteLength);
   memcpy(target->Data+(y+thickness)*LineByteLength,Data+y*LineByteLength,(Height-y)*LineByteLength);
   p=target->Data+y*LineByteLength;
  }
 else
  {
   if(MemoryLength<LineByteLength*(Height+thickness))
   {
    Data=(UBColor*)realloc(Data,LineByteLength*(Height+thickness));
    MemoryLength=LineByteLength*(Height+thickness);
   }

   memmove(Data+(y+thickness)*LineByteLength, Data+y*LineByteLength, (Height-y)*LineByteLength);
   p=Data+y*LineByteLength;

   Height+=thickness;
   ByteLength=LineByteLength*Height;
   Length=Width*Height;
  }

 switch(ColorModel)
 {
 case ubmRGB24:
  if(color.c == 0 || (color.rgb.r == color.rgb.b == color.rgb.g))
   memset(p, color.rgb.b, thickness*LineByteLength);
  else
  {
   for(int i=0;i<thicksize;i+=3)
   {
    *p++=color.rgb.b;
    *p++=color.rgb.g;
    *p++=color.rgb.r;
   }
  }
 break;

 case ubmY8:
  memset(p, color.ycrcb.y, thickness*LineByteLength);
 break;

 case ubmY32:
  if(color.c == 0 || (color.rgb.r == color.rgb.b == color.rgb.g == color.rgb.d))
   memset(p, color.rgb.b, thickness*LineByteLength);
  else
  {
   p2=reinterpret_cast<unsigned*>(p);
   for(int i=0;i<thicksize;i+=4)
    *p2++=color.c;
  }
 break;
 }
}

// Удаляет горизонтальную полосу толщиной 'thickness'
// начиная с позиции с верхней y-координатой
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::RemoveHorLine(int y, int thickness, UBitmap *target)
{
 if(thickness < 0 || y < 0 || y >= Height || Height-thickness<0)
  return;

 if(y+thickness >= Height)
  thickness=Height-y;

 int LineByteLength=CalcLineByteLength(Width,ColorModel);

 if(target)
  {
   if(Height-thickness == 0)
    {
     target->SetRes(Width,0);
     return;
    }

   target->SetRes(Width,Height-thickness,ColorModel);
   memcpy(target->Data,Data,y*LineByteLength);
   memcpy(target->Data+y*LineByteLength,Data+(y+thickness)*LineByteLength,
                                           (Height-y-thickness)*LineByteLength);
  }
 else
  {
   if(Height-thickness == 0)
    {
     SetRes(Width,0);
     return;
    }

   memmove(Data+y*LineByteLength, Data+(y+thickness)*LineByteLength,
                                   (Height-y-thickness)*LineByteLength);
   Height-=thickness;
   ByteLength=Height*LineByteLength;
   Length=Width*Height;
  }
}

// Вставляет вертикальную полосу толщиной 'thickness'
// начиная с позиции с левой x-координатой
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::InsertVertLine(int x, int thickness, UColorT color, UBitmap *target)
{
 int height;
 UBColor *p, *pend, *p2, *out;

 if(thickness < 0 || x < 0 || x >= Width)
  return;

 int LineByteLength=CalcLineByteLength(Width,ColorModel);
 int nLineByteLength=CalcLineByteLength(Width+thickness,ColorModel);
// if(nLineByteLength % 2)
//  ++nLineByteLength;
 int PixelByteLength=LineByteLength/Width;

 if(target)
  {
   target->SetRes(Width+thickness,Height,ColorModel);

   p=Data; pend=Data+ByteLength;
   p2=target->Data;
   for(;p<pend;p+=LineByteLength,p2+=nLineByteLength)
    {
     memcpy(p2,p,x*PixelByteLength);
     memcpy(p2+(x+thickness)*PixelByteLength,p+x*PixelByteLength,(Width-x)*PixelByteLength);
    }

   p=p2=target->Data+x*PixelByteLength;
   height=target->Height;
  }
 else
  {
   MemoryLength=nLineByteLength*Height;
   out=p2=new UBColor[MemoryLength];
   p=Data; pend=Data+ByteLength;
   for(;p<pend;p+=LineByteLength,p2+=nLineByteLength)
    {
     memcpy(p2,p,x*PixelByteLength);
     memcpy(p2+(x+thickness)*PixelByteLength,p+x*PixelByteLength,(Width-x)*PixelByteLength);
    }
   delete []Data;
   Data=out;

   Width+=thickness;
   Length=Width*Height;
   ByteLength=MemoryLength;

   height=Height;
   p=p2=Data+x*PixelByteLength;
  }

 // Заполняем полосу цветом
 switch(ColorModel)
 {
 case ubmRGB24:
  if(color == 0 || (color.rgb.r == color.rgb.b == color.rgb.g))
  {
   for(int i=0;i<height;i++,p+=nLineByteLength)
    memset(p, color.rgb.b, thickness*PixelByteLength);
  }
  else
  {
   for(int j=0;j<thickness;j++)
   {
    *p++=color.rgb.b;
    *p++=color.rgb.g;
    *p++=color.rgb.r;
   }

   p=p2;
   p2+=nLineByteLength;
   for(int i=1;i<height;i++,p2+=nLineByteLength)
    memcpy(p2, p, thickness*PixelByteLength);
  }
 break;

 case ubmY8:
  for(int i=0;i<height;i++,p+=nLineByteLength)
   memset(p, color.rgb.b, thickness*PixelByteLength);
 break;

 case ubmY32:
  if(color == 0 || (color.rgb.r == color.rgb.b == color.rgb.g == color.rgb.d))
  {
   for(int i=0;i<height;i++,p+=nLineByteLength)
    memset(p, color.rgb.b, thickness*PixelByteLength);
  }
  else
  {
   for(int j=0;j<thickness;j++)
   {
    *p++=color.rgb.b;
    *p++=color.rgb.g;
    *p++=color.rgb.r;
    *p++=color.rgb.d;
   }

   p=p2;
   p2+=nLineByteLength;
   for(int i=1;i<height;i++,p2+=nLineByteLength)
    memcpy(p2, p, thickness*PixelByteLength);
  }
 break;
 }
}

// Удаляет вертикальную полосу толщиной 'thickness'
// начиная с позиции с левой x-координатой
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::RemoveVertLine(int x, int thickness, UBitmap *target)
{
 UBColor *p, *pend, *p2;

 if(thickness < 0 || x < 0 || x >= Width || Width-thickness<0)
  return;

 if(x+thickness >= Width)
  thickness=Width-x;

 int LineByteLength=CalcLineByteLength(Width,ColorModel);
 int nLineByteLength=CalcLineByteLength(Width-thickness,ColorModel);
 int PixelByteLength=LineByteLength/Width;

 if(target)
  {
   target->SetRes(Width-thickness,Height,ColorModel);

   p=Data; pend=Data+ByteLength;
   p2=target->Data;
   for(;p<pend;p+=LineByteLength,p2+=(Width-thickness)*PixelByteLength)
    {
     memcpy(p2,p,x*PixelByteLength);
     memcpy(p2+x*PixelByteLength,p+(x+thickness)*PixelByteLength,(Width-x-thickness)*PixelByteLength);
    }
  }
 else
  {
   p=Data+x*PixelByteLength; pend=Data+ByteLength;
   p2=Data+(x+thickness)*PixelByteLength;
   for(;p2<pend;p+=(Width-thickness)*PixelByteLength,p2+=LineByteLength)
    memmove(p,p2,(Width-thickness)*PixelByteLength);

   Width-=thickness;
   Length=Width*Height;
   ByteLength=nLineByteLength*Height;
  }
}

// Контрастирование. Производит пересчёт значений яркости в масштаб 0 - 255.
// Запись результата производится в target
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::Contrasting(UBitmap *target)
{
 switch(ColorModel)
 {
  case ubmRGB24:
   Contrasting(0,UColorT(255,255,255,0),target);
  break;

  case ubmY8:
   Contrasting(0,UColorT(255,0,0),target);
  break;

  case ubmY32:
   Contrasting(0,255,target);
  break;
 }
}

// Контрастирование. Производит пересчёт значений яркости в масштаб minb - maxb.
// Запись результата производится в target
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::Contrasting(UColorT minb, UColorT maxb, UBitmap *target)
{
 UColorT minf, maxf; // Минимум и максимум освщённости
 UBColor *out=0,*pout=0,*pdata=0;
 unsigned int dif_b,dif_c;

 if(target)
  {
   target->SetRes(Width,Height,ColorModel);
   out=target->Data;
  }
 else
  out=Data;

 FindColorRange(minf,maxf);
 switch(ColorModel)
 {
 case ubmRGB24:
  maxf.rgb.b=(maxf.rgb.b<maxf.rgb.g)?maxf.rgb.g:maxf.rgb.b;
  maxf.rgb.b=(maxf.rgb.b<maxf.rgb.r)?maxf.rgb.r:maxf.rgb.b;
  minf.rgb.b=(minf.rgb.b>minf.rgb.g)?minf.rgb.g:minf.rgb.b;
  minf.rgb.b=(minf.rgb.b>minf.rgb.r)?minf.rgb.r:minf.rgb.b;
  dif_b=maxf.rgb.b-minf.rgb.b;
  if(!dif_b)
   dif_b=1;

  pdata=Data;
  pout=out;
  for(int i=0;i<ByteLength;i+=3)
  {
   *pout++=(unsigned)((int(*pdata++) - int(minf.rgb.b))*int(maxb.rgb.b)/dif_b+int(minb.rgb.b));
   *pout++=(unsigned)((int(*pdata++) - int(minf.rgb.b))*int(maxb.rgb.b)/dif_b+int(minb.rgb.b));
   *pout++=(unsigned)((int(*pdata++) - int(minf.rgb.b))*int(maxb.rgb.b)/dif_b+int(minb.rgb.b));
  }
 break;

 case ubmY8:
  dif_b=maxf.rgb.b-minf.rgb.b;
  if(minf.rgb.b==maxf.rgb.b)
   break;

  pdata=Data;
  pout=out;
  for(int i=0;i<ByteLength;i++)
   *pout++=(unsigned)(((int(*pdata++) - int(minf.rgb.b))*(int(maxb.rgb.b)-int(minb.rgb.b)))/dif_b+int(minb.rgb.b));
 break;

 case ubmY32:
  dif_c=maxf.c-minf.c;
  if(minf.c==maxf.c)
   break;

  pdata=Data;
  pout=out;
  for(int i=0;i<ByteLength;i+=4,pout+=4,pdata+=4)
   *reinterpret_cast<unsigned*>(pout)=
    ((*reinterpret_cast<unsigned*>(pdata)-minf.c)*maxb.c)/dif_c+minb.c;
 break;
 }
}


// Контрастирование в заданной области. Производит пересчёт значений яркости в масштаб minb - maxb.
// Если 'target' != 0 то результат операции сохраняется в него
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::Contrasting(UBRect rect, UColorT minb, UColorT maxb, UColorT threshold, UBitmap *target)
{
 UColorT minf, maxf; // Минимум и максимум освщённости
 UBColor *out=0,*pout=0,*pdata=0;
 unsigned int dif_b,dif_c;


 if(rect.X<0)
  rect.X=0;

 if(rect.Y<0)
  rect.Y=0;

 if(rect.X2()>Width)
  rect.X2(Width-1);

 if(rect.Y2()>Height)
  rect.Y2(Height-1);

 if(target)
  {
   *target=*this;
   out=target->Data;
  }
 else
  out=Data;

 FindColorRange(rect,minf,maxf);
 switch(ColorModel)
 {
 case ubmRGB24:
  maxf.rgb.b=(maxf.rgb.b<maxf.rgb.g)?maxf.rgb.g:maxf.rgb.b;
  maxf.rgb.b=(maxf.rgb.b<maxf.rgb.r)?maxf.rgb.r:maxf.rgb.b;
  minf.rgb.b=(minf.rgb.b>minf.rgb.g)?minf.rgb.g:minf.rgb.b;
  minf.rgb.b=(minf.rgb.b>minf.rgb.r)?minf.rgb.r:minf.rgb.b;
  dif_b=maxf.rgb.b-minf.rgb.b;
  if(!dif_b)
   dif_b=1;

  pdata=Data+rect.Y*LineByteLength+rect.X*PixelByteLength;
  pout=out+rect.Y*LineByteLength+rect.X*PixelByteLength;
  for(int i=0;i<rect.Height;i++)
  {
   for(int j=0;j<rect.Width;j++)
   {
    *pout++=(unsigned)((int(*pdata++) - int(minf.rgb.b))*int(maxb.rgb.b)/dif_b+int(minb.rgb.b));
    *pout++=(unsigned)((int(*pdata++) - int(minf.rgb.b))*int(maxb.rgb.b)/dif_b+int(minb.rgb.b));
    *pout++=(unsigned)((int(*pdata++) - int(minf.rgb.b))*int(maxb.rgb.b)/dif_b+int(minb.rgb.b));
   }
   pout+=LineByteLength-rect.Width*PixelByteLength;
   pdata+=LineByteLength-rect.Width*PixelByteLength;
  }
 break;

 case ubmY8:
  dif_b=maxf.rgb.b-minf.rgb.b;
  if(minf.rgb.b==maxf.rgb.b)
   break;

  if(dif_b<unsigned(threshold.rgb.b))
   break;

  pdata=Data+rect.Y*LineByteLength+rect.X*PixelByteLength;
  pout=out+rect.Y*LineByteLength+rect.X*PixelByteLength;
  for(int i=0;i<rect.Height;i++)
  {
   for(int j=0;j<rect.Width;j++)
   {
    *pout++=(unsigned)((int(*pdata++) - int(minf.rgb.b))*int(maxb.rgb.b)/dif_b+int(minb.rgb.b));
   }
   pout+=LineByteLength-rect.Width*PixelByteLength;
   pdata+=LineByteLength-rect.Width*PixelByteLength;
  }
 break;

 case ubmY32:
  dif_c=maxf.c-minf.c;
  if(minf.c==maxf.c)
   break;

//  if(dif_c<threshold.c)
//   break;

  pdata=Data+rect.Y*LineByteLength+rect.X*PixelByteLength;
  pout=out+rect.Y*LineByteLength+rect.X*PixelByteLength;
  for(int i=0;i<rect.Height;i++)
  {
   for(int j=0;j<rect.Width;j++)
   {
    *reinterpret_cast<unsigned*>(pout)=
    ((*reinterpret_cast<unsigned*>(pdata)-minf.c)*maxb.c)/dif_c+minb.c;
   }
   pout+=LineByteLength-rect.Width*PixelByteLength;
   pdata+=LineByteLength-rect.Width*PixelByteLength;
  }

 break;
 }
}

// Производит бинаризацию изображения с порогом threshold
// Если 'target' != 0 то результат операции сохраняется в него
// Пиксели с яркостями ниже порога устанавливаются равными 'minval'
// Пиксели с яркостями выше порога устанавливаются равными 'maxval'
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::Binarization(UColorT threshold, UColorT minval, UColorT maxval,
                          UBitmap *target)
{
 UBColor *out,*data;

 if(target)
  {
   target->SetRes(Width,Height,ColorModel);
   out=target->Data;
  }
 else
  {
   out=Data;
  }

 data=Data;
 switch(ColorModel)
  {
   case ubmRGB24:
    for(int i=0;i<ByteLength;i++)
     {
      *out++=(*data++ >= threshold.rgb.b)?maxval.rgb.b:minval.rgb.b;
      *out++=(*data++ >= threshold.rgb.g)?maxval.rgb.g:minval.rgb.g;
      *out++=(*data++ >= threshold.rgb.r)?maxval.rgb.r:minval.rgb.r;
     }
   break;

   case ubmY8:
    for(int i=0;i<ByteLength;i++)
     *out++=(*data++ >= threshold.ycrcb.y)?maxval.ycrcb.y:minval.ycrcb.y;
   break;

   case ubmY32:
    for(int i=0;i<ByteLength;i+=4,out+=4,data+=4)
     *reinterpret_cast<unsigned*>(out)=(*reinterpret_cast<unsigned*>(data) >= threshold.c)?maxval.c:minval.c;
   break;
  }
}


// Производит автоматическую бинаризацию изображения по
// порогу, представляющему собой средневзвешенную яркость изображения
// Если 'target' != 0 то результат операции сохраняется в него
// Пиксели с яркостями ниже порога устанавливаются равными 'minval'
// Пиксели с яркостями выше порога устанавливаются равными 'maxval'
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::Binarization(UColorT minval, UColorT maxval,
                            UBitmap *target)
{
 Binarization(FindAverageColor(), minval, maxval, target);
}

// Инвертирует изображение
// Если 'target' != 0 то результат операции сохраняется в него
// Для всех цветовых моделей база устанавливается == 255
// т.о. модель ubmY32 необходим нормировать в [0:255] перед инверсией
// Поддерживает режимы ubmY8, ubmRGB24, ubmY32
void UBitmap::Inverse(UBitmap *target)
{
 UBColor *out,*data;

 if(target)
  {
   target->SetRes(Width,Height,ColorModel);
   out=target->Data;
  }
 else
  {
   out=Data;
  }

 data=Data;

 switch(ColorModel)
  {
   case ubmRGB24:
    for(int i=0;i<ByteLength;i++)
     *out++=255-*data++;
   break;

   case ubmY8:
    for(int i=0;i<ByteLength;i++)
     *out++=255-*data++;
   break;

   case ubmY32:
    for(int i=0;i<ByteLength;++i,out+=4,data+=4)
     *reinterpret_cast<unsigned*>(out)=255-*reinterpret_cast<unsigned*>(data);
   break;
  }
}
// -------------------------


// -----------------------
// Перегруженные операторы
// -----------------------
// Оператор присваивания
// Цветовая модель приемника всегда замещается моделью источника
UBitmap& UBitmap::operator = (const UBitmap &bitmap)
{
 if(Width != bitmap.Width || Height != bitmap.Height || ColorModel != bitmap.ColorModel)
  {
   Width=bitmap.Width;
   Height=bitmap.Height;
   Length=bitmap.Length;
   ByteLength=bitmap.ByteLength;
   ColorModel=bitmap.ColorModel;
   LineByteLength=bitmap.LineByteLength;
   PixelByteLength=bitmap.PixelByteLength;

   if(MemoryLength<bitmap.MemoryLength)
   {
    if(Data)
    {
     delete[] Data;
     Data=0;
    }
    Data=new UBColor[ByteLength];
    MemoryLength=ByteLength;
   }
   memcpy(ChannelOffset,bitmap.ChannelOffset,sizeof(ChannelOffset));
  }
 if(ByteLength)
  memcpy(Data,bitmap.Data,ByteLength);
 PData=Data+(bitmap.PData-bitmap.Data);

 return *this;
}
// -----------------------


// Скрытые методы
// -----------------------
// Методы расчета параметров изображения
// -----------------------
// Вычисляет длину строки изображения в байтах
int UBitmap::CalcLineByteLength(int width, UBMColorModel cmodel) const
{
 switch (cmodel)
 {
 case ubmUnknown:
  return 0;

 case ubmY32:
  return width*sizeof(unsigned int);

 case ubmRGB24:
  return width*3;

 case ubmRGB32:
  return width*4;

 case ubmRGB555:
  return width*2;

 case ubmRGB565:
  return width*2;

 case ubmRGB8:
  return width;

 case ubmRGB1:
  return width/8;

 case ubmYUY2:
  return width*2;

 case ubmYUYV:
  return width*2;

 case ubmYVU9:
  return width*9>>8;

 case ubmYVYU:
  return width*2;

 case ubmUYVY:
  return width*2;

 case ubmY211:
  return width;

 case ubmY8:
  return width;

 case ubmYCrCb411:
  return width+width/2;

 case ubmYCrCb422:
  return width*2;

 case ubmYCrCb444:
  return width*3;

 case ubmHSI:
  return width*3;

 case ubmHSV:
  return width*3;

 case ubmF32:
  return width*sizeof(float);

 default:
  return 0;
 }
}

// Возвращает длину буфера изображения в байтах, по заданным размерам и цветовой модели
int UBitmap::CalcByteLength(int width, int height, UBMColorModel cmodel) const
{
 return CalcLineByteLength(width,cmodel)*height;
}

// Расчитывает смещения каналов
void UBitmap::CalcChannelOffset(int width, int height, UBMColorModel cmodel, int *offset)
{
 switch (cmodel)
 {
 case ubmY32:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmRGB24:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmRGB32:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmY8:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmYUYV:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmYVYU:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmUYVY:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmYCrCb411:
  offset[0]=0;
  offset[1]=width*height;
  offset[2]=offset[1]+(width*height>>2);
  offset[3]=0;
 break;

 case ubmYCrCb422:
  offset[0]=0;
  offset[1]=width*height;
  offset[2]=offset[1]+(width*height>>1);
  offset[3]=0;
 break;

 case ubmYCrCb444:
  offset[0]=0;
  offset[1]=width*height;
  offset[2]=offset[1]<<1;
  offset[3]=0;
 break;

 case ubmHSI:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmHSV:
  memset(offset,0,4*sizeof(int));
 break;

 case ubmF32:
  memset(offset,0,4*sizeof(int));
 break;


 default:
  memset(offset,0,4*sizeof(int));
 }
}

// Создает новый буфер с заданными параметрами и инициализирует все внутренние переменные
void UBitmap::CreateData(int width, int height, UBMColorModel cmodel)
{
 int bytelength=CalcByteLength(width,height,cmodel);

 if(bytelength > MemoryLength)
 {
  MemoryLength=bytelength;
  if(Data)
  {
   delete[] Data;
   Data=0;
  }
  Data=new UBColor[MemoryLength];
 }

 Width=width;
 Height=height;
 Length=Width*Height;
 ColorModel=cmodel;
 ByteLength=bytelength;
 CalcChannelOffset(Width, Height, ColorModel, ChannelOffset);

 LineByteLength=CalcLineByteLength(Width,ColorModel);
 PixelByteLength=CalcLineByteLength(1,ColorModel);

 PData=Data;
}
// -----------------------

// -----------------------
// Методы преобразования цветовых моделей
// -----------------------
void UBitmap::ColorConvert(UBColor *source, UBMColorModel sourcecmodel,
UBColor *dest, UBMColorModel destcmodel) const
{
   switch (destcmodel)
   {
   case ubmY32:
    switch (sourcecmodel)
    {
        case ubmRGB24:
         ColorConvertRGB24_Y32(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_Y32(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_Y32(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_Y32(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_Y32(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_Y32(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_Y32(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_Y32(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_Y32(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_Y32(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_Y32(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_Y32(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_Y32(source, dest);
        break;
    }
   break;

   case ubmRGB24:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_RGB24(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_RGB24(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_RGB24(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_RGB24(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_RGB24(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_RGB24(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_RGB24(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_RGB24(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_RGB24(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_RGB24(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_RGB24(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_RGB24(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_RGB24(source, dest);
        break;
    }
   break;

   case ubmRGB32:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_RGB32(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_RGB32(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_RGB32(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_RGB32(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_RGB32(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_RGB32(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_RGB32(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_RGB32(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_RGB32(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_RGB32(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_RGB32(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_RGB32(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_RGB32(source, dest);
        break;
    }
   break;

   case ubmY8:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_Y8(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_Y8(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_Y8(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_Y8(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_Y8(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_Y8(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_Y8(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_Y8(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_Y8(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_Y8(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_Y8(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_Y8(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_Y8(source, dest);
        break;
    }
   break;

   case ubmYUYV:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_YUYV(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_YUYV(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_YUYV(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_YUYV(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_YUYV(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_YUYV(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_YUYV(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_YUYV(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_YUYV(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_YUYV(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_YUYV(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_YUYV(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_YUYV(source, dest);
        break;
    }
   break;

   case ubmYVYU:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_YVYU(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_YVYU(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_YVYU(source, dest);
        break;

        case ubmY8:
         ColorConvertUYVY_YVYU(source, dest);
        break;

        case ubmYUYV:
         ColorConvertUYVY_YVYU(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_YVYU(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_YVYU(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_YVYU(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_YVYU(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_YVYU(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_YVYU(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_YVYU(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_YVYU(source, dest);
        break;
    }
   break;

   case ubmUYVY:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_UYVY(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_UYVY(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_UYVY(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_UYVY(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_UYVY(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_UYVY(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_UYVY(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_UYVY(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_UYVY(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_UYVY(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_UYVY(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_UYVY(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_UYVY(source, dest);
        break;
    }
   break;

   case ubmYUY2:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_YUY2(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_YUY2(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_YUY2(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_YUY2(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_YUY2(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_YUY2(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_YUY2(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_YUY2(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_YUY2(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_YUY2(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_YUY2(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_YUY2(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_YUY2(source, dest);
        break;
    }
   break;

   case ubmHSI:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_HSI(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_HSI(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_HSI(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_HSI(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_HSI(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_HSI(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_HSI(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_HSI(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_HSI(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_HSI(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_HSI(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_HSI(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_HSI(source, dest);
        break;
    }
   break;

   case ubmHSV:
    switch (ColorModel)
    {
        case ubmY32:
         ColorConvertY32_HSV(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_HSV(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_HSV(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_HSV(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_HSV(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_HSV(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_HSV(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_HSV(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_HSV(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_HSV(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_HSV(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_HSV(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_HSV(source, dest);
        break;
    }
   break;

   case ubmYCrCb411:
    switch (sourcecmodel)
    {
        case ubmY32:
         ColorConvertY32_YCrCb411(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_YCrCb411(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_YCrCb411(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_YCrCb411(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_YCrCb411(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_YCrCb411(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_YCrCb411(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_YCrCb411(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_YCrCb411(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_YCrCb411(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_YCrCb411(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_YCrCb411(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_YCrCb411(source, dest);
        break;
    }
   break;

   case ubmYCrCb422:
    switch (sourcecmodel)
    {
        case ubmY32:
         ColorConvertY32_YCrCb422(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_YCrCb422(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_YCrCb422(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_YCrCb422(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_YCrCb422(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_YCrCb422(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_YCrCb422(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_YCrCb422(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_YCrCb422(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_YCrCb422(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_YCrCb422(source, dest);
        break;

        case ubmYCrCb444:
         ColorConvertYCrCb444_YCrCb422(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_YCrCb422(source, dest);
        break;
    }
   break;

   case ubmYCrCb444:
    switch (sourcecmodel)
    {
        case ubmY32:
         ColorConvertY32_YCrCb444(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_YCrCb444(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_YCrCb444(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_YCrCb444(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_YCrCb444(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_YCrCb444(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_YCrCb444(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_YCrCb444(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_YCrCb444(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_YCrCb444(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_YCrCb444(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_YCrCb444(source, dest);
        break;

        case ubmF32:
         ColorConvertF32_YCrCb444(source, dest);
        break;
    }
   break;


   case ubmF32:
    switch (sourcecmodel)
    {
        case ubmY32:
         ColorConvertY32_F32(source, dest);
        break;

        case ubmRGB24:
         ColorConvertRGB24_F32(source, dest);
        break;

        case ubmRGB32:
         ColorConvertRGB32_F32(source, dest);
        break;

        case ubmY8:
         ColorConvertY8_F32(source, dest);
        break;

        case ubmYUYV:
         ColorConvertYUYV_F32(source, dest);
        break;

        case ubmYVYU:
         ColorConvertYVYU_F32(source, dest);
        break;

        case ubmUYVY:
         ColorConvertUYVY_F32(source, dest);
        break;

        case ubmYUY2:
         ColorConvertYUY2_F32(source, dest);
        break;

        case ubmHSI:
         ColorConvertHSI_F32(source, dest);
        break;

        case ubmHSV:
         ColorConvertHSV_F32(source, dest);
        break;

        case ubmYCrCb411:
         ColorConvertYCrCb411_F32(source, dest);
        break;

        case ubmYCrCb422:
         ColorConvertYCrCb422_F32(source, dest);
        break;

        case ubmF32:
         ColorConvertYCrCb444_F32(source, dest);
        break;
    }
   break;
   }
}

#ifdef __BORLANDC__
#pragma warn -8057
#endif
void UBitmap::ColorConvertRGB24_Y32(UBColor *source, UBColor *dest) const
{
 unsigned int* pdest=reinterpret_cast<unsigned int*>(dest);
 for(int i=0;i<ByteLength;i+=3)
 {
  unsigned c=*(source++); c+=*(source++); c+=*(source++);
  *(pdest++)=c/3;
 }
}

void UBitmap::ColorConvertRGB24_RGB32(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;i+=3)
 {
  *(dest++)=*(source++);
  *(dest++)=*(source++);
  *(dest++)=*(source++);
  *(dest++)=0;
 }
}

void UBitmap::ColorConvertRGB24_Y8(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;i+=3)
 {
  unsigned c=*(source++); c+=*(source++); c+=*(source++);
  *dest++=static_cast<UBColor>(c/3);
 }
}

void UBitmap::ColorConvertRGB24_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB24_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB24_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB24_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB24_HSI(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;i+=3)
 {
  unsigned b=*(source++),g=*(source++),r=*(source++);
  unsigned tmin=(b<g)?b:g; tmin=(tmin<r)?tmin:r;
  unsigned tmax=(b>g)?b:g; tmax=(tmax>r)?tmax:r;
  float res=0;
  if(tmax ==tmin)
   res=0;
  else
  if(tmax == r && g>=b)
   res=static_cast<float>(60.0*(g-b)/(tmax-tmin));
  else
  if(tmax == r && g<b)
   res=static_cast<float>(60.0*(g-b)/(tmax-tmin)+360.0);
  else
  if(tmax == g)
   res=static_cast<float>(60.0*(b-r)/(tmax-tmin)+120.0);
  else
  if(tmax == b)
   res=static_cast<float>(60.0*(r-g)/(tmax-tmin)+240.0);

  // преобразуем интервал [0-360] в [0-255]
  *dest++=static_cast<UBColor>(res*255.0/360.0);

  unsigned l=(tmax+tmin)/2; // яркость

  if(l == 0 || tmax == tmin)
   res=0;
  else
  if(l>0 && l<=128)
   res=static_cast<float>((255*(tmax-tmin))/(tmax+tmin));
  else
  if(l>128 && l < 255)
   res=static_cast<float>((255*(tmax-tmin))/(512-tmax-tmin));
  else
  if(l == 255)
   res=255;

  *dest++=static_cast<UBColor>(res);

  *dest++=l;
 }
}

void UBitmap::ColorConvertRGB24_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB24_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB24_YCrCb422(UBColor *source, UBColor *dest) const
{
 //Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16
 //U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128
 //V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128
 UBColor *y=dest,*cr=y+Length,*cb=cr+(Length>>1);
 UBColor *b=source,*g=source+1,*r=source+2;
 int size=Length>>1;
 for(int i=0;i<size;i++)
 {
  *y++=((*r*66*256 + *g*129*256 + *b*25*256+128*256)>>16)+16;
  *cr=((*r*(-38)*256 + *g*(-74)*256 + *b*112*256+128*256)>>16)+128;
  r+=3; g+=3; b+=3;
  *y++=((*r*66*256 + *g*129*256 + *b*25*256+128*256)>>16)+16;
  *cb=((*r*112*256 + *g*(-94)*256 + *b*(-18)*256+128*256)>>16)+128;
  r+=3; g+=3; b+=3;
  ++cr; ++cb;
 }
}

void UBitmap::ColorConvertRGB24_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB24_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_Y32(UBColor *source, UBColor *dest) const
{
 unsigned int* pdest=reinterpret_cast<unsigned int*>(dest);

 for(int i=0;i<ByteLength;i+=4,++source)
 {
  unsigned c=*(source++); c+=*(source++); c+=*(source++);
  *(pdest++)=static_cast<UBColor>(c/3);
 }
}

void UBitmap::ColorConvertRGB32_RGB24(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;i+=4,++source)
 {
  *(dest++)=*(source++);
  *(dest++)=*(source++);
  *(dest++)=*(source++);
 }
}

void UBitmap::ColorConvertRGB32_Y8(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;i+=4,++source)
 {
  unsigned c=*(source++); c+=*(source++); c+=*(source++);
  *dest++=static_cast<UBColor>(c/3);
 }
}

void UBitmap::ColorConvertRGB32_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertRGB32_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_RGB24(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;i+=4,source+=4)
 {
  unsigned char c=*source;
  *(dest++)=c;
  *(dest++)=c;
  *(dest++)=c;
 }
}

void UBitmap::ColorConvertY32_RGB32(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;i+=4,source+=4)
 {
  unsigned char c=*source;
  *(dest++)=c;
  *(dest++)=c;
  *(dest++)=c;
  *(dest++)=0;
 }
}

void UBitmap::ColorConvertY32_Y8(UBColor *source, UBColor *dest) const
{
 unsigned int* psource=reinterpret_cast<unsigned int*>(source);

 for(int i=0;i<ByteLength;i+=4)
  *(dest++)=*(psource++);
}

void UBitmap::ColorConvertY32_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY32_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_RGB24(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;++i)
 {
  *(dest++)=*(source);
  *(dest++)=*(source);
  *(dest++)=*(source++);
 }
}

void UBitmap::ColorConvertY8_RGB32(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<ByteLength;++i)
 {
  *(dest++)=*(source);
  *(dest++)=*(source);
  *(dest++)=*(source++);
  *(dest++)=0;
 }
}

void UBitmap::ColorConvertY8_Y32(UBColor *source, UBColor *dest) const
{
 unsigned int* pdest=reinterpret_cast<unsigned int*>(dest);

 for(int i=0;i<ByteLength;i+=4)
  *(pdest++)=*(source++);
}

void UBitmap::ColorConvertY8_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertY8_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_RGB24(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_Y8(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<Length;++i,source+=2)
  *(dest++)=*source;
}

void UBitmap::ColorConvertYUYV_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUYV_F32(UBColor *source, UBColor *dest) const
{
}


void UBitmap::ColorConvertYVYU_RGB24(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_Y8(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<Length;++i,source+=2)
  *(dest++)=*source;
}

void UBitmap::ColorConvertYVYU_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYVYU_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_RGB24(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_Y8(UBColor *source, UBColor *dest) const
{
 ++source;
 for(int i=0;i<Length;++i,source+=2)
  *(dest++)=*source;
}

void UBitmap::ColorConvertUYVY_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertUYVY_F32(UBColor *source, UBColor *dest) const
{
}


void UBitmap::ColorConvertYUY2_RGB24(UBColor *source, UBColor *dest) const
{
//R = Y + 1.13983 * V;
//G = Y - 0.39465 * U - 0.58060 * V;
//B = Y + 2.03211 * U;

 UBColor *y1=source, *u=source+1,*y2=source+2,*v=source+3;
 UBColor *b=dest,*g=dest+1,*r=dest+2;
 int size=Length>>1;
 for(int i=0;i<size;i++)
 {
  int ytemp=(*y1-16)*65536;
  int u1temp=133176*(*u-128);
  int u2temp=25863*(*u-128);
  int v1temp=38050*(*v-128);
  int v2temp=74699*(*v-128);
  int res=(ytemp+u1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *b=res;
  res=(ytemp-u2temp-v1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *g=res;
  res=(ytemp+v2temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *r=res;
  b+=3; g+=3; r+=3;

  ytemp=(*y2-16)*65536;
  res=(ytemp+u1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *b=res;
  res=(ytemp-u2temp-v1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *g=res;
  res=(ytemp+v2temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *r=res;

  b+=3; g+=3; r+=3;

  y1+=4; y2+=4;
  u+=4; v+=4;
 }
}

void UBitmap::ColorConvertYUY2_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_Y32(UBColor *source, UBColor *dest) const
{
 unsigned int* pdest=reinterpret_cast<unsigned int*>(dest);
 for(int i=0;i<Length;++i,source+=2)
  *(pdest++)=*source;
}

void UBitmap::ColorConvertYUY2_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_Y8(UBColor *source, UBColor *dest) const
{
 for(int i=0;i<Length;++i,source+=2)
  *(dest++)=*source;
}

void UBitmap::ColorConvertYUY2_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_YCrCb422(UBColor *source, UBColor *dest) const
{
 UBColor *y=dest,*cr=y+Length,*cb=cr+(Length>>1);
 int size=Length>>1;
 for(int i=0;i<size;i++)
 {
  *y++=*source++;
  *cr++=*source++;
  *y++=*source++;
  *cb++=*source++;
 }
}

void UBitmap::ColorConvertYUY2_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYUY2_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_RGB24(UBColor *source, UBColor *dest) const
{
 for(int j=0;j<ByteLength;j+=3)
 {
  float h=static_cast<float>(*(source++)/255.0),
        s=static_cast<float>(*(source++)/255.0),
        i=static_cast<float>(*(source++)/255.0);
//  float hk=h/255;
  float q;

  if(i < 0.5)
   q=i*(1+s);
  else
   q=(i+s-(i*s));

  float p=2*i-q;

  // b
  float res=static_cast<float>(h-1.0/3.0);
  if(res<0)
   res+=1;
  else
  if(res>1)
   res-=1;

  if(res<1.0/6.0)
   *dest++=static_cast<UBColor>((p+((q-p)*6*res))*255);
  else
  if(res>=1.0/6.0 && res<0.5)
   *dest++=static_cast<UBColor>(q*255);
  else
  if(res>=0.5 && res < 2.0/3.0)
   *dest++=static_cast<UBColor>((p+((q-p)*6*(2.0/3.0-res)))*255);
  else
   *dest++=static_cast<UBColor>(p*255);

  // g
  res=h;
  if(res<0)
   res+=1.0;
  else
  if(res>1.0)
   res-=1;

  if(res<1.0/6.0)
   *dest++=static_cast<UBColor>((p+((q-p)*6*res))*255);
  else
  if(res>=1.0/6.0 && res<0.5)
   *dest++=static_cast<UBColor>(q*255);
  else
  if(res>=0.5 && res < 2.0/3.0)
   *dest++=static_cast<UBColor>((p+((q-p)*6*(2.0/3.0-res)))*255);
  else
   *dest++=static_cast<UBColor>(p*255);

  // r
  res=static_cast<float>(h+1.0/3.0);
  if(res<0)
   res+=1;
  else
  if(res>1)
   res-=1;

  if(res<1.0/6.0)
   *dest++=static_cast<UBColor>((p+((q-p)*6*res))*255);
  else
  if(res>=1.0/6.0 && res<0.5)
   *dest++=static_cast<UBColor>(q*255);
  else
  if(res>=0.5 && res < 2.0/3.0)
   *dest++=static_cast<UBColor>((p+((q-p)*6*(2.0/3.0-res)))*255);
  else
   *dest++=static_cast<UBColor>(p*255);
 }
}

void UBitmap::ColorConvertHSI_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_Y8(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSI_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_RGB24(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_Y8(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertHSV_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_RGB24(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_Y8(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb411_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_RGB24(UBColor *source, UBColor *dest) const
{
//R = Y + 1.13983 * V;
//G = Y - 0.39465 * U - 0.58060 * V;
//B = Y + 2.03211 * U;

 UBColor *y=source, *u=y+Length,*v=u+(Length>>1);
 UBColor *b=dest,*g=dest+1,*r=dest+2;
 int size=Length>>1;
 for(int i=0;i<size;i++)
 {
  int ytemp=(*y-16)*65536;
  int u1temp=133176*(*u-128);
  int u2temp=25863*(*u-128);
  int v1temp=38050*(*v-128);
  int v2temp=74699*(*v-128);
  int res=(ytemp+u1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *b=res;
  res=(ytemp-u2temp-v1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *g=res;
  res=(ytemp+v2temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *r=res;
  b+=3; g+=3; r+=3; ++y;

  ytemp=(*y-16)*65536;
  res=(ytemp+u1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *b=res;
  res=(ytemp-u2temp-v1temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *g=res;
  res=(ytemp+v2temp)>>16;
  if(res<0) res=0;
  if(res>255) res=255;
  *r=res;

  b+=3; g+=3; r+=3; ++y;

  u++; v++;
 }
}

void UBitmap::ColorConvertYCrCb422_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_Y8(UBColor *source, UBColor *dest) const
{
 memcpy(dest,source,Length);
}

void UBitmap::ColorConvertYCrCb422_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_YCrCb444(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb422_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_RGB24(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_Y8(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertYCrCb444_F32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_RGB24(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_RGB32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_Y32(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_YUYV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_YVYU(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_Y8(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_UYVY(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_YUY2(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_HSI(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_HSV(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_YCrCb411(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_YCrCb422(UBColor *source, UBColor *dest) const
{
}

void UBitmap::ColorConvertF32_YCrCb444(UBColor *source, UBColor *dest) const
{
}
#ifdef __BORLANDC__
#pragma warn .8057
#endif
// -----------------------

// Методы UBPoint
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBPoint::UBPoint(void)
{
 X=Y=0;
}

UBPoint::UBPoint(int x, int y)
{
 X=x;
 Y=y;
}

UBPoint::~UBPoint(void)
{
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Операторы сравнения
bool UBPoint::operator == (const UBPoint &value) const
{
 return (X == value.X && Y == value.Y);
}

bool UBPoint::operator != (const UBPoint &value) const
{
 return !(*this == value);
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBColorPoint::UBColorPoint(void)
{
}

UBColorPoint::~UBColorPoint(void)
{
}
// --------------------------


// Методы UBRect
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBRect::UBRect(void)
{
 Width=Height=0;
//2=Y2=0;
}

UBRect::UBRect(int x, int y, int width, int height)
 : UBPoint(x,y)
{
 Width=width;
 Height=height;
}
/*
UBRect::UBRect(int x, int y, int x2, int y2)
 : UBPoint(x,y)
{
 X2=x2;
 Y2=y2;
}    */

UBRect::~UBRect(void)
{
}
// --------------------------

// ---------------------
// Методы доступа к данным
// ---------------------
int UBRect::X1(void) const
{
 return X;
}

int UBRect::Y1(void) const
{
 return Y;
}

int UBRect::X2(void) const
{
 return X+Width-1;
}

int UBRect::Y2(void) const
{
 return Y+Height-1;
}

void UBRect::X2(int value)
{
 if(value<X)
  Width=value-X;
 else
  Width=value-X+1;
}

void UBRect::Y2(int value)
{
 if(value<Y)
  Height=value-Y;
 else
  Height=value-Y+1;
}

/*
int UBRect::Width(void) const
{
 return (X2>X)?X2-X:X-X2;

// return X2-X;
}

int UBRect::Height(void) const
{
 return (Y2>Y)?Y2-Y:Y-Y2;
// return Y2-Y;
}

void UBRect::Width(int value)
{
 X2=X+value-1;

}

void UBRect::Height(int value)
{
 Y2=Y+value-1;
}     */
// ---------------------

// ---------------------
// Операторы
// ---------------------
bool UBRect::operator == (const UBRect &value) const
{
 return (X == value.X) & (Y == value.Y) & (Width == value.Width) & (Height == value.Height);
// return (X == value.X) & (Y == value.Y) & (X2 == value.X2) & (Y2 == value.Y2);
}

bool UBRect::operator != (const UBRect &value) const
{
 return !(*this == value);
}
// ---------------------

// Методы UBHistogramElement
// ---------------------
// Операторы
// ---------------------
bool UBHistogramElement::operator < (const UBHistogramElement &value) const
{
 return Number.Int<value.Number.Int;
}

bool UBHistogramElement::operator > (const UBHistogramElement &value) const
{
 return Number.Int>value.Number.Int;
}

extern "C" {

// Функция сравнения двух элементов гистограммы
int UBHistogramElementCompare(const void *e1, const void *e2)
{
 if(*static_cast<const UBHistogramElement*>(e1) < *static_cast<const UBHistogramElement*>(e2))
  return -1;
 else
 if(*static_cast<const UBHistogramElement*>(e1) > *static_cast<const UBHistogramElement*>(e2))
  return 1;

 return 0;
}

}
// ---------------------

// class UBHistogram
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBHistogram::UBHistogram(void)
{
 // Данные гистограммы
 Data=0;

 // Размер гистограммы
 RealSize=Size=0;

 NumPixels=1;

 NormalizeFlag=false;
}

UBHistogram::~UBHistogram(void)
{
 if(Data)
 {
  delete []Data;
  Data=0;
  Size=0;
  RealSize=0;
 }
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Размер гистограммы
int UBHistogram::GetSize(void) const
{
 return Size;
}

bool UBHistogram::Resize(int value)
{
 if(Size == value)
  return true;

 if(RealSize < value)
 {
  delete []Data;
  Data=new UBHistogramElement[value];
  RealSize=value;
 }
 Size=value;
 return true;
}

// Возвращает состояние нормировки гистограммы
// если true - то гистограмму следует рассматривать как нормированную в веществнных числах
bool UBHistogram::IsNormalized(void) const
{
 return NormalizeFlag;
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
// Данные гистограммы
UBHistogramElement* UBHistogram::GetData(void) const
{
 return Data;
}

// Реальный размер массива
int UBHistogram::GetRealSize(void) const
{
 return RealSize;
}

// Минимум и максимум гистограммы
UBHistogramElement UBHistogram::GetMin(void) const
{
 return Min;
}

bool UBHistogram::SetMin(UBHistogramElement value)
{
 Min=value;
 return true;
}

UBHistogramElement UBHistogram::GetMax(void) const
{
 return Max;
}

bool UBHistogram::SetMax(UBHistogramElement value)
{
 Max=value;
 return true;
}

// Число элементов по которым строилась гистограмма
// Минимальное возвращаемое значение = 1
int UBHistogram::GetNumPixels(void) const
{
 if(NumPixels <= 0)
  return 1;

 return NumPixels;
}

bool UBHistogram::SetNumPixels(int value)
{
 if(NumPixels == value)
  return true;

 if(value<=0) value=1;
 NumPixels=value;
 return true;
}
// --------------------------

// --------------------------
// Методы счета
// --------------------------
// Создает данные гистограммы в соответствии с цветовой моделью
// Поддерживает режимы ubmY8, ubmRGB24, ubmRGB32
bool UBHistogram::Prepare(UBMColorModel model)
{
 switch(model)
 {
 case ubmY8:
 case ubmRGB24:
 case ubmRGB32:
  Resize(256);
  UBHistogramElement* pdata=Data;
  for(int i=0;i<Size;i++,pdata++)
  {
   pdata->Color.ycrcb.y=i;
   pdata->Number.Int=0;
  }
  return true;
 }
 return false;
}

bool UBHistogram::Prepare(const UBitmap &bmp)
{
 return Prepare(bmp.GetColorModel());
}

// Заполняет гистограмму заданным значением
void UBHistogram::Fill(unsigned value)
{
 if(NormalizeFlag == false)
 {
  UBHistogramElement* pdata=Data;
  for(int i=0;i<Size;i++,pdata++)
   pdata->Number.Int=value;
 }
}

void UBHistogram::Fill(float value)
{
 if(NormalizeFlag == true)
 {
  UBHistogramElement* pdata=Data;
  for(int i=0;i<Size;i++,pdata++)
   pdata->Number.Float=value;
 }
}

// Вычисление гистограммы по индексу цветового канала
// Поддерживает режимы ubmY8, ubmRGB24, ubmRGB32
void UBHistogram::Calc(const UBitmap &bmp, int x, int y, int width, int height, int channel)
{
 if(!bmp.GetLength() || !bmp.GetData())
  return;

 if(!Prepare(bmp))
  return;

 if(x<0 || x>=bmp.GetWidth())
  x=0;

 if(width<0 || width>=bmp.GetWidth())
  width=bmp.GetWidth();

 if(y<0 || y>=bmp.GetHeight())
  y=0;

 if(height<0 || height>=bmp.GetHeight())
  height=bmp.GetHeight();

 int x2,y2;

 x2=x+width;
 y2=y+height;
 if(x2>=bmp.GetWidth())
 {
  x2=bmp.GetWidth()-1;
 }
 if(y2>=bmp.GetHeight())
 {
  y2=bmp.GetHeight()-1;
 }

 if(x==x2 || y == y2)
 {
  NumPixels=0;
  return;
 }

 UBColor *p=0;

 bool normalize_flag=NormalizeFlag;
 Normalize(false,false);

 switch(bmp.GetColorModel())
 {
 case ubmY8:
/*  p=bmp.GetData()+y*Width+x;
  for(int j=y,k=0;j<=y2;j++,k++)
  {
   for(int i=x,l=0;i<=x2;i++,l++)
   {
    if(*p<Size)
     ++Data[*p++].Number.Int;
    else
     ++p;
   }
   p+=Width-x2+x-1;
  }
 break;
  */
 case ubmRGB24:
 case ubmRGB32:
  p=bmp.GetData()+y*bmp.GetLineByteLength()+x*bmp.GetPixelByteLength()+channel;

  for(int j=y,k=0;j<=y2;j++,k++)
  {
   for(int i=x,l=0;i<=x2;i++,l++)
   {
    if(*p<Size)
     ++Data[*p++].Number.Int;
    else
     ++p;
   }
   p+=bmp.GetLineByteLength()-(x2-x+1)*bmp.GetPixelByteLength();
  }
 break;
 }

 NumPixels=width*height;
 Normalize(normalize_flag,true);
 CalcHistogramRange();
}

void UBHistogram::Calc(const UBitmap &bmp, int channel)
{
 Calc(bmp,0,0,-1,-1,channel);
}

// Вычисление оценки интегрального распределения по существующей гистограмме
// Автоматически вычисляет диапазон значений распределения
// Если текущая гистограмма - нормированная, то распределение вычисляется нормированным
void UBHistogram::CalcCumulativeDistribution(UBHistogram &output)
{
 UBHistogramElement *pin, *pout;
 output=*this;
 pout=output.GetData();

 output.Normalize(NormalizeFlag,false);

 if(NormalizeFlag)
 {
  float res;
  output.Min.Number.Float=10e7;
  output.Max.Number.Float=0;
  for(int j=0;j<Size;j++,pout++)
  {
   res=0;
   pin=Data;
   for(int i=0;i<=j;i++,pin++)
    res+=pin->Number.Float;
   pout->Number.Float=res;
   if(output.Min.Number.Float>res)
    output.Min=*pout;
   if(output.Max.Number.Float<res)
    output.Max=*pout;
  }
 }
 else
 {
  unsigned res;
  output.Min.Number.Int=0xFFFFFFFF;
  output.Max.Number.Int=0;
  for(int j=0;j<Size;j++,pout++)
  {
   res=0;
   pin=Data;
   for(int i=0;i<=j;i++,pin++)
    res+=pin->Number.Int;
   pout->Number.Int=res;
   if(output.Min.Number.Int>res)
    output.Min=*pout;
   if(output.Max.Number.Int<res)
    output.Max=*pout;
  }
 }
}


// Меняет флаг нормировки и вычисляет значения гистограммы если 'process' == true
bool UBHistogram::Normalize(bool value, bool process)
{
 if(NormalizeFlag == value)
  return true;

 NormalizeFlag = value;

 if(process && Size && NumPixels)
 {
  UBHistogramElement* pdata=Data;
  for(int i=0;i<Size;i++,pdata++)
   pdata->Number.Float=float(pdata->Number.Int)/float(NumPixels);
  CalcHistogramRange();
 }
 return true;
}

// Заполнение дырок в гистограмме интерполированными данными
void UBHistogram::CalcZeroSmoothHistogram(UBHistogram &result, UBColor min, UBColor max)
{
 if(!NumPixels || !Size)
  return;

 result.Resize(Size);
 UBHistogramElement *presult=result.Data+min;
 int left=min, right=min;
 int left_value,right_value;
 int i=min,j;
 left_value=presult->Number.Int;

 result=*this;
 for(i=min;i<max;i++)
 {
  if(Data[i].Number.Int == 0)
  {
   for(j=i;j>=min;j--)
    if(Data[j].Number.Int != 0)
    {
     left=j;
     left_value=Data[j].Number.Int;
     break;
    }

   if(j <= min)
   {
    left=min;
    left_value=Data[min].Number.Int;
   }

   for(j=i;j<=max;j++)
    if(Data[j].Number.Int != 0)
    {
     right=j;
     right_value=Data[j].Number.Int;
     break;
    }

   if(j >= max)
   {
    right=max;
    right_value=Data[max].Number.Int;
   }
   // Нашли левую и правую ненулевые границы дырки
   for(j=left+1;j<=right-1;j++)
    result[j].Number.Int=abs(right_value-left_value)/2;

   min=right;
  }

 }

 // Квазинормировка
 unsigned numpixels=0;
 presult=result.Data;
 for(i=0;i<Size;i++)
  numpixels+=(presult++)->Number.Int;

 int diff=(numpixels-NumPixels)/Size;
 for(i=0;i<Size;i++,presult++)
  presult->Number.Int-=diff;
}

// Вычисление диапазона значений гистограммы
void UBHistogram::CalcHistogramRange(void)
{
 UBHistogramElement *pout;
 pout=Data;

 if(!Data)
  return;

 if(NormalizeFlag)
 {
  Min.Number.Float=10e7;
  Max.Number.Float=0;
  for(int j=0;j<Size;j++,pout++)
  {
   if(Min.Number.Float>pout->Number.Float)
    Min=*pout;
   if(Max.Number.Float<pout->Number.Float)
    Max=*pout;
  }
 }
 else
 {
  Min.Number.Int=0xFFFFFFFF;
  Max.Number.Int=0;
  for(int j=0;j<Size;j++,pout++)
  {
   if(Min.Number.Int>pout->Number.Int)
    Min=*pout;
   if(Max.Number.Int<pout->Number.Int)
    Max=*pout;
  }
 }

}

// Сортировка гистограммы по возрастанию значений Y
// Если output != 0, то записывает результат в него
void UBHistogram::Sort(UBHistogram *output)
{
 UBHistogramElement *data=0;
 int size=0;
 if(output)
 {
  *output=*this;
  data=output->Data;
  size=output->Size;
 }
 else
 {
  data=Data;
  size=Size;
 }

 qsort(data, size, sizeof(UBHistogramElement), UBHistogramElementCompare);
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
UBHistogramElement& UBHistogram::operator [] (int index) const
{
 return Data[index];
}

UBHistogram& UBHistogram::operator = (const UBHistogram &value)
{
 Resize(value.GetSize());
 memcpy(Data,value.GetData(),Size*sizeof(UBHistogramElement));
 NumPixels=value.NumPixels;
 Min=value.Min;
 Max=value.Max;
 NormalizeFlag=value.NormalizeFlag;

 return *this;
}
// --------------------------

}

#endif

