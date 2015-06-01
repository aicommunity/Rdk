//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoRegistratorFrameUnit.h"
#include "UGEngineControlFormUnit.h"
#include "TUBitmap.h"
#include "VideoOutputFormUnit.h"
#include "VideoOutputFrameUnit.h"
#include "UGEngineControlFormUnit.h"

#include <Vcl.FileCtrl.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma resource "*.dfm"
TTVideoRegistratorFrame *TVideoRegistratorFrame;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoGetBitmapFrameThread::TVideoGetBitmapFrameThread(TTVideoRegistratorFrame *frame, bool CreateSuspended)
: Frame(frame), TThread(CreateSuspended)
{
 SourceMode=-1;
 SourceUnlock=CreateEvent(0,TRUE,TRUE,0);
 SourceWriteUnlock=CreateEvent(0,TRUE,TRUE,0);
 FrameNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 CalcCompleteEvent=CreateEvent(0,TRUE,TRUE,0);
 ReadSource=&Source[0];
 WriteSource=&Source[1];
 FreeOnTerminate=false;
}

__fastcall TVideoGetBitmapFrameThread::~TVideoGetBitmapFrameThread(void)
{
 CloseHandle(SourceUnlock);
 CloseHandle(SourceWriteUnlock);
 CloseHandle(FrameNotInProgress);
 CloseHandle(CalcCompleteEvent);
}
// --------------------------
// Управление параметрами
// --------------------------
/// Источник видео
int TVideoGetBitmapFrameThread::GetSourceMode(void) const
{
 return SourceMode;
}

/// Индекс канала в библиотеке аналитики, управляемый тредом
int TVideoGetBitmapFrameThread::GetChannelIndex(void) const
{
 return ChannelIndex;
}

bool TVideoGetBitmapFrameThread::SetChannelIndex(int value)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 ChannelIndex=value;
 return true;
}
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Указатель на владельца
TTVideoRegistratorFrame* TVideoGetBitmapFrameThread::GetFrame(void) const
{
 return Frame;
}

bool TVideoGetBitmapFrameThread::SetFrame(TTVideoRegistratorFrame * frame)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 Frame=frame;
 return true;
}
// --------------------------
// Управление событиями
// --------------------------
/// Выставляется по завершении захвата нового кадра
HANDLE TVideoGetBitmapFrameThread::GetFrameNotInProgress(void) const
{
 return FrameNotInProgress;
}

/// Выставлено всегда. Сбрасывается на время доступа к изображению
HANDLE TVideoGetBitmapFrameThread::GetSourceUnlock(void) const
{
 return SourceUnlock;
}

/// Сбрасывается на время ожидания расчета
HANDLE TVideoGetBitmapFrameThread::GetCalcCompleteEvent(void) const
{
 return CalcCompleteEvent;
}
// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoGetBitmapFrameThread::Start(void)
{

}

void __fastcall TVideoGetBitmapFrameThread::Stop(void)
{

}

void __fastcall TVideoGetBitmapFrameThread::BeforeCalculate(void)
{

}

void __fastcall TVideoGetBitmapFrameThread::AfterCalculate(void)
{

}



void __fastcall TVideoGetBitmapFrameThread::Execute(void)
{
 while(!Terminated)
 {
  ResetEvent(CalcCompleteEvent);
  ResetEvent(FrameNotInProgress);
  BeforeCalculate();
  Calculate();
  AfterCalculate();
  SetEvent(CalcCompleteEvent);
  SetEvent(FrameNotInProgress);
 }
}
/// Возвращает копию изображения с блокировкой
bool TVideoGetBitmapFrameThread::ReadSourceSafe(RDK::UBitmap& dest, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceUnlock);
 RDK::UBitmap* source=ReadSource;

 if(reflect)
  source->ReflectionX(&dest);
 else
  dest=*source;

 SetEvent(SourceUnlock);
 return true;
}

bool TVideoGetBitmapFrameThread::ReadSourceSafe(Graphics::TBitmap *dest, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceUnlock);
 RDK::UBitmap* source=ReadSource;
 UBitmapToTBitmap(*source, dest, reflect);

 SetEvent(SourceUnlock);
 return true;
}

/// Записывает изображение в тред с блокировкой
bool TVideoGetBitmapFrameThread::WriteSourceSafe(const RDK::UBitmap& src, bool reflect)
{
 if(WaitForSingleObject(SourceWriteUnlock,100) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceWriteUnlock);
 if(reflect)
  const_cast<RDK::UBitmap&>(src).ReflectionX(WriteSource);
 else
  *WriteSource=src;
 SetEvent(SourceWriteUnlock);

 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);

 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}

bool TVideoGetBitmapFrameThread::WriteSourceSafe(Graphics::TBitmap *src, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceUnlock);
 TBitmapToUBitmap(*WriteSource, src, reflect);

 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}
//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoGetBitmapFrameFromVideoThread::TVideoGetBitmapFrameFromVideoThread(TTVideoRegistratorFrame *frame, bool CreateSuspended)
: TVideoGetBitmapFrameThread(frame, CreateSuspended)
{

}

__fastcall TVideoGetBitmapFrameFromVideoThread::~TVideoGetBitmapFrameFromVideoThread(void)
{

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
bool TVideoGetBitmapFrameFromVideoThread::SetFrameIndex(const int &value)
{
 if(FrameIndex == value)
  return true;

 FrameIndex=value;
 return true;
}

const int& TVideoGetBitmapFrameFromVideoThread::GetFrameIndex(void) const
{
 return FrameIndex;
}

bool TVideoGetBitmapFrameFromVideoThread::SetVideoFrame(TVideoOutputFrame* videoFrame)
{
 if(VideoOutputFrame == videoFrame)
  return true;

 VideoOutputFrame=videoFrame;
 return true;
}

TVideoOutputFrame* TVideoGetBitmapFrameFromVideoThread::GetVideoFrame(void) const
{
 return VideoOutputFrame;
}
// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoGetBitmapFrameFromVideoThread::Start(void)
{
 TVideoGetBitmapFrameThread::Start();
}

void __fastcall TVideoGetBitmapFrameFromVideoThread::Stop(void)
{
 TVideoGetBitmapFrameThread::Stop();
}


void __fastcall TVideoGetBitmapFrameFromVideoThread::BeforeCalculate(void)
{

}

void __fastcall TVideoGetBitmapFrameFromVideoThread::AfterCalculate(void)
{
  TVideoGetBitmapFrameThread::AfterCalculate();
}

void __fastcall TVideoGetBitmapFrameFromVideoThread::Calculate(void)
{
 if(VideoOutputFrame)
 {
  double time_stamp;
  VideoOutputFrame->CaptureThread->ReadSourceSafe(TempBitmap, time_stamp, false);
  if(TempBitmap.GetData())
   WriteSourceSafe(TempBitmap, false);

  else
  {
   TempBitmap.SetRes(640, 480, RDK::ubmRGB24);
   TempBitmap.Fill(0);
   WriteSourceSafe(TempBitmap, true);
  }
 }
}
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoGetBitmapFrameFromComponentThread::TVideoGetBitmapFrameFromComponentThread(TTVideoRegistratorFrame *frame, bool CreateSuspended)
: TVideoGetBitmapFrameThread(frame, CreateSuspended)
{

}

__fastcall TVideoGetBitmapFrameFromComponentThread::~TVideoGetBitmapFrameFromComponentThread(void)
{

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
bool TVideoGetBitmapFrameFromComponentThread::SetComponentName(const std::string &comp_name)
{
 if(ComponentName == comp_name)
  return true;

 ComponentName = comp_name;
 return true;
}

const std::string TVideoGetBitmapFrameFromComponentThread::GetComponentName(void)
{
 return ComponentName;
}

bool TVideoGetBitmapFrameFromComponentThread::SetPropertyName(const std::string &property_name)
{
 if(PropertyName == property_name)
  return true;

 PropertyName = property_name;
 return true;
}

const std::string TVideoGetBitmapFrameFromComponentThread::GetPropertyName(void)
{
 return PropertyName;
}

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoGetBitmapFrameFromComponentThread::Start(void)
{
 TVideoGetBitmapFrameThread::Start();
}

void __fastcall TVideoGetBitmapFrameFromComponentThread::Stop(void)
{
 TVideoGetBitmapFrameThread::Stop();
}


void __fastcall TVideoGetBitmapFrameFromComponentThread::BeforeCalculate(void)
{

}

void __fastcall TVideoGetBitmapFrameFromComponentThread::AfterCalculate(void)
{
  TVideoGetBitmapFrameThread::AfterCalculate();
}

void __fastcall TVideoGetBitmapFrameFromComponentThread::Calculate(void)
{
 if(ComponentName != "" && PropertyName != "")
 {
  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentBitmapOutput(ComponentName.c_str(), PropertyName.c_str());
  if(bmp && bmp->GetData())
  {
   TempBitmap=*bmp;
   WriteSourceSafe(TempBitmap, true);
  }
  else
  {
   TempBitmap.SetRes(640, 480, RDK::ubmRGB24);
   TempBitmap.Fill(0);
   WriteSourceSafe(TempBitmap, true);
  }
 }
}
//---------------------------------------------------------------------------
__fastcall TTVideoRegistratorFrame::TTVideoRegistratorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 // Заполнение массива ошибок
 FillErrorsArray();

 // Загрузка списка компонент
 MyComponentsListForm=new TUComponentsListForm(this);
}
//---------------------------------------------------------------------------
__fastcall TTVideoRegistratorFrame::~TTVideoRegistratorFrame(void)
{
 if(BitmapFrameThread)
 {
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }

 if(MyComponentsListForm)
  delete MyComponentsListForm;
}
//---------------------------------------------------------------------------
// Методы
void __fastcall TTVideoRegistratorFrame::AssignListToComboBox (TComboBox* ComboBox, String List, int Index)
{
 ComboBox->Items->Text = List;
 if (ComboBox->Items->Count > 0)
 {
  ComboBox->ItemIndex = Index;
 }
}
// Заполнение массива ошибок
void TTVideoRegistratorFrame::FillErrorsArray(void)
{
 Errors[XCAMSNOTFOUND]="Cameras not found";
 Errors[XREINITSYSREM]="Dll already init";
 Errors[XCAMALREADYINIT]="Camera already init";
 Errors[XCAMALREADYSTART]="Camera already started";
 Errors[XNOIP]="IP not found";
 Errors[XINITERROR]="Init error";
 Errors[XSTARTERROR]="Camera start error";
 Errors[XCAMNOTINIT]="Camera not init";
 Errors[XFRAMEREADERROR]="Frame read error";
 Errors[XCAMNOTSTARTED]="Camera not started";
 Errors[XCAMFRAMETYPEERROR]="Frame type error";
 Errors[XCAMCOULDNOTSTOP]="Camera could not stopped";

 FrameIndex=0;
}
//---------------------------------------------------------------------------
// Обновление доступного интерфеса
void __fastcall TTVideoRegistratorFrame::RefreshDeviceControls(void)
{
 bool CanUseCompressors=(RecordingMethodComboBox->ItemIndex != 0) && (RecordingMethodComboBox->ItemIndex != 8);
 bool CanUseStoragePath=(StoragePathRadioGroup->ItemIndex == 1);
 bool CanUseRecTimer=(RecordingModeComboBox->ItemIndex!=0 || !CanUseCompressors);
 bool CanUsePreAllocate=(UsePreallocatedFileCheckBox->Checked==true);

 PreallocatedFileSizeLabeledEdit->Enabled=CanUsePreAllocate;
 PreallocatedFileCreateButton->Enabled=CanUsePreAllocate;

 BrowseStoragePathButton->Enabled = CanUseStoragePath;
 StoragePathLabeledEdit->Enabled = CanUseStoragePath;

 RecordingModeComboBox->Enabled = CanUseCompressors;
 VideoCompressorComboBox->Enabled = CanUseCompressors;
 UsePreallocatedFileCheckBox->Enabled = CanUseCompressors;
 VideoCompressorSettingsButton->Enabled = CanUseCompressors;
 VideoCompressionModeComboBox->Enabled = CanUseCompressors;

 RecordingTimerLabeledEdit->Enabled=CanUseRecTimer;
 RecordingTimerComboBox->Enabled=CanUseRecTimer;

}
//---------------------------------------------------------------------------
// Логгирование ошибок
bool TTVideoRegistratorFrame::WriteLogMessage(const int &err)
{
 if(Errors[err] != "")
 {
  Engine_LogMessage(RDK_EX_DEBUG, Errors[err].c_str());
  return false;
 }

 return true;
}
//---------------------------------------------------------------------------
// Установка флага показа preview
bool TTVideoRegistratorFrame::SetPreviewFlag(const bool &value)
{
 if(PreviewFlag == value)
  return true;

 PreviewFlag = value;
 return true;
}
//---------------------------------------------------------------------------
// Инициализирует DLL
int TTVideoRegistratorFrame::Init(void)
{
 // Формат сжатия
 VideoCompressorComboBox->Items->Clear();
 AssignListToComboBox(VideoCompressorComboBox, VideoGrabber->VideoCompressors, VideoGrabber->VideoCompressor);

 InitStreamingSettings();
 InitRecordingSettings();
 RefreshDeviceControls();
 return 0;
}
//---------------------------------------------------------------------------
// Деинициализирует DLL
int TTVideoRegistratorFrame::UnInit(void)
{
 //
 return 0;
}
//---------------------------------------------------------------------------
// Инициализирует настройки direct network streaming TVideoGrabber
int TTVideoRegistratorFrame::InitStreamingSettings(void)
{
 VideoGrabber->ASFVideoMaxKeyFrameSpacing = StrToIntDef(MaxKeyFrameSpacingLabeledEdit->Text, 100);
 VideoGrabber->ASFBufferWindow = StrToIntDef(BufferWindowLabeledEdit->Text, 100);
 VideoGrabber->ASFVideoWidth = StrToIntDef(VideoWidthLabeledEdit->Text, 320);
 VideoGrabber->ASFVideoHeight = StrToIntDef(VideoHeightLabeledEdit->Text, 240);
 VideoGrabber->ASFVideoBitRate = StrToIntDef(BitRateLabeledEdit->Text, 2000000);
 VideoGrabber->NetworkStreaming = ns_ASFDirectNetworkStreaming;
 VideoGrabber->NetworkStreamingType = nst_VideoStreaming;
 VideoGrabber->ASFNetworkPort = StrToIntDef(PortLabeledEdit->Text, 45050);
 VideoGrabber->FrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 VideoGrabber->ASFVideoFrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 VideoGrabber->ASFNetworkMaxUsers = StrToIntDef(MaxUsersLabeledEdit->Text, 4);
 return 0;
}
//---------------------------------------------------------------------------
// Инициализирует настройки записи в файл TVideoGrabber
int TTVideoRegistratorFrame::InitRecordingSettings(void)
{
 VideoGrabber->ASFVideoWidth = StrToIntDef(RecordWidthLabeledEdit->Text, 320);
 VideoGrabber->ASFVideoHeight = StrToIntDef(RecordHeightLabeledEdit->Text, 240);
 VideoGrabber->ASFVideoFrameRate = StrToIntDef(RecordingFrameRateLabeledEdit->Text, 30);
 VideoGrabber->FrameRate = StrToIntDef(RecordingFrameRateLabeledEdit->Text, 30);
 VideoGrabber->RecordingFileName = RecordingFileNameLabeledEdit->Text;
 VideoGrabber->RecordingMethod = RecordingMethodComboBox->ItemIndex;
 if(StoragePathRadioGroup->ItemIndex == 0 || StoragePathLabeledEdit->Text=="")
 {
  VideoGrabber->StoragePath=UGEngineControlForm->ProjectPath;
 }
 else
 {
  VideoGrabber->StoragePath=StoragePathLabeledEdit->Text;
 }


 if((RecordingMethodComboBox->ItemIndex != 0) && (RecordingMethodComboBox->ItemIndex != 8))
 {
  VideoGrabber->VideoCompressor = VideoCompressorComboBox->ItemIndex;
  VideoGrabber->CompressionMode = VideoCompressionModeComboBox->ItemIndex;
 }
 else
 {
  VideoCompressionModeComboBox->ItemIndex=0;
  VideoGrabber->CompressionMode = 0;
 }

 // Запись по таймеру
 if(RecordingModeComboBox->ItemIndex != 0)
 {
  int recTimerMode=RecordingModeComboBox->ItemIndex;
  VideoGrabber->RecordingTimer=recTimerMode;

  switch(recTimerMode)
  {
   case 0:
   {
	VideoGrabber->RecordingTimerInterval = StrToIntDef(RecordingTimerLabeledEdit->Text, 10);
   }
   case 1:
   {
	VideoGrabber->RecordingTimerInterval = StrToIntDef(RecordingTimerLabeledEdit->Text, 10)*60;
   }
   case 2:
   {
	VideoGrabber->RecordingTimerInterval = StrToIntDef(RecordingTimerLabeledEdit->Text, 10)*3600;
   }
  }

 }

 // Preallocated file
 if(UsePreallocatedFileCheckBox->Checked)
 {
  VideoGrabber->PreallocCapFileEnabled=UsePreallocatedFileCheckBox->Checked;
  VideoGrabber->PreallocCapFileSizeInMB=StrToIntDef(PreallocatedFileSizeLabeledEdit->Text, 100);
  VideoGrabber->PreallocCapFileName=RecordingFileNameLabeledEdit->Text;
 }
 return 0;
}
//---------------------------------------------------------------------------
int TTVideoRegistratorFrame::GetBitmapFrame(void)
{
 if(!BitmapFrameThread)
  return 0;

 BitmapFrameThread->ReadSourceSafe(InputFrameBitmap, false);
 return 0;
}

// Создание и подготовка TBitmap для хранения кадра с камеры
int TTVideoRegistratorFrame::PrepareBitmapFrame(void)
{
 if(InputFrameBitmap)
  return 0;

 InputFrameBitmap = new TBitmap;

 /*
 struct
 {
  TLogPalette lpal;
  TPaletteEntry dummy[256];
 } SysPal;

 InputFrameBitmap->HandleType=bmDIB;

 InputFrameBitmap->PixelFormat=pf8bit;

 SysPal.lpal.palVersion=0x300;
 SysPal.lpal.palNumEntries=256;
 for(int i=0;i<256;i++)
 {
  SysPal.lpal.palPalEntry[i].peRed=SysPal.lpal.palPalEntry[i].peGreen=SysPal.lpal.palPalEntry[i].peBlue=i;
  SysPal.lpal.palPalEntry[i].peFlags=0;
 }

 if(InputFrameBitmap->Palette)
  DeleteObject(InputFrameBitmap->Palette);

 InputFrameBitmap->Palette=CreatePalette((const tagLOGPALETTE *)&SysPal.lpal);
 */
 return 0;
}
//---------------------------------------------------------------------------
// Установка фрейма источника при работе от фрейма
bool TTVideoRegistratorFrame::SetVideoFrameSource(TVideoOutputFrame* sourceFrame)
{
 if(BitmapFrameThread && Mode==1)
 {
  TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
  return thread->SetVideoFrame(sourceFrame);
 }

 return false;
}
//---------------------------------------------------------------------------
TVideoOutputFrame* TTVideoRegistratorFrame::GetVideoFrameSourc(void)
{
 if(BitmapFrameThread && Mode==1)
 {
  TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
  return thread->GetVideoFrame();
 }

 return 0;
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::NetworkStreamingButtonClick(TObject *Sender)
{
 StopNetworkStreamingButtonClick(this);

 PrepareBitmapFrame();
 InitStreamingSettings();
 PreviewFlag=true;
 VideoGrabber->VideoSource=vs_JPEGsOrBitmaps;
 Mode=SourcePageControl->ActivePageIndex;

 switch(Mode)
 {
  case 0:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromComponentThread(this, true);
   TVideoGetBitmapFrameFromComponentThread* thread=dynamic_cast<TVideoGetBitmapFrameFromComponentThread*>(BitmapFrameThread);
   thread->SetComponentName(AnsiString(ComponentNameLabeledEdit->Text).c_str());
   thread->SetPropertyName(AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
   thread->Resume();
   break;
  }
  case 1:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromVideoThread(this, true);
   TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
   thread->SetVideoFrame(VideoOutputForm->GetVideoOutputFrame(StrToIntDef(FrameIndexLabeledEdit->Text, 0)));
   thread->Resume();
   break;
  }
 }

 VideoGrabber->StartPreview();
 if(VideoGrabber->StreamingURL!="")
 {
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming started: ")+AnsiString(VideoGrabber->StreamingURL).c_str()).c_str());
 }

 else
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming not running")).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopNetworkStreamingButtonClick(TObject *Sender)
{
 if(VideoGrabber->NetworkStreaming == ns_Disabled)
  return;

 VideoGrabber->NetworkStreaming=ns_Disabled;
 VideoGrabber->StopPreview();
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming stopped")).c_str());

 if(BitmapFrameThread)
 {
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::VideoGrabberVideoFromBitmapsNextFrameNeeded(TObject *Sender,
		  bool FirstSample)
{
 HBITMAP BitmapHandle;

 GetBitmapFrame();

 if(InputFrameBitmap)
 {
  BitmapHandle = InputFrameBitmap->Handle;
  VideoGrabber->SendImageToVideoFromBitmaps ("", BitmapHandle, false, false);
 }
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::GetStreamingHostButtonClick(TObject *Sender)
{
 if(VideoGrabber->StreamingURL != "")
  Engine_LogMessage(RDK_EX_INFO, (AnsiString(VideoGrabber->StreamingURL)).c_str());

 else
  Engine_LogMessage(RDK_EX_INFO, (std::string("Network streaming not running").c_str()));
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::InitButtonClick(TObject *Sender)
{
 //
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::VideoGrabberFrameCaptureCompleted(TObject *Sender,
          Pointer FrameBitmap, int BitmapWidth, int BitmapHeight, DWORD FrameNumber,
          __int64 FrameTime, TFrameCaptureDest DestType, UnicodeString FileName,
          bool Success, int FrameId)
{
 if(PreviewFlag)
 {
  PreviewImage->Picture->Bitmap->Assign(InputFrameBitmap);
  PreviewImage->Update();
  PreviewImage->Repaint();
 }
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::StartRecordingButtonClick(TObject *Sender)
{
 StopRecordingButtonClick(this);

 PrepareBitmapFrame();

 // recording settings
 InitRecordingSettings();
 VideoGrabber->VideoSource = vs_JPEGsOrBitmaps;
 PreviewFlag = true;
 Mode=SourcePageControl->ActivePageIndex;

 switch(Mode)
 {
  case 0:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromComponentThread(this, true);
   TVideoGetBitmapFrameFromComponentThread* thread=dynamic_cast<TVideoGetBitmapFrameFromComponentThread*>(BitmapFrameThread);
   thread->SetComponentName(AnsiString(ComponentNameLabeledEdit->Text).c_str());
   thread->SetPropertyName(AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
   thread->Resume();
   break;
  }
  case 1:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromVideoThread(this, true);
   TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
   thread->SetVideoFrame(VideoOutputForm->GetVideoOutputFrame(StrToIntDef(FrameIndexLabeledEdit->Text, 0)));
   thread->Resume();
   break;
  }
 }

 if(VideoGrabber->StartRecording())
 {
  Engine_LogMessage(RDK_EX_INFO, (std::string("Recording started").c_str()));
 }
 else
 {
  Engine_LogMessage(RDK_EX_INFO, (std::string("Recording not started").c_str()));
 }
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopRecordingButtonClick(TObject *Sender)
{
 VideoGrabber->StopRecording();
 Engine_LogMessage(RDK_EX_INFO, (std::string("Recording stopped").c_str()));

 if(BitmapFrameThread)
 {
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::VideoGrabberAVIDurationUpdated(TObject *Sender,
          UnicodeString FileName, DWORD FrameCount, double &FrameRate, __int64 &Duration)

{
 FrameRate = StrToIntDef(RecordingFrameRateLabeledEdit->Text, 30);
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StartPreviewButtonClick(TObject *Sender)

{
 PrepareBitmapFrame();

 VideoGrabber->VideoSource = vs_JPEGsOrBitmaps;
 PreviewFlag = true;
 Mode=SourcePageControl->ActivePageIndex;

 switch(Mode)
 {
  case 0:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromComponentThread(this, true);
   TVideoGetBitmapFrameFromComponentThread* thread=dynamic_cast<TVideoGetBitmapFrameFromComponentThread*>(BitmapFrameThread);
   thread->SetComponentName(AnsiString(ComponentNameLabeledEdit->Text).c_str());
   thread->SetPropertyName(AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
   thread->Resume();
   break;
  }
  case 1:
  {
   BitmapFrameThread=new TVideoGetBitmapFrameFromVideoThread(this, true);
   TVideoGetBitmapFrameFromVideoThread* thread=dynamic_cast<TVideoGetBitmapFrameFromVideoThread*>(BitmapFrameThread);
   thread->SetVideoFrame(VideoOutputForm->GetVideoOutputFrame(StrToIntDef(FrameIndexLabeledEdit->Text, 0)));
   thread->Resume();
   break;
  }
 }

 VideoGrabber->FrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 if(VideoGrabber->StartPreview());
  Engine_LogMessage(RDK_EX_INFO, (std::string("Preview started").c_str()));
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopButtonClick(TObject *Sender)
{
 VideoGrabber->StopPreview();
 Engine_LogMessage(RDK_EX_INFO, (std::string("Preview stopped").c_str()));

 if(BitmapFrameThread)
 {
  BitmapFrameThread->Terminate();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),1000);
  BitmapFrameThread->WaitFor();
  delete BitmapFrameThread;
  BitmapFrameThread=0;
 }
}
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void TTVideoRegistratorFrame::ABeforeCalculate(void)
{

}

// Метод, вызываемый перед сбросом
void TTVideoRegistratorFrame::ABeforeReset(void)
{

}

// Метод, вызываемый после шага расчета
void TTVideoRegistratorFrame::AAfterCalculate(void)
{

}


// Обновление интерфейса
void TTVideoRegistratorFrame::AUpdateInterface(void)
{

}

// Сохраняет параметры интерфейса в xml
void TTVideoRegistratorFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 // Network Streaming
 xml.WriteString("ComponentName", AnsiString(ComponentNameLabeledEdit->Text).c_str());
 xml.WriteString("ComponentProperty", AnsiString(ComponentPropertyNameLabeledEdit->Text).c_str());
 xml.WriteString("FrameIndex", AnsiString(FrameIndexLabeledEdit->Text).c_str());
 xml.WriteString("Port", AnsiString(PortLabeledEdit->Text).c_str());
 xml.WriteString("BufferWindow", AnsiString(BufferWindowLabeledEdit->Text).c_str());
 xml.WriteString("BitRate", AnsiString(BitRateLabeledEdit->Text).c_str());
 xml.WriteString("MaxKeyFrameSpacing", AnsiString(MaxKeyFrameSpacingLabeledEdit->Text).c_str());
 xml.WriteString("FrameRate", AnsiString(FrameRateLabeledEdit->Text).c_str());
 xml.WriteString("MaxUsers", AnsiString(MaxUsersLabeledEdit->Text).c_str());
 xml.WriteString("VideoHeight", AnsiString(VideoHeightLabeledEdit->Text).c_str());
 xml.WriteString("VideoWidth", AnsiString(VideoWidthLabeledEdit->Text).c_str());

 // Recording
 xml.WriteString("RecordingFileName", AnsiString(RecordingFileNameLabeledEdit->Text).c_str());
 xml.WriteString("RecordingFrameRate", AnsiString(RecordingFrameRateLabeledEdit->Text).c_str());
 xml.WriteString("RecordWidth", AnsiString(RecordWidthLabeledEdit->Text).c_str());
 xml.WriteString("RecordHeight", AnsiString(RecordHeightLabeledEdit->Text).c_str());
 xml.WriteInteger("RecordingMethod", RecordingMethodComboBox->ItemIndex);
 xml.WriteInteger("VideoCompressor", VideoCompressorComboBox->ItemIndex);
 xml.WriteInteger("VideoCompressionMode", VideoCompressionModeComboBox->ItemIndex);
 xml.WriteInteger("RecordingMode", RecordingModeComboBox->ItemIndex);
 xml.WriteInteger("RecordingTimer", StrToIntDef(RecordingTimerLabeledEdit->Text, 10));
 xml.WriteInteger("PreallocatedFileSize", StrToIntDef(PreallocatedFileSizeLabeledEdit->Text, 100));
 xml.WriteBool("UsePreallocatedFile", UsePreallocatedFileCheckBox->Checked);
 xml.WriteInteger("StoragePathMode", StoragePathRadioGroup->ItemIndex);
 xml.WriteString("StoragePath", AnsiString(StoragePathLabeledEdit->Text).c_str());
 xml.WriteInteger("RecTimerMode", RecordingTimerComboBox->ItemIndex);

}

// Загружает параметры интерфейса из xml
void TTVideoRegistratorFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
  // Network Streaming
 ComponentNameLabeledEdit->Text=(xml.ReadString("ComponentName", "")).c_str();
 ComponentPropertyNameLabeledEdit->Text=(xml.ReadString("ComponentProperty", "")).c_str();
 FrameIndexLabeledEdit->Text=(xml.ReadString("FrameIndex", "")).c_str();
 PortLabeledEdit->Text=(xml.ReadString("Port", "")).c_str();
 BufferWindowLabeledEdit->Text=(xml.ReadString("BufferWindow", "")).c_str();
 BitRateLabeledEdit->Text=(xml.ReadString("BitRate", "").c_str());
 MaxKeyFrameSpacingLabeledEdit->Text=(xml.ReadString("MaxKeyFrameSpacing", "")).c_str();
 FrameRateLabeledEdit->Text=(xml.ReadString("FrameRate", "")).c_str();
 MaxUsersLabeledEdit->Text=(xml.ReadString("MaxUsers", "")).c_str();
 VideoHeightLabeledEdit->Text=(xml.ReadString("VideoHeight", "")).c_str();
 VideoWidthLabeledEdit->Text=(xml.ReadString("VideoWidth", "")).c_str();

 // Recording
 RecordingFileNameLabeledEdit->Text=(xml.ReadString("RecordingFileName", "")).c_str();
 RecordingFrameRateLabeledEdit->Text=(xml.ReadString("RecordingFrameRate", "")).c_str();
 RecordWidthLabeledEdit->Text=(xml.ReadString("RecordWidth", "")).c_str();
 RecordHeightLabeledEdit->Text=(xml.ReadString("RecordHeight", "")).c_str();
 RecordingMethodComboBox->ItemIndex=(xml.ReadInteger("RecordingMethod", 3));
 VideoCompressorComboBox->ItemIndex=(xml.ReadInteger("VideoCompressor", 0));
 VideoCompressionModeComboBox->ItemIndex=(xml.ReadInteger("VideoCompressionMode", 0));
 RecordingModeComboBox->ItemIndex=(xml.ReadInteger("RecordingMode", 0));
 RecordingTimerLabeledEdit->Text=(xml.ReadInteger("RecordingTimer", 10));
 PreallocatedFileSizeLabeledEdit->Text=(xml.ReadInteger("PreallocatedFileSize", 100));
 UsePreallocatedFileCheckBox->Checked=(xml.ReadBool("UsePreallocatedFile", false));
 StoragePathRadioGroup->ItemIndex=(xml.ReadInteger("StoragePathMode", 0));
 StoragePathLabeledEdit->Text=(xml.ReadString("StoragePath", "")).c_str();
 RecordingTimerComboBox->ItemIndex=(xml.ReadInteger("RecTimerMode", 0));

 RefreshDeviceControls();
 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::BrowseComponentButtonClick(TObject *Sender)
{
 MyComponentsListForm->UpdateInterface(true);
 if(MyComponentsListForm->ShowIOSelect() != mrOk)
  return;

 ComponentNameLabeledEdit->Text=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str();
 ComponentPropertyNameLabeledEdit->Text=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput().c_str();
}
//---------------------------------------------------------------------------


void __fastcall TTVideoRegistratorFrame::BrowseFileNameButtonClick(TObject *Sender)
{
 RecordingFileOpen->Execute();
 RecordingFileNameLabeledEdit->Text=RecordingFileOpen->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::VideoCompressorComboBoxChange(TObject *Sender)
{
 //VideoOutputFrame->InitPrimarySettings();
 //TMenuItem *videoCodecItem=VideoOutputFrame->RecordingPopupMenu->Items->Find("Video Codec");
 //String currentItem=VideoCompressorComboBox->Items->operator [](VideoCompressorComboBox->ItemIndex);
 //TMenuItem *currentCodec=videoCodecItem->Find(currentItem);
 //currentCodec->Checked=true;
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::VideoCompressorSettingsButtonClick(TObject *Sender)

{
 VideoGrabber->VideoCompressor = VideoCompressorComboBox->ItemIndex;
 VideoGrabber->ShowDialog(dlg_VideoCompressor);
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::PreallocatedFileCreateButtonClick(TObject *Sender)
{
 InitRecordingSettings();
 VideoGrabber->CreatePreallocCapFile();
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::RecordingMethodComboBoxChange(TObject *Sender)
{
 InitStreamingSettings();
 InitRecordingSettings();
 RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StoragePathRadioGroupClick(TObject *Sender)
{
 RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::BrowseStoragePathButtonClick(TObject *Sender)

{
 String chosenDir=""; //ExtractFilePath(Application->ExeName);

 if(SelectDirectory("Select image sequence directory", ExtractFilePath(Application->ExeName), chosenDir,TSelectDirExtOpts() << sdNewFolder << sdNewUI))
 {
  StoragePathLabeledEdit->Text=chosenDir;
 }
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::RecordingModeComboBoxChange(TObject *Sender)
{
 RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::UsePreallocatedFileCheckBoxClick(TObject *Sender)
{
 RefreshDeviceControls();
}
//---------------------------------------------------------------------------

