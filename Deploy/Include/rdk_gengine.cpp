#ifndef RDK_GENGINE_CPP
#define RDK_GENGINE_CPP

#include "rdk_engine.cpp"
#include "../../Core/Graphics/UBEngine.h"

// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
void RDKEnv_SetNumInputImages(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetNumInputImages(number);
}

// ������ ����� ������� �����
void RDKEnv_SetNumOutputImages(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetNumOutputImages(number);
}

// ������ ����� ������ �����
int RDKEnv_GetNumInputImages(void)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetNumInputImages();
}

// ������ ����� ������� �����
int RDKEnv_GetNumOutputImages(void)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetNumInputImages();
}

// ������ ���������� �� ��������� (������� ����������)
void RDKEnv_SetInputRes(int number, int width, int height)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetInputRes(number, width, height);
}

void RDKEnv_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_SetInputImage(number, image, width, height,cmodel);
}


// ���������� ���������� �� ��������� (������� ����������)
int RDKEnv_GetInputImageWidth(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageWidth(number);
}

int RDKEnv_GetInputImageHeight(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageHeight(number);
}

int RDKEnv_GetInputImageColorModel(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetInputImageColorModel(number);
}

// ���������� ������� �������� ����������
int RDKEnv_GetOutputImageWidth(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageWidth(number);
}

int RDKEnv_GetOutputImageHeight(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageHeight(number);
}

int RDKEnv_GetOutputImageColorModel(int number)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetOutputImageColorModel(number);
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

// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const RDK::UBitmap* const RDKEnv_GetComponentOutput(const char *stringid, int index)
{
 return dynamic_cast<RDK::UBEngine*>(PEngine)->Env_GetComponentOutput(stringid, index);
}
// --------------------------
#endif

