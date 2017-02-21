#ifndef JpegIntegration_CPP
#define JpegIntegration_CPP

#include "JpegIntegration.h"

#include "jpge.cpp"
#include "jpgd.cpp"


namespace RDK {

/// Преобразовывает UBitmap в jpeg
int ConvertBitmapToJpeg(const UBitmap &bmp, std::vector<uint8_t> &jpeg_buf, std::vector<uint8_t> &temp_buf)
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
   comp_params.m_quality=100;
   temp_buf.resize(bmp.GetByteLength());
   int buf_size(0);
   if(!jpge::compress_image_to_jpeg_file_in_memory(&temp_buf[0], buf_size, bmp.GetWidth(), bmp.GetHeight(), 3, bmp.GetData(), comp_params))
   {
	jpeg_buf.resize(buf_size);
	if(buf_size>0)
	 memcpy(&jpeg_buf[0],&temp_buf[0],buf_size);
   }
 return 0;
}

}



#endif
