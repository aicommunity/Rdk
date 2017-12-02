#ifndef JpegIntegration_CPP
#define JpegIntegration_CPP

#include "JpegIntegration.h"

#include "jpge.cpp"
#include "jpgd.cpp"


namespace RDK {

/// Преобразовывает UBitmap в jpeg
int ConvertBitmapToJpeg(const UBitmap &bmp, std::vector<uint8_t> &jpeg_buf, std::vector<uint8_t> &temp_buf, bool order, int quality)
{
   if(bmp.GetColorModel() != ubmRGB24)
   {
	// ошибка
	jpeg_buf.clear();
	return 1;
   }

   if(bmp.GetByteLength() == 0)
   {
	// ошибка
	jpeg_buf.clear();
	return 2;
   }

   jpge::params comp_params;
   comp_params.m_quality=quality;
   temp_buf.resize(bmp.GetByteLength());
   int buf_size(bmp.GetByteLength());
   if(jpge::compress_image_to_jpeg_file_in_memory(&temp_buf[0], buf_size, bmp.GetWidth(), bmp.GetHeight(), 3, bmp.GetData(), comp_params,order))
   {
	jpeg_buf.resize(buf_size);
	if(buf_size>0)
	 memcpy(&jpeg_buf[0],&temp_buf[0],buf_size);
   }
 return 0;
}

/// Преобразовывает jpeg в UBitmap
/// order определяет порядок следования цветовых каналов: RGB - true, BGR - false
int ConvertJpegToBitmap(const std::vector<uint8_t> &jpeg_buf, UBitmap &bmp, bool order)
{
 if(jpeg_buf.empty())
  return 1;

 int actual_comps(0);
 int width(0), height(0);
 unsigned char * buf=jpgd::decompress_jpeg_image_from_memory(&jpeg_buf[0], int(jpeg_buf.size()), &width, &height, &actual_comps, 3, order);
 if(!buf)
  return 2;

 bmp.AttachBuffer(width,height,buf,ubmRGB24);
 return 0;
}

/// Возвращает разрешение изображения
int ReadJpegSize(const std::vector<uint8_t> &jpeg_buf, int &width, int &height)
{
 if(jpeg_buf.empty())
  return 1;

 width=height=0;
 jpgd::param_image(&jpeg_buf[0], int(jpeg_buf.size()), &width, &height);
 return 0;
}



}



#endif
