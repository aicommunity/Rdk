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
#include <vector>
#include <stdlib.h>

namespace RDK {

//extern UBitmap BmpIOBuffer;

using namespace std;

struct RDK_LIB_TYPE UBmpHeader
{
// short type;
 int32_t filesize;
 int16_t r1,r2;
 int32_t OffBits;
 int32_t size;
 int32_t width;
 int32_t height;
 int16_t Planes;
 int16_t BitCount;
 int32_t compression;
 int32_t SizeImage;
 int32_t XPelPerMetr;
 int32_t YPelPerMetr;
 int32_t ClrUsed;
 int32_t ClrImportant;
};

// -----------------------
// ������� �����-������ �����������
// -----------------------
// ��������� ����������� � �����
template <typename CharT>
basic_ostream<CharT>& operator << (basic_ostream<CharT> &stream, const UBitmap &bmp)
{
 const UBitmap *pbmp=&bmp;
 UBitmap BmpIOBuffer;

 UBmpHeader header;
 int16_t type=0x4D42;

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

// ��������� ����������� �� ������
template <typename CharT>
basic_istream<CharT>& operator >> (basic_istream<CharT> &stream, UBitmap &bmp)
{
 UBmpHeader header;
 int16_t type=0;

 stream.read(reinterpret_cast<CharT*>(&type),sizeof(type)/sizeof(CharT));
 stream.read(reinterpret_cast<CharT*>(&header),sizeof(header)/sizeof(CharT));

 bool default_order=true;
 if(header.height<0)
 {
  header.height=-header.height;
  default_order=false;
 }


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

 if(!(header.width % 4) && default_order)
 {
  int read_size=header.filesize-header.OffBits;
  read_size/=sizeof(CharT);
   stream.read(reinterpret_cast<CharT*>(bmp.GetData()),read_size);
 }
 else
 {
  unsigned char* img_pointer = bmp.GetData();
  if(!default_order)
   img_pointer+=bmp.GetByteLength()-bmp.GetLineByteLength();
  vector <unsigned char> black_hole; /// ������ ������� � ����� ������ ������
  int read_img_line = header.width * bmp.GetPixelByteLength();
  int read_bmp_line = bmp.GetLineByteLength();
  while (read_bmp_line % 4)
   read_bmp_line++;

  for (int i = 0; i< header.height; i++)
  {
   stream.read(reinterpret_cast<CharT*>(img_pointer), read_img_line);
   (default_order)?img_pointer+=read_img_line:img_pointer-=read_img_line;
   black_hole.resize(read_bmp_line-read_img_line);
   if(read_bmp_line-read_img_line>0)
    stream.read(reinterpret_cast<CharT*>(&black_hole[0]), read_bmp_line-read_img_line);
  }
 }
 return stream;
};


// ��������� ����������� � ����
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

// ��������� ����������� �� �����
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

RDK_LIB_TYPE bool LoadJpegFromFile(const char* filename, UBitmap &bmp);
RDK_LIB_TYPE bool LoadJpegFromMemory(const std::vector<uint8_t> &buffer, UBitmap &bmp);
// -----------------------

}
#endif
