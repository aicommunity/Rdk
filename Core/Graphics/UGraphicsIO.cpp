/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef UGraphics_IO_CPP
#define UGraphics_IO_CPP

#include "UGraphicsIO.h"
#include "../../ThirdParty/ThirdParty.h"


namespace RDK {

bool LoadJpegFromFile(const char* filename, UBitmap &bmp)
{
 int width(0),height(0), actual_comps(0);
 unsigned char *bytes=jpgd::decompress_jpeg_image_from_file(filename, &width, &height, &actual_comps, 3);
 if(!bytes)
  return false;

 if(actual_comps == 1)
  bmp.SetRes(width,height,ubmY8);
 else
 if(actual_comps == 3)
  bmp.SetRes(width,height,ubmRGB24);
 else
  return false;

 memcpy(bmp.GetData(),bytes,bmp.GetByteLength());
 return true;
}

}
#endif
