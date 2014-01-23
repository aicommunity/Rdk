//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoRegistratorFrameUnit.h"
#include "rdk_initdll.h"
#include "UGEngineControlFormUnit.h"
#include "TUBitmap.h"
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
 CaptureEnabled=CreateEvent(0,TRUE,0,0);
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
 CloseHandle(CaptureEnabled);
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
/// Возвращает число изображений в последовательности
//virtual long long GetNumBitmaps(void) const=0;
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

/// Выставляется на время работы видеозахвата
HANDLE TVideoGetBitmapFrameThread::GetCaptureEnabled(void) const
{
 return CaptureEnabled;
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
 SetEvent(CaptureEnabled);
}

void __fastcall TVideoGetBitmapFrameThread::Stop(void)
{
 ResetEvent(CaptureEnabled);
}

void __fastcall TVideoGetBitmapFrameThread::BeforeCalculate(void)
{
// if(Frame)
//  Frame->BeforeCalculate();
}

void __fastcall TVideoGetBitmapFrameThread::AfterCalculate(void)
{
// Frame->UpdateInterface();
// if(!UEngineMonitorForm || !UEngineMonitorForm->EngineMonitorFrame)
//  return;
// if(GetNumEngines() == 1)
//  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,LastTimeStamp);
// else
//  if(GetNumEngines() > ChannelIndex)
//   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,LastTimeStamp);
/*
 UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]=
  UEngineMonitorForm->EngineMonitorFrame->GetServerTimeStamp(ChannelIndex);
	/*
 TIdTcpResultBroadcasterFrame *tcp_frame=IdTcpResultBroadcasterForm->GetBroadcasterFrame(ChannelIndex);
 if(tcp_frame)
  tcp_frame->AfterCalculate();
 TIdHttpResultBroadcasterFrame *http_frame=IdHttpResultBroadcasterForm->GetBroadcasterFrame(ChannelIndex);
 if(http_frame)
  http_frame->AfterCalculate();
	  */
 //RDK::UIVisualControllerStorage::AfterCalculate();
// RDK::UIVisualControllerStorage::UpdateInterface();
// if(ChannelIndex == GetNumEngines()-1)
// {
// }
}



void __fastcall TVideoGetBitmapFrameThread::Execute(void)
{
 while(!Terminated)
 {
  //if(WaitForSingleObject(CaptureEnabled,30) == WAIT_TIMEOUT)
  // continue;

  ResetEvent(CalcCompleteEvent);
  /*
  if(SyncMode == 1)
  {
   if(WaitForSingleObject(CalcCompleteEvent,30) == WAIT_TIMEOUT)
	continue;
   else
	ResetEvent(CalcCompleteEvent);
  }
  */
  //Model_GetComponentBitmapInputByIndex


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
 //time_stamp=LastTimeStamp;
 RDK::UBitmap* source=ReadSource;
 SetEvent(SourceUnlock);

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
 //time_stamp=LastTimeStamp;
 RDK::UBitmap* source=ReadSource;
 SetEvent(SourceUnlock);
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
 /*
 if(reflect)
  const_cast<RDK::UBitmap&>(src).ReflectionX(WriteSource);
 else
  *WriteSource=src;

 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);

 //LastTimeStamp=time_stamp;
 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true; */
}

bool TVideoGetBitmapFrameThread::WriteSourceSafe(Graphics::TBitmap *src, bool reflect)
{
 TBitmapToUBitmap(*WriteSource, src, reflect);

 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceUnlock);

 //LastTimeStamp=time_stamp;
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
 //TempBitmap=new RDK::UBitmap();
}

__fastcall TVideoGetBitmapFrameFromVideoThread::~TVideoGetBitmapFrameFromVideoThread(void)
{
 //if(TempBitmap)
 //{
  //delete TempBitmap;
  //TempBitmap=0;
 //}
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
  Sleep(30);
}

void __fastcall TVideoGetBitmapFrameFromVideoThread::Calculate(void)
{
 VideoOutputFrame=VideoOutputForm->GetVideoOutputFrame(FrameIndex);

 if(VideoOutputFrame)
 {
  //RDK::UBitmap temp;
  double time_stamp;
  VideoOutputFrame->CaptureThread->ReadSourceSafe(TempBitmap, time_stamp, false);
  if(TempBitmap.GetData())
   WriteSourceSafe(TempBitmap, false);
 }
}
//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoGetBitmapFrameFromComponentThread::TVideoGetBitmapFrameFromComponentThread(TTVideoRegistratorFrame *frame, bool CreateSuspended)
: TVideoGetBitmapFrameThread(frame, CreateSuspended)
{
 TempBitmap=new Graphics::TBitmap;
}

__fastcall TVideoGetBitmapFrameFromComponentThread::~TVideoGetBitmapFrameFromComponentThread(void)
{
 if(TempBitmap)
 {
  delete TempBitmap;
  TempBitmap=0;
 }
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
  Sleep(30);
}

void __fastcall TVideoGetBitmapFrameFromComponentThread::Calculate(void)
{
 if(ComponentName != "" && PropertyName != "")
  TempBitmap->Assign((TBitmap*)Model_GetComponentBitmapOutput(ComponentName.c_str(), PropertyName.c_str()));
  //TempBitmap.AttachBuffer((unsigned char*)Model_GetComponentBitmapOutput(ComponentName.c_str(), PropertyName.c_str()));

 if(TempBitmap)
  WriteSourceSafe(TempBitmap, false);
}
//---------------------------------------------------------------------------
__fastcall TTVideoRegistratorFrame::TTVideoRegistratorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 FillErrorsArray();
 RecordingMethodComboBox->Items->Add("rm_AVI");
 RecordingMethodComboBox->Items->Add("rm_ASF");
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
}
//---------------------------------------------------------------------------
// Методы
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
}
//---------------------------------------------------------------------------
// Логгирование ошибок
bool TTVideoRegistratorFrame::WriteLogMessage(const int &err)
{
 if(Errors[err] != "")
 {
  LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+" "+Errors[err].c_str());
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
 /*
 // Грузим настройки приложения
 AnsiString ExecDir = ExtractFileDir(ParamStr(0));
 TIniFile *app_ini = new TIniFile(ExecDir + "\\XenethVideoServer.ini");
 CameraIPLabeledEdit->Text = app_ini->ReadString("General", "CameraIP", "not found");
 MaxKeyFrameSpacingLabeledEdit->Text = app_ini->ReadString("General", "MaxKeyFrameSpacing", "not found");
 BufferWindowLabeledEdit->Text = app_ini->ReadString("General", "BufferWindow", "not found");
 VideoWidthLabeledEdit->Text = app_ini->ReadString("General", "VideoWidth", "not found");
 VideoHeightLabeledEdit->Text = app_ini->ReadString("General", "VideoHeight", "not found");
 BitRateLabeledEdit->Text = app_ini->ReadString("General", "BitRate", "not found");
 PortLabeledEdit->Text = app_ini->ReadString("General", "Port", "not found");
 FrameRateLabeledEdit->Text = app_ini->ReadString("General", "FrameRate", "not found");
 MaxUsersLabeledEdit->Text = app_ini->ReadString("General", "MaxUsers", "not found");
 delete app_ini;
 */
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
 VideoGrabber->ASFNetworkPort = StrToInt(PortLabeledEdit->Text);
 VideoGrabber->FrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 VideoGrabber->ASFVideoFrameRate = StrToIntDef(FrameRateLabeledEdit->Text, 30);
 VideoGrabber->ASFNetworkMaxUsers = StrToIntDef(MaxUsersLabeledEdit->Text, 4);
}
//---------------------------------------------------------------------------
// Инициализирует настройки записи в файл TVideoGrabber
int TTVideoRegistratorFrame::InitRecordingSettings(void)
{
 VideoGrabber->FrameRate = StrToIntDef(RecordingFrameRateLabeledEdit->Text, 30);
 VideoGrabber->RecordingFileName = RecordingFileNameLabeledEdit->Text;
 VideoGrabber->RecordingMethod = RecordingMethodComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
int TTVideoRegistratorFrame::GetBitmapFrame(void)
{
 BitmapFrameThread->ReadSourceSafe(InputFrameBitmap, false);
}

// Создание и подготовка TBitmap для хранения кадра с камеры
int TTVideoRegistratorFrame::PrepareBitmapFrame(void)
{
 InputFrameBitmap = new TBitmap;
 //frame = new unsigned char [frameheight_*framewidth_];

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
 return 0;
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::NetworkStreamingButtonClick(TObject *Sender)
{
 PrepareBitmapFrame();
 InitStreamingSettings();
 PreviewFlag=true;
 VideoGrabber->VideoSource=vs_JPEGsOrBitmaps;
 Mode=SorceModeComboBox->ItemIndex;

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
   thread->SetFrameIndex(StrToIntDef(FrameIndexLabeledEdit->Text, 0));
   thread->Resume();
   break;
  }
 }
 UGEngineControlForm->Start1Click(this);

 VideoGrabber->StartPreview();
 if(VideoGrabber->StreamingURL!="")
  LogMemo->Lines->Add(VideoGrabber->StreamingURL);

 else
  LogMemo->Lines->Add("Network streaming not running");
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopNetworkStreamingButtonClick(TObject *Sender)
{
 VideoGrabber->NetworkStreaming=ns_Disabled;
 VideoGrabber->StopPreview();
 LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+ " Network streaming stopped");

 if(BitmapFrameThread)
 {
  BitmapFrameThread->Stop();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),30);
 }

 UGEngineControlForm->Pause1Click(this);
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


void __fastcall TTVideoRegistratorFrame::ClearMemoButtonClick(TObject *Sender)
{
 LogMemo->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::SaveToIniButtonClick(TObject *Sender)
{
/*
 // Сохраняем настройки приложения
 AnsiString ExecDir = ExtractFileDir(ParamStr(0));
 TIniFile *app_ini = new TIniFile(ExecDir + "\\XenethVideoServer.ini");
 app_ini->WriteString("General", "CameraIP", CameraIPLabeledEdit->Text);
 app_ini->WriteString("General", "MaxKeyFrameSpacing", MaxKeyFrameSpacingLabeledEdit->Text);
 app_ini->WriteString("General", "BufferWindow", BufferWindowLabeledEdit->Text);
 app_ini->WriteString("General", "VideoWidth", VideoWidthLabeledEdit->Text);
 app_ini->WriteString("General", "VideoHeight", VideoHeightLabeledEdit->Text);
 app_ini->WriteString("General", "BitRate", BitRateLabeledEdit->Text);
 app_ini->WriteString("General", "Port", PortLabeledEdit->Text);
 app_ini->WriteString("General", "FrameRate", FrameRateLabeledEdit->Text);
 app_ini->WriteString("General", "MaxUsers", MaxUsersLabeledEdit->Text);
 delete app_ini;
 */
}
//---------------------------------------------------------------------------
void __fastcall TTVideoRegistratorFrame::GetStreamingHostButtonClick(TObject *Sender)
{
 if(VideoGrabber->StreamingURL != "")
  LogMemo->Lines->Add(VideoGrabber->StreamingURL);

 else
  LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+ " Network streaming not running");
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
 PrepareBitmapFrame();
 // recording settings

 InitRecordingSettings();
 VideoGrabber->VideoSource = vs_JPEGsOrBitmaps;
 PreviewFlag = true;
 Mode=SorceModeComboBox->ItemIndex;

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
   thread->SetFrameIndex(StrToIntDef(FrameIndexLabeledEdit->Text, 0));
   thread->Resume();
   break;
  }
 }
 UGEngineControlForm->Start1Click(this);

 VideoGrabber->StartRecording();
 LogMemo->Lines->Add("Recording started");
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopRecordingButtonClick(TObject *Sender)

{
 VideoGrabber->Stop();
 if(BitmapFrameThread)
 {
  BitmapFrameThread->Stop();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),30);
 }

 UGEngineControlForm->Pause1Click(this);
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
 Mode=SorceModeComboBox->ItemIndex;

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
   thread->SetFrameIndex(StrToIntDef(FrameIndexLabeledEdit->Text, 0));
   thread->Resume();
   break;
  }
 }
 UGEngineControlForm->Start1Click(this);

 VideoGrabber->StartPreview();
 LogMemo->Lines->Add("Preview started");
}
//---------------------------------------------------------------------------

void __fastcall TTVideoRegistratorFrame::StopButtonClick(TObject *Sender)
{
 VideoGrabber->StopPreview();
 LogMemo->Lines->Add(Now().DateString()+" "+Now().TimeString()+ " Preview stopped");

 if(BitmapFrameThread)
 {
  BitmapFrameThread->Stop();
  WaitForSingleObject(BitmapFrameThread->GetFrameNotInProgress(),30);
 }

 UGEngineControlForm->Pause1Click(this);
}
//---------------------------------------------------------------------------
// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void TTVideoRegistratorFrame::ABeforeCalculate(void)
{  /*
// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 0)
  if(CaptureThread)
  {
   long long time_stamp=0;
   CaptureThread->ReadSourceSafe(BmpSource,time_stamp,false);

	std::string sstamp;
	RDK::UTimeStamp stamp(double(time_stamp/1000.0),25);
	stamp>>sstamp;
	TimeEdit->Text=sstamp.c_str();

	TrackBar->Max=CaptureThread->GetNumBitmaps();
	TrackBar->Position=CaptureThread->GetPosition();
	TrackBar->UpdateControlState();
	UpdateVideo();
  }

// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 0)
 {
  SendToComponentIO();
  if(SendPointsByStepCheckBox->Checked)
  {
   SendAsMatrixButtonClick(this);
   Button1Click(this);
  }
  if(DeletePointsAfterSendCheckBox->Checked)
  {
   MyVideoOutputToolsForm->DelAllPointsButtonClick(this);
  }

 if(Model_Check())
 {
  if(BmpSource.GetByteLength()>0)
  {
   if(GetNumEngines() == 1)
	Model_SetComponentBitmapOutput("", "Output", &BmpSource,true); // Заглушка!!
	//в модели должна быть возможность задания множества выходов
   else
   {
	if(GetNumEngines()>FrameIndex)
 	 MModel_SetComponentBitmapOutput(FrameIndex, "", "Output", &BmpSource,true);
   }
  }
 }
 }   */
}

// Метод, вызываемый перед сбросом
void TTVideoRegistratorFrame::ABeforeReset(void)
{  /*
 if(Model_Check())
 {
  if(BmpSource.GetByteLength()>0)
  {
   if(GetNumEngines() == 1)
	Model_SetComponentBitmapOutput("", "Output", &BmpSource,true); // Заглушка!!
	//в модели должна быть возможность задания множества выходов
   else
   {
	if(GetNumEngines()>FrameIndex)
 	 MModel_SetComponentBitmapOutput(FrameIndex, "", "Output", &BmpSource,true);
   }
  }
 } */
}

// Метод, вызываемый после шага расчета
void TTVideoRegistratorFrame::AAfterCalculate(void)
{  /*
 if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 0)
 {
  if(CaptureThread)
   SetEvent(CaptureThread->GetCalcCompleteEvent());
 }
 else
 {
  if(Mode == 4)
   if(CaptureThread)
    SetEvent(CaptureThread->GetCalcCompleteEvent());
 }    */
}


// Обновление интерфейса
void TTVideoRegistratorFrame::AUpdateInterface(void)
{
// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 1)
// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() != 0 /*&& !IsStarted*/)
 /* if(CaptureThread)
  {
   long long time_stamp=0;
	CaptureThread->ReadSourceSafe(BmpSource,time_stamp,false);

   if(Mode == 4)
   {
//	std::string sstamp;
//	RDK::UTimeStamp stamp(double(time_stamp),1);
//	stamp>>sstamp;
	TimeEdit->EditMask="";
	TimeEdit->Text=IntToStr(time_stamp);
   }
   else
   {
	std::string sstamp;
	RDK::UTimeStamp stamp(double(time_stamp/1000.0),25);
	stamp>>sstamp;
	TimeEdit->EditMask="000\:00\:00\:00;1;_";
	TimeEdit->Text=sstamp.c_str();
   }

	TrackBar->Max=CaptureThread->GetNumBitmaps();
	TrackBar->Position=CaptureThread->GetPosition();
	TrackBar->UpdateControlState();
	UpdateVideo();
  }

 if(LinkedComponentName.size() == 0)
 {
  SendImageToComponentInput1->Caption="Send Image To Component Input...";
  SendImageToComponentOutput1->Caption="Send Image To Component Output...";
  SendImageToComponentProperty1->Caption="Send Image To Component Property...";
 }
 else
 {
  switch(LinkedMode)
  {
  case 0:
   SendImageToComponentInput1->Caption=String("Send Image To Input: ")+String(LinkedComponentName.c_str())+String("[")+String(LinkedIndex.c_str())+String("]");
   SendImageToComponentOutput1->Caption="Send Image To Component Output...";
   SendImageToComponentProperty1->Caption="Send Image To Component Property...";
  break;

  case 1:
   SendImageToComponentInput1->Caption="Send Image To Component Input...";
   SendImageToComponentProperty1->Caption="Send Image To Component Property...";
   SendImageToComponentOutput1->Caption=String("Send Image To Output: ")+String(LinkedComponentName.c_str())+String("[")+String(LinkedIndex.c_str())+String("]");
  break;

  case 2:
   SendImageToComponentInput1->Caption="Send Image To Component Input...";
   SendImageToComponentProperty1->Caption="Send Image To Component Output...";
   SendImageToComponentOutput1->Caption=String("Send Image To Property: ")+String(LinkedComponentName.c_str())+String("[")+String(LinkedComponentPropertyName.c_str())+String("]");
  break;
  }
 }

 if(!SelectedComponentSName.empty())
 {
  SendToEdit->Text=String("State ")+SelectedComponentSName.c_str()+String(":")+SelectedComponentStateName.c_str();
 }
 else
 if(!SelectedComponentPName.empty())
 {
  SendToEdit->Text=String("Parameter ")+SelectedComponentPName.c_str()+String(":")+SelectedComponentParameterName.c_str();
 }
 else
 if(!SelectedComponentMatrixName.empty())
 {
  SendToEdit->Text=String("Mat-Property ")+SelectedComponentMatrixName.c_str()+String(":")+SelectedComponentPropertyMatrixName.c_str();
 }
 else
  SendToEdit->Text="";   */
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

 UpdateInterface();
}
//---------------------------------------------------------------------------


