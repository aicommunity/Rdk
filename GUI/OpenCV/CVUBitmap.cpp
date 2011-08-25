/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2008.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru
Version:        3.0.0

This file - part of the project: RDK

File License:        BSD License
Project License:    BSD License
See file license.txt for more information
*********************************************************** */


#include "CVUBitmap.h"

namespace RDK {

//---------------------------------------------------------------------------
// Отправляет данные объекта UBitmap в IplImage
void operator >> (UBitmap &source, IplImage* &target)
{
 if(target &&  (target->width != source.GetWidth() ||
                target->height != source.GetHeight() ||
                (source.GetColorModel() == ubmRGB32 && target->nChannels != 4) ||
                (source.GetColorModel() == ubmY8 && target->nChannels != 1)  ||
                (source.GetColorModel() == ubmRGB24 && target->nChannels != 3) ))
 {
    cvReleaseImage( &target );
    target=0;
 }

 switch(source.GetColorModel())
 {
 case ubmRGB24:
  if(!target)
   target = cvCreateImage( cvSize(source.GetWidth(),source.GetHeight()), IPL_DEPTH_8U, 3 );

  memcpy(target->imageData,source.GetData(),source.GetByteLength());
 break;

 case ubmRGB32:
  if(!target)
   target = cvCreateImage( cvSize(source.GetWidth(),source.GetHeight()), IPL_DEPTH_8U, 4 );

  memcpy(target->imageData,source.GetData(),source.GetByteLength());
 break;

 case ubmY8:
  if(!target)
   target = cvCreateImage( cvSize(source.GetWidth(),source.GetHeight()), IPL_DEPTH_8U, 1 );

  memcpy(target->imageData,source.GetData(),source.GetByteLength());
 break;
 }
}

// Отправляет данные объекта IplImage в UBitmap
void operator << (UBitmap &target, const IplImage *source)
{
 if(!source)
  return;

 if(source->nChannels == 1 && source->depth == IPL_DEPTH_8U)
 {
  target.SetRes(source->width,source->height,ubmY8);
  memcpy(target.GetData(), source->imageData,target.GetByteLength());
 }
 else
 if(source->nChannels == 3 && source->depth == IPL_DEPTH_8U)
 {
  target.SetRes(source->width,source->height,ubmRGB24);
  memcpy(target.GetData(), source->imageData,target.GetByteLength());
 }
}
//---------------------------------------------------------------------------

}

#pragma package(smart_init)
