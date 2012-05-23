/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2011.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UBEngine_CPP
#define UBEngine_CPP

#include "UBEngine.h"
#include "Libraries/Libraries.h"


namespace RDK{

// --------------------------
// ������������ � �����������
// --------------------------
UBEngine::UBEngine(void)
{
}

UBEngine::~UBEngine(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� ����������� �������������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � ���������� ���������
// --------------------------
// --------------------------


// --------------------------
// ������ ������� � ������
// --------------------------
// ���������� ��������� �� �����
UBAEnvironment* UBEngine::GetEnvironment(void)
{
 return dynamic_pointer_cast<UBAEnvironment>(Environment);
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� ������ ������
bool UBEngine::Init(void)
{
 return UEngine::Init();
}

// ���������������� ������ ������
// � ��������� ������� ���������
bool UBEngine::UnInit(void)
{
 return UEngine::UnInit();
}

// ��������� �������
bool UBEngine::Start(void)
{
 return UEngine::Start();
}

// ���������������� �������
bool UBEngine::Pause(void)
{
 return UEngine::Pause();
}

// ������������� �������
bool UBEngine::Stop(void)
{
 return UEngine::Stop();
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// ������ ����� ������ �����
void UBEngine::Env_SetNumInputImages(int number)
{
 GetEnvironment()->SetNumInputImages(number);
}

// ������ ����� ������� �����
void UBEngine::Env_SetNumOutputImages(int number)
{
 GetEnvironment()->SetNumOutputImages(number);
}

// ���������� ����� ������ �����
int UBEngine::Env_GetNumInputImages(void)
{
 return GetEnvironment()->GetNumInputImages();
}

// ���������� ����� ������� �����
int UBEngine::Env_GetNumOutputImages(void)
{
 return GetEnvironment()->GetNumOutputImages();
}

// ������ ���������� �� ��������� (������� ����������)
void UBEngine::Env_SetInputRes(int number, int width, int height)
{
 GetEnvironment()->SetInputImageRes(number,width,height);
}

// ������ ���� ��������� �������� ����������� ������ �������������� ���
void UBEngine::Env_SetReflectionXFlag(bool value)
{
 GetEnvironment()->SetReflectionXFlag(value);
}

// ���������� ���������� �� ��������� (������� ����������)
int UBEngine::Env_GetInputImageWidth(int number)
{
 return GetEnvironment()->GetInputImageWidth(number);
}

int UBEngine::Env_GetInputImageHeight(int number)
{
 return GetEnvironment()->GetInputImageHeight(number);
}

int UBEngine::Env_GetInputImageColorModel(int number)
{
 return GetEnvironment()->GetInputImageColorModel(number);
}

// ���������� ������� �������� ����������
int UBEngine::Env_GetOutputImageWidth(int number)
{
 return GetEnvironment()->GetOutputImageWidth(number);
}

int UBEngine::Env_GetOutputImageHeight(int number)
{
 return GetEnvironment()->GetOutputImageHeight(number);
}

int UBEngine::Env_GetOutputImageColorModel(int number)
{
 return GetEnvironment()->GetOutputImageColorModel(number);
}

void UBEngine::Env_SetInputImage(int number, unsigned char* image, int width, int height,int cmodel)
{
 TempBmp.AttachBuffer(width,height,image,RDK::UBMColorModel(cmodel));
 RDK::UBResizeEdges.SetNewWidth(GetEnvironment()->GetInputImageWidth(number));
 RDK::UBResizeEdges.SetNewHeight(GetEnvironment()->GetInputImageHeight(number));
 RDK::UBResizeEdges(TempBmp,TempBmp2);
 GetEnvironment()->SetInputImage(number,TempBmp2);
 TempBmp.DetachBuffer();
}

unsigned char* UBEngine::Env_GetInputImage(int index)
{
 return GetEnvironment()->GetInputImage(index).GetData();
}

unsigned char* UBEngine::Env_GetOutputImage(int index)
{
// GetEnvironment()->GetOutputImage(index).SetColorModel(RDK::ubmRGB24);
 return GetEnvironment()->GetOutputImage(index).GetData();
}

unsigned char* UBEngine::Env_GetOutputImageY8(int index)
{
// GetEnvironment()->GetOutputImage(index).SetColorModel(RDK::ubmY8);
 return GetEnvironment()->GetOutputImage(index).GetData();
}
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ��������� �� ����� � �������� 'index' ���������� 'id'
const RDK::UBitmap* const UBEngine::Model_GetComponentOutput(const char *stringid, int index)
{
 UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

 if(!cont)
  return 0;

 if(index<0 || index >= cont->GetNumOutputs())
  return 0;

 return cont->GetOutputs()[index];
}
// --------------------------



// --------------------------
// ������ ����������� ���������� �������
// --------------------------
// ��������� ����� ����������������� ���������
int UBEngine::LoadPredefinedLibraries(void)
{
 bool res=UEngine::LoadPredefinedLibraries();
 if(res)
  return res;

 if(!Storage)
  return 1;

 UEPtr<RDK::UBAStorage> bstorage=dynamic_pointer_cast<RDK::UBAStorage>(Storage);

 if(!bstorage)
  return 2;

 RDK::UBAbstract *filter=new RDK::UBAQLinearContrasting;
 filter->Default();
 filter->SetName("QLinerarContrasting");
 bstorage->AddClass(filter,"QLinerarContrasting");

 filter=new RDK::UBAQSolarize;
 filter->Default();
 filter->SetName("QSolarize");
 bstorage->AddClass(filter,"QSolarize");

 filter=new RDK::UBAQEqualizeHist;
 filter->Default();
 filter->SetName("QEqualizeHist");
 bstorage->AddClass(filter,"QEqualizeHist");

 filter=new RDK::UBAQCropHist;
 filter->Default();
 filter->SetName("QCropHist");
 bstorage->AddClass(filter,"QCropHist");

 filter=new RDK::UBAQModifyHist;
 filter->Default();
 filter->SetName("QModifyHist");
 bstorage->AddClass(filter,"QModifyHist");

 filter=new RDK::UBAQLocalEqualizeHist;
 filter->Default();
 filter->SetName("QLocalEqualizeHist");
 bstorage->AddClass(filter,"QLocalEqualizeHist");

 filter=new RDK::UBAQCombineHist;
 filter->Default();
 filter->SetName("QCombineHist");
 bstorage->AddClass(filter,"QCombineHist");

 filter=new RDK::UBPipeline;
 filter->Default();
 filter->SetName("Pipeline");
 bstorage->AddClass(filter,"Pipeline");

 filter=new RDK::UBParallelPipeline;
 filter->Default();
 filter->SetName("ParallelPipeline");
 bstorage->AddClass(filter,"ParallelPipeline");

 filter=new RDK::UBAResizeEdges;
 filter->Default();
 filter->SetName("ResizeEdges");
 bstorage->AddClass(filter,"ResizeEdges");

 filter=new RDK::UBAModel;
 filter->Default();
 filter->SetName("Model");
 bstorage->AddClass(filter,"Model");

 filter=new RDK::UBASource;
 filter->Default();
 filter->SetName("Source");
 bstorage->AddClass(filter,"Source");

 filter=new RDK::UBAReceiver;
 filter->Default();
 filter->SetName("Receiver");
 bstorage->AddClass(filter,"Receiver");

 filter=new RDK::UBAVideoSimulatorSimple;
 filter->Default();
 filter->SetName("VideoSimulatorSimple");
 bstorage->AddClass(filter,"VideoSimulatorSimple");

 filter=new RDK::UBADifferenceFrameSimple;
 filter->Default();
 filter->SetName("DifferenceFrameSimple");
 bstorage->AddClass(filter,"DifferenceFrameSimple");

 filter=new RDK::UBAMaskFilteringSimple;
 filter->Default();
 filter->SetName("MaskFilteringSimple");
 bstorage->AddClass(filter,"MaskFilteringSimple");

 filter=new RDK::UBAColorConvert;
 filter->Default();
 filter->SetName("ColorConvert");
 bstorage->AddClass(filter,"ColorConvert");

 filter=new RDK::UBASearchNCC;
 filter->Default();
 filter->SetName("SearchNCC");
 bstorage->AddClass(filter,"SearchNCC");

 filter=new RDK::UBAShowObjectsSimple;
 filter->Default();
 filter->SetName("ShowObjectsSimple");
 bstorage->AddClass(filter,"ShowObjectsSimple");

 filter=new RDK::UBADataSimulatorSimple;
 filter->Default();
 filter->SetName("DataSimulatorSimple");
 bstorage->AddClass(filter,"DataSimulatorSimple");
                 
#ifdef WIN32
 filter=new RDK::UBWDllCameraCalibrator;
 filter->Default();
 filter->SetName("CameraCalibrator");
 bstorage->AddClass(filter,"WDllCameraCalibrator");
#endif

 return 0;
}
// --------------------------

// --------------------------
// ������ ����������� ���������� ��������
// --------------------------
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UBEngine::ADefault(void)
{
 if(!UEngine::ADefault())
  return false;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UBEngine::ABuild(void)
{
 if(!UEngine::ABuild())
  return false;

 return true;
}

// ����� �������� �����.
bool UBEngine::AReset(void)
{
 if(!UEngine::AReset())
  return false;

 return true;
}

// ��������� ������ ����� �������
bool UBEngine::ACalculate(void)
{
 if(!UEngine::ACalculate())
  return false;

 return true;
}
// --------------------------

}
#endif

