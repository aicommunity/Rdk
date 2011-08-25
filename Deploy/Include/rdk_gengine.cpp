#ifndef RDK_GENGINE_CPP
#define RDK_GENGINE_CPP

#include "rdk_engine.cpp"
#include "../../Core/Graphics/UBEngine.h"

// --------------------------
// Методы управления средой
// --------------------------
void RDKEnv_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetInputImage(number, image, width, height,cmodel);
}

unsigned char* RDKEnv_GetInputImage(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImage(index);
}

unsigned char* RDKEnv_GetOutputImage(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImage(index);
}

unsigned char* RDKEnv_GetOutputImageY8(int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageY8(index);
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* const RDKEnv_GetComponentOutput(const char *stringid, int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetComponentOutput(stringid, index);
}
// --------------------------
#endif

