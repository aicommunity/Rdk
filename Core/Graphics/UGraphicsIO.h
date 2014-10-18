/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UGraphics_IO_H
#define UGraphics_IO_H

#include "UBitmap.h"
#include <ostream>
#include <istream>
#include <fstream>

namespace RDK {

//extern UBitmap BmpIOBuffer;

using namespace std;

struct RDK_LIB_TYPE UBmpHeader
{
// short type;
 int filesize;
 short r1,r2;
 int OffBits;
 int size;
 long width;
 long height;
 short Planes;
 short BitCount;
 int compression;
 int SizeImage;
 long XPelPerMetr;
 long YPelPerMetr;
 int ClrUsed;
 int ClrImportant;
};

// -----------------------
// Функции ввода-вывода изображений
// -----------------------
// Сохраняет изображение в поток
template <typename CharT>
basic_ostream<CharT>& operator << (basic_ostream<CharT> &stream, const UBitmap &bmp)
{
 const UBitmap *pbmp=&bmp;

 UBmpHeader header;
 short type=0x4D42;

 switch(bmp.GetColorModel())
 {
 case ubmRGB24:
  header.BitCount=24;
 break;

 case ubmRGB32:
  header.BitCount=32;
 break;

 default:
 {
  UBitmap BmpIOBuffer;
  BmpIOBuffer.SetRes(bmp.GetWidth(),bmp.GetHeight(),ubmRGB24);
  bmp.ConvertTo(BmpIOBuffer);
  pbmp=&BmpIOBuffer;
  header.BitCount=24;
 }
 }

 header.filesize=2+sizeof(header)+pbmp->GetByteLength();
 header.r1=header.r2=0;
 header.OffBits=54;
 header.size=40;
 header.width=pbmp->GetWidth();
 header.height=pbmp->GetHeight();
 header.Planes=1;
 header.compression=0;
 header.SizeImage=0;
 header.XPelPerMetr=2835;
 header.YPelPerMetr=2835;
 header.ClrUsed=0;
 header.ClrImportant=0;

 stream.write(reinterpret_cast<CharT*>(&type),sizeof(type)/sizeof(CharT));
 stream.write(reinterpret_cast<CharT*>(&header),sizeof(header)/sizeof(CharT));

 stream.write(reinterpret_cast<CharT*>(pbmp->GetData()),(header.filesize-header.OffBits)/sizeof(CharT));

 return stream;
};

// Загружает изображение из потока
template <typename CharT>
basic_istream<CharT>& operator >> (basic_istream<CharT> &stream, UBitmap &bmp)
{
 UBmpHeader header;
 short type=0;

 stream.read(reinterpret_cast<CharT*>(&type),sizeof(type)/sizeof(CharT));
 stream.read(reinterpret_cast<CharT*>(&header),sizeof(header)/sizeof(CharT));

 switch(header.BitCount)
 {
 case 24:
  bmp.SetRes(header.width,header.height,ubmRGB24);
 break;

 case 32:
  bmp.SetRes(header.width,header.height,ubmRGB32);
 break;

 default:
  stream.seekg((header.filesize-header.OffBits)/sizeof(CharT),ios_base::cur);
  return stream;
 }

 stream.read(reinterpret_cast<CharT*>(bmp.GetData()),(header.filesize-header.OffBits)/sizeof(CharT));

 return stream;
};


// Сохраняет изображение в файл
template <typename CharT>
bool SaveBitmapToFile(const CharT *filename, const UBitmap &bmp)
{
 basic_ofstream<CharT> file(filename,ios::out | ios::binary | ios::trunc);

 if(!file.is_open())
  return false;

 if(file.eof())
  return false;

 file<<bmp;
 file.close();
 return true;
};

// Загружает изображение из файла
template <typename CharT>
bool LoadBitmapFromFile(const CharT *filename, UBitmap &bmp)
{
 basic_ifstream<CharT> file(filename,ios::in | ios::binary);

 if(!file.is_open())
  return false;

 if(file.eof())
  return false;
 
 file>>bmp;
 file.close();
 return true;
};
// -----------------------

}
#endif
