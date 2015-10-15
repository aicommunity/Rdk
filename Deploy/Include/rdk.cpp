#ifndef RDK_CPP
#define RDK_CPP

#include <time.h>
#include "rdk.h"

//#include "../../Core/Graphics/UDrawEngine.cpp"
//#include "../../Core/Application/UApplication.cpp"
//#include "../../Core/Graphics/Graphics.cpp"
//#include "../../Core/Engine/Engine.cpp"

namespace RDK {

#ifdef __GNUC__
void RdkDummyFunc(void)
{
 time_t temp_time_data;
 RDK::get_text_time(temp_time_data);

 USerStorageBinary bin_storage;
 USerStorageXML xml_storage;
 UBitmap bmp;
 bin_storage<<bmp;
 bin_storage>>bmp;
 xml_storage<<bmp;
 xml_storage>>bmp;

 int a = 0;
 bin_storage << a;
 bin_storage >> a;

 UGraphics gr;

 NCC2D ncc;

 jpge::params jpegparams;
 jpge::compress_image_to_jpeg_file("", 0, 0, 3,
								   0,jpegparams);

 LoadJpegFromFile("", bmp);
}
#endif

}
#endif





