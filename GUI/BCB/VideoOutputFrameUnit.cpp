//---------------------------------------------------------------------------
#include <vcl.h>
#include <jpeg.hpp>
#pragma hdrstop

#include "VideoOutputFrameUnit.h"
#include "TUBitmap.h"
#include "TVideoGrabberControlFormUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "myrdk.h"
#include "rdk_initdll.h"
//#include "USharedMemoryLoader.h"
//#include "TUFileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma link "TUVisualControllerFrameUnit"
#pragma link "TUHttpServerUnit"
#pragma resource "*.dfm"
TVideoOutputFrame *VideoOutputFrame;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThread::TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended)
: ChannelIndex(0), Frame(frame), TThread(CreateSuspended)
{
 SourceMode=-1;
 LastTimeStamp=0;
 CaptureEnabled=CreateEvent(0,TRUE,0,0);
 SourceUnlock=CreateEvent(0,TRUE,TRUE,0);
 FrameNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 CalcCompleteEvent=CreateEvent(0,TRUE,TRUE,0);
 ReadSource=&Source[0];
 WriteSource=&Source[1];
}

__fastcall TVideoCaptureThread::~TVideoCaptureThread(void)
{
 CloseHandle(CaptureEnabled);
 CloseHandle(SourceUnlock);
 CloseHandle(FrameNotInProgress);
 CloseHandle(CalcCompleteEvent);
}
// --------------------------


// --------------------------
// Управление параметрами
// --------------------------
/// Источник видео
int TVideoCaptureThread::GetSourceMode(void) const
{
 return SourceMode;
}

/// Индекс канала в библиотеке аналитики, управляемый тредом
int TVideoCaptureThread::GetChannelIndex(void) const
{
 return ChannelIndex;
}

bool TVideoCaptureThread::SetChannelIndex(int value)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 ChannelIndex=value;
 return true;
}

/// Режим синхронизации с расчетом
int TVideoCaptureThread::GetSyncMode(void) const
{
 return SyncMode;
}

bool TVideoCaptureThread::SetSyncMode(int mode)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 SyncMode=mode;
 return true;
}
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Указатель на владельца
TVideoOutputFrame* TVideoCaptureThread::GetFrame(void) const
{
 return Frame;
}

bool TVideoCaptureThread::SetFrame(TVideoOutputFrame * frame)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 Frame=frame;
 return true;
}
// --------------------------

// --------------------------
// Управление событиями
// --------------------------
/// Выставляется по завершении захвата нового кадра
HANDLE TVideoCaptureThread::GetFrameNotInProgress(void) const
{
 return FrameNotInProgress;
}

/// Выставлено всегда. Сбрасывается на время доступа к изображению
HANDLE TVideoCaptureThread::GetSourceUnlock(void) const
{
 return SourceUnlock;
}

/// Выставляется на время работы видеозахвата
HANDLE TVideoCaptureThread::GetCaptureEnabled(void) const
{
 return CaptureEnabled;
}

/// Сбрасывается на время ожидания расчета
HANDLE TVideoCaptureThread::GetCalcCompleteEvent(void) const
{
 return CalcCompleteEvent;
}
// --------------------------



// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThread::Start(void)
{
 SetEvent(CalcCompleteEvent);
 SetEvent(CaptureEnabled);
}

void __fastcall TVideoCaptureThread::Stop(void)
{
 ResetEvent(CaptureEnabled);
}

void __fastcall TVideoCaptureThread::BeforeCalculate(void)
{
// if(Frame)
//  Frame->BeforeCalculate();
}

void __fastcall TVideoCaptureThread::AfterCalculate(void)
{
 if(GetNumEngines() == 1)
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,LastTimeStamp);
 else
  if(GetNumEngines() > ChannelIndex)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,LastTimeStamp);
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



void __fastcall TVideoCaptureThread::Execute(void)
{
 while(!Terminated)
 {
  if(WaitForSingleObject(CaptureEnabled,30) == WAIT_TIMEOUT)
   continue;

  if(SyncMode == 1)
  {
   if(WaitForSingleObject(CalcCompleteEvent,30) == WAIT_TIMEOUT)
	continue;
   else
    ResetEvent(CalcCompleteEvent);

  }

  ResetEvent(FrameNotInProgress);
  BeforeCalculate();
  Calculate();
  AfterCalculate();
  ResetEvent(CalcCompleteEvent);
  SetEvent(FrameNotInProgress);
 }
}

/// Возвращает копию изображения с блокировкой
bool TVideoCaptureThread::ReadSourceSafe(RDK::UBitmap& dest, long long &time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);
 time_stamp=LastTimeStamp;
 RDK::UBitmap* source=ReadSource;
 SetEvent(SourceUnlock);

 if(reflect)
  source->ReflectionX(&dest);
 else
  dest=*source;

// SetEvent(SourceUnlock);
 return true;
}

/// Записывает изображение в тред с блокировкой
bool TVideoCaptureThread::WriteSourceSafe(const RDK::UBitmap& src, long long time_stamp, bool reflect)
{
 if(reflect)
  const_cast<RDK::UBitmap&>(src).ReflectionX(WriteSource);
 else
  *WriteSource=src;

 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}

bool TVideoCaptureThread::WriteSourceSafe(Graphics::TBitmap *src, long long time_stamp, bool reflect)
{
 TBitmapToUBitmap(*WriteSource, src, reflect);

 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}

// Меняет временную метку с блокировкой
bool TVideoCaptureThread::SetLastTimeStampSafe(long long time_stamp)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 SetEvent(SourceUnlock);
 return true;
}
// --------------------------


//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadBmp::TVideoCaptureThreadBmp(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThread(frame, CreateSuspended)
{
 TempBitmap=new Graphics::TBitmap;
}

__fastcall TVideoCaptureThreadBmp::~TVideoCaptureThreadBmp(void)
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
/// Имя файла изображения
const std::string& TVideoCaptureThreadBmp::GetFileName(void) const
{
 return FileName;
}

bool TVideoCaptureThreadBmp::SetFileName(const std::string& value)
{
 try
 {
  FileName=value;
  String filename=value.c_str();
  if(filename.Pos(".jpg") || filename.Pos(".jpeg") )
  {
   TJPEGImage* JpegIm=new TJPEGImage;
   JpegIm->LoadFromFile(filename);
   TempBitmap->Assign(JpegIm);
   TempSource<<TempBitmap;
   delete JpegIm;
  }
  else
  if(filename.Pos(".bmp"))
  {
   LoadBitmapFromFile(AnsiString(filename).c_str(),&TempSource);
  }
  else
   TempSource.Fill(0);
 }
 catch (EFOpenError &exception) {
  TempSource.SetRes(0,0);
 }
 TempSource.SetColorModel(RDK::ubmRGB24);
 long long time_stamp=0;
 WriteSourceSafe(TempSource,time_stamp,false);
 return true;
}

/// Возвращает число изображений в последовательности
long long TVideoCaptureThreadBmp::GetNumBitmaps(void) const
{
 return 1;
}

/// Устанавливает текущую позицию в последовательности
long long TVideoCaptureThreadBmp::GetPosition(void) const
{
 return 0;
}

bool TVideoCaptureThreadBmp::SetPosition(long long index)
{
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadBmp::Start(void)
{
 TVideoCaptureThread::Start();
}

void __fastcall TVideoCaptureThreadBmp::Stop(void)
{
 TVideoCaptureThread::Stop();
}


void __fastcall TVideoCaptureThreadBmp::BeforeCalculate(void)
{

}

void __fastcall TVideoCaptureThreadBmp::AfterCalculate(void)
{
  TVideoCaptureThread::AfterCalculate();
  Sleep(30);
}

void __fastcall TVideoCaptureThreadBmp::Calculate(void)
{
 long long time_stamp=GetTickCount();
 SetLastTimeStampSafe(time_stamp);
}
// --------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadBmpSequence::TVideoCaptureThreadBmpSequence(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThread(frame, CreateSuspended)
{
 TempBitmap=new Graphics::TBitmap;
}

__fastcall TVideoCaptureThreadBmpSequence::~TVideoCaptureThreadBmpSequence(void)
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
/// Имя файла изображения
const std::string& TVideoCaptureThreadBmpSequence::GetPathName(void) const
{
 return PathName;
}

bool TVideoCaptureThreadBmpSequence::SetPathName(const std::string& value)
{
 PathName=value+"\\";

 RDK::FindFilesList(PathName, "*.bmp", true, BmpSequenceNames);
 if(BmpSequenceNames.size() == 0)
  RDK::FindFilesList(PathName, "*.jpg", true, BmpSequenceNames);
 if(BmpSequenceNames.size() == 0)
  RDK::FindFilesList(PathName, "*.jpeg", true, BmpSequenceNames);

 CurrentBmpSequenceIndex=0;

 LastReadSequenceIndex=-1;
 if(BmpSequenceNames.size()>0)
 {
  LoadImageFromSequence(0,TempSource);
  TempSource.SetColorModel(RDK::ubmRGB24);
  LastReadSequenceIndex=0;
 }
 else
 {
  TempSource.Clear();
  LastReadSequenceIndex=-1;
 }
 long long time_stamp=0;
 WriteSourceSafe(TempSource,time_stamp,false);

 CurrentBmpSequenceIndex=0;
// Calculate();
 return true;
}

/// Возвращает число изображений в последовательности
long long TVideoCaptureThreadBmpSequence::GetNumBitmaps(void) const
{
 return int(BmpSequenceNames.size());
}

/// Устанавливает текущую позицию в последовательности
long long TVideoCaptureThreadBmpSequence::GetPosition(void) const
{
 return CurrentBmpSequenceIndex;
}

bool TVideoCaptureThreadBmpSequence::SetPosition(long long index)
{
 return SetLastTimeStampSafe(index);
}

// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadBmpSequence::AfterCalculate(void)
{
  TVideoCaptureThread::AfterCalculate();
  Sleep(30);
}

void __fastcall TVideoCaptureThreadBmpSequence::Calculate(void)
{
 if(BmpSequenceNames.size()>0)
 {
  LoadImageFromSequence(CurrentBmpSequenceIndex,TempSource);
  TempSource.SetColorModel(RDK::ubmRGB24);
  LastReadSequenceIndex=0;
 }
 else
 {
  TempSource.Clear();
  LastReadSequenceIndex=-1;
 }
 long long time_stamp=CurrentBmpSequenceIndex;
 WriteSourceSafe(TempSource,time_stamp,false);

 CurrentBmpSequenceIndex++;
 if(CurrentBmpSequenceIndex>=int(BmpSequenceNames.size()))
 {
  --CurrentBmpSequenceIndex;
  Stop();
 }
}

// Загружает выбранную картинку по индеку в массиве имен
bool TVideoCaptureThreadBmpSequence::LoadImageFromSequence(int index, RDK::UBitmap &bmp)
{
 if(LastReadSequenceIndex == index)
  return true;

 if(BmpSequenceNames[index].find(".bmp") != std::string::npos)
 {
  RDK::LoadBitmapFromFile((PathName+BmpSequenceNames[index]).c_str(),bmp);
 }
 else
 if(BmpSequenceNames[index].find(".jpg") != std::string::npos || BmpSequenceNames[index].find(".jpeg") != std::string::npos)
 {
  TJPEGImage* JpegIm=new TJPEGImage;
  JpegIm->LoadFromFile((PathName+BmpSequenceNames[index]).c_str());
  TempBitmap->Assign(JpegIm);
  bmp<<TempBitmap;
  delete JpegIm;
 }
 else
  bmp.Fill(0);
 LastReadSequenceIndex = index;
 return true;
}


// Меняет временную метку с блокировкой
bool TVideoCaptureThreadBmpSequence::SetLastTimeStampSafe(long long time_stamp)
{
 TVideoCaptureThread::SetLastTimeStampSafe(time_stamp);
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;
 CurrentBmpSequenceIndex=time_stamp;
 return true;
}

// --------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadHttpServer::TVideoCaptureThreadHttpServer(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThread(frame, CreateSuspended)
{
 HttpServerCompleted=CreateEvent(0,TRUE,0,0);
 TempBitmap=new Graphics::TBitmap;
 UHttpServerFrame=new TUHttpServerFrame(frame);
 UHttpServerFrame->IdHTTPServer->OnCommandGet=IdHTTPServerCommandGet;
}

__fastcall TVideoCaptureThreadHttpServer::~TVideoCaptureThreadHttpServer(void)
{
 if(UHttpServerFrame)
 {
  delete UHttpServerFrame;
  UHttpServerFrame=0;
 }

 if(TempBitmap)
 {
  delete TempBitmap;
  TempBitmap=0;
 }
 CloseHandle(HttpServerCompleted);
}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя файла изображения
int TVideoCaptureThreadHttpServer::GetListenPort(void) const
{
 return ListenPort;
}

bool TVideoCaptureThreadHttpServer::SetListenPort(int value)
{
 ListenPort=value;

 UHttpServerFrame->IdHTTPServer->Bindings->DefaultPort=ListenPort;
 TIdSocketHandle* bind=0;
 if(UHttpServerFrame->IdHTTPServer->Bindings->Count == 0)
  bind=UHttpServerFrame->IdHTTPServer->Bindings->Add();
 else
  bind=UHttpServerFrame->IdHTTPServer->Bindings->Items[0];

 bind->Port=ListenPort;

 return true;
}


/// Возвращает число изображений в последовательности
long long TVideoCaptureThreadHttpServer::GetNumBitmaps(void) const
{
 return 0;
}

/// Устанавливает текущую позицию в последовательности
long long TVideoCaptureThreadHttpServer::GetPosition(void) const
{
 return 0;
}

bool TVideoCaptureThreadHttpServer::SetPosition(long long index)
{
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadHttpServer::Start(void)
{
 UHttpServerFrame->IdHTTPServer->Active=true;
 TVideoCaptureThread::Start();
}

void __fastcall TVideoCaptureThreadHttpServer::Stop(void)
{
 TVideoCaptureThread::Stop();
 UHttpServerFrame->IdHTTPServer->Active=false;
}

void __fastcall TVideoCaptureThreadHttpServer::BeforeCalculate(void)
{

}

void __fastcall TVideoCaptureThreadHttpServer::AfterCalculate(void)
{

}

void __fastcall TVideoCaptureThreadHttpServer::Calculate(void)
{
 if(WaitForSingleObject(HttpServerCompleted,30) == WAIT_TIMEOUT)
  return;
 ResetEvent(HttpServerCompleted);
}

void __fastcall TVideoCaptureThreadHttpServer::IdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo)
{
 std::vector<char> &time_stamp=UHttpServerFrame->ParsedRequestArgs["TimeStamp"];
 long long l_time_stamp=0;
 if(!time_stamp.empty())
 {
  std::string temp_stamp;
  temp_stamp.assign(&time_stamp[0],time_stamp.size());
  l_time_stamp=RDK::atoi(temp_stamp);
 }

 UHttpServerFrame->IdHTTPServerCommandGet(AContext, ARequestInfo, AResponseInfo);
 if(ARequestInfo->Document == "/images.cgi")
 {
  std::vector<char> &image_data=UHttpServerFrame->ParsedRequestArgs["Image"];
  RDK::UBitmap &bmp=UHttpServerFrame->DecodeParamAsBitmap(image_data);
  WriteSourceSafe(bmp,l_time_stamp,false);
  AResponseInfo->ContentText="OK";
  SetEvent(HttpServerCompleted);
  TVideoCaptureThread::AfterCalculate();
 }
 else
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="404 Not Found";
  AResponseInfo->ContentText="404 Not Found";
 }
}

// --------------------------
//---------------------------------------------------------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber::TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended)
 : VideoGrabber(new TVideoGrabber(frame)), TVideoCaptureThread(frame,CreateSuspended)
{
 VideoGrabber->OnFrameCaptureCompleted=OnFrameCaptureCompleted;//VideoGrabberFrameCaptureCompleted;

 VideoGrabber->Display_AutoSize = false;
 VideoGrabber->PlayerRefreshPausedDisplay = false;
 VideoGrabber->AutoStartPlayer = false;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->BurstInterval = 0;
 VideoGrabber->BurstMode = true;
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->Synchronized=true;

 ConvertBitmap=new Graphics::TBitmap;

 VideoGrabberCompleted=CreateEvent(0,TRUE,0,0);
}

__fastcall TVideoCaptureThreadVideoGrabber::~TVideoCaptureThreadVideoGrabber(void)
{
 if(VideoGrabber)
 {
  Terminate();
  WaitForSingleObject(GetFrameNotInProgress(),1000);
  delete VideoGrabber;
  VideoGrabber=0;
 }

 if(ConvertBitmap)
 {
  delete ConvertBitmap;
  ConvertBitmap=0;
 }

 CloseHandle(VideoGrabberCompleted);
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
TVideoGrabber* TVideoCaptureThreadVideoGrabber::GetVideoGrabber(void)
{
 return VideoGrabber;
}

void __fastcall TVideoCaptureThreadVideoGrabber::OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId)
{
 Graphics::TBitmap *Frame_Bitmap;

 Frame_Bitmap = (Graphics::TBitmap*) FrameBitmap;

 switch (DestType)
 {
 case fc_TBitmap:
  if(Frame_Bitmap->PixelFormat == pf24bit)
  {
   LastTimeStamp=FrameTime/10000;
   *WriteSource<<Frame_Bitmap;
  // WriteSourceSafe(Frame_Bitmap, FrameTime/10000, false);
  }
  else
  {
   LastTimeStamp=FrameTime/10000;
   ConvertBitmap->Assign(Frame_Bitmap);
   ConvertBitmap->PixelFormat=pf24bit;
   *WriteSource<<ConvertBitmap;
//   WriteSourceSafe(ConvertBitmap, FrameTime/10000, false);
  }

//	 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
//	  return;
//	 ResetEvent(SourceUnlock);
	 RDK::UBitmap* old_read_source=ReadSource;
	 ReadSource=WriteSource;
	 WriteSource=old_read_source;
//	 LastTimeStamp=time_stamp;
//	 SetEvent(SourceUnlock);

  SetEvent(VideoGrabberCompleted);
  TVideoCaptureThread::AfterCalculate();
 break;
 }
}

void __fastcall TVideoCaptureThreadVideoGrabber::Calculate(void)
{
 if(WaitForSingleObject(VideoGrabberCompleted,30) == WAIT_TIMEOUT)
  return;
 ResetEvent(VideoGrabberCompleted);
}


void __fastcall TVideoCaptureThreadVideoGrabber::BeforeCalculate(void)
{

}

void __fastcall TVideoCaptureThreadVideoGrabber::AfterCalculate(void)
{

}

/// Возвращает число изображений в последовательности
long long TVideoCaptureThreadVideoGrabber::GetNumBitmaps(void) const
{
 if(VideoGrabber)
 {
  return VideoGrabber->PlayerDuration/10000000;
 }
 return 0;
}

/// Устанавливает текущую позицию в последовательности
long long TVideoCaptureThreadVideoGrabber::GetPosition(void) const
{
 if(VideoGrabber)
 {
  return VideoGrabber->PlayerTimePosition/10000000;
 }

 return 0;
}

bool TVideoCaptureThreadVideoGrabber::SetPosition(long long index)
{
 if(VideoGrabber)
 {
  if(!VideoGrabber->InFrameProgressEvent)
   VideoGrabber->PlayerTimePosition = index*10000000;
  return true;
 }
 return false;
}
// --------------------------

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberAvi::TVideoCaptureThreadVideoGrabberAvi(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThreadVideoGrabber(frame, CreateSuspended)
{
 if(VideoGrabber)
  VideoGrabber->VideoSource=vs_VideoFileOrURL;
}

__fastcall TVideoCaptureThreadVideoGrabberAvi::~TVideoCaptureThreadVideoGrabberAvi(void)
{

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя канала общей памяти
const std::string& TVideoCaptureThreadVideoGrabberAvi::GetFileName(void) const
{
 return FileName;
}

bool TVideoCaptureThreadVideoGrabberAvi::SetFileName(const std::string& value)
{
 Stop();
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 FileName=value;
 VideoGrabber->PlayerFileName=FileName.c_str();
 VideoGrabber->OpenPlayer();
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
// Меняет временную метку с блокировкой
bool TVideoCaptureThreadVideoGrabberAvi::SetLastTimeStampSafe(long long time_stamp)
{
 TVideoCaptureThread::SetLastTimeStampSafe(time_stamp);
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;
 VideoGrabber->PlayerTimePosition=time_stamp*10000;
 return true;
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::Start(void)
{
 if(VideoGrabber)
 {
  VideoGrabber->StartSynchronized();
  VideoGrabber->RunPlayer();
 }
 TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::Stop(void)
{
 TVideoCaptureThreadVideoGrabber::Stop();
 if(VideoGrabber)
  VideoGrabber->PausePlayer();
}
// --------------------------
//---------------------------------------------------------------------------

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberCamera::TVideoCaptureThreadVideoGrabberCamera(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThreadVideoGrabber(frame, CreateSuspended)
{
 if(VideoGrabber)
  VideoGrabber->VideoSource=vs_VideoCaptureDevice;
}

__fastcall TVideoCaptureThreadVideoGrabberCamera::~TVideoCaptureThreadVideoGrabberCamera(void)
{

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
int TVideoCaptureThreadVideoGrabberCamera::GetCameraIndex(void) const
{
 return CameraIndex;
}

int TVideoCaptureThreadVideoGrabberCamera::GetInputIndex(void) const
{
 return InputIndex;
}

int TVideoCaptureThreadVideoGrabberCamera::GetSizeIndex(void) const
{
 return SizeIndex;
}

int TVideoCaptureThreadVideoGrabberCamera::GetSubtypeIndex(void) const
{
 return SubtypeIndex;
}

int TVideoCaptureThreadVideoGrabberCamera::GetAnalogIndex(void) const
{
 return AnalogIndex;
}

bool TVideoCaptureThreadVideoGrabberCamera::Init(int camera_index, int input_index, int size_index, int subtype_index, int analog_index)
{
 Stop();
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 VideoGrabber->VideoSource=vs_VideoCaptureDevice;
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->BurstMode = True;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->VideoDevice=camera_index;
 VideoGrabber->VideoInput=input_index;
 VideoGrabber->VideoSize=size_index;
 VideoGrabber->VideoSubtype=subtype_index;
 VideoGrabber->AnalogVideoStandard=analog_index;
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberCamera::Start(void)
{
 if(VideoGrabber)
 {
  VideoGrabber->StartPreview();
  VideoGrabber->StartSynchronized();
 }
 TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberCamera::Stop(void)
{
 TVideoCaptureThreadVideoGrabber::Stop();
 if(VideoGrabber)
  VideoGrabber->PausePreview();
}
// --------------------------

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberIpCamera::TVideoCaptureThreadVideoGrabberIpCamera(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThreadVideoGrabber(frame, CreateSuspended)
{
 if(VideoGrabber)
  VideoGrabber->VideoSource=vs_IPCamera;
}

__fastcall TVideoCaptureThreadVideoGrabberIpCamera::~TVideoCaptureThreadVideoGrabberIpCamera(void)
{

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Имя камеры
const String& TVideoCaptureThreadVideoGrabberIpCamera::GetUrl(void) const
{
 return Url;
}

/// Имя пользователя
const String& TVideoCaptureThreadVideoGrabberIpCamera::GetUserName(void) const
{
 return UserName;
}

/// Пароль
const String& TVideoCaptureThreadVideoGrabberIpCamera::GetPassword(void) const
{
 return Password;
}


bool TVideoCaptureThreadVideoGrabberIpCamera::Init(const String camera_url, const String user_name, const String user_password)
{
 Stop();
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 Url=camera_url;
 UserName=user_name;
 Password=user_password;

 VideoGrabber->VideoSource=vs_IPCamera;
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->BurstMode = True;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->IPCameraURL=camera_url.c_str();

 VideoGrabber->SetAuthentication(at_IPCamera,user_name,user_password);
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::Start(void)
{
 if(VideoGrabber)
 {
   VideoGrabber->StartPreview();
   VideoGrabber->StartSynchronized();
 }
 TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::Stop(void)
{
 TVideoCaptureThreadVideoGrabber::Stop();
 if(VideoGrabber)
  VideoGrabber->PausePreview();
}
// --------------------------

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadSharedMemory::TVideoCaptureThreadSharedMemory(TVideoOutputFrame *frame, bool CreateSuspended)
: TVideoCaptureThread(frame,CreateSuspended)
{
 SourceMode = 6;
}

__fastcall TVideoCaptureThreadSharedMemory::~TVideoCaptureThreadSharedMemory(void)
{

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Индекс канала общей памяти
int TVideoCaptureThreadSharedMemory::GetPipeIndex(void) const
{
 return PipeIndex;
}

bool TVideoCaptureThreadSharedMemory::SetPipeIndex(int value)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 PipeIndex=value;
 return true;
}

/// Имя канала общей памяти
const std::string& TVideoCaptureThreadSharedMemory::GetPipeName(void) const
{
 return PipeName;
}

bool TVideoCaptureThreadSharedMemory::SetPipeName(const std::string& value)
{
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 PipeName=value;
 return true;
}

/// Размер канала общей памяти
int TVideoCaptureThreadSharedMemory::GetSharedMemoryPipeSize(void) const
{
 return SharedMemoryPipeSize;
}

/// Возвращает число изображений в последовательности
long long TVideoCaptureThreadSharedMemory::GetNumBitmaps(void) const
{
 return 0;
}

/// Устанавливает текущую позицию в последовательности
long long TVideoCaptureThreadSharedMemory::GetPosition(void) const
{
 return LastTimeStamp;
}

bool TVideoCaptureThreadSharedMemory::SetPosition(long long index)
{
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadSharedMemory::BeforeCalculate(void)
{
 TVideoCaptureThread::BeforeCalculate();
}

void __fastcall TVideoCaptureThreadSharedMemory::AfterCalculate(void)
{
 TVideoCaptureThread::AfterCalculate();
}

void __fastcall TVideoCaptureThreadSharedMemory::Calculate(void)
{
  if(SourceMode == 6)
  {
   if(Usm_IsPipeInit)
   {
	int real_size=0;
	if(Usm_IsPipeInit(PipeIndex)<0)
	{
	 Synchronize(UnsafeInit);
	 if(Usm_IsPipeInit(PipeIndex)<0)
	 {
	  Sleep(20);
	  return;
	 }
	}

	SharedMemoryPipeSize=Usm_GetPipeSize(PipeIndex);
	if(SharedMemoryPipeSize<0)
	 SharedMemoryPipeSize=0;
	PipeBuffer.resize(SharedMemoryPipeSize);
	if(!SharedMemoryPipeSize || SharedMemoryPipeSize<16)
	 return;

	real_size=Usm_ReadData(PipeIndex,&PipeBuffer[0],12);
	int mem_size=0;
	if(real_size==12)
	{
	 int shift=0;
	 long long time_stamp;
	 memcpy(&time_stamp,&PipeBuffer[0],sizeof(time_stamp));
	 memcpy(&mem_size,&PipeBuffer[8],sizeof(mem_size));

	 if(LastTimeStamp == time_stamp)
	 {
	  Sleep(20);
	  return;
	 }

 	 real_size=Usm_ReadData(PipeIndex,&PipeBuffer[0],mem_size);
	 shift+=sizeof(time_stamp)+sizeof(mem_size);

	 int width=0;
	 int height=0;
	 int color_model=0;
	 memcpy(&width,&PipeBuffer[shift],sizeof(width));
	 shift+=sizeof(width);
	 memcpy(&height,&PipeBuffer[shift],sizeof(height));
	 shift+=sizeof(height);
	 memcpy(&color_model,&PipeBuffer[shift],sizeof(color_model));
	 shift+=sizeof(color_model);

	 WriteSource->SetRes(width,height,(RDK::UBMColorModel)color_model);
	 if(shift<SharedMemoryPipeSize)
	 {
	  int image_size=WriteSource->GetByteLength();
	  if(image_size>SharedMemoryPipeSize-20)
	   image_size=SharedMemoryPipeSize-20;
	  memcpy(WriteSource->GetData(),&PipeBuffer[shift],image_size);
	 }

	 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
	  return;
	 ResetEvent(SourceUnlock);
	 RDK::UBitmap* old_read_source=ReadSource;
	 ReadSource=WriteSource;
	 WriteSource=old_read_source;
	 LastTimeStamp=time_stamp;
	 SetEvent(SourceUnlock);
	}
   }
  }

}

void __fastcall TVideoCaptureThreadSharedMemory::UnsafeInit(void)
{
 if(!Usm_GetNumPipes || Usm_GetNumPipes() <= PipeIndex)
  return;

 Usm_InitPipe(PipeIndex,SharedMemoryPipeSize,0,PipeName.c_str());
}

// --------------------------

//---------------------------------------------------------------------------
__fastcall TVideoOutputFrame::TVideoOutputFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 FrameIndex=0;
// Capture=0;

 // Модуль графики
 Graph.SetCanvas(&BmpCanvas);

 // Отрисовка геометрии
 GeometryGraphics.SetGraphics(&Graph);

 x1b=x2b=y1b=y2b=-1;
 corrx1b=corry1b=corrx2b=corry2b=-1;
 left=-1; top=-1; width=-1; height=-1;
 PointIndex=-1;
 FigureIndex=-1;

 CorrSelectFlag=false;
 ZoneSelectEnable=false;

 ZoneSelectEvent=CreateEvent(0,0,0,0);
 UpdateFlag=false;

 PointFlag=1;
// FigureFlag=false;

 MyVideoOutputToolsForm=new TVideoOutputToolsForm(this,
    this,
    GeometryGraphics,
//    SampleGeometryGraphics,
//    Figure,
    FigureIndex,
//    FigureFlag,
    PointIndex,
    PointFlag);

 MyVideoGrabberControlForm=new TVideoGrabberControlForm(this);
 MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this,0);//VideoGrabber);

// ConvertBitmap=new Graphics::TBitmap;

 MyComponentsListForm=new TUComponentsListForm(this);

 IsStarted=false;
 CaptureThread=0;//new TVideoCaptureThreadSharedMemory(this,false);
}

__fastcall TVideoOutputFrame::~TVideoOutputFrame(void)
{
 IsStarted=false;
 CloseHandle(ZoneSelectEvent);
 delete MyVideoOutputToolsForm;
 MyVideoOutputToolsForm=0;

 delete MyVideoGrabberControlForm;
 MyVideoGrabberControlForm=0;

// delete ConvertBitmap;

 if(MyComponentsListForm)
  delete MyComponentsListForm;
	   /*
 if(CaptureThread)
 {
  CaptureThread->Terminate();
  CaptureThread->WaitFor();
  delete CaptureThread;
  CaptureThread=0;
 }       */
}

//---------------------------------------------------------------------------
/// Уничтожает созданный поток
bool TVideoOutputFrame::DestroyCaptureThread(void)
{
 if(CaptureThread)
 {
  CaptureThread->Stop();
  CaptureThread->Terminate();
  if(WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),1000) != WAIT_TIMEOUT)
  {
   delete CaptureThread;
   CaptureThread=0;
  }
  else
   return false;
 }

 return true;
}


// Инициализация фрейма avi-файлом
void TVideoOutputFrame::InitByAvi(const String &filename)
{
 StopButtonClick(this);
/*
 VideoGrabber->PlayerFileName=filename;

   VideoGrabber->Display_AutoSize = false;
   VideoGrabber->PlayerRefreshPausedDisplay = false;
   VideoGrabber->AutoStartPlayer = false;
   VideoGrabber->BurstCount = 0;
   VideoGrabber->BurstInterval = 0;
   VideoGrabber->BurstMode = true;
   VideoGrabber->BurstType = fc_TBitmap;
   VideoGrabber->Synchronized=true;

   VideoGrabber->OpenPlayer();  */
   Mode=1;
//   UpdateInterfaceFlag=true;
//   TrackBar->Position=0;
//   UpdateInterfaceFlag=false;

 if(!DestroyCaptureThread())
  return;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadVideoGrabberAvi(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadVideoGrabberAvi* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberAvi*>(CaptureThread);
  if(thread)
  {
   thread->SetFileName(AnsiString(filename).c_str());
   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
  }
 }

 UpdateInterface(true);
}

// Инициализация фрейма bmp-файлом
void TVideoOutputFrame::InitByBmp(const String &filename)
{
 StopButtonClick(this);
/* try {
  if(filename.Pos(".jpg") || filename.Pos(".jpeg") )
  {
   TJPEGImage* JpegIm=new TJPEGImage;
   JpegIm->LoadFromFile(filename);
   Image->Picture->Bitmap->Assign(JpegIm);
   BmpSource<<Image->Picture->Bitmap;
   delete JpegIm;
  }
  else
  if(filename.Pos(".bmp"))
  {
   LoadBitmapFromFile(AnsiString(filename).c_str(),&BmpSource);
  }
  else
   BmpSource.Fill(0);
 }
 catch (EFOpenError &exception) {
  BmpSource.SetRes(0,0);
 }
 BmpSource.SetColorModel(RDK::ubmRGB24);  */

 if(!DestroyCaptureThread())
  return;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadBmp(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadBmp* thread=dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread);
  if(thread)
  {
   thread->SetFileName(AnsiString(filename).c_str());
   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
  }
 }
 Mode=0;
 UpdateInterface(true);
}

// Устанавливает отдельное изображение
bool TVideoOutputFrame::InitByBmp(const RDK::UBitmap &bmp)
{
 StopButtonClick(this);
// BmpSource=bmp;
// BmpSource.SetColorModel(RDK::ubmRGB24);

 if(!DestroyCaptureThread())
  return false;

 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadBmp(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadBmp* thread=dynamic_cast<TVideoCaptureThreadBmp*>(CaptureThread);
  if(thread)
  {
   long long time_stamp=0;
   thread->WriteSourceSafe(bmp,time_stamp,false);
   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
  }
 }
 Mode=0;

 UpdateInterface(true);
 return true;//UpdateVideo();
}


// Инициализация фрейма камерой
void TVideoOutputFrame::InitByCamera(int camera_index, int input_index, int size_index, int subtype_index, int analog_index)
{
 StopButtonClick(this);
/* VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->BurstMode = True;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->VideoDevice=camera_index;
 VideoGrabber->VideoInput=input_index;
 VideoGrabber->VideoSize=size_index;
 VideoGrabber->VideoSubtype=subtype_index;
 VideoGrabber->AnalogVideoStandard=analog_index;
 Mode=2; */
// StartButtonClick(this);
// UpdateVideo();

 if(!DestroyCaptureThread())
  return;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadVideoGrabberCamera(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadVideoGrabberCamera* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberCamera*>(CaptureThread);
  if(thread)
  {
   thread->Init(camera_index, input_index, size_index, subtype_index, analog_index);
   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
  }
 }
 UpdateInterface(true);
}

// Инициализация фрейма IP-камерой
void TVideoOutputFrame::InitByIPCamera(const String camera_url, const String user_name, const String user_password)
{
 StopButtonClick(this);
/* VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->BurstMode = True;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->IPCameraURL=camera_url;

 VideoGrabber->SetAuthentication(at_IPCamera,user_name,user_password);*/
 if(!DestroyCaptureThread())
  return;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadVideoGrabberIpCamera(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadVideoGrabberIpCamera* thread=dynamic_cast<TVideoCaptureThreadVideoGrabberIpCamera*>(CaptureThread);
  if(thread)
  {
   thread->Init(camera_url, user_name, user_password);
   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, thread->GetVideoGrabber());
  }
 }

 Mode=3;
// StartButtonClick(this);
// UpdateVideo();
 UpdateInterface(true);
}

// Инициализация последовательностью изображений
bool TVideoOutputFrame::InitByImageSequence(const String &pathname)
{
 StopButtonClick(this);
 Mode=4;
// StartButtonClick(this);
/*
 BmpSequencePathName=AnsiString(pathname+"\\").c_str();

 RDK::FindFilesList(BmpSequencePathName, "*.bmp", true, BmpSequenceNames);
 if(BmpSequenceNames.size() == 0)
  RDK::FindFilesList(BmpSequencePathName, "*.jpg", true, BmpSequenceNames);
 if(BmpSequenceNames.size() == 0)
  RDK::FindFilesList(BmpSequencePathName, "*.jpeg", true, BmpSequenceNames);

 CurrentBmpSequenceIndex=0;

 LastReadSequenceIndex=-1;
 if(BmpSequenceNames.size()>0)
 {
  LoadImageFromSequence(0,BmpSource);
  BmpSource.SetColorModel(RDK::ubmRGB24);
  LastReadSequenceIndex=0;
 }
 else
 {
  BmpSource.Clear();
  LastReadSequenceIndex=-1;
 }

 UpdateInterfaceFlag=true;
 UpdateInterfaceFlag=false;*/
 if(!DestroyCaptureThread())
  return false;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadBmpSequence(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadBmpSequence* thread=dynamic_cast<TVideoCaptureThreadBmpSequence*>(CaptureThread);
  if(thread)
  {
   thread->SetPathName(AnsiString(pathname).c_str());
   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
  }
 }
// CurrentBmpSequenceIndex=0;
 TrackBar->Position=0;
 UpdateInterface(true);

 return true;
}
/*
// Загружает выбранную картинку по индеку в массиве имен
bool TVideoOutputFrame::LoadImageFromSequence(int index, RDK::UBitmap &bmp)
{
 if(LastReadSequenceIndex == index)
  return true;

 if(BmpSequenceNames[index].find(".bmp") != std::string::npos)
 {
  RDK::LoadBitmapFromFile((BmpSequencePathName+BmpSequenceNames[index]).c_str(),bmp);
 }
 else
 if(BmpSequenceNames[index].find(".jpg") != std::string::npos || BmpSequenceNames[index].find(".jpeg") != std::string::npos)
 {
  TJPEGImage* JpegIm=new TJPEGImage;
  JpegIm->LoadFromFile((BmpSequencePathName+BmpSequenceNames[index]).c_str());
  Image->Picture->Bitmap->Assign(JpegIm);
  bmp<<Image->Picture->Bitmap;
  delete JpegIm;
 }
 else
  bmp.Fill(0);
 LastReadSequenceIndex = index;
 return true;
}
             */
// Инициализация http-сервера
bool TVideoOutputFrame::InitByHttpServer(int listen_port)
{
 StopButtonClick(this);
 Mode=5;
 if(!DestroyCaptureThread())
  return false;
 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadHttpServer(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadHttpServer* thread=dynamic_cast<TVideoCaptureThreadHttpServer*>(CaptureThread);
  if(thread)
  {
   thread->SetListenPort(listen_port);
   MyVideoGrabberControlForm->VideoGrabberControlFrame->Init(this, 0);
  }
 }
/* UHttpServerFrame->IdHTTPServer->Bindings->DefaultPort=listen_port;
 TIdSocketHandle* bind=0;
 if(UHttpServerFrame->IdHTTPServer->Bindings->Count == 0)
  bind=UHttpServerFrame->IdHTTPServer->Bindings->Add();
 else
  bind=UHttpServerFrame->IdHTTPServer->Bindings->Items[0];

 bind->Port=listen_port;*/
 return true;
}

// Инициализация общей памяти
bool TVideoOutputFrame::InitBySharedMemory(int pipe_index, const std::string &pipe_name)
{
 StopButtonClick(this);
/* PipeIndex=pipe_index;
 PipeName=pipe_name;
 SharedMemoryPipeSize=0;*/
 Mode=6;

 if(!Usm_GetNumPipes)
  return false;

 if(Usm_GetNumPipes() <= pipe_index)
  Usm_SetNumPipes(pipe_index+1);

 if(CaptureThread)
 {
  CaptureThread->Stop();
  CaptureThread->Terminate();
  if(WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),1000) != WAIT_TIMEOUT)
  {
   delete CaptureThread;
   CaptureThread=0;
  }
  else
   return false;
 }

 if(!CaptureThread)
 {
  CaptureThread=new TVideoCaptureThreadSharedMemory(this,false);
  CaptureThread->SetChannelIndex(FrameIndex);
  TVideoCaptureThreadSharedMemory* thread=dynamic_cast<TVideoCaptureThreadSharedMemory*>(CaptureThread);
  if(thread)
  {
   thread->SetPipeIndex(pipe_index);
   thread->SetPipeName(pipe_name);
  }
 }

 UpdateInterface(true);
 return true;
}

// Устанавливает название окна
bool TVideoOutputFrame::SetTitle(String title)
{
 if(title == "")
  GroupBox->Caption="";
 else
  GroupBox->Caption=String(" ")+title+String(" ");

 return true;
}

// Устанавливает координаты отображаемой зоны
// Если все координаты == -1 то зона не отображается
bool TVideoOutputFrame::SetFrameRect(int x,int y, int x_width, int y_height, TColor color)
{
 left=x; top=y; width=x_width; height=y_height;
 SelColor=color;
 UpdateVideo();
 return true;
}

// Обновляет отрисовку окна
bool TVideoOutputFrame::UpdateVideo(void)
{
 Graphics::TBitmap *bmp=Image->Picture->Bitmap;
 if(!bmp)
  return false;

  bmp->Width=BmpSource.GetWidth();
  bmp->Height=BmpSource.GetHeight();
  bmp->HandleType=bmDIB;
  switch(BmpSource.GetPixelByteLength())
  {
  case 1:
   bmp->PixelFormat=pf8bit;
  break;

  case 2:
   bmp->PixelFormat=pf16bit;
  break;

  case 3:
   bmp->PixelFormat=pf24bit;
  break;

  case 4:
   bmp->PixelFormat=pf32bit;
  break;
  }
 BmpSource.ReflectionX(&BmpCanvas);
// BmpCanvas=BmpSource;

 GeometryGraphics.Repaint();
 if(ShowCentralPointCheckBox->Checked)
 {
  RDK::UGraphics gr(&BmpCanvas);
  gr.SetPenColor(0x000000FF);
  gr.SetPenWidth(1);
  int x=StrToInt(PointXEdit->Text);
  int y=StrToInt(PointYEdit->Text);
  gr.Line(x,0,x,BmpCanvas.GetHeight());
  gr.Line(0,y,BmpCanvas.GetWidth(),y);
//  Image->Canvas->Pen->Color=clRed;
//  Image->Canvas->Pen->Width=1;
//  Image->Canvas->MoveTo(Image->Width/2,0);
//  Image->Canvas->LineTo(Image->Width/2,Image->Height-1);
//  Image->Canvas->MoveTo(0,Image->Height/2);
//  Image->Canvas->LineTo(Image->Width-1,Image->Height/2);
 }

 BmpCanvas.ReflectionX();
 BmpCanvas>>bmp;

// DrawCapture(Image->Picture->Bitmap);
 Image->Repaint();
 if(left != -1 || top != -1 || width != -1 || height != -1)
  DrawFrameRect(Image, left, top, left+width, top+height, 2, SelColor);

 Image->Repaint();
 UpdateGeometryList(MyVideoOutputToolsForm->GeometryCheckListBox,
					MyVideoOutputToolsForm->PointsCheckListBox);
  /*
 if(Mode == 4)
 {
  UpdateFlag=true;
  std::string sstamp;
  RDK::UTimeStamp stamp(long(CurrentBmpSequenceIndex),1);
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
  TrackBar->Max=int(BmpSequenceNames.size())-1;
  TrackBar->Enabled=true;
  TrackBar->Position=CurrentBmpSequenceIndex;
  UpdateFlag=false;
 }  */

 return true;
}

// Отрисовываем текущее состояние видеопотока
void TVideoOutputFrame::DrawCapture(Graphics::TBitmap *bmp)
{
/* if(!bmp)
  return;

  bmp->Width=BmpSource.GetWidth();
  bmp->Height=BmpSource.GetHeight();
  bmp->HandleType=bmDIB;
  switch(BmpSource.GetPixelByteLength())
  {
  case 1:
   bmp->PixelFormat=pf8bit;
  break;

  case 2:
   bmp->PixelFormat=pf16bit;
  break;

  case 3:
   bmp->PixelFormat=pf24bit;
  break;

  case 4:
   bmp->PixelFormat=pf32bit;
  break;
  }
 BmpSource.ReflectionX(&BmpCanvas);
// BmpCanvas=BmpSource;

 GeometryGraphics.Repaint();
 BmpCanvas.ReflectionX();
 BmpCanvas>>bmp;
 */
}

// Обновляем список точек
void TVideoOutputFrame::UpdateGeometryList(TCheckListBox *GeometryCheckListBox, TCheckListBox *PointsCheckListBox)
{
 int ix=GeometryCheckListBox->ItemIndex;

 GeometryCheckListBox->Clear();
 for(size_t i=0;i<GeometryGraphics.GetNumGeometries();i++)
  GeometryCheckListBox->Items->Add(GeometryGraphics.Description(i).Name.c_str());
 for(size_t i=0;i<GeometryGraphics.GetNumGeometries();i++)
  GeometryCheckListBox->Checked[i]=GeometryGraphics.Description(i).Visible;

 if(ix>=0)
  GeometryCheckListBox->ItemIndex=ix;
 else
  GeometryCheckListBox->ItemIndex=0;

 if(GeometryCheckListBox->ItemIndex<0)
  return;
/*
 if(FigureFlag)
 {
  GeometryCheckListBox->ItemIndex=FigureIndex;
  GeometryCheckListBox->Enabled=false;
 }
 else
  GeometryCheckListBox->Enabled=true;
  */
 if(int(GeometryGraphics.GetNumGeometries())<=FigureIndex)
  return;

 ix=PointsCheckListBox->ItemIndex;

// PointsCheckListBox->Clear();
 if(GeometryCheckListBox->ItemIndex>=0 && int(CurrentGeometryGraphics.GetNumGeometries())>GeometryCheckListBox->ItemIndex)
 {
  if(GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex) == CurrentGeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex))
   return;
 }
 else
  CurrentGeometryGraphics=GeometryGraphics;

 CurrentGeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex)=GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex);
 int new_value=GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).GetNumVertices();
 int diff=abs(PointsCheckListBox->Count-new_value);
 if(PointsCheckListBox->Count<new_value)
 {
  for(int i=0;i<diff;i++)
   PointsCheckListBox->Items->Add("");
 }
 else
 if(PointsCheckListBox->Count>new_value)
 {
  for(int i=0;i<diff;i++)
   PointsCheckListBox->Items->Delete(PointsCheckListBox->Count-1);
 }
 //PointsCheckListBox->Count=new_value;
 for(int i=0;i<new_value;i++)
 {
  std::stringstream stream;
  stream<<GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).VertexName(i)<<" ";
  stream<<GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).Vertex(i);
  PointsCheckListBox->Items->Strings[i]=stream.str().c_str();// Add(stream.str().c_str());
//  PointsCheckListBox->Items->Add(stream.str().c_str());
   if(GeometryGraphics.Geometry(GeometryCheckListBox->ItemIndex).Vertex(i).z>=0)
   {
    PointsCheckListBox->Checked[i]=true;
   }
   else
    PointsCheckListBox->Checked[i]=false;
 }

 if(ix>=0)
  PointsCheckListBox->ItemIndex=ix;
 else
  PointsCheckListBox->ItemIndex=0;
 /*
 if(PointFlag > 0)
 {
  PointsCheckListBox->Enabled=false;
 }
 else
 {
  PointsCheckListBox->Enabled=true;
 } */
}

// Метод отрисовки прямоугольной зоны
void __fastcall TVideoOutputFrame::DrawFrameRect(TImage *image, int x1, int y1, int x2,
                                                int y2, int framewidth, TColor color)
{
 TRect rect;
 int realwidth;

 image->Picture->Bitmap->Canvas->Brush->Color = color;

 if(framewidth > 0)
  realwidth=framewidth;
 else
  {
   int w1=image->Picture->Bitmap->Width/image->Width;
   int w2=image->Picture->Bitmap->Height/image->Height;
   realwidth=(w1>w2)?w1:w2;
   if(realwidth <= 0)
    realwidth=1;

   realwidth++;
  }

 for(int i=0;i<realwidth;i++)
  {
   rect.left=x1+i; rect.top=y1+i;
   rect.right=x2-i; rect.bottom=y2-i;
   image->Picture->Bitmap->Canvas->FrameRect(rect);
  }
}

// Добавляет очередной элемент фигуры
void TVideoOutputFrame::AddFigureRect(double l,double t,double w,double h)
{
 if(l<0 || t<0)
  return;

 if(FigureIndex<0)
  return;

 GeometryGraphics.Geometry(FigureIndex).Vertex(PointIndex).x=l+w/2;
 GeometryGraphics.Geometry(FigureIndex).Vertex(PointIndex).y=/*BmpCanvas.GetHeight()-*/(t+h/2);

// GeometryGraphics.Geometry(FigureIndex)=Figure;

 UpdateVideo();
}


// Устанавливает образец графики
void TVideoOutputFrame::SetSampleGeometryGraphics(RDK::MGraphics<double,2>& samplegraphics)
{
// SampleGeometryGraphics=samplegraphics;
// GeometryGraphics=SampleGeometryGraphics;
 UpdateVideo();
 FigureIndex=0;
 PointIndex=0;
 MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex=PointIndex;
 MyVideoOutputToolsForm->GeometryCheckListBox->ItemIndex=FigureIndex;
}

// -------------------------
// Методы ввода вывода точек геометрии из параметров и переменных состояния компонент
// -------------------------
// Отправляет набор точек в свойство компонента
void TVideoOutputFrame::SendToComponentPropertyMatrix(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 if(figure_index<0)
  return;

 RDK::MDMatrix<double> matrix;
 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 int num_points=int(points.size());
 matrix.Resize(num_points,2);
 for(int i=0;i<num_points;i++)
 {
  matrix(i,0)=points[i][0];
  matrix(i,1)=points[i][1];
 }
 Model_SetComponentPropertyData(stringid.c_str(), parameter_name.c_str(), &matrix);
}

// Отправляет набор точек в параметр компонента
void TVideoOutputFrame::SendToComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 if(figure_index<0)
  return;

 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 Model_SetComponentPropertyData(stringid.c_str(), parameter_name.c_str(), &points);
// RDK::WriteParameterValue(stringid, parameter_name, points);
}

// Отправляет набор точек в переменную состояния компонента
void TVideoOutputFrame::SendToComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 if(figure_index<0)
  return;

 const std::vector<RDK::MVector<double,2> > &points=GeometryGraphics.Geometry(figure_index).GetVertices();
 Model_SetComponentPropertyData(stringid.c_str(), state_name.c_str(), &points);
// RDK::WriteStateValue(stringid, state_name, points);
}

// Считывает набор точек из параметра компонента
void TVideoOutputFrame::ReceiveFromComponentParameter(const std::string &stringid, const std::string &parameter_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 points=*(const std::vector<RDK::MVector<double,2> > *)Model_GetComponentPropertyData(stringid.c_str(), parameter_name.c_str());
// RDK::ReadParameterValue(stringid, parameter_name, points);
}

// Считывает набор точек из переменной состояния компонента
void TVideoOutputFrame::ReceiveFromComponentState(const std::string &stringid, const std::string &state_name, int figure_index)
{
 std::vector<RDK::MVector<double,2> > points;
 points=*(const std::vector<RDK::MVector<double,2> > *)Model_GetComponentPropertyData(stringid.c_str(), state_name.c_str());
// RDK::ReadStateValue(stringid, state_name, points);
}
// -------------------------

// -------------------------
// Методы вывода изображений во входы-выходы компонент
// -------------------------
// Отправляет изображение в выбранный компонент
void TVideoOutputFrame::SendToComponentIO(void)
{
 if(LinkedComponentName.empty())
  return;
 BmpSource.ReflectionX(&ReflectedBmpSource);
 switch(LinkedMode)
 {
 case 0:
  Model_SetComponentBitmapInput(LinkedComponentName.c_str(), LinkedIndex.c_str(), &ReflectedBmpSource);
 break;

 case 1:
  Model_SetComponentBitmapOutput(LinkedComponentName.c_str(), LinkedIndex.c_str(), &ReflectedBmpSource);
 break;

 case 2:
  Model_SetComponentPropertyData(LinkedComponentName.c_str(), LinkedComponentPropertyName.c_str(), &ReflectedBmpSource);
 break;
 }
}
// -------------------------


// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед шагом расчета
void TVideoOutputFrame::ABeforeCalculate(void)
{
 if(!IsStarted)
  return;

 if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 0)
  if(CaptureThread)
  {
   long long time_stamp=0;
   CaptureThread->ReadSourceSafe(BmpSource,time_stamp,false);

	std::string sstamp;
	RDK::UTimeStamp stamp(double(time_stamp/1000.0),25);
	stamp>>sstamp;
	TimeEdit->Text=sstamp.c_str();

	UpdateVideo();
  }

   /*
 if(Mode == 4)
 {
  CurrentBmpSequenceIndex++;
  UpdateVideo();
  long long time_stamp=GetTickCount();
  if(GetNumEngines() == 1)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,time_stamp);
  else
  if(GetNumEngines() > FrameIndex)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(FrameIndex,time_stamp);

  Sleep(0);
 }
 else
 if(Mode == 6)
 {
 if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() != 1)
 {
  if(Usm_IsPipeInit)
  {
   int real_size=0;
   if(Usm_IsPipeInit(PipeIndex)<0)
   {
	StartButtonClick(this);
	if(Usm_IsPipeInit(PipeIndex)<0)
	 return;
   }

   SharedMemoryPipeSize=Usm_GetPipeSize(PipeIndex);
   if(SharedMemoryPipeSize<0)
	SharedMemoryPipeSize=0;
   PipeBuffer.resize(SharedMemoryPipeSize);
   if(!SharedMemoryPipeSize || SharedMemoryPipeSize<16)
	return;

   real_size=Usm_ReadData(PipeIndex,&PipeBuffer[0],PipeBuffer.size());
   if(real_size>0)
   {
	int shift=0;
	long long time_stamp;
	memcpy(&time_stamp,&PipeBuffer[0],sizeof(time_stamp));


	shift+=sizeof(time_stamp);

	int width=0;
	int height=0;
	memcpy(&width,&PipeBuffer[shift],sizeof(width));
	shift+=sizeof(width);
	memcpy(&height,&PipeBuffer[shift],sizeof(height));
	shift+=sizeof(height);
	BmpSource.SetRes(width,height,RDK::ubmRGB24);
	if(shift<SharedMemoryPipeSize)
	{
	 int image_size=width*height*3;
	 if(image_size>SharedMemoryPipeSize-16)
	  image_size=SharedMemoryPipeSize-16;
	 memcpy(BmpSource.GetData(),&PipeBuffer[shift],image_size);
	}

	if(GetNumEngines() == 1)
	 UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,time_stamp);
	else
	if(GetNumEngines() > FrameIndex)
	 UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(FrameIndex,time_stamp);

	std::string sstamp;
	RDK::UTimeStamp stamp(double(time_stamp/1000.0),25);
	stamp>>sstamp;
	TimeEdit->Text=sstamp.c_str();

	UpdateVideo();
   }
  }
 }
 }  */

 if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() != 1)
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
 }
}


// Метод, вызываемый после шага расчета
void TVideoOutputFrame::AAfterCalculate(void)
{
 if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 0)
 {
  if(CaptureThread)
   SetEvent(CaptureThread->GetCalcCompleteEvent());
 }
}


// Обновление интерфейса
void TVideoOutputFrame::AUpdateInterface(void)
{
// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() == 1)
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
  SendToEdit->Text="";
}

// Сохраняет параметры интерфейса в xml
void TVideoOutputFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("LinkedComponentName",LinkedComponentName);
 xml.WriteString("LinkedComponentPropertyName",LinkedComponentPropertyName);
 xml.WriteInteger("LinkedMode",LinkedMode);
 xml.WriteString("LinkedIndex",LinkedIndex);

 xml.WriteString("SelectedComponentSName",SelectedComponentSName);
 xml.WriteString("SelectedComponentStateName",SelectedComponentStateName);
 xml.WriteString("SelectedComponentPName",SelectedComponentPName);
 xml.WriteString("SelectedComponentParameterName",SelectedComponentParameterName);
 xml.WriteString("SelectedComponentMatrixName",SelectedComponentMatrixName);
 xml.WriteString("SelectedComponentPropertyMatrixName",SelectedComponentPropertyMatrixName);
 xml.WriteInteger("SendPointsByStep",SendPointsByStepCheckBox->Checked);
 xml.WriteInteger("DeletePointsAfterSendCheckBox",DeletePointsAfterSendCheckBox->Checked);
}

// Загружает параметры интерфейса из xml
void TVideoOutputFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 LinkedComponentName=xml.ReadString("LinkedComponentName","");
 LinkedComponentPropertyName=xml.ReadString("LinkedComponentPropertyName","");
 LinkedMode=xml.ReadInteger("LinkedMode",1);
 LinkedIndex=xml.ReadString("LinkedIndex","");

 SelectedComponentSName=xml.ReadString("SelectedComponentSName","");
 SelectedComponentStateName=xml.ReadString("SelectedComponentStateName","");
 SelectedComponentPName=xml.ReadString("SelectedComponentPName","");
 SelectedComponentParameterName=xml.ReadString("SelectedComponentParameterName","");
 SelectedComponentMatrixName=xml.ReadString("SelectedComponentMatrixName","");
 SelectedComponentPropertyMatrixName=xml.ReadString("SelectedComponentPropertyMatrixName","");
 SendPointsByStepCheckBox->Checked=xml.ReadInteger("SendPointsByStep",0);
 DeletePointsAfterSendCheckBox->Checked=xml.ReadInteger("DeletePointsAfterSendCheckBox",0);

 UpdateInterface();
}
// -----------------------------



//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TimerTimer(TObject *Sender)
{
/*
// UpdateFlag=false;
// if(UEngineMonitorForm->EngineMonitorFrame->GetChannelsMode() != 1)
 {
  ABeforeCalculate();
  if(CaptureThread)
  {
   long long time_stamp=0;
   CaptureThread->ReadSourceSafe(BmpSource,time_stamp,false);

	std::string sstamp;
	RDK::UTimeStamp stamp(double(time_stamp/1000.0),25);
	stamp>>sstamp;
	TimeEdit->Text=sstamp.c_str();

	UpdateVideo();
  }
 }*/
// Image->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StartButtonClick(TObject *Sender)
{
 if(Mode == 4)
 {
  TVideoCaptureThreadBmpSequence* thread=dynamic_cast<TVideoCaptureThreadBmpSequence*>(CaptureThread);
  if(thread)
  {
   thread->SetSyncMode(1);
  }

 }

  if(CaptureThread)
  {
   CaptureThread->Start();
   WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),30);
  }
 Timer->Enabled=true;


 IsStarted=true;
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::StopButtonClick(TObject *Sender)
{
 IsStarted=false;
 Timer->Enabled=false;


 if(CaptureThread)
 {
  CaptureThread->Stop();
  WaitForSingleObject(CaptureThread->GetFrameNotInProgress(),30);
 }
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::ImageMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(!ZoneSelectEnable)
  return;

 if(Button != mbLeft)
  return;

 //ImageTrackBarChange(Sender);
 DrawFrameRect(Image, left, top, left+width, top+height,2, SelColor);

 double iwidth,iheight,bwidth,bheight;
 int k1x,k1y,k2x,k2y;

 iwidth=Image->Width;
 iheight=Image->Height;
 bwidth=Image->Picture->Bitmap->Width;
 bheight=Image->Picture->Bitmap->Height;

 if(Shift.Contains(ssShift))
  {
   corrx1b=X, corry1b=Y;
   CorrSelectFlag=true;
  }
 else
  {
   x1b=X; y1b=Y;

   k1x=X*bwidth/iwidth;
   k2x=x1b*bwidth/iwidth;
   k1y=Y*bheight/iheight;
   k2y=y1b*bheight/iheight;
   left=k1x;
   top=k1y;
   width=(k2x-k1x);
   height=(k2y-k1y);
   CorrSelectFlag=false;
  }
// if(FigureFlag && !PointFlag)
 if(PointFlag == 1)
 {
  if(FigureIndex<0)
   MyVideoOutputToolsForm->AddFigureButtonClick(Sender);
  GeometryGraphics.Geometry(FigureIndex).SetNumVertices(GeometryGraphics.Geometry(FigureIndex).GetNumVertices()+1);
  PointIndex=GeometryGraphics.Geometry(FigureIndex).GetNumVertices()-1;
  GeometryGraphics.Geometry(FigureIndex).VertexName(PointIndex)=std::string(AnsiString(MyVideoOutputToolsForm->PointNameEdit->Text).c_str())+RDK::sntoa(PointIndex+1,3);
 }
 else
 if(PointFlag == 2)
 {
//  if(FigureIndex<0 || PointIndex<0)
//  {
//   PointFlag=0;
//   return;
//  }

//  MyVideoOutputToolsForm->EditPointButtonClick(Sender);
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::ImageMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
 if(!ZoneSelectEnable)
  return;

 String str;
 int cardwidth,cardheight;
 double iwidth,iheight,bwidth,bheight;

 iwidth=Image->Width;
 iheight=Image->Height;
 bwidth=Image->Picture->Bitmap->Width;
 bheight=Image->Picture->Bitmap->Height;


 //DrawCapture(Image->Picture->Bitmap);
 UpdateVideo();
 Image->Canvas->Pen->Color=clLime;
 Image->Canvas->PenPos=TPoint(X*bwidth/iwidth,0);
 Image->Canvas->LineTo(X*bwidth/iwidth,Image->Height*bheight/iheight);
 Image->Canvas->PenPos=TPoint(0,Y*bheight/iheight);
 Image->Canvas->LineTo(Image->Width*bwidth/iwidth,Y*bheight/iheight);

 if(!CorrSelectFlag)
  {
   if(x1b!=-1)
    {
     int i;
     int k1x,k1y,k2x,k2y;


     if(X>x1b)
      {
       k1x=x1b*bwidth/iwidth;
       k2x=X*bwidth/iwidth;
      }
     else
      {
       k1x=X*bwidth/iwidth;
       k2x=x1b*bwidth/iwidth;
      }
     if(Y>y1b)
      {
       k1y=y1b*bheight/iheight;
       k2y=Y*bheight/iheight;
      }
     else
      {
       k1y=Y*bheight/iheight;
       k2y=y1b*bheight/iheight;
      }

//     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 3, (TColor)(255<<16));

     left=k1x;
     top=k1y;
     width=(k2x-k1x);
     height=(k2y-k1y);
//     DrawCapture(Image->Picture->Bitmap);
     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 2, SelColor);
//     ImageTrackBarChange(Sender);
    }
  }
 else
  {
   if(corrx1b!=-1)
    {
     int i;
     int k1x,k1y,k2x,k2y;

     if(X>corrx1b)
      {
       k1x=corrx1b*bwidth/iwidth;
       k2x=X*bwidth/iwidth;
      }
     else
      {
       k1x=X*bwidth/iwidth;
       k2x=corrx1b*bwidth/iwidth;
      }
     if(Y>corry1b)
      {
       k1y=corry1b*bheight/iheight;
       k2y=Y*bheight/iheight;
      }
     else
      {
       k1y=Y*bheight/iheight;
       k2y=corry1b*bheight/iheight;
      }

 //     DrawCapture(Image->Picture->Bitmap);
     DrawFrameRect(Image, k1x, k1y, k2x, k2y, 2, SelColor);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::ImageMouseUp(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button == mbRight)
 {
  if(PointIndex<0)
   return;

//  Figure.SetNumVertices(Figure.GetNumVertices()-1);
//  if(PointIndex>=Figure.GetNumVertices())
//   --PointIndex;

//  GeometryGraphics.Geometry(FigureIndex)=Figure;

  UpdateVideo();
  return;
 }

 if(!ZoneSelectEnable)
  return;

 String str;
 int cardwidth,cardheight;

 if(!CorrSelectFlag)
  {
 /*  if((X==x1b || Y==y1b))
    {
     x1b=x2b=y1b=y2b=-1;
     return;
    }*/
   x2b=X; y2b=Y;

   x1b=x2b=y1b=y2b=-1;
  }
 else
  {
/*   if((X==corrx1b || Y==corry1b))
    {
     corrx1b=corrx2b=corry1b=corry2b=-1;
     return;
    }*/
   corrx2b=X; corry2b=Y;

   corrx1b=corrx2b=corry1b=corry2b=-1;
  }

 // Если добавляем геометрический объект, то...
 if(PointFlag)
 {
  AddFigureRect(left,top,width,height);
/*
 if(Shift.Contains(ssShift))
  MyVideoOutputToolsForm->PointsCheckListBox->Checked[MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex] = false;
 else
  MyVideoOutputToolsForm->PointsCheckListBox->Checked[MyVideoOutputToolsForm->PointsCheckListBox->ItemIndex] = true;
 */
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::TimeEditChange(TObject *Sender)
{
 if(UpdateFlag)
  return;

 /*
 if(Mode != 4)
 {
  int selstart=TimeEdit->SelStart;
  int sellength=TimeEdit->SelLength;

  std::string sstamp;
  RDK::UTimeStamp stamp(0.0,VideoGrabber->CurrentFrameRate);
  sstamp=AnsiString(TimeEdit->Text).c_str();
  stamp<<sstamp;

  UpdateVideo();
  TimeEdit->SelStart=selstart;
  TimeEdit->SelLength=sellength;
 }
 else
 if (Mode == 4)
 {
  std::string sstamp;
  RDK::UTimeStamp stamp(0.0,1);
  sstamp=AnsiString(TimeEdit->Text).c_str();
  stamp<<sstamp;
  CurrentBmpSequenceIndex=stamp.Frames;
  LoadImageFromSequence(CurrentBmpSequenceIndex,BmpSource);
  BmpSource.SetColorModel(RDK::ubmRGB24);
  UpdateVideo();
 }   */
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::N1Click(TObject *Sender)
{
 MyVideoOutputToolsForm->Show();
}
//---------------------------------------------------------------------------
								  /*
void __fastcall TVideoOutputFrame::VideoGrabberFrameCaptureCompleted(TObject *Sender,
          Pointer FrameBitmap, int BitmapWidth, int BitmapHeight, DWORD FrameNumber,
          __int64 FrameTime, TFrameCaptureDest DestType, UnicodeString FileName,
          bool Success, int FrameId)
{
Graphics::TBitmap *Frame_Bitmap;

   Frame_Bitmap = (Graphics::TBitmap*) FrameBitmap;

   switch (DestType) {
      case fc_TBitmap:

         if(Frame_Bitmap->PixelFormat == pf24bit)
          BmpSource<<Frame_Bitmap;
         else
         {
          ConvertBitmap->Assign(Frame_Bitmap);
          ConvertBitmap->PixelFormat=pf24bit;
          BmpSource<<ConvertBitmap;
         }

         UpdateFlag=true;
         std::string sstamp;
		 RDK::UTimeStamp stamp(CurrentFrameNumber,VideoGrabber->CurrentFrameRate);
		 stamp>>sstamp;
         TimeEdit->Text=sstamp.c_str();
         UpdateFlag=false;

         CurrentFrameNumber=FrameNumber;
//         DrawCapture(Image->Picture->Bitmap);
		 UpdateVideo();

	  break;
   }

 if(GetNumEngines() == 1)
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,FrameTime);
 else
 if(GetNumEngines() > FrameIndex)
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(FrameIndex,FrameTime);
}          */
//---------------------------------------------------------------------------
void __fastcall TVideoOutputFrame::TrackBarChange(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;

 if(CaptureThread)
 {
  CaptureThread->SetPosition(TrackBar->Position);
/*
  TVideoCaptureThreadVideoGrabber *vg_thread=dynamic_cast<TVideoCaptureThreadVideoGrabber*>(CaptureThread);
  if(vg_thread)
  {
   if(!vg_thread->GetVideoGrabber()->InFrameProgressEvent)
	vg_thread->GetVideoGrabber()->PlayerFramePosition = TrackBar->Position;
  }

  TVideoCaptureThreadBmpSequence *bmpseq_thread=dynamic_cast<TVideoCaptureThreadBmpSequence*>(CaptureThread);
  if(bmpseq_thread)
  {
   if(TrackBar->Position < bmpseq_thread->GetNumBitmaps())
   {
	bmpseq_thread->SetPosition(TrackBar->Position);
	UpdateInterface();
   }
  }*/
 }
	/*
 if(Mode != 4)
 {
  if(!VideoGrabber->InFrameProgressEvent)
   VideoGrabber->PlayerFramePosition = TrackBar->Position;
 }
 else
 {
  if(TrackBar->Position < BmpSequenceNames.size())
  {
   CurrentBmpSequenceIndex=TrackBar->Position;
   LoadImageFromSequence(CurrentBmpSequenceIndex, BmpSource);
   BmpSource.SetColorModel(RDK::ubmRGB24);
   UpdateVideo();
  }
 }     */
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SourceControl1Click(TObject *Sender)
{
 MyVideoGrabberControlForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SendToComponentClick(TObject *Sender)
{
 if(MyComponentsListForm->ShowParameterSelect() != mrOk)
  return;

 SelectedComponentPName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentParameterName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentParameterName();
 SelectedComponentSName="";
 SelectedComponentStateName="";
 SendToComponentParameter(SelectedComponentPName, SelectedComponentParameterName, FigureIndex);
 UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SendToComponentState1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowStateSelect() != mrOk)
  return;

 SelectedComponentSName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentStateName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
 SelectedComponentPName="";
 SelectedComponentParameterName="";
 SendToComponentState(SelectedComponentSName, SelectedComponentStateName, FigureIndex);
 UpdateInterface();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendImageToComponentOutput1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=1;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendImageToComponentInput1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=0;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentInput();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::Parameter1Click(TObject *Sender)
{
 SendToComponentClick(Sender);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::State1Click(TObject *Sender)
{
 SendToComponentState1Click(Sender);
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::Button1Click(TObject *Sender)
{
 if(!SelectedComponentSName.empty())
 {
  SendToComponentState(SelectedComponentSName, SelectedComponentStateName, FigureIndex);
 }
 else
 if(!SelectedComponentPName.empty())
 {
  SendToComponentParameter(SelectedComponentPName, SelectedComponentParameterName, FigureIndex);
 }
}
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SendImageToComponentProperty1Click(TObject *Sender)

{
 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 LinkedMode=2;
 LinkedComponentName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 LinkedComponentPropertyName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
// LinkedIndex=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::SendAsMatrixButtonClick(TObject *Sender)
{
 if(!SelectedComponentMatrixName.empty())
 {
  SendToComponentPropertyMatrix(SelectedComponentMatrixName, SelectedComponentPropertyMatrixName, FigureIndex);
 }
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputFrame::PropertyMatrix1Click(TObject *Sender)
{
 if(MyComponentsListForm->ShowParameterSelect() != mrOk)
  return;

 SelectedComponentPropertyMatrixName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentStateName();
 if(SelectedComponentPropertyMatrixName=="")
  SelectedComponentPropertyMatrixName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentParameterName();

 SelectedComponentMatrixName=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 SelectedComponentSName="";
 SelectedComponentStateName="";
 SelectedComponentPName="";
 SelectedComponentParameterName="";
 SendToComponentPropertyMatrix(SelectedComponentMatrixName, SelectedComponentPropertyMatrixName, FigureIndex);
 UpdateInterface();
}
//---------------------------------------------------------------------------
					   /*
void __fastcall TVideoOutputFrame::UHttpServerFrameIdHTTPServerCommandGet(TIdContext *AContext,
		  TIdHTTPRequestInfo *ARequestInfo, TIdHTTPResponseInfo *AResponseInfo)

{
 std::vector<char> &time_stamp=UHttpServerFrame->ParsedRequestArgs["TimeStamp"];
 if(!time_stamp.empty())
 {
  std::string temp_stamp;
  temp_stamp.assign(&time_stamp[0],time_stamp.size());
  ServerTimeStamp=RDK::atoi(temp_stamp);

  if(GetNumEngines() == 1)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,ServerTimeStamp);
  else
  if(GetNumEngines() > FrameIndex)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(FrameIndex,ServerTimeStamp);

  std::string sstamp;
  RDK::UTimeStamp stamp(double(ServerTimeStamp/1000),25);
  stamp>>sstamp;
  TimeEdit->Text=sstamp.c_str();
 }

 UHttpServerFrame->IdHTTPServerCommandGet(AContext, ARequestInfo, AResponseInfo);
 if(ARequestInfo->Document == "/images.cgi")
 {
  std::vector<char> &image_data=UHttpServerFrame->ParsedRequestArgs["Image"];
  RDK::UBitmap &bmp=UHttpServerFrame->DecodeParamAsBitmap(image_data);
  BmpSource=bmp;
//  UpdateVideo();
  AResponseInfo->ContentText="OK";
 }
 else
 {
  AResponseInfo->ResponseNo=404;
  AResponseInfo->ResponseText="404 Not Found";
  AResponseInfo->ContentText="404 Not Found";
 }
}*/
//---------------------------------------------------------------------------


void __fastcall TVideoOutputFrame::SaveImage1Click(TObject *Sender)
{
 if(!SavePictureDialog->Execute())
  return;
 Image->Picture->SaveToFile(SavePictureDialog->FileName);
}
//---------------------------------------------------------------------------

