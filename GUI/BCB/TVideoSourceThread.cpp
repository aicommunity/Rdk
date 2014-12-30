//---------------------------------------------------------------------------

#pragma hdrstop

#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "TUBitmap.h"
#include "rdk_initdll.h"
//#include "TMultiStartFormUnit.h"
#include <Jpeg.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)

extern String TVGrabberLicenseString;

TVideoCaptureThreadCmdDescr::TVideoCaptureThreadCmdDescr(void)
 : Id(tvcNone), ExecTime(0.0)
{

}

TVideoCaptureThreadCmdDescr::TVideoCaptureThreadCmdDescr(TVideoCaptureThreadCommands id, double exec_time)
 : Id(id), ExecTime(exec_time)
{

}


// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThread::TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended)
: ChannelIndex(0), Frame(frame), TThread(CreateSuspended)
{
 SourceMode=-1;
 LastTimeStamp=0;
 RealLastTimeStamp=0;
 CaptureEnabled=CreateEvent(0,TRUE,0,0);
 SourceUnlock=CreateEvent(0,TRUE,TRUE,0);
 SourceWriteUnlock=CreateEvent(0,TRUE,TRUE,0);
 FrameNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 CalcCompleteEvent=CreateEvent(0,TRUE,TRUE,0);
 SourceStoppedEvent=CreateEvent(0,FALSE,FALSE,0);
 CommandUnlockEvent=CreateEvent(0,TRUE,TRUE,0);
 ReadSource=&Source[0];
 WriteSource=&Source[1];
 RepeatFlag=false;
 RestartMode=0;

 FreeOnTerminate=false;
 ConnectionState=0;
// CommandMutex=new TMutex(false);
// ThreadState=0;
 RestartInterval=200000;
 LastStartTime=0;
 MaxInterstepInterval=30000;
 Priority = tpLower;
 DesiredWidth=640;
 DesiredHeight=480;
 DesiredResolutionFlag=false;
}

__fastcall TVideoCaptureThread::~TVideoCaptureThread(void)
{
 ClearCommandQueue();
/* Stop();
 while(WaitForSingleObject(CaptureEnabled,INFINITE) != WAIT_TIMEOUT)
 {
  Sleep(1);
 }
  */
// delete CommandMutex;
// CommandMutex=0;
 CloseHandle(CaptureEnabled);
 CloseHandle(SourceUnlock);
 CloseHandle(SourceWriteUnlock);
 CloseHandle(FrameNotInProgress);
 CloseHandle(CalcCompleteEvent);
 CloseHandle(SourceStoppedEvent);
 CloseHandle(CommandUnlockEvent);
}
// --------------------------

// --------------------------
// ���������� ���������
// --------------------------
/// ��������� ������� � �������
void TVideoCaptureThread::AddCommand(TVideoCaptureThreadCmdDescr value)
{
 WaitForSingleObject(CommandUnlockEvent,INFINITE);
 ResetEvent(CommandUnlockEvent);
 std::pair<double,TVideoCaptureThreadCmdDescr> cmd(TDateTime::CurrentDateTime().operator double(),value);
 CommandQueue.push_back(cmd);
// CommandQueue[TDateTime::CurrentDateTime().operator double()]=value;
 SetEvent(CommandUnlockEvent);
}

/// ������� �������
void TVideoCaptureThread::ClearCommandQueue(void)
{
 WaitForSingleObject(CommandUnlockEvent,INFINITE);
 ResetEvent(CommandUnlockEvent);
 CommandQueue.clear();
 SetEvent(CommandUnlockEvent);
}

/// ������������ ��������� ��������� ������� �� �������
void TVideoCaptureThread::ProcessCommandQueue(void)
{
 double cmd_time=0;
 TVideoCaptureThreadCmdDescr cmd;
 double curr_time=TDateTime::CurrentDateTime().operator double();
 WaitForSingleObject(CommandUnlockEvent,INFINITE);
 ResetEvent(CommandUnlockEvent);
 std::list<std::pair<double,TVideoCaptureThreadCmdDescr> >::iterator I=CommandQueue.begin();
 for(;I != CommandQueue.end();++I)
 {
  if(I->second.ExecTime<=curr_time)
  {
   cmd_time=I->first;
   cmd=I->second;
   CommandQueue.erase(I);
   break;
  }
 }
 SetEvent(CommandUnlockEvent);

 switch(cmd.Id)
 {
 case tvcNone:
 break;

 case tvcStart:
  RunCapture();
//  SetThreadState(1);
  SetEvent(CaptureEnabled);
 break;

 case tvcStop:
  ResetEvent(CaptureEnabled);
//  SetThreadState(0);
//  ConnectionState=1;
  StopCapture();
 break;

 case tvcTerminate:
 break;

 case tvcRecreate:
  ResetEvent(CaptureEnabled);
//  SetThreadState(0);
  ConnectionState=0;
  RecreateCapture();
 break;

 case tvcHalt:
  HaltCapture();
 break;


 default:
  ;
 }
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

/// ������������� �������� FPS
double TVideoCaptureThread::GetFps(void) const
{
 return 0;
}

bool TVideoCaptureThread::SetFps(double fps)
{
 return false;
}

/// �������� ����� ��������� ������� � ���������, ��
int TVideoCaptureThread::GetRestartInterval(void) const
{
 return RestartInterval;
}

bool TVideoCaptureThread::SetRestartInterval(int value)
{
 RestartInterval=value;
 return true;
}

/// �������� ���������� �������
int TVideoCaptureThread::GetDesiredWidth(void) const
{
 return DesiredWidth;
}

bool TVideoCaptureThread::SetDesiredWidth(int value)
{
 if(DesiredWidth == value)
  return true;
 Stop(0);
 DesiredWidth=value;
 return true;
}

int TVideoCaptureThread::GetDesiredHeight(void) const
{
 return DesiredHeight;
}

bool TVideoCaptureThread::SetDesiredHeight(int value)
{
 if(DesiredHeight == value)
  return true;
 Stop(0);
 DesiredHeight=value;
 return true;
}

/// ���� ��������� ������ ��������� ���������� �������
bool TVideoCaptureThread::GetDesiredResolutionFlag(void) const
{
 return DesiredResolutionFlag;
}

bool TVideoCaptureThread::SetDesiredResolutionFlag(bool value)
{
 if(DesiredResolutionFlag == value)
  return true;

 Stop(0);
 DesiredResolutionFlag=value;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ���� ��������� �����
/// 0 - ����������
/// 1 - �������
/*
int TVideoCaptureThread::GetThreadState(void) const
{
// ThreadStateMutex->Acquire();
// int res=ThreadState;
// ThreadStateMutex->Release();
 return ThreadState;
}     */

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

/// ���������� ���������� ������
int TVideoCaptureThread::GetWidth(void)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return -1;
 ResetEvent(SourceUnlock);
 int res=0;
 if(ReadSource)
  res=ReadSource->GetWidth();
 SetEvent(SourceUnlock);

 return res;
}

int TVideoCaptureThread::GetHeight(void)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return -1;
 ResetEvent(SourceUnlock);
 int res=0;
 if(ReadSource)
  res=ReadSource->GetHeight();
 SetEvent(SourceUnlock);

 return res;
}

/// ���������� �������� � xml
bool TVideoCaptureThread::SaveParameters(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 return SaveParametersEx(xml);
}

bool TVideoCaptureThread::SaveParametersEx(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("SourceMode",SourceMode);
 xml.WriteInteger("SyncMode",SyncMode);
 xml.WriteBool("RepeatFlag",RepeatFlag);

 xml.WriteInteger("DesiredWidth",DesiredWidth);
 xml.WriteInteger("DesiredHeight",DesiredHeight);
 xml.WriteBool("DesiredResolutionFlag",DesiredResolutionFlag);

 if(!ASaveParameters(xml))
  return false;
 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThread::LoadParameters(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");

 return LoadParametersEx(xml);
}

bool TVideoCaptureThread::LoadParametersEx(RDK::USerStorageXML &xml)
{
 SyncMode=xml.ReadInteger("SyncMode",SyncMode);
 RepeatFlag=xml.ReadBool("RepeatFlag",RepeatFlag);
 DesiredWidth=xml.ReadInteger("DesiredWidth",DesiredWidth);
 DesiredHeight=xml.ReadInteger("DesiredHeight",DesiredHeight);
 DesiredResolutionFlag=xml.ReadBool("DesiredResolutionFlag",DesiredResolutionFlag);

 if(!ALoadParameters(xml))
  return false;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ���������� �������� � xml
bool TVideoCaptureThread::ASaveParameters(RDK::USerStorageXML &xml)
{
 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThread::ALoadParameters(RDK::USerStorageXML &xml)
{
 return true;
}
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
void __fastcall TVideoCaptureThread::Start(double time)
{
 AddCommand(TVideoCaptureThreadCmdDescr(tvcRecreate,time));
 AddCommand(TVideoCaptureThreadCmdDescr(tvcStart,time));
 AStart(time);
}

void __fastcall TVideoCaptureThread::Stop(double time)
{
 AStop(time);
 AddCommand(TVideoCaptureThreadCmdDescr(tvcStop,time));
}

void __fastcall TVideoCaptureThread::BeforeCalculate(void)
{
}

void __fastcall TVideoCaptureThread::AfterCalculate(void)
{
 if(!UEngineMonitorForm || !UEngineMonitorForm->EngineMonitorFrame)
  return;
 if(GetNumEngines() == 1)
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(0,LastTimeStamp*86400.0*1000.0);
 else
  if(GetNumEngines() > ChannelIndex)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,LastTimeStamp*86400.0*1000.0);
}



void __fastcall TVideoCaptureThread::Execute(void)
{
 ExecuteCaptureInit();
 while(!Terminated)
 {
  if(WaitForSingleObject(CaptureEnabled,30) == WAIT_TIMEOUT)
  {
   ProcessCommandQueue();
   SetEvent(FrameNotInProgress);
   continue;
  }
  ProcessCommandQueue();

  double curr_time=TDateTime::CurrentDateTime().operator double();
  if(CheckConnection() == 2 && curr_time-RealLastTimeStamp>double(MaxInterstepInterval)/(86400.0*1000.0))
  {
   ConnectionState=10;
//   if(CheckConnection() == 10)
   {
	LastStartTime=TDateTime::CurrentDateTime().operator double();
	SetEvent(FrameNotInProgress);
	Sleep(5);
	continue;
   }
  }

  if(CheckConnection() == 10 && CheckCaptureThreadState() == 1)
  {
  switch(RestartMode)
   {
	case 0:
	{
	 break;
	}

	case 1:
	{
	 if(curr_time-LastStartTime<double(RestartInterval)/(86400.0*1000.0))
	 {
	  Sleep(5);
	  SetEvent(FrameNotInProgress);
	  continue;
	 }
	 AddCommand(TVideoCaptureThreadCmdDescr(tvcHalt,0));
	 AddCommand(TVideoCaptureThreadCmdDescr(tvcRecreate,0));
	 AddCommand(TVideoCaptureThreadCmdDescr(tvcStart,curr_time+(100+Random(3000))/(86400.0*1000.0)));
	 SetEvent(FrameNotInProgress);
	 continue;
 //	 StopCapture();
//	 RunCapture();
	 break;
	}

	case 2:
	{
	 AddCommand(TVideoCaptureThreadCmdDescr(tvcStop,0));
	 SetEvent(FrameNotInProgress);
	 continue;
//	 StopCapture();
	 break;
	}
   };

   Sleep(10);
   SetEvent(FrameNotInProgress);
   continue;
  }

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
 Synchronize(ExecuteCaptureUnInit);
}

void __fastcall TVideoCaptureThread::ExecuteCaptureInit(void)
{

}

void __fastcall TVideoCaptureThread::ExecuteCaptureUnInit(void)
{

}


/// ���������� ����� ����������� � �����������
bool TVideoCaptureThread::ReadSourceSafe(RDK::UBitmap& dest, double &time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30000) == WAIT_TIMEOUT)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::ReadSourceSafe: SourceUnlock timeout!").c_str());
  return false;
 }
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
bool TVideoCaptureThread::WriteSourceSafe(const RDK::UBitmap& src, double time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceWriteUnlock,10000) == WAIT_TIMEOUT)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceWriteUnlock timeout!").c_str());
  return false;
 }

 ResetEvent(SourceWriteUnlock);
 if(reflect)
  const_cast<RDK::UBitmap&>(src).ReflectionX(WriteSource);
 else
  *WriteSource=src;
 SetEvent(SourceWriteUnlock);

 if(WaitForSingleObject(SourceUnlock,30000) == WAIT_TIMEOUT)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceUnlock timeout!").c_str());
  return false;
 }
 ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();
 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}

bool TVideoCaptureThread::WriteSourceSafe(Graphics::TBitmap *src, double time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceWriteUnlock,10000) == WAIT_TIMEOUT)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceWriteUnlock timeout!").c_str());
  return false;
 }

 ResetEvent(SourceWriteUnlock);
 TBitmapToUBitmap(*WriteSource, src, reflect);
 SetEvent(SourceWriteUnlock);

 if(WaitForSingleObject(SourceUnlock,30000) == WAIT_TIMEOUT)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceUnlock timeout!").c_str());
  return false;
 }
 ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();
 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 SetEvent(SourceUnlock);
 return true;
}

// ������ ��������� ����� � �����������
bool TVideoCaptureThread::SetLastTimeStampSafe(double time_stamp)
{
 if(WaitForSingleObject(SourceUnlock,30000) == WAIT_TIMEOUT)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::SetLastTimeStampSafe: SourceUnlock timeout!").c_str());
  return false;
 }
 ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();

 SetEvent(SourceUnlock);
 return true;
}

// ��������� ��������� ����� � �����������
double TVideoCaptureThread::GetLastTimeStampSafe(void) const
{
 if(WaitForSingleObject(SourceUnlock,30000) == WAIT_TIMEOUT)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::GetLastTimeStampSafe: SourceUnlock timeout!").c_str());
  return 0.0;
 }
 ResetEvent(SourceUnlock);

 double res=LastTimeStamp;
 SetEvent(SourceUnlock);
 return res;
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
bool __fastcall TVideoCaptureThread::RunCapture(void)
{
 double curr_time=TDateTime::CurrentDateTime().operator double();
 LastStartTime=curr_time;
 RealLastTimeStamp=curr_time;
 ConnectionState=10;
 Synchronize(ARunCapture);
// ARunCapture();
 Sleep(100);
 return true;
}

bool __fastcall TVideoCaptureThread::StopCapture(void)
{
 LastStartTime=0;
 HaltCapture();
 ConnectionState=1;
 Sleep(100);
 return true;
}

/// ������������� ����������� ������ �� ����� ������� �����
bool __fastcall TVideoCaptureThread::HaltCapture(void)
{
 Synchronize(AStopCapture);
// AStopCapture();
 return true;
}


bool __fastcall TVideoCaptureThread::RecreateCapture(void)
{
// StopCapture();

 RDK::USerStorageXML xml;
 SaveParameters(xml);
 Synchronize(ARecreateCapture);
// ARecreateCapture();
 LoadParameters(xml);
// ConnectionState=1;
 Sleep(100);
 return true;
}

void __fastcall TVideoCaptureThread::ARecreateCapture(void)
{

}

   /*
bool TVideoCaptureThread::SetThreadState(int value)
{
 ThreadState=value;
 return true;
}            */
// --------------------------

//---------------------------------------------------------------------------
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadBmp::TVideoCaptureThreadBmp(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThread(frame, CreateSuspended)
{
 TempBitmap=new Graphics::TBitmap;
 Fps=25.0;
 CurrentTimeStamp=0;
 SourceMode=0;
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
std::string TVideoCaptureThreadBmp::GetFileName(void) const
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
 catch (EFOpenError &exception)
 {
  TempSource.SetRes(0,0);
 }
 TempSource.SetColorModel(RDK::ubmRGB24);
 double time_stamp=0;
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


/// ������������� �������� FPS
double TVideoCaptureThreadBmp::GetFps(void) const
{
 return Fps;
}

bool TVideoCaptureThreadBmp::SetFps(double fps)
{
 if(Fps == fps)
  return true;

 Fps=fps;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadBmp::AStart(double time)
{
 CurrentTimeStamp=0;
}

void __fastcall TVideoCaptureThreadBmp::AStop(double time)
{
}


void __fastcall TVideoCaptureThreadBmp::BeforeCalculate(void)
{

}

void __fastcall TVideoCaptureThreadBmp::AfterCalculate(void)
{
 if(Fps>0)
 {
  CurrentTimeStamp+=(1.0/Fps)/86400.0;
 }
 else
 {
  CurrentTimeStamp+=1.0/86400.0;
 }
 TVideoCaptureThread::AfterCalculate();
/* if(Fps>0)
  Sleep(1000.0/Fps);
 else*/
  Sleep(30);
}

void __fastcall TVideoCaptureThreadBmp::Calculate(void)
{
 double time_stamp=CurrentTimeStamp;//TDateTime::CurrentDateTime().operator double();
 SetLastTimeStampSafe(time_stamp);
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadBmp::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadBmp(frame,create_suspended);
}

/// ���������� �������� � xml
bool TVideoCaptureThreadBmp::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("FileName",FileName);
 xml.WriteFloat("Fps",Fps);

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadBmp::ALoadParameters(RDK::USerStorageXML &xml)
{
 SetFileName(xml.ReadString("FileName",FileName));
 SetFps(xml.ReadFloat("Fps",Fps));

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadBmp::ARunCapture(void)
{
}

void __fastcall TVideoCaptureThreadBmp::AStopCapture(void)
{
}

void __fastcall TVideoCaptureThreadBmp::ARecreateCapture(void)
{

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
 Fps=25.0;
 SourceMode=4;
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
std::string TVideoCaptureThreadBmpSequence::GetPathName(void) const
{
 return PathName;
}

bool TVideoCaptureThreadBmpSequence::SetPathName(const std::string& value)
{
 if(!value.empty() && (value[value.size()-1] != '\\' && value[value.size()-1] != '/'))
  PathName=value+"\\";
 else
  PathName=value;

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
 CurrentTimeStamp=0;
 WriteSourceSafe(TempSource,CurrentTimeStamp,false);

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
 long long diff=index-CurrentBmpSequenceIndex;
 CurrentBmpSequenceIndex=index;
 CurrentTimeStamp+=(double(diff)/Fps)/86400.0;
 SetLastTimeStampSafe(CurrentTimeStamp);
 return true;
}

/// ������������� �������� FPS
double TVideoCaptureThreadBmpSequence::GetFps(void) const
{
 return Fps;
}

bool TVideoCaptureThreadBmpSequence::SetFps(double fps)
{
 if(Fps == fps)
  return true;

 Fps=fps;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadBmpSequence::AStart(double time)
{
 CurrentTimeStamp=0;
 SetSyncMode(1);
// TVideoCaptureThread::Start();
}

void __fastcall TVideoCaptureThreadBmpSequence::AStop(double time)
{
// TVideoCaptureThread::Stop();
}

void __fastcall TVideoCaptureThreadBmpSequence::AfterCalculate(void)
{
 TVideoCaptureThread::AfterCalculate();
 CurrentBmpSequenceIndex++;
 if(Fps>0)
  CurrentTimeStamp+=(1.0/Fps)/86400.0;
 else
  CurrentTimeStamp+=1.0/86400.0;
 if(CurrentBmpSequenceIndex>=int(BmpSequenceNames.size()))
 {
//  MultiStartForm->MultiStartFrame1->NextSource();
  if(RepeatFlag)
  {
   CurrentBmpSequenceIndex=0;
  }
  else
  {
   --CurrentBmpSequenceIndex;
   Stop(0);
  }
 }

// Sleep(30);
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
// long long time_stamp=CurrentBmpSequenceIndex;
 WriteSourceSafe(TempSource,CurrentTimeStamp,false);
}

// ��������� ��������� �������� �� ������ � ������� ����
bool TVideoCaptureThreadBmpSequence::LoadImageFromSequence(int index, RDK::UBitmap &bmp)
{
 if(LastReadSequenceIndex == index)
  return true;

 if(BmpSequenceNames[index].find(".bmp") != std::string::npos)
 {
  TempBitmap->LoadFromFile((PathName.Get()+BmpSequenceNames[index]).c_str());
  bmp<<TempBitmap;
//  RDK::LoadBitmapFromFile((PathName+BmpSequenceNames[index]).c_str(),bmp);
 }
 else
 if(BmpSequenceNames[index].find(".jpg") != std::string::npos || BmpSequenceNames[index].find(".jpeg") != std::string::npos)
 {
  TJPEGImage* JpegIm=new TJPEGImage;
  JpegIm->LoadFromFile((PathName.Get()+BmpSequenceNames[index]).c_str());
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
bool TVideoCaptureThreadBmpSequence::SetLastTimeStampSafe(double time_stamp)
{
 TVideoCaptureThread::SetLastTimeStampSafe(time_stamp);
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;
// CurrentBmpSequenceIndex=time_stamp;
 return true;
}

/// ���������� 0 ���� ���� ��������� �� ����������
/// ���������� 1 ���� ���� ��� ����������� � ���������
/// ���������� 2 ���� ���� ���� ����������� � ���������
int TVideoCaptureThread::CheckConnection(void) const
{
 return 2;
}

/// ���������� 0 ���� ������ �� �������� � 1 ���� �������
int TVideoCaptureThread::CheckCaptureThreadState(void) const
{
 if(WaitForSingleObject(CaptureEnabled, 0) != WAIT_TIMEOUT)
  return 1;

 return 0;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadBmpSequence::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadBmpSequence(frame,create_suspended);
}

/// ���������� �������� � xml
bool TVideoCaptureThreadBmpSequence::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("PathName",PathName);
 xml.WriteFloat("Fps",Fps);

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadBmpSequence::ALoadParameters(RDK::USerStorageXML &xml)
{
 SetPathName(xml.ReadString("PathName",PathName));
 SetFps(xml.ReadFloat("Fps",Fps));

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadBmpSequence::ARunCapture(void)
{
}

void __fastcall TVideoCaptureThreadBmpSequence::AStopCapture(void)
{
}

void __fastcall TVideoCaptureThreadBmpSequence::ARecreateCapture(void)
{

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
 SourceMode=5;
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
void __fastcall TVideoCaptureThreadHttpServer::AStart(double time)
{
// TVideoCaptureThread::Start();
}

void __fastcall TVideoCaptureThreadHttpServer::AStop(double time)
{
// TVideoCaptureThread::Stop();
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
 double l_time_stamp=0;
 if(!time_stamp.empty())
 {
  std::string temp_stamp;
  temp_stamp.assign(&time_stamp[0],time_stamp.size());
  l_time_stamp=RDK::atof(temp_stamp);
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

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadHttpServer::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadHttpServer(frame,create_suspended);
}

/// ���������� �������� � xml
bool TVideoCaptureThreadHttpServer::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("ListenPort",ListenPort);

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadHttpServer::ALoadParameters(RDK::USerStorageXML &xml)
{
 SetListenPort(xml.ReadInteger("ListenPort",ListenPort));
 return true;
}
// --------------------------


// --------------------------
// ������� ������ ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadHttpServer::ARunCapture(void)
{
 UHttpServerFrame->IdHTTPServer->Active=true;
}

void __fastcall TVideoCaptureThreadHttpServer::AStopCapture(void)
{
 UHttpServerFrame->IdHTTPServer->Active=false;
}

void __fastcall TVideoCaptureThreadHttpServer::ARecreateCapture(void)
{

}
// --------------------------

//---------------------------------------------------------------------------

// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber::TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended)
 : VideoGrabber(0), TVideoCaptureThread(frame,CreateSuspended)
{
 ConvertBitmap=new Graphics::TBitmap;

 VideoGrabberCompleted=CreateEvent(0,TRUE,0,0);

 RestartMode=1;
 ConnectionState=0;
}

__fastcall TVideoCaptureThreadVideoGrabber::~TVideoCaptureThreadVideoGrabber(void)
{
 if(!Terminated && !Suspended)
 {
  Terminate();
  WaitFor();
  WaitForSingleObject(GetFrameNotInProgress(),10000);
//  delete VideoGrabber;
//  VideoGrabber=0;
 }

 if(ConvertBitmap)
 {
  delete ConvertBitmap;
  ConvertBitmap=0;
 }

 CloseHandle(VideoGrabberCompleted);
}
// --------------------------
// ���������� �����������
// --------------------------
/// ������������� �������� FPS
double TVideoCaptureThreadVideoGrabber::GetFps(void) const
{
 return Fps;
}

bool TVideoCaptureThreadVideoGrabber::SetFps(double fps)
{
 Fps=fps;
 return true;
}
// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabber::ExecuteCaptureInit(void)
{
 VideoGrabber=new TVideoGrabber(GetFrame());//(TComponent*) NULL);
// VideoGrabber->Parent=GetFrame();
 VideoGrabber->OnFrameCaptureCompleted=OnFrameCaptureCompleted;
// VideoGrabber->OnFrameBitmap=VideoGrabberFrameBitmap;
 VideoGrabber->OnLog=VideoGrabberLog;
 VideoGrabber->OnDeviceLost=VideoGrabberDeviceLost;
 VideoGrabber->OnPlayerEndOfStream = VideoGrabberPlayerEndOfStream;
 VideoGrabber->OnPlayerOpened=VideoGrabberOnPlayerOpened;
//  VideoGrabber->OnThreadSync=VideoGrabberOnThreadSync;

 VideoGrabber->Display_AutoSize = false;
 VideoGrabber->PlayerRefreshPausedDisplay = false;
 VideoGrabber->AutoStartPlayer = false;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->BurstInterval = 0;
 VideoGrabber->BurstMode = true;
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->Synchronized=false;
 VideoGrabber->SetIPCameraSetting(ips_ConnectionTimeout, 5000);
 VideoGrabber->SetIPCameraSetting(ips_ReceiveTimeout, 5000);
 VideoGrabber->FrameGrabberRGBFormat=fgf_RGB24;
 VideoGrabber->LicenseString=TVGrabberLicenseString;
 if(DesiredResolutionFlag)
 {
  VideoGrabber->FrameCaptureWidth=DesiredWidth;
  VideoGrabber->FrameCaptureHeight=DesiredHeight;
 }
 else
 {
  VideoGrabber->FrameCaptureWidth=-1;
  VideoGrabber->FrameCaptureHeight=-1;
 }

// VideoGrabber->OpenURLAsync=false;

// VideoGrabber->EnableThreadMode();
}

void __fastcall TVideoCaptureThreadVideoGrabber::ExecuteCaptureUnInit(void)
{
 if(VideoGrabber)
 {
  VideoGrabber->Parent=0;
  delete VideoGrabber;
  VideoGrabber=0;
 }
}


TVideoGrabber* TVideoCaptureThreadVideoGrabber::GetVideoGrabber(void)
{
 return VideoGrabber;
}

void __fastcall TVideoCaptureThreadVideoGrabber::OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId)
{
 if(CheckCaptureThreadState())
 {
  ConnectionState=2;
  RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();
 }
	  /*
 if(Fps > 0)
 {
  double diffTime=double(FrameTime)/(10000000.0*86400)-GetLastTimeStampSafe();
  if(diffTime<(1.0/Fps)/86400.0)
  {
	return;
  }
 }   */

 Graphics::TBitmap *Frame_Bitmap;

 Frame_Bitmap = (Graphics::TBitmap*) FrameBitmap;
// if(Frame_Bitmap)
// Engine_LogMessage(exception.GetType(), (std::string("Core-OpenProject Exception: (Name=")+std::string(AnsiString(Name).c_str())+std::string(") ")+exception.CreateLogMessage()).c_str());

 ConvertUBitmap<<Frame_Bitmap;
 ConvertTimeStamp=double(FrameTime);
// SetLastTimeStampSafe(double(FrameTime)/(10000000.0*86400));

/*
 switch (DestType)
 {
 case fc_TBitmap:
  if(Frame_Bitmap->PixelFormat == pf24bit)
  {
   WriteSourceSafe(Frame_Bitmap, double(FrameTime)/(10000000.0*86400), false);
  }
  else
  {
   ConvertBitmap->Assign(Frame_Bitmap);
   ConvertBitmap->PixelFormat=pf24bit;
   WriteSourceSafe(ConvertBitmap, double(FrameTime)/(10000000.0*86400), false);
  }   */
//  if(GetNumEngines() > ChannelIndex)
//   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,GetLastTimeStampSafe()*86400.0*1000.0);

  SetEvent(VideoGrabberCompleted);
// break;
// }
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberPlayerEndOfStream(TObject *Sender)
{
	//ShowMessage("Stream ended");
	/*if(restarter)
	{
        restarter->RestartSource();
	} */
	PulseEvent(SourceStoppedEvent);
	if(RepeatFlag)
	{
	   /*	VideoGrabber->PlayerFramePosition=1;
		VideoGrabber->StartPreview() */
		VideoGrabber->RunPlayer();
		SetLastTimeStampSafe(0);
	}
	else
	{
     MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("VideoGrabber stopped by end of frames").c_str());
	 Stop(0);
	}
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberOnPlayerOpened(System::TObject* Sender)
{
 MEngine_LogMessage(ChannelIndex, RDK_EX_DEBUG, std::string("VideoGrabber player opened").c_str());
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberOnThreadSync(System::TObject* Sender, TThreadSyncPoint ThreadSyncPoint)
{

}


void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberFrameBitmap(TObject *Sender,
	  pFrameInfo FrameInfo, pFrameBitmapInfo BitmapInfo)
{
 if(CheckCaptureThreadState())
 {
  ConnectionState=2;
  RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();
 }

 ConvertTimeStamp=FrameInfo->FrameTime;

 unsigned int BitmapLinePtr = (unsigned int) BitmapInfo->BitmapDataPtr;
 int bmp_width=BitmapInfo->BitmapWidth;
 int bmp_height=BitmapInfo->BitmapHeight;

 if (BitmapInfo->BitmapBitsPerPixel == 24)
 {   // case where FrameGrabberRGBFormat is set to fgf_RGB24 (you can select it in the "frame grabber" tab)
  ConvertUBitmap.SetRes(bmp_width, bmp_height, RDK::ubmRGB24);
 }
 else
 if (BitmapInfo->BitmapBitsPerPixel == 32)
 {
  ConvertUBitmap.SetRes(bmp_width, bmp_height, RDK::ubmRGB32);
 }

 int conv_line_bl=ConvertUBitmap.GetLineByteLength();
  unsigned char *p=ConvertUBitmap.GetData();
  for (int i = 0 ; i < bmp_height ; i++)
  {
   memcpy(p,(void*)BitmapLinePtr,conv_line_bl);
   p+=conv_line_bl;
   BitmapLinePtr += BitmapInfo->BitmapLineSize;
  }
/*
 if (BitmapInfo->BitmapBitsPerPixel == 24)
 {   // case where FrameGrabberRGBFormat is set to fgf_RGB24 (you can select it in the "frame grabber" tab)
  unsigned char *p=ConvertUBitmap.GetData();
  for (int i = 0 ; i < bmp_height ; i++)
  {
//   TRGBTriple *RGB24Line = (TRGBTriple*) BitmapLinePtr;
   memcpy(p,(void*)BitmapLinePtr,conv_line_bl);
   p+=conv_line_bl;
   BitmapLinePtr += BitmapInfo->BitmapLineSize;
  }
 }
 else
 if (BitmapInfo->BitmapBitsPerPixel == 32)
 {   // case where FrameGrabberRGBFormat is set to fgf_RGB32 (default setting) (you can select it in the "frame grabber" tab)
  unsigned char *p=ConvertUBitmap.GetData();
  for (int i = 0 ; i < bmp_height ; i++)
  {
   TRGBQuad *RGB32Line = (TRGBQuad*) BitmapLinePtr;
   for (int iCol = 0 ; iCol < bmp_width ; iCol ++, RGB32Line++)
   {
	*p++=RGB32Line->rgbBlue;
	*p++=RGB32Line->rgbGreen;
	*p++=RGB32Line->rgbRed;
   }
   BitmapLinePtr += BitmapInfo->BitmapLineSize;
  }
 }  */

  SetEvent(VideoGrabberCompleted);
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberLog(TObject *Sender,
	  TLogType LogType, String Severity, String InfoMsg)
{
 MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, (std::string("VideoGrabber [")+std::string(AnsiString(Severity).c_str())+std::string("] ")+AnsiString(InfoMsg).c_str() ).c_str());
 if(Severity == "ERROR")
 {
  LastStartTime=TDateTime::CurrentDateTime().operator double();
  ConnectionState=10;
 }
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberDeviceLost(TObject *Sender)
{
 MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, "VideoGrabber Device lost");
 LastStartTime=TDateTime::CurrentDateTime().operator double();
 ConnectionState=10;
/* if(RestartMode == 1)
 {
  Start();
 }
 else
 if(RestartMode == 2)
 {
  Stop();
 }*/
}


void __fastcall TVideoCaptureThreadVideoGrabber::Calculate(void)
{
 if(Terminated)
  return;

 if(!VideoGrabber)
  return;

 int wait_time=30;
 double fps=1;
 if(VideoGrabber->FrameRate>0)
 {
  wait_time=1000.0/VideoGrabber->FrameRate;
  fps=VideoGrabber->FrameRate;
 }
 if(VideoGrabber->PlayerFrameRate>0)
 {
  wait_time=1000.0/VideoGrabber->PlayerFrameRate;
  fps=VideoGrabber->PlayerFrameRate;
 }

 GetFrame()->UpdateInterval=wait_time;
 //Sleep(1);
 if(WaitForSingleObject(VideoGrabberCompleted, 10) == WAIT_TIMEOUT)
 {
//  Sleep(10);
  return;
 }
 else
 {
  ResetEvent(VideoGrabberCompleted);
/*
  if(Fps > 0)
  {
   double diffTime=ConvertTimeStamp/double(10000000.0*86400)-GetLastTimeStampSafe();
   if(diffTime<(1.0/Fps)/86400.0)
   {
	return;
   }
  }
*/
  ConvertResult.SetRes(ConvertUBitmap.GetWidth(),ConvertUBitmap.GetHeight(),RDK::ubmRGB24);
  ConvertUBitmap.ConvertTo(ConvertResult);

  bool bmp_res=WriteSourceSafe(ConvertResult, ConvertTimeStamp/double(10000000.0*86400), false);

  if(bmp_res)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,GetLastTimeStampSafe()*86400.0*1000.0);

  Sleep(0);
 }

}


void __fastcall TVideoCaptureThreadVideoGrabber::BeforeCalculate(void)
{

}

void __fastcall TVideoCaptureThreadVideoGrabber::AfterCalculate(void)
{
// Application->HandleMessage();
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

/// ���������� 0 ���� ���� ��������� �� ����������
/// ���������� 1 ���� ���� ��� ����������� � ���������
/// ���������� 2 ���� ���� ���� ����������� � ���������
int TVideoCaptureThreadVideoGrabber::CheckConnection(void) const
{
 return ConnectionState;
}

void __fastcall TVideoCaptureThreadVideoGrabber::ARecreateCapture(void)
{
// return;
 if(VideoGrabber)
 {
//  VideoGrabber->StopPreview();
//  VideoGrabber->StopPlayer();
  delete VideoGrabber;
 }
 ExecuteCaptureInit();
 MEngine_LogMessage(ChannelIndex, RDK_EX_DEBUG, (std::string("TVideoCaptureThreadVideoGrabberIpCamera::ARecreateCapture ")).c_str());
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ���������� �������� � xml
bool TVideoCaptureThreadVideoGrabber::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThread::ASaveParameters(xml))
  return false;

 xml.WriteString("Fps", AnsiString(FloatToStr(Fps)).c_str());

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadVideoGrabber::ALoadParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThread::ALoadParameters(xml))
  return false;

 Fps=StrToFloatDef(xml.ReadString("Fps", "").c_str(), 0.0);

 return true;
}
// --------------------------

//---------------------------------------------------------------------------
// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberAvi::TVideoCaptureThreadVideoGrabberAvi(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThreadVideoGrabber(frame, CreateSuspended)
{
 SourceMode=1;
}

__fastcall TVideoCaptureThreadVideoGrabberAvi::~TVideoCaptureThreadVideoGrabberAvi(void)
{

}
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ��� ������ ����� ������
std::string TVideoCaptureThreadVideoGrabberAvi::GetFileName(void) const
{
 return FileName;
}

bool TVideoCaptureThreadVideoGrabberAvi::SetFileName(const std::string& value)
{
 if(!VideoGrabber)
 {
  FileName=value;
  return true;
 }
 if(CheckConnection() == 2)
  Stop(0);
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 FileName=value;
 VideoGrabber->ClosePlayer();
 VideoGrabber->PlayerFileName=FileName.Get().c_str();
 VideoGrabber->FrameGrabberRGBFormat=fgf_RGB24;
 VideoGrabber->OpenPlayer();
 return true;
}

bool TVideoCaptureThreadVideoGrabberAvi::GetProcessAllFramesFlag(void) const
{
 return ProcessAllFramesFlag;
}

bool TVideoCaptureThreadVideoGrabberAvi::SetProcessAllFramesFlag(bool value)
{
 ProcessAllFramesFlag=value;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberAvi::ExecuteCaptureInit(void)
{
 TVideoCaptureThreadVideoGrabber::ExecuteCaptureInit();
 if(VideoGrabber)
  VideoGrabber->VideoSource=vs_VideoFileOrURL;
}

// ������ ��������� ����� � �����������
bool TVideoCaptureThreadVideoGrabberAvi::SetLastTimeStampSafe(double time_stamp)
{
 TVideoCaptureThread::SetLastTimeStampSafe(time_stamp);
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;
 if(VideoGrabber)
  VideoGrabber->PlayerTimePosition=time_stamp*10000000.0*86400.0;
 return true;
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AStart(double time)
{
// TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AStop(double time)
{
// TVideoCaptureThreadVideoGrabber::Stop();
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AfterCalculate(void)
{
 if(!VideoGrabber)
  return;

 if(GetProcessAllFramesFlag())
 {
  VideoGrabber->BurstCount=1;
  if(WaitForSingleObject(CaptureEnabled,0) != WAIT_TIMEOUT)
	 VideoGrabber->RunPlayer();
 }
 else
 {
  VideoGrabber->BurstCount=0;
 }


 /*if(VideoGrabber->PlayerFramePosition>0 && VideoGrabber->PlayerFramePosition>=VideoGrabber->PlayerFrameCount-100)
 {
  if(RepeatFlag)
   VideoGrabber->PlayerFramePosition=0;
  else
   Stop();
 } */
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadVideoGrabberAvi::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadVideoGrabberAvi(frame,create_suspended);
}

/// ���������� �������� � xml
bool TVideoCaptureThreadVideoGrabberAvi::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ASaveParameters(xml))
  return false;

 xml.WriteString("FileName", FileName);
 xml.WriteBool("ProcessAllFramesFlag",ProcessAllFramesFlag);

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadVideoGrabberAvi::ALoadParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ALoadParameters(xml))
  return false;

 SetFileName(xml.ReadString("FileName", FileName));
 SetProcessAllFramesFlag(xml.ReadBool("ProcessAllFramesFlag",ProcessAllFramesFlag));

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
bool __fastcall TVideoCaptureThreadVideoGrabberAvi::RecreateCapture(void)
{
 return true;
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::ARunCapture(void)
{
 if(VideoGrabber)
 {
  if(VideoGrabber->IsPlayerVideoStreamAvailable == ts_Undefined)
  {
   VideoGrabber->ClosePlayer();
   VideoGrabber->PlayerFileName=FileName.Get().c_str();
   VideoGrabber->FrameGrabberRGBFormat=fgf_RGB24;
   VideoGrabber->OpenPlayer();
  }
  VideoGrabber->RunPlayer();
  VideoGrabber->FrameGrabberRGBFormat=fgf_RGB24;
 }
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AStopCapture(void)
{
 if(VideoGrabber)
  VideoGrabber->PausePlayer();
}
// --------------------------

//---------------------------------------------------------------------------

// --------------------------
// ������������ � �����������
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberCamera::TVideoCaptureThreadVideoGrabberCamera(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThreadVideoGrabber(frame, CreateSuspended)
{
 SourceMode=2;
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
 if(!VideoGrabber)
 {
  return true;
 }

 if(CheckConnection() == 2)
  Stop(0);
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
void __fastcall TVideoCaptureThreadVideoGrabberCamera::ExecuteCaptureInit(void)
{
 TVideoCaptureThreadVideoGrabber::ExecuteCaptureInit();
 if(VideoGrabber)
  VideoGrabber->VideoSource=vs_VideoCaptureDevice;
}

void __fastcall TVideoCaptureThreadVideoGrabberCamera::AStart(double time)
{
// TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberCamera::AStop(double time)
{
// TVideoCaptureThreadVideoGrabber::Stop();
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadVideoGrabberCamera::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadVideoGrabberCamera(frame,create_suspended);
}

/// ���������� �������� � xml
bool TVideoCaptureThreadVideoGrabberCamera::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ASaveParameters(xml))
  return false;

 xml.WriteInteger("CameraIndex",CameraIndex);
 xml.WriteInteger("InputIndex",InputIndex);
 xml.WriteInteger("SizeIndex",SizeIndex);
 xml.WriteInteger("SubtypeIndex",SubtypeIndex);
 xml.WriteInteger("AnalogIndex",AnalogIndex);

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadVideoGrabberCamera::ALoadParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ALoadParameters(xml))
  return false;

 CameraIndex=xml.ReadInteger("CameraIndex",CameraIndex);
 InputIndex=xml.ReadInteger("InputIndex",InputIndex);
 SizeIndex=xml.ReadInteger("SizeIndex",SizeIndex);
 SubtypeIndex=xml.ReadInteger("SubtypeIndex",SubtypeIndex);
 AnalogIndex=xml.ReadInteger("AnalogIndex",AnalogIndex);

 Init(CameraIndex, InputIndex, SizeIndex, SubtypeIndex, AnalogIndex);

 return true;
}
// --------------------------


// --------------------------
// ������� ������ ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberCamera::ARunCapture(void)
{
 if(VideoGrabber)
 {
  VideoGrabber->StartPreview();
//  VideoGrabber->StartSynchronized();
 }
}

void __fastcall TVideoCaptureThreadVideoGrabberCamera::AStopCapture(void)
{
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
 SourceMode=3;
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
String TVideoCaptureThreadVideoGrabberIpCamera::GetUrl(void) const
{
 return Url;
}

/// ��� ������������
String TVideoCaptureThreadVideoGrabberIpCamera::GetUserName(void) const
{
 return UserName;
}

/// ������
String TVideoCaptureThreadVideoGrabberIpCamera::GetPassword(void) const
{
 return Password;
}


bool TVideoCaptureThreadVideoGrabberIpCamera::Init(const String camera_url, const String user_name, const String user_password)
{
 if(!VideoGrabber)
 {
  Url=camera_url;
  UserName=user_name;
  Password=user_password;

  return true;
 }

 if(CheckConnection() == 2)
  Stop(0);
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
 VideoGrabber->FrameGrabberRGBFormat=fgf_RGB24;
// VideoGrabber->OpenURLAsync=false;

 VideoGrabber->SetAuthentication(at_IPCamera,user_name,user_password);
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::AStart(double time)
{
// TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::AStop(double time)
{
// TVideoCaptureThreadVideoGrabber::Stop();
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadVideoGrabberIpCamera::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadVideoGrabberIpCamera(frame,create_suspended);
}

/// ���������� �������� � xml
bool TVideoCaptureThreadVideoGrabberIpCamera::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ASaveParameters(xml))
  return false;

 xml.WriteString("Url",AnsiString(Url).c_str());
 xml.WriteString("UserName", AnsiString(UserName).c_str());
 xml.WriteString("Password", AnsiString(Password).c_str());

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadVideoGrabberIpCamera::ALoadParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ALoadParameters(xml))
  return false;

 Init(xml.ReadString("Url",AnsiString(Url).c_str()).c_str(),
	  xml.ReadString("UserName", AnsiString(UserName).c_str()).c_str(),
	  xml.ReadString("Password", AnsiString(Password).c_str()).c_str());

 return true;
}
// --------------------------


// --------------------------
// ������� ������ ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::ARunCapture(void)
{
 if(VideoGrabber)
 {
  VideoGrabber->StartPreview();
//  VideoGrabber->StartSynchronized();
 }
 MEngine_LogMessage(ChannelIndex, RDK_EX_DEBUG, (std::string("TVideoCaptureThreadVideoGrabberIpCamera::ARunCapture ")+AnsiString(Url).c_str()).c_str());
}

void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::AStopCapture(void)
{
 if(VideoGrabber)
  VideoGrabber->PausePreview();

 MEngine_LogMessage(ChannelIndex, RDK_EX_DEBUG, (std::string("TVideoCaptureThreadVideoGrabberIpCamera::AStopCapture ")+AnsiString(Url).c_str()).c_str());
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

 if(!Usm_GetNumPipes || !Usm_SetNumPipes)
  return false;

 if(Usm_GetNumPipes() <= value)
  Usm_SetNumPipes(value+1);

 PipeIndex=value;

 return true;
}

/// ��� ������ ����� ������
std::string TVideoCaptureThreadSharedMemory::GetPipeName(void) const
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
 return GetLastTimeStampSafe()*86400.0*1000.0;
}

bool TVideoCaptureThreadSharedMemory::SetPosition(long long index)
{
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadSharedMemory::AStart(double time)
{

}

void __fastcall TVideoCaptureThreadSharedMemory::AStop(double time)
{

}

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

	 if(GetLastTimeStampSafe() == time_stamp/(86400.0*1000.0))
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

	 RDK::UBitmap temp_bmp;

//	 WriteSource->SetRes(width,height,(RDK::UBMColorModel)color_model);
	 if(shift<SharedMemoryPipeSize)
	 {
//	  int image_size=WriteSource->GetByteLength();
//	  if(image_size>SharedMemoryPipeSize-20)
//	   image_size=SharedMemoryPipeSize-20;
	  temp_bmp.AttachBuffer(width,height,(unsigned char*)&PipeBuffer[shift],(RDK::UBMColorModel)color_model);
	  WriteSourceSafe(temp_bmp,time_stamp/(86400.0*1000.0),false);
	  temp_bmp.DetachBuffer();
	 }

	}
   }
  }

}

void __fastcall TVideoCaptureThreadSharedMemory::UnsafeInit(void)
{
 if(!Usm_GetNumPipes || Usm_GetNumPipes() <= PipeIndex)
  return;

 int pipe_byte_size=0;
 Usm_InitPipe(PipeIndex.Get(),pipe_byte_size,0,PipeName.Get().c_str());
 SharedMemoryPipeSize=pipe_byte_size;
}

// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ������� ����� ����� ������
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadSharedMemory::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadSharedMemory(frame,create_suspended);
}

/// ���������� �������� � xml
bool TVideoCaptureThreadSharedMemory::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("PipeIndex",PipeIndex);
 xml.WriteString("PipeName",PipeName);
 xml.WriteInteger("SharedMemoryPipeSize",SharedMemoryPipeSize);

 return true;
}

/// �������� � ���������� �������� �� xml
bool TVideoCaptureThreadSharedMemory::ALoadParameters(RDK::USerStorageXML &xml)
{
 PipeIndex=xml.ReadInteger("PipeIndex",PipeIndex);
 PipeName=xml.ReadString("PipeName",PipeName);
 SharedMemoryPipeSize=xml.ReadInteger("SharedMemoryPipeSize",SharedMemoryPipeSize);

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� �������
// --------------------------
void __fastcall TVideoCaptureThreadSharedMemory::ARunCapture(void)
{
}

void __fastcall TVideoCaptureThreadSharedMemory::AStopCapture(void)
{
}
// --------------------------
