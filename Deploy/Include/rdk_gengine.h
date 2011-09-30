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
// Задает число входов среды
RDK_LIB_TYPE void RDKEnv_SetNumInputImages(int number);

// Задает число выходов среды
RDK_LIB_TYPE void RDKEnv_SetNumOutputImages(int number);

// Возвращает число входов среды
RDK_LIB_TYPE int RDKEnv_GetNumInputImages(void);

// Возвращает число выходов среды
RDK_LIB_TYPE int RDKEnv_GetNumOutputImages(void);

// Задает разрешение по умолчанию (рабочее разрешение)
RDK_LIB_TYPE void RDKEnv_SetInputRes(int number, int width, int height);

RDK_LIB_TYPE void RDKEnv_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel);

// Возвращает разрешение по умолчанию (рабочее разрешение)
RDK_LIB_TYPE int RDKEnv_GetInputImageWidth(int number);
RDK_LIB_TYPE int RDKEnv_GetInputImageHeight(int number);
RDK_LIB_TYPE int RDKEnv_GetInputImageColorModel(int number);

// Возвращает текущее выходное разрешение
RDK_LIB_TYPE int RDKEnv_GetOutputImageWidth(int number);
RDK_LIB_TYPE int RDKEnv_GetOutputImageHeight(int number);
RDK_LIB_TYPE int RDKEnv_GetOutputImageColorModel(int number);

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
