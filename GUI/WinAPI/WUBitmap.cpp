#ifndef WUBITMAP_CPP
#define WUBITMAP_CPP

#include "WUBitmap.h"


namespace RDK {

HBITMAP CreateBitmapFromPixel( HDC hDC, UINT uWidth, UINT uHeight, UINT uBitsPerPixel, LPVOID pBits )
{
      HBITMAP hBitmap = 0;
      if ( !uWidth || !uHeight || !uBitsPerPixel )
         return hBitmap;
      LONG lBmpSize = uWidth * uHeight * (uBitsPerPixel/8) ;       
BITMAPINFO bmpInfo = { 0 };
      bmpInfo.bmiHeader.biBitCount = uBitsPerPixel;
      bmpInfo.bmiHeader.biHeight = uHeight;
      bmpInfo.bmiHeader.biWidth = uWidth;
      bmpInfo.bmiHeader.biPlanes = 1;
      bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      // Pointer to access the pixels of bitmap       
      UINT * pPixels = 0;
      
      hBitmap = CreateDIBSection( hDC, (BITMAPINFO *)&
                                  bmpInfo, DIB_RGB_COLORS, (void **)&
                                  pPixels , NULL, 0);

      if ( !hBitmap )
          return hBitmap; // return if invalid bitmaps
      SetBitmapBits( hBitmap, lBmpSize,pBits);
      /* Or Directly Write
       memcpy(ptPixels, pBitmapBitmapBits, lBmpSize );
      */
      return hBitmap;
}

// Преобразует UBitmap в HBITMAP
void* operator >> (const UBitmap &bmp, HBITMAP &bmphandle)
{
 DeleteObject(bmphandle);
 BITMAPINFO binfo;
 UINT iUsage;
 void* ppvBits;
 UINT palette[256];

/* if(bmp.GetColorModel() == ubmMath)
 {
 }
 else
 {
  bmphandle=CreateBitmapFromPixel( 0, bmp.GetWidth(), bmp.GetHeight(), 32, ppvBits );
 }*/

 if(bmp.GetColorModel() == ubmMath)
 {
  iUsage=DIB_PAL_COLORS;
  for(unsigned int i=0;i<256;i++)
   palette[i]=i+(i<<8)+(i<<16);
//  binfo.bmiColors=&palette;
  binfo.bmiHeader.biBitCount=8;
  binfo.bmiHeader.biSizeImage=bmp.GetLength();
 }
 else
 {
  iUsage=DIB_RGB_COLORS;
//  binfo.bmiColors=0;
  binfo.bmiHeader.biBitCount=32;
  binfo.bmiHeader.biSizeImage=0;
 }

 binfo.bmiHeader.biSize=sizeof(binfo.bmiHeader);
 binfo.bmiHeader.biWidth=bmp.GetWidth();
 binfo.bmiHeader.biHeight=bmp.GetHeight();
 binfo.bmiHeader.biPlanes=1;
 binfo.bmiHeader.biCompression=BI_RGB;
 binfo.bmiHeader.biXPelsPerMeter=1000;
 binfo.bmiHeader.biYPelsPerMeter=1000;
 binfo.bmiHeader.biClrUsed=0;
 binfo.bmiHeader.biClrImportant=0;

 bmphandle=CreateDIBSection(0, &binfo, iUsage, &ppvBits, 0, 0); 

 if(ppvBits)
 {
  if(bmp.GetColorModel() != ubmMath)
   memcpy(ppvBits,bmp.GetData(),bmp.GetLength()*sizeof(UColorT));
  else
  {
   for(int i=0;i<bmp.GetLength();i++)
    ((unsigned char*)ppvBits)[i]=bmp[i];
  }
 }

 return ppvBits;
}

// Преобразует HBITMAP в UBitmap 
// если isresize == true то размеры нового изображения берутся из bmp
UBitmap& operator << (UBitmap &bmp, const HBITMAP bmphandle)
{
 bool isresize=true;
 DIBSECTION descr;

 HDC hdcSrc = CreateCompatibleDC(NULL);
 HDC hdcDst = CreateCompatibleDC(NULL);
 HBITMAP hbmNew,hbmOld,hbmOld2;
 BITMAP bm;
 if(!GetObject(bmphandle, sizeof(bm), &bm))
 {
  DeleteDC(hdcSrc);
  DeleteDC(hdcDst);
  return bmp;
 }
 hbmOld = (HBITMAP)SelectObject(hdcSrc, bmphandle);

 // -----------------------------------
 BITMAPINFO binfo;
 UINT iUsage;
 void* ppvBits;
 BOOL res;

 iUsage=DIB_RGB_COLORS;
//  binfo.bmiColors=0;
 binfo.bmiHeader.biBitCount=32;
 binfo.bmiHeader.biSizeImage=0;

 binfo.bmiHeader.biSize=sizeof(binfo.bmiHeader);

 if(isresize)
 {
  binfo.bmiHeader.biWidth=bmp.GetWidth();
  binfo.bmiHeader.biHeight=bmp.GetHeight();
 }
 else
 {
  binfo.bmiHeader.biWidth=bm.bmWidth;
  binfo.bmiHeader.biHeight=bm.bmHeight;
 }
 binfo.bmiHeader.biPlanes=1;
 binfo.bmiHeader.biCompression=BI_RGB;
 binfo.bmiHeader.biXPelsPerMeter=1000;
 binfo.bmiHeader.biYPelsPerMeter=1000;
 binfo.bmiHeader.biClrUsed=0;
 binfo.bmiHeader.biClrImportant=0;

 hbmNew=CreateDIBSection(0, &binfo, iUsage, &ppvBits, 0, 0);  
 // -----------------------------------

 hbmOld2 = (HBITMAP)SelectObject(hdcDst, hbmNew);
 res= StretchBlt(hdcDst, 0, 0, binfo.bmiHeader.biWidth, 
            binfo.bmiHeader.biHeight, 
            hdcSrc, 
            0, 0, bm.bmWidth, 
            bm.bmHeight, SRCCOPY);

//     BitBlt(hdcDst, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, SRCCOPY);
 SelectObject(hdcSrc, hbmOld);
 DeleteDC(hdcSrc);
 DeleteDC(hdcDst);

 if(!res)
  return bmp;

 if(!GetObject(hbmNew, sizeof(descr), &descr))
  return bmp;

 if(!descr.dsBm.bmBits)
  return bmp;

 bmp.SetRes(descr.dsBm.bmWidth,descr.dsBm.bmHeight);
 bmp.SetColorModel(ubmColor,false);

 memcpy(bmp.GetData(),descr.dsBm.bmBits,bmp.GetLength()*sizeof(UColorT));

 DeleteObject(hbmNew);

 return bmp;
}

// Устанавливает в UBitmap разрешение такое же как у HBITMAP
UBitmap& ULoadBitmapRes(UBitmap &bmp, const HBITMAP bmphandle)
{
 BITMAP bm;
 if(!GetObject(bmphandle, sizeof(bm), &bm))
  return bmp;

 bmp.SetRes(bm.bmWidth,bm.bmHeight);

 return bmp;
}


}
#endif
