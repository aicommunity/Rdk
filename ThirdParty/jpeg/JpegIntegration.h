#ifndef JpegIntegration_H
#define JpegIntegration_H

// This library was obtained from https://code.google.com/p/jpeg-compressor/
#include "jpge.h"
#include "jpgd.h"
#include "../../Core/Graphics/UBitmap.h"
#include <vector>

namespace RDK {

/// Преобразовывает UBitmap в jpeg
RDK_LIB_TYPE int ConvertBitmapToJpeg(const UBitmap &bmp, std::vector<uint8_t> &jpeg_buf, std::vector<uint8_t> &temp_buf);

}

#endif
