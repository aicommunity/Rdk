//---------------------------------------------------------------------------

#pragma hdrstop

#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "TUBitmap.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThread::TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended)
: ChannelIndex(0), Frame(frame), TThread(CreateSuspended)
{
 SourceMode=-1;
 LastTimeStamp=0;
 CaptureEnabled=CreateEvent(0,TRUE,0,0);
 SourceUnlock=CreateEvent(0,TRUE,TRUE,0);
 SourceWriteUnlock=CreateEvent(0,TRUE,TRUE,0);
 FrameNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 CalcCompleteEvent=CreateEvent(0,TRUE,TRUE,0);
 ReadSource=&Source[0];
 WriteSource=&Source[1];
 RepeatFlag=false;

 FreeOnTerminate=false;
}

__fastcall TVideoCaptureThread::~TVideoCaptureThread(void)
{
 CloseHandle(CaptureEnabled);
 CloseHandle(SourceUnlock);
 CloseHandle(SourceWriteUnlock);
 CloseHandle(FrameNotInProgress);
 CloseHandle(CalcCompleteEvent);
}
// --------------------------


// --------------------------
// ���������� �����������
// --------------------------
/// �������� �����
int TVideoCaptureThread::GetSourceMode(void) const
{
 return SourceMode;
}

/// ������ ������ � ���������� ���������, ����������� ������
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

/// ����� ������������� � ��������
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

/// ���� ������� ��������������� ������� ����� ����������
bool TVideoCaptureThread::GetRepeatFlag(void) const
{
 return RepeatFlag;
}

bool TVideoCaptureThread::SetRepeatFlag(bool value)
{
 RepeatFlag=value;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ��������� �� ���������
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
// ���������� ���������
// --------------------------
/// ������������ �� ���������� ������� ������ �����
HANDLE TVideoCaptureThread::GetFrameNotInProgress(void) const
{
 return FrameNotInProgress;
}

/// ���������� ������. ������������ �� ����� ������� � �����������
HANDLE TVideoCaptureThread::GetSourceUnlock(void) const
{
 return SourceUnlock;
}

/// ������������ �� ����� ������ ������������
HANDLE TVideoCaptureThread::GetCaptureEnabled(void) const
{
 return CaptureEnabled;
}

/// ������������ �� ����� �������� �������
HANDLE TVideoCaptureThread::GetCalcCompleteEvent(void) const
{
 return CalcCompleteEvent;
}
// --------------------------



// --------------------------
// ���������� �������
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
// Frame->UpdateInterface();
 if(!UEngineMonitorForm || !UEngineMonitorForm->EngineMonitorFrame)
  return;
 if(GetNumEngines() == 1)
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,LastTimeStamp);
 else
  if(GetNumEngines() > ChannelIndex)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,LastTimeStamp);
}



void __fastcall TVideoCaptureThread::Execute(void)
{
 while(!Terminated)
 {
  if(WaitForSingleObject(CaptureEnabled,30) == WAIT_TIMEOUT)
   continue;

  if(SyncMode == 1)
  {
   if(WaitForSingleObject(CalcCompleteEvent,10) == WAIT_TIMEOUT)
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

/// ���������� ����� ����������� � �����������
bool TVideoCaptureThread::ReadSourceSafe(RDK::UBitmap& dest, long long &time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);
 time_stamp=LastTimeStamp;
 RDK::UBitmap* source=ReadSource;

 if(reflect)
  source->ReflectionX(&dest);
 else
  dest=*source;
 SetEvent(SourceUnlock);

// SetEvent(SourceUnlock);
 return true;
}

/// ���������� ����������� � ���� � �����������
bool TVideoCaptureThread::WriteSourceSafe(const RDK::UBitmap& src, long long time_stamp, bool reflect)
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

// ������ ��������� ����� � �����������
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
// ������������ � �����������
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
// ���������� �����������
// --------------------------
/// ��� ����� �����������
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
   TempBitmap->PixelFormat=pf24bit;
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

/// ���������� ����� ����������� � ������������������
long long TVideoCaptureThreadBmp::GetNumBitmaps(void) const
{
 return 1;
}

/// ������������� ������� ������� � ������������������
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
// ���������� �������
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
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadBmpSequence::TVideoCaptureThreadBmpSequence(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThread(frame, CreateSuspended)
{
 TempBitmap=new Graphics::TBitmap;
 SyncMode=1;
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
// ���������� �����������
// --------------------------
/// ��� ����� �����������
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

/// ���������� ����� ����������� � ������������������
long long TVideoCaptureThreadBmpSequence::GetNumBitmaps(void) const
{
 return int(BmpSequenceNames.size());
}

/// ������������� ������� ������� � ������������������
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
// ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadBmpSequence::AfterCalculate(void)
{
 TVideoCaptureThread::AfterCalculate();
 CurrentBmpSequenceIndex++;
 if(CurrentBmpSequenceIndex>=int(BmpSequenceNames.size()))
 {
  if(RepeatFlag)
  {
   CurrentBmpSequenceIndex=0;
  }
  else
  {
   --CurrentBmpSequenceIndex;
   Stop();
  }
 }

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
}

// ��������� ��������� �������� �� ������ � ������� ����
bool TVideoCaptureThreadBmpSequence::LoadImageFromSequence(int index, RDK::UBitmap &bmp)
{
 if(LastReadSequenceIndex == index)
  return true;

 if(BmpSequenceNames[index].find(".bmp") != std::string::npos)
 {
  TempBitmap->LoadFromFile((PathName+BmpSequenceNames[index]).c_str());
  bmp<<TempBitmap;
//  RDK::LoadBitmapFromFile((PathName+BmpSequenceNames[index]).c_str(),bmp);
 }
 else
 if(BmpSequenceNames[index].find(".jpg") != std::string::npos || BmpSequenceNames[index].find(".jpeg") != std::string::npos)
 {
  TJPEGImage* JpegIm=new TJPEGImage;
  JpegIm->LoadFromFile((PathName+BmpSequenceNames[index]).c_str());
  TempBitmap->Assign(JpegIm);
  TempBitmap->PixelFormat=pf24bit;
  bmp<<TempBitmap;
  delete JpegIm;
 }
 else
  bmp.Fill(0);
 LastReadSequenceIndex = index;
 return true;
}


// ������ ��������� ����� � �����������
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
// ������������ � �����������
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
// ���������� �����������
// --------------------------
/// ��� ����� �����������
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


/// ���������� ����� ����������� � ������������������
long long TVideoCaptureThreadHttpServer::GetNumBitmaps(void) const
{
 return 0;
}

/// ������������� ������� ������� � ������������������
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
// ���������� �������
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
// ������������ � �����������
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
// ���������� �������
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
//   LastTimeStamp=FrameTime/10000;
//   *WriteSource<<Frame_Bitmap;
   WriteSourceSafe(Frame_Bitmap, FrameTime/10000, false);
  }
  else
  {
//   LastTimeStamp=FrameTime/10000;
   ConvertBitmap->Assign(Frame_Bitmap);
   ConvertBitmap->PixelFormat=pf24bit;
//   *WriteSource<<ConvertBitmap;
   WriteSourceSafe(ConvertBitmap, FrameTime/10000, false);
  }

//	 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
//	  return;
//	 ResetEvent(SourceUnlock);
//	 RDK::UBitmap* old_read_source=ReadSource;
//	 ReadSource=WriteSource;
//	 WriteSource=old_read_source;
//	 LastTimeStamp=time_stamp;
//	 SetEvent(SourceUnlock);

  SetEvent(VideoGrabberCompleted);
//  TVideoCaptureThread::AfterCalculate();
 break;
 }
}

void __fastcall TVideoCaptureThreadVideoGrabber::Calculate(void)
{
 if(Terminated)
  return;

 int wait_time=30;
 if(VideoGrabber->FrameRate>0)
  wait_time=1000.0/VideoGrabber->FrameRate;
 if(VideoGrabber->PlayerFrameRate>0)
  wait_time=1000.0/VideoGrabber->PlayerFrameRate;

 Frame->UpdateInterval=wait_time;
 if(WaitForSingleObject(VideoGrabberCompleted, wait_time) == WAIT_TIMEOUT)
 {
  if(WaitForSingleObject(CaptureEnabled,10) != WAIT_TIMEOUT)
  {
   ++LastTimeStamp;
   if(MIsEngineInit(Frame->FrameIndex))
    MEnv_CallSourceController(Frame->FrameIndex);
   TVideoCaptureThread::AfterCalculate();
  }

  ResetEvent(VideoGrabberCompleted);
  return;
 }
 ResetEvent(VideoGrabberCompleted);
}


void __fastcall TVideoCaptureThreadVideoGrabber::BeforeCalculate(void)
{

}

void __fastcall TVideoCaptureThreadVideoGrabber::AfterCalculate(void)
{

}

/// ���������� ����� ����������� � ������������������
long long TVideoCaptureThreadVideoGrabber::GetNumBitmaps(void) const
{
 if(VideoGrabber)
 {
  return VideoGrabber->PlayerDuration/10000000;
 }
 return 0;
}

/// ������������� ������� ������� � ������������������
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
// ������������ � �����������
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
// ���������� �����������
// --------------------------
/// ��� ������ ����� ������
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
// ���������� �������
// --------------------------
// ������ ��������� ����� � �����������
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

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AfterCalculate(void)
{
 if(VideoGrabber->PlayerFramePosition>0 && VideoGrabber->PlayerFramePosition>=VideoGrabber->PlayerFrameCount-100)
 {
  if(RepeatFlag)
   VideoGrabber->PlayerFramePosition=0;
  else
   Stop();
//  Sleep(1);
//  Start();
//  Sleep(1);
 }
}
// --------------------------
//---------------------------------------------------------------------------

// --------------------------
// ������������ � �����������
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
// ���������� �����������
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
// ���������� �������
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
// ������������ � �����������
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
// ���������� �����������
// --------------------------
/// ��� ������
const String& TVideoCaptureThreadVideoGrabberIpCamera::GetUrl(void) const
{
 return Url;
}

/// ��� ������������
const String& TVideoCaptureThreadVideoGrabberIpCamera::GetUserName(void) const
{
 return UserName;
}

/// ������
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
// ���������� �������
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
// ������������ � �����������
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
// ���������� �����������
// --------------------------
/// ������ ������ ����� ������
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

/// ��� ������ ����� ������
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

/// ������ ������ ����� ������
int TVideoCaptureThreadSharedMemory::GetSharedMemoryPipeSize(void) const
{
 return SharedMemoryPipeSize;
}

/// ���������� ����� ����������� � ������������������
long long TVideoCaptureThreadSharedMemory::GetNumBitmaps(void) const
{
 return 0;
}

/// ������������� ������� ������� � ������������������
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
// ���������� �������
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

