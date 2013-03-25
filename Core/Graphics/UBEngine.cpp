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
// Конструкторы и деструкторы
// --------------------------
UBEngine::UBEngine(void)
{
}

UBEngine::~UBEngine(void)
{
}
// --------------------------

// --------------------------
// Методы управления параметрами инициализации
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к переменным состояния
// --------------------------
// --------------------------


// --------------------------
// Методы доступа к данным
// --------------------------
// Возвращает указатель на среду
UBAEnvironment* UBEngine::GetEnvironment(void)
{
 return dynamic_pointer_cast<UBAEnvironment>(Environment);
}

UBAbstract* UBEngine::GetModel(void)
{
 return dynamic_pointer_cast<UBAbstract>(Environment->GetModel());
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Инициализирует данные движка
void UBEngine::Init(void)
{
 // Заглушка
}

bool UBEngine::Init(UEPtr<UStorage> storage, UEPtr<UEnvironment> env)
{
 return UEngine::Init(storage, env);
}

// Деинициализирует данные движка
// и сохраняет текущие настройки
void UBEngine::UnInit(void)
{
 UEngine::UnInit();
}

// Запускает систему
bool UBEngine::Start(void)
{
 return UEngine::Start();
}

// Приостанавливает систему
bool UBEngine::Pause(void)
{
 return UEngine::Pause();
}

// Останавливает систему
bool UBEngine::Stop(void)
{
 return UEngine::Stop();
}
// --------------------------

// --------------------------
// Методы управления средой
// --------------------------
// Задает число входов среды
void UBEngine::Env_SetNumInputImages(int number)
{
 GetEnvironment()->SetNumInputImages(number);
}

// Задает число выходов среды
void UBEngine::Env_SetNumOutputImages(int number)
{
 GetEnvironment()->SetNumOutputImages(number);
}

// Возвращает число входов среды
int UBEngine::Env_GetNumInputImages(void)
{
 return GetEnvironment()->GetNumInputImages();
}

// Возвращает число выходов среды
int UBEngine::Env_GetNumOutputImages(void)
{
 return GetEnvironment()->GetNumOutputImages();
}

// Задает разрешение по умолчанию (рабочее разрешение)
void UBEngine::Env_SetInputRes(int number, int width, int height)
{
 GetEnvironment()->SetInputImageRes(number,width,height);
}

// Задает флаг отражения входного изображения вокруг горизонтальной оси
void UBEngine::Env_SetReflectionXFlag(bool value)
{
 GetEnvironment()->SetReflectionXFlag(value);
}

// Возвращает разрешение по умолчанию (рабочее разрешение)
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

// Возвращает текущее выходное разрешение
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
 BResizeEdges.SetNewWidth(GetEnvironment()->GetInputImageWidth(number));
 BResizeEdges.SetNewHeight(GetEnvironment()->GetInputImageHeight(number));
 BResizeEdges(TempBmp,TempBmp2);
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
// Методы управления моделью
// --------------------------
// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* UBEngine::Model_GetComponentOutput(const char *stringid, int index)
{
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumOutputs())
   return 0;

  return cont->GetOutputs()[index];
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает указатель на выход с индексом 'index' компонента 'id'
const RDK::UBitmap* UBEngine::Model_GetComponentBitmapOutput(const char *stringid, int index)
{
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumOutputs())
   return 0;

  return cont->GetOutputs()[index];
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Возвращает указатель на вход с индексом 'index' компонента 'id'
const RDK::UBitmap* UBEngine::Model_GetComponentBitmapInput(const char *stringid, int index)
{
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return 0;

  if(index<0 || index >= cont->GetNumInputs())
   return 0;

  return cont->GetInputs()[index];
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
 return 0;
}

// Замещает изображение выхода с индексом 'index' компонента 'id'
void UBEngine::Model_SetComponentBitmapOutput(const char *stringid, int index, const RDK::UBitmap* const bmp)
{
 try{
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return;

  if(index<0 || index >= cont->GetNumOutputs())
   return;

  UBitmap *output=cont->GetOutputs()[index];

  if(!output)
   return;

  *output=*bmp;
//  UBitmap conversion;
//  conversion.AttachBuffer(bmp->GetWidth(),bmp->GetHeight(),bmp->GetData(),bmp->GetColorModel());
//  conversion.ConvertTo(*output);
//  conversion.DetachBuffer();
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}

// Замещает изображение входа с индексом 'index' компонента 'id'
void UBEngine::Model_SetComponentBitmapInput(const char *stringid, int index, const RDK::UBitmap* const bmp)
{
 try {
  UEPtr<RDK::UBAbstract> cont=dynamic_pointer_cast<RDK::UBAbstract>(FindComponent(stringid));

  if(!cont)
   return;

  if(index<0 || index >= cont->GetNumInputs())
   return;

  UBitmap *input=cont->GetInputs()[index];

  if(!input)
   return;

//  UBitmap conversion;
//  conversion.AttachBuffer(bmp->GetWidth(),bmp->GetHeight(),bmp->GetData(),bmp->GetColorModel());
//  input->SetColorModel(ubmY8,false);
//  conversion.ConvertTo(*input);
//  conversion.DetachBuffer();
  *input=*bmp;
 }
 catch (UException &exception)
 {
  ProcessException(exception);
 }
}
// --------------------------



// --------------------------
// Методы внутреннего управления движком
// --------------------------
// Загружает набор предустановленных библиотек
int UBEngine::LoadPredefinedLibraries(void)
{
 int res=UEngine::LoadPredefinedLibraries();
 if(res)
  return res;

 if(!Storage)
  return 1;

 UEPtr<RDK::UStorage> bstorage=Storage;

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

 filter=new RDK::UBSearchSimulator;
 filter->Default();
 filter->SetName("BSearch");
 bstorage->AddClass(filter,"UBSearchSimulator");

 filter=new RDK::UBSearchNCC;
 filter->Default();
 filter->SetName("BSearch");
 bstorage->AddClass(filter,"SearchNCC");

 filter=new RDK::UBTrackerSimulator;
 filter->Default();
 filter->SetName("Tracker");
 bstorage->AddClass(filter,"UBTrackerSimulator");

 filter=new RDK::UBAShowObjectsSimple;
 filter->Default();
 filter->SetName("ShowObjectsSimple");
 bstorage->AddClass(filter,"ShowObjectsSimple");

 filter=new RDK::UBADataSimulatorSimple;
 filter->Default();
 filter->SetName("DataSimulatorSimple");
 bstorage->AddClass(filter,"DataSimulatorSimple");

 filter=new RDK::UBKeyPointsClarificatorAvg;
 filter->Default();
 filter->SetName("UBKeyPointsClarificator");
 bstorage->AddClass(filter,"BKeyPointsClarificatorAvg");

 filter=new RDK::UBObjectGeometry;
 filter->Default();
 filter->SetName("ObjectGeometry");
 bstorage->AddClass(filter,"ObjectGeometry");    
 
 filter=new RDK::UBOrientationSimulatorExhaustive;
 filter->Default();
 filter->SetName("OrientationSimulator");
 bstorage->AddClass(filter,"BOrientationSimulatorExhaustive");

#ifdef RDK_WINAPI
 filter=new RDK::UBWDllCameraCalibrator;
 filter->Default();
 filter->SetName("CameraCalibrator");
 bstorage->AddClass(filter,"WDllCameraCalibrator");

 filter=new RDK::UBDllOpenCvUndistortion;
 filter->Default();
 filter->SetName("BUndistortion");
 bstorage->AddClass(filter,"BDllOpenCvUndistortion");

 filter=new RDK::UBPtzCameraControlHttpVcl;
 filter->Default();
 filter->SetName("PtzCameraControl");
 bstorage->AddClass(filter,"PtzCameraControlHttpVcl");     
 
 filter=new RDK::TDllTldTracking;
 filter->Default();
 filter->SetName("Tracker");
 bstorage->AddClass(filter,"TDllTldTracking");     

#endif

 filter=new RDK::UBABitmapSourceSimple;
 filter->Default();
 filter->SetName("BitmapSourceSimple");
 bstorage->AddClass(filter,"BitmapSourceSimple");

 filter=new RDK::UBABitmapSourceFile;
 filter->Default();
 filter->SetName("BitmapSourceFile");
 bstorage->AddClass(filter,"BitmapSourceFile");

//#define RDK_OPENCV
#ifdef RDK_OPENCV
 filter=new RDK::UBOpenCVSurf;
 filter->Default();
 filter->SetName("OpenCVSurf");
 bstorage->AddClass(filter,"OpenCVSurf");
#endif

 filter=new RDK::UBAVideo3DSimulatorSimple;
 filter->Default();
 filter->SetName("Video3DSimulatorSimple");
 bstorage->AddClass(filter,"Video3DSimulatorSimple");

 filter=new RDK::UBStatisticSimple;
 filter->Default();
 filter->SetName("BStatisticSimple");
 bstorage->AddClass(filter,"BStatisticSimple");

 filter=new RDK::UBMathOperator;
 filter->Default();
 filter->SetName("BMathOperator");
 bstorage->AddClass(filter,"BMathOperator");

 filter=new RDK::UBACameraSimple;
 filter->Default();
 filter->SetName("Camera");
 bstorage->AddClass(filter,"CameraSimple");

 filter=new RDK::UBPtzTrackerControlBasic;
 filter->Default();
 filter->SetName("PtzTrackerControl");
 bstorage->AddClass(filter,"BPtzTrackerControlBasic");

 return 0;
}
// --------------------------

// --------------------------
// Методы внутреннего управления консолью
// --------------------------
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UBEngine::ADefault(void)
{
 if(!UEngine::ADefault())
  return false;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UBEngine::ABuild(void)
{
 if(!UEngine::ABuild())
  return false;

 return true;
}

// Сброс процесса счета.
bool UBEngine::AReset(void)
{
 if(!UEngine::AReset())
  return false;

 return true;
}

// Выполняет расчет этого объекта
bool UBEngine::ACalculate(void)
{
 if(!UEngine::ACalculate())
  return false;

 return true;
}
// --------------------------

}
#endif

