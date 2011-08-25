#ifndef RDK_GENGINE_H
#define RDK_GENGINE_H

#include "rdk_engine.h"
#include "../../Core/Graphics/UBitmap.h"

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------
// Методы управления средой
// --------------------------
RDK_LIB_TYPE void RDKEnv_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

RDK_LIB_TYPE unsigned char* RDKEnv_GetInputImage(int index);

RDK_LIB_TYPE unsigned char* RDKEnv_GetOutputImage(int index);

RDK_LIB_TYPE unsigned char* RDKEnv_GetOutputImageY8(int index);

// Возвращает указатель на выход с индексом 'index' компонента 'id'
RDK_LIB_TYPE const RDK::UBitmap* const RDKEnv_GetComponentOutput(const char *stringid, int index);
// --------------------------



#ifdef __cplusplus
}
#endif

#endif
