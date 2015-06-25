//---------------------------------------------------------------------------

#pragma hdrstop

#include "TVideoSourceThread.h"
#include "VideoOutputFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "TUBitmap.h"
#include "TVideoGrabberFrame.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#include "TGrabberThread.h"
#include "TImageFrame.h"
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


HANDLE TVideoCaptureThread::GlobalStartUnlockMutex=NULL;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThread::TVideoCaptureThread(TVideoOutputFrame *frame, bool CreateSuspended)
: ChannelIndex(0), Frame(frame), TThread(CreateSuspended)
{
 SourceMode=-1;
 LastTimeStamp=0;
 RealLastTimeStamp=0;
 CaptureEnabled=CreateEvent(0,TRUE,0,0);
 SourceUnlock=CreateMutex(0,FALSE,0);//CreateEvent(0,TRUE,TRUE,0);
 SourceWriteUnlock=CreateMutex(0,FALSE,0);//CreateEvent(0,TRUE,TRUE,0);
 FrameNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 CalcCompleteEvent=CreateEvent(0,TRUE,TRUE,0);
 SourceStoppedEvent=CreateEvent(0,FALSE,FALSE,0);
 CommandUnlockMutex=CreateMutex(0,FALSE,0);//CreateEvent(0,TRUE,TRUE,0);
 StartInProgressEvent=CreateEvent(0,TRUE,0,0);
 ReadSource=&Source[0];
 WriteSource=&Source[1];
 RepeatFlag=false;
 RestartMode=0;

 FreeOnTerminate=false;
 ConnectionState=0;
// CommandMutex=new TMutex(false);
// ThreadState=0;
 RestartInterval=20000;
 LastStartTime=0;
 MaxInterstepInterval=30000;
 Priority = tpLower;
 DesiredWidth=640;
 DesiredHeight=480;
 DesiredResolutionFlag=false;

 if(!GlobalStartUnlockMutex)
  TVideoCaptureThread::GlobalStartUnlockMutex=CreateMutex(0,FALSE,0);
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
 CloseHandle(CommandUnlockMutex);
 CloseHandle(StartInProgressEvent);
}
// --------------------------

// --------------------------
// Управление командами
// --------------------------
/// Добавляет команду в очередь
void TVideoCaptureThread::AddCommand(TVideoCaptureThreadCmdDescr value)
{
 WaitForSingleObject(CommandUnlockMutex,INFINITE);
 std::pair<double,TVideoCaptureThreadCmdDescr> cmd(TDateTime::CurrentDateTime().operator double(),value);
 CommandQueue.push_back(cmd);
// CommandQueue[TDateTime::CurrentDateTime().operator double()]=value;
 ReleaseMutex(CommandUnlockMutex);
}

/// Очищает очередь
void TVideoCaptureThread::ClearCommandQueue(void)
{
 WaitForSingleObject(CommandUnlockMutex,INFINITE);
 CommandQueue.clear();
 ReleaseMutex(CommandUnlockMutex);
}

/// Осуществляет обработку очередной команды из очереди
void TVideoCaptureThread::ProcessCommandQueue(void)
{
 double cmd_time=0;
 TVideoCaptureThreadCmdDescr cmd;
 double curr_time=TDateTime::CurrentDateTime().operator double();
 WaitForSingleObject(CommandUnlockMutex,INFINITE);
 std::list<std::pair<double,TVideoCaptureThreadCmdDescr> >::iterator I=CommandQueue.begin();
 for(;I != CommandQueue.end();++I)
 {
/*
  if(I->second.Id == tvcStart && I->second.ExecTime<=curr_time)
  {
   if(WaitForSingleObject(GlobalStartUnlockMutex, 10) != WAIT_OBJECT_0)
   {
//	CommandQueue.erase(I);
	break;
   }
//   ResetEvent(GlobalStartUnlockEvent);
  }

  if(I->second.ExecTime<=curr_time)*/
  {
   cmd_time=I->first;
   cmd=I->second;
   CommandQueue.erase(I);
   break;
  }
 }
 ReleaseMutex(CommandUnlockMutex);

 switch(cmd.Id)
 {
 case tvcNone:
 break;

 case tvcStart:
  RunCapture();
  SetEvent(CaptureEnabled);
 break;

 case tvcStop:
  ResetEvent(CaptureEnabled);
  StopCapture();
 break;

 case tvcTerminate:
 break;

 case tvcRecreate:
  ResetEvent(CaptureEnabled);
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

/// Флаг повтора воспроизведения сначала после завершения
bool TVideoCaptureThread::GetRepeatFlag(void) const
{
 return RepeatFlag;
}

bool TVideoCaptureThread::SetRepeatFlag(bool value)
{
 RepeatFlag=value;
 return true;
}

/// Устанавливает значение FPS
double TVideoCaptureThread::GetFps(void) const
{
 return 0;
}

bool TVideoCaptureThread::SetFps(double fps)
{
 return false;
}

/// Интервал между последним стартом и рестартом, мс
int TVideoCaptureThread::GetRestartInterval(void) const
{
 return RestartInterval;
}

bool TVideoCaptureThread::SetRestartInterval(int value)
{
 RestartInterval=value;
 return true;
}

/// Желаемое разрешение захвата
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

/// Флаг включения выбора желаемого разрешения захвата
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
// Управление данными
// --------------------------
/// Флаг состояния треда
/// 0 - остановлен
/// 1 - Запущен
/*
int TVideoCaptureThread::GetThreadState(void) const
{
// ThreadStateMutex->Acquire();
// int res=ThreadState;
// ThreadStateMutex->Release();
 return ThreadState;
}     */

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

/// Возвращает разрешение потока
int TVideoCaptureThread::GetWidth(void)
{
 if(WaitForSingleObject(SourceUnlock,30) != WAIT_OBJECT_0)
  return -1;
// ResetEvent(SourceUnlock);
 int res=0;
 if(ReadSource)
  res=ReadSource->GetWidth();
// SetEvent(SourceUnlock);
 ReleaseMutex(SourceUnlock);

 return res;
}

int TVideoCaptureThread::GetHeight(void)
{
 if(WaitForSingleObject(SourceUnlock,30) != WAIT_OBJECT_0)
  return -1;
// ResetEvent(SourceUnlock);
 int res=0;
 if(ReadSource)
  res=ReadSource->GetHeight();
 ReleaseMutex(SourceUnlock);

 return res;
}

/// Сохранение настроек в xml
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

/// Загрузка и применение настроек из xml
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
// Управление данными
// --------------------------
/// Сохранение настроек в xml
bool TVideoCaptureThread::ASaveParameters(RDK::USerStorageXML &xml)
{
 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThread::ALoadParameters(RDK::USerStorageXML &xml)
{
 return true;
}
// --------------------------
// Управление событиями
// --------------------------
/// Выставляется по завершении захвата нового кадра
HANDLE TVideoCaptureThread::GetFrameNotInProgress(void) const
{
 return FrameNotInProgress;
}

/// Выставлено всегда. Сбрасывается на время доступа к изображению
/*HANDLE TVideoCaptureThread::GetSourceUnlock(void) const
{
 return SourceUnlock;
} */

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
void __fastcall TVideoCaptureThread::Start(double time)
{
 ClearCommandQueue();
// AddCommand(TVideoCaptureThreadCmdDescr(tvcRecreate,time));
 AddCommand(TVideoCaptureThreadCmdDescr(tvcStart,time));
 AStart(time);
}

void __fastcall TVideoCaptureThread::Stop(double time)
{
 AStop(time);
 ClearCommandQueue();
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
// Synchronize(ExecuteCaptureInit);
 ExecuteCaptureInit();
 while(!Terminated)
 {
//  if(WaitForSingleObject(StartInProgressEvent,0) == WAIT_TIMEOUT)
//   ReleaseMutex(GlobalStartUnlockMutex);

  if(WaitForSingleObject(CaptureEnabled,30) == WAIT_TIMEOUT)
  {
   ProcessCommandQueue();
   SetEvent(FrameNotInProgress);
   continue;
  }
  ProcessCommandQueue();
  Sleep(10);
//  Application->HandleMessage();
 // Application->ProcessMessages();
	  /*
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
//	 AddCommand(TVideoCaptureThreadCmdDescr(tvcHalt,0));
//	 AddCommand(TVideoCaptureThreadCmdDescr(tvcRecreate,0));
//	 AddCommand(TVideoCaptureThreadCmdDescr(tvcStop,0));
	 LastStartTime=TDateTime::CurrentDateTime().operator double();
//	 AddCommand(TVideoCaptureThreadCmdDescr(tvcStart,curr_time));
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
       */
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
// Synchronize(ExecuteCaptureUnInit);
 ExecuteCaptureUnInit();
}

void __fastcall TVideoCaptureThread::ExecuteCaptureInit(void)
{

}

void __fastcall TVideoCaptureThread::ExecuteCaptureUnInit(void)
{

}


/// Возвращает копию изображения с блокировкой
bool TVideoCaptureThread::ReadSourceSafe(RDK::UBitmap& dest, double &time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceUnlock,30000) != WAIT_OBJECT_0)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::ReadSourceSafe: SourceUnlock timeout!").c_str());
  return false;
 }
// ResetEvent(SourceUnlock);
 time_stamp=LastTimeStamp;
 RDK::UBitmap* source=ReadSource;

 if(reflect)
  source->ReflectionX(&dest);
 else
  dest=*source;
 ReleaseMutex(SourceUnlock);
// SetEvent(SourceUnlock);

// SetEvent(SourceUnlock);
 return true;
}

/// Записывает изображение в тред с блокировкой
bool TVideoCaptureThread::WriteSourceSafe(const RDK::UBitmap& src, double time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceWriteUnlock,10000) != WAIT_OBJECT_0)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceWriteUnlock timeout!").c_str());
  return false;
 }

// ResetEvent(SourceWriteUnlock);
 if(reflect)
  const_cast<RDK::UBitmap&>(src).ReflectionX(WriteSource);
 else
  *WriteSource=src;
// SetEvent(SourceWriteUnlock);

 if(WaitForSingleObject(SourceUnlock,30000) != WAIT_OBJECT_0)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceUnlock timeout!").c_str());
  return false;
 }
// ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();
 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
 ReleaseMutex(SourceUnlock);
 ReleaseMutex(SourceWriteUnlock);
// SetEvent(SourceUnlock);
 return true;
}

bool TVideoCaptureThread::WriteSourceSafe(Graphics::TBitmap *src, double time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceWriteUnlock,10000) != WAIT_OBJECT_0)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceWriteUnlock timeout!").c_str());
  return false;
 }

// ResetEvent(SourceWriteUnlock);
 TBitmapToUBitmap(*WriteSource, src, reflect);
// SetEvent(SourceWriteUnlock);

 if(WaitForSingleObject(SourceUnlock,30000) != WAIT_OBJECT_0)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::WriteSourceSafe: SourceUnlock timeout!").c_str());
  return false;
 }
// ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();
 RDK::UBitmap* old_read_source=ReadSource;
 ReadSource=WriteSource;
 WriteSource=old_read_source;
// SetEvent(SourceUnlock);
 ReleaseMutex(SourceWriteUnlock);
 ReleaseMutex(SourceUnlock);
 return true;
}

// Меняет временную метку с блокировкой
bool TVideoCaptureThread::SetLastTimeStampSafe(double time_stamp)
{
 if(WaitForSingleObject(SourceUnlock,30000) != WAIT_OBJECT_0)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::SetLastTimeStampSafe: SourceUnlock timeout!").c_str());
  return false;
 }
// ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 RealLastTimeStamp=TDateTime::CurrentDateTime().operator double();

 ReleaseMutex(SourceUnlock);
// SetEvent(SourceUnlock);
 return true;
}

// Считывает временную метку с блокировкой
double TVideoCaptureThread::GetLastTimeStampSafe(void) const
{
 if(WaitForSingleObject(SourceUnlock,30000) != WAIT_OBJECT_0)
 {
  MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, std::string("TVideoCaptureThread::GetLastTimeStampSafe: SourceUnlock timeout!").c_str());
  return 0.0;
 }
// ResetEvent(SourceUnlock);

 double res=LastTimeStamp;
// SetEvent(SourceUnlock);
 ReleaseMutex(SourceUnlock);
 return res;
}
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
bool __fastcall TVideoCaptureThread::RunCapture(void)
{
/*
 if(WaitForSingleObject(StartInProgressEvent,0) != WAIT_TIMEOUT)
  return true;
 SetEvent(StartInProgressEvent);

 double curr_time=TDateTime::CurrentDateTime().operator double();
 LastStartTime=curr_time;
 RealLastTimeStamp=curr_time;
 ConnectionState=10;
// Synchronize(ARunCapture);
*/
 ARunCapture();
/* Sleep(100);
// SetEvent(CaptureEnabled);
*/
 return true;
}

bool __fastcall TVideoCaptureThread::StopCapture(void)
{
// ResetEvent(CaptureEnabled);
 LastStartTime=0;
 HaltCapture();
 ConnectionState=1;
 ResetEvent(StartInProgressEvent);
 Sleep(100);
 return true;
}

/// Останавливает фактический захват не меняя статуса треда
bool __fastcall TVideoCaptureThread::HaltCapture(void)
{
// Synchronize(AStopCapture);
 AStopCapture();
 return true;
}


bool __fastcall TVideoCaptureThread::RecreateCapture(void)
{
 return true;
// ResetEvent(CaptureEnabled);
// ConnectionState=0;

 RDK::USerStorageXML xml;
 SaveParameters(xml);
// Synchronize(ARecreateCapture);
 ARecreateCapture();
 LoadParameters(xml);
 Sleep(100);
 return true;
}

void __fastcall TVideoCaptureThread::ARecreateCapture(void)
{

}

void __fastcall TVideoCaptureThread::ReloadParameters(void)
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
// Конструкторы и деструкторы
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
// Управление параметрами
// --------------------------
/// Имя файла изображения
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


/// Устанавливает значение FPS
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
// Управление потоком
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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadBmp::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadBmp(frame,create_suspended);
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadBmp::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("FileName",FileName);
 xml.WriteFloat("Fps",Fps);

 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThreadBmp::ALoadParameters(RDK::USerStorageXML &xml)
{
 SetFileName(xml.ReadString("FileName",FileName));
 SetFps(xml.ReadFloat("Fps",Fps));

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления потоком
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
// Конструкторы и деструкторы
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
// Управление параметрами
// --------------------------
/// Имя файла изображения
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
 long long diff=index-CurrentBmpSequenceIndex;
 CurrentBmpSequenceIndex=index;
 CurrentTimeStamp+=(double(diff)/Fps)/86400.0;
 SetLastTimeStampSafe(CurrentTimeStamp);
 return true;
}

/// Устанавливает значение FPS
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
// Управление потоком
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

// Загружает выбранную картинку по индеку в массиве имен
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


// Меняет временную метку с блокировкой
bool TVideoCaptureThreadBmpSequence::SetLastTimeStampSafe(double time_stamp)
{
 TVideoCaptureThread::SetLastTimeStampSafe(time_stamp);
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;
// CurrentBmpSequenceIndex=time_stamp;
 return true;
}

/// Возвращает 0 если если состояние не определено
/// Возвращает 1 если если нет подключения к источнику
/// Возвращает 2 если если есть подключение к источнику
int TVideoCaptureThread::CheckConnection(void) const
{
 return 2;
}

/// Возвращает 0 если захват не работает и 1 если активен
int TVideoCaptureThread::CheckCaptureThreadState(void) const
{
 if(WaitForSingleObject(CaptureEnabled, 0) != WAIT_TIMEOUT)
  return 1;

 return 0;
}
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadBmpSequence::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadBmpSequence(frame,create_suspended);
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadBmpSequence::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteString("PathName",PathName);
 xml.WriteFloat("Fps",Fps);

 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThreadBmpSequence::ALoadParameters(RDK::USerStorageXML &xml)
{
 SetPathName(xml.ReadString("PathName",PathName));
 SetFps(xml.ReadFloat("Fps",Fps));

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления потоком
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
// Конструкторы и деструкторы
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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadHttpServer::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadHttpServer(frame,create_suspended);
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadHttpServer::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("ListenPort",ListenPort);

 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThreadHttpServer::ALoadParameters(RDK::USerStorageXML &xml)
{
 SetListenPort(xml.ReadInteger("ListenPort",ListenPort));
 return true;
}
// --------------------------


// --------------------------
// Скрытые методы управления потоком
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
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabber::TVideoCaptureThreadVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended)
 : VideoGrabber(0), TVideoCaptureThread(frame,CreateSuspended)
{
 ConvertBitmap=new Graphics::TBitmap;
 OverlayMaskBitmap=new Graphics::TBitmap;

 VideoGrabberCompleted=CreateEvent(0,TRUE,0,0);
 ConvertMutex=CreateMutex(0,FALSE,0);
 OSDMutex=CreateMutex(0,FALSE,0);

 RestartMode=1;
 ConnectionState=0;

 CaptureTimeout=5000;
 ConnectionTimeout=5000;
 OverlayHandle=0;
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

 if(OverlayMaskBitmap)
 {
  delete OverlayMaskBitmap;
  OverlayMaskBitmap=0;
 }

 CloseHandle(VideoGrabberCompleted);
 CloseHandle(ConvertMutex);
 CloseHandle(OSDMutex);
}
// --------------------------
// Управление параметрами
// --------------------------
/// Устанавливает значение FPS
double TVideoCaptureThreadVideoGrabber::GetFps(void) const
{
 return Fps;
}

bool TVideoCaptureThreadVideoGrabber::SetFps(double fps)
{
 Fps=fps;
 return true;
}

/// Таймаут соединения с камерой
int TVideoCaptureThreadVideoGrabber::GetConnectionTimeout(void) const
{
 return ConnectionTimeout;
}

bool TVideoCaptureThreadVideoGrabber::SetConnectionTimeout(int value)
{
 if(ConnectionTimeout == value)
  return true;

 ConnectionTimeout=value;
 return true;
}

/// Таймаут захвата
int TVideoCaptureThreadVideoGrabber::GetCaptureTimeout(void) const
{
 return CaptureTimeout;
}

bool TVideoCaptureThreadVideoGrabber::SetCaptureTimeout(int value)
{
 if(CaptureTimeout == value)
  return true;

 CaptureTimeout=value;
 return true;
}
// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabber::ExecuteCaptureInit(void)
{
 if(!VideoGrabber)
 {
  VideoGrabber=new TVideoGrabber((TComponent*) NULL);
  if(OverlayHandle)
  {
   VideoGrabber->Parent=OverlayHandle;
   VideoGrabber->Visible=true;
   switch(AutoScaleMode)
   {
   case 0:
	VideoGrabber->Display_AutoSize = true;
	VideoGrabber->Align=alNone;
	VideoGrabber->Display_AspectRatio=ar_NoResize;
   break;

   case 1:
   {
	VideoGrabber->Display_AutoSize = false;
	VideoGrabber->Align=alNone;
	VideoGrabber->Display_AspectRatio=ar_Stretch;
	double stretch=1;

	if(VideoGrabber->VideoHeight>0)
	 stretch=(VideoGrabber->VideoWidth)/double(VideoGrabber->VideoHeight);

	if(OverlayHandle->ClientHeight/VideoGrabber->VideoHeight<OverlayHandle->ClientWidth/VideoGrabber->VideoWidth || VideoGrabber->VideoHeight>OverlayHandle->ClientHeight)
	{
	 VideoGrabber->Height=OverlayHandle->ClientHeight;
	 VideoGrabber->Width=ceil((OverlayHandle->ClientHeight)*stretch);
	}
	else
	if(OverlayHandle->ClientWidth/VideoGrabber->VideoWidth<+OverlayHandle->ClientHeight/VideoGrabber->VideoHeight || VideoGrabber->VideoWidth>OverlayHandle->ClientWidth)
	{
	 VideoGrabber->Width=OverlayHandle->ClientWidth;
	 VideoGrabber->Height=ceil((OverlayHandle->ClientWidth)/stretch); //-V537
	}
	else
	{
	 VideoGrabber->Height=VideoGrabber->VideoHeight;
	 VideoGrabber->Width=VideoGrabber->VideoWidth;
	}
   }
   break;

   break;

   case 2:
	VideoGrabber->Display_AutoSize = false;
	VideoGrabber->Align=alNone;
	VideoGrabber->Display_AspectRatio=ar_NoResize;
   break;
   }
  }
  else
  {
   VideoGrabber->Parent=0;//GetFrame();
   VideoGrabber->Visible=false;
   VideoGrabber->Display_AutoSize = false;
  }
 }
 VideoGrabber->OnMouseDown=VideoGrabberOnVideoMouseDown;
 VideoGrabber->OnMouseUp=VideoGrabberOnVideoMouseUp;
 VideoGrabber->OnMouseMove=VideoGrabberOnVideoMouseMove;
 VideoGrabber->OnFrameCaptureCompleted=OnFrameCaptureCompleted;
 VideoGrabber->OnFrameBitmap=VideoGrabberFrameBitmap;
 VideoGrabber->OnLog=VideoGrabberLog;
 VideoGrabber->OnDeviceLost=VideoGrabberDeviceLost;
 VideoGrabber->OnPlayerEndOfStream = VideoGrabberPlayerEndOfStream;
 VideoGrabber->OnPlayerOpened=VideoGrabberOnPlayerOpened;
 VideoGrabber->OnPreviewStarted=VideoGrabberOnPreviewStarted;
//  VideoGrabber->OnThreadSync=VideoGrabberOnThreadSync;

 VideoGrabber->PlayerRefreshPausedDisplay = false;
 VideoGrabber->AutoStartPlayer = false;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->BurstInterval = 0;
 VideoGrabber->BurstMode = true;
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->Synchronized=false;
 VideoGrabber->SetIPCameraSetting(ips_ConnectionTimeout, ConnectionTimeout);
 VideoGrabber->SetIPCameraSetting(ips_ReceiveTimeout, CaptureTimeout);
 VideoGrabber->FrameGrabber=fg_CaptureStream;
 VideoGrabber->FrameCaptureWithoutOverlay=true;
 VideoGrabber->FrameGrabberRGBFormat=fgf_RGB24;
 std::string lic_string=AnsiString(TVGrabberLicenseString).c_str();
 std::vector<std::string> licenses;
 RDK::separatestring(lic_string,licenses,'|');
 for(size_t i=0;i<licenses.size();i++)
  VideoGrabber->LicenseString=licenses[i].c_str();//TVGrabberLicenseString;
 VideoGrabber->SyncCommands=false;
 VideoGrabber->EventNotificationSynchrone=false;
// VideoGrabber->OnFrameBitmapEventSynchrone=true;
 VideoGrabber->OpenURLAsync=true;

 if(DesiredResolutionFlag)
 {
  VideoGrabber->FrameCaptureZoomSize=100;
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

 if(WaitForSingleObject(ConvertMutex,10000) == WAIT_OBJECT_0)
 {
  ConvertUBitmap<<Frame_Bitmap;
  ReleaseMutex(ConvertMutex);
 }
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
	PulseEvent(SourceStoppedEvent);
	if(RepeatFlag)
	{
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
 if(WaitForSingleObject(OSDMutex,10) == WAIT_OBJECT_0)
 {
  if(OverlayMaskBitmap->Width == BitmapInfo->BitmapWidth && OverlayMaskBitmap->Height == BitmapInfo->BitmapHeight)
  {
   unsigned char* dest = (unsigned char*) BitmapInfo->BitmapDataPtr;
   unsigned char* source = (unsigned char*) OverlayMaskBitmap->ScanLine[OverlayMaskBitmap->Height-1];
   for(int i=0;i<BitmapInfo->BitmapSize;i++)
   {
	if(*source>0)
	 *dest=*source;
	++dest;
	++source;
   }
  }
  ReleaseMutex(OSDMutex);
 }
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberLog(TObject *Sender,
	  TLogType LogType, String Severity, String InfoMsg)
{
 MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, (std::string("VideoGrabber [")+std::string(AnsiString(Severity).c_str())+std::string("] ")+AnsiString(InfoMsg).c_str() ).c_str());
 if(Severity == "ERROR")
 {
  if(LogType == 82)
   return;
  LastStartTime=TDateTime::CurrentDateTime().operator double();
  ConnectionState=10;

  if(WaitForSingleObject(StartInProgressEvent,0) != WAIT_TIMEOUT)
  {
   //SetEvent(GlobalStartUnlockEvent);
//   ReleaseMutex(GlobalStartUnlockMutex);
   ResetEvent(StartInProgressEvent);
  }
 }

/* if(LogType == e_failed_to_start_preview)
 {
  if(WaitForSingleObject(StartInProgressEvent,0) != WAIT_TIMEOUT)
  {
   //SetEvent(GlobalStartUnlockEvent);
   ReleaseMutex(GlobalStartUnlockMutex);
   ResetEvent(StartInProgressEvent);
  }
 }*/
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
 if(WaitForSingleObject(StartInProgressEvent,0) != WAIT_TIMEOUT)
 {
  //SetEvent(GlobalStartUnlockEvent);
//  ReleaseMutex(GlobalStartUnlockMutex);
  ResetEvent(StartInProgressEvent);
//  ReleaseMutex(GlobalStartUnlockMutex);
 }
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberOnPreviewStarted(TObject *Sender)
{
// if(WaitForSingleObject(StartInProgressEvent,0) != WAIT_TIMEOUT)
 {
  //SetEvent(GlobalStartUnlockEvent);
//  ReleaseMutex(GlobalStartUnlockMutex);
  ResetEvent(StartInProgressEvent);
//  ReleaseMutex(GlobalStartUnlockMutex);
 }
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberOnVideoMouseUp(System::TObject* Sender, int VideoWindow, System::Uitypes::TMouseButton Button, System::Classes::TShiftState Shift, int X, int Y)
{
 if(VideoWindow == -1)
  return;

 if(!OverlayHandle)
  return;

 TScrollBox *overlay=dynamic_cast<TScrollBox *>(OverlayHandle);

 if(!overlay)
  return;

 overlay->OnMouseUp(Sender, Button, Shift, X, Y);
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberOnVideoMouseDown(System::TObject* Sender, int VideoWindow, System::Uitypes::TMouseButton Button, System::Classes::TShiftState Shift, int X, int Y)
{
 if(VideoWindow == -1)
  return;

 if(!OverlayHandle)
  return;

 TScrollBox *overlay=dynamic_cast<TScrollBox *>(OverlayHandle);

 if(!overlay)
  return;



 overlay->OnMouseDown(Sender, Button, Shift, X, Y);
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberOnVideoMouseMove(System::TObject* Sender, int VideoWindow, System::Classes::TShiftState Shift, int X, int Y)
{
 if(VideoWindow == -1)
  return;

 if(!OverlayHandle)
  return;

 TScrollBox *overlay=dynamic_cast<TScrollBox *>(OverlayHandle);

 if(!overlay)
  return;

 overlay->OnMouseMove(Sender, Shift, X, Y);
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
 if(WaitForSingleObject(ConvertMutex,10000) == WAIT_OBJECT_0)
 {
  ConvertResult.SetRes(ConvertUBitmap.GetWidth(),ConvertUBitmap.GetHeight(),RDK::ubmRGB24);
  ConvertUBitmap.ConvertTo(ConvertResult);
  ReleaseMutex(ConvertMutex);
 }

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

/// Возвращает 0 если если состояние не определено
/// Возвращает 1 если если нет подключения к источнику
/// Возвращает 2 если если есть подключение к источнику
int TVideoCaptureThreadVideoGrabber::CheckConnection(void) const
{
 return ConnectionState;
}

/// Режим масштабирования отображения на экране
/// 0 - Изображение выводится оригинального размера
/// 1 - Изображение масштабируется пропорционально, по границам окна
/// 2 - Изображение масштабиуется с растяжением по всем сторонам
int TVideoCaptureThreadVideoGrabber::GetAutoScaleMode(void) const
{
 return AutoScaleMode;
}

bool TVideoCaptureThreadVideoGrabber::SetAutoScaleMode(int value)
{
 AutoScaleMode=value;
 if(VideoGrabber)
 {
  if(OverlayHandle)
  {
   switch(AutoScaleMode)
   {
   case 0:
	VideoGrabber->Display_AutoSize = true;
	VideoGrabber->Align=alNone;
	VideoGrabber->Display_AspectRatio=ar_NoResize;
	if(VideoGrabber->Width != VideoGrabber->VideoWidth+4)
	 VideoGrabber->Width=VideoGrabber->VideoWidth+4;
	if(VideoGrabber->Height != VideoGrabber->VideoHeight+4)
	 VideoGrabber->Height=VideoGrabber->VideoHeight+4;
   break;

   case 1:
   {
	VideoGrabber->Display_AutoSize = false;
	VideoGrabber->Align=alNone;
	VideoGrabber->Display_AspectRatio=ar_Stretch;
	double stretch=1;

	if(VideoGrabber->VideoHeight>0)
	 stretch=(VideoGrabber->VideoWidth)/double(VideoGrabber->VideoHeight);

	if(OverlayHandle->ClientHeight/VideoGrabber->VideoHeight<OverlayHandle->ClientWidth/VideoGrabber->VideoWidth || VideoGrabber->VideoHeight>OverlayHandle->ClientHeight)
	{
	 VideoGrabber->Height=OverlayHandle->ClientHeight;
	 VideoGrabber->Width=ceil((OverlayHandle->ClientHeight)*stretch);
	}
	else
	if(OverlayHandle->ClientWidth/VideoGrabber->VideoWidth<+OverlayHandle->ClientHeight/VideoGrabber->VideoHeight || VideoGrabber->VideoWidth>OverlayHandle->ClientWidth)
	{
	 VideoGrabber->Width=OverlayHandle->ClientWidth;
	 VideoGrabber->Height=ceil((OverlayHandle->ClientWidth)/stretch); //-V537
	}
	else
	{
	 VideoGrabber->Height=VideoGrabber->VideoHeight;
	 VideoGrabber->Width=VideoGrabber->VideoWidth;
	}
   }
   break;

   case 2:
	VideoGrabber->Display_AutoSize = true;
	VideoGrabber->Align=alClient;
	VideoGrabber->Display_AspectRatio=ar_Box;
   break;

   }
  }
 }
 return true;
}


void __fastcall TVideoCaptureThreadVideoGrabber::ARecreateCapture(void)
{
// return;
 if(VideoGrabber)
 {
  VideoGrabber->Stop();
  WaitForSingleObject(GetFrameNotInProgress(),10000);
  delete VideoGrabber;
  VideoGrabber=0;
 }
 ExecuteCaptureInit();
 MEngine_LogMessage(ChannelIndex, RDK_EX_DEBUG, (std::string("TVideoCaptureThreadVideoGrabberIpCamera::ARecreateCapture ")).c_str());
}
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Сохранение настроек в xml
bool TVideoCaptureThreadVideoGrabber::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThread::ASaveParameters(xml))
  return false;

 xml.WriteString("Fps", AnsiString(FloatToStr(Fps)).c_str());
 xml.WriteString("CaptureTimeout", AnsiString(IntToStr(CaptureTimeout)).c_str());
 xml.WriteString("ConnectionTimeout", AnsiString(IntToStr(ConnectionTimeout)).c_str());

 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThreadVideoGrabber::ALoadParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThread::ALoadParameters(xml))
  return false;

 Fps=StrToFloatDef(xml.ReadString("Fps", "").c_str(), 0.0);
 SetCaptureTimeout(xml.ReadInteger("CaptureTimeout", 5000));
 SetConnectionTimeout(xml.ReadInteger("ConnectionTimeout", 5000));

 return true;
}

/// Хендл окна в которое необходимо выводить данные захвата
TWinControl* TVideoCaptureThreadVideoGrabber::GetOverlayHandle(void) const
{
 return OverlayHandle;
}

bool TVideoCaptureThreadVideoGrabber::SetOverlayHandle(TWinControl* value)
{
 if(OverlayHandle == value)
  return true;

 OverlayHandle = value;
 if(VideoGrabber)
 {
  VideoGrabber->Parent=OverlayHandle;

  if(OverlayHandle)
  {
   VideoGrabber->Visible=true;
   VideoGrabber->Display_AutoSize = true;
   switch(AutoScaleMode)
   {
   case 0:
	VideoGrabber->Display_AutoSize = true;
	VideoGrabber->Align=alNone;
	VideoGrabber->Display_AspectRatio=ar_NoResize;
   break;

   case 1:
   {
	VideoGrabber->Display_AutoSize = false;
	VideoGrabber->Align=alNone;
	VideoGrabber->Display_AspectRatio=ar_Stretch;
	double stretch=1;

	if(VideoGrabber->VideoHeight>0)
	 stretch=(VideoGrabber->VideoWidth)/double(VideoGrabber->VideoHeight);

	if(OverlayHandle->ClientHeight/VideoGrabber->VideoHeight<OverlayHandle->ClientWidth/VideoGrabber->VideoWidth || VideoGrabber->VideoHeight>OverlayHandle->ClientHeight)
	{
	 VideoGrabber->Height=OverlayHandle->ClientHeight;
	 VideoGrabber->Width=ceil((OverlayHandle->ClientHeight)*stretch);
	}
	else
	if(OverlayHandle->ClientWidth/VideoGrabber->VideoWidth<+OverlayHandle->ClientHeight/VideoGrabber->VideoHeight || VideoGrabber->VideoWidth>OverlayHandle->ClientWidth)
	{
	 VideoGrabber->Width=OverlayHandle->ClientWidth;
	 VideoGrabber->Height=ceil((OverlayHandle->ClientWidth)/stretch); //-V537
	}
	else
	{
	 VideoGrabber->Height=VideoGrabber->VideoHeight;
	 VideoGrabber->Width=VideoGrabber->VideoWidth;
	}
   }
   break;

   case 2:
	VideoGrabber->Display_AutoSize = true;
	VideoGrabber->Align=alClient;
	VideoGrabber->Display_AspectRatio=ar_NoResize;
   break;
   }
  }
  else
  {
   VideoGrabber->Visible=false;
   VideoGrabber->Display_AutoSize = false;
  }
 }

 return true;
}

/// Управление маской для OSD
Graphics::TBitmap* TVideoCaptureThreadVideoGrabber::GetOverlayMaskBitmap(void)
{
 return OverlayMaskBitmap;
}

bool TVideoCaptureThreadVideoGrabber::SetOverlayMaskBitmap(Graphics::TBitmap* value)
{
 if(WaitForSingleObject(OSDMutex,10) != WAIT_OBJECT_0)
  return false;
/* TRect Rect;
 Rect.Top=0;
 Rect.Left=0;
 Rect.Right=OverlayMaskBitmap->Width-1;
 Rect.Bottom=OverlayMaskBitmap->Height-1;
 if(!value && Rect.Right>0 && Rect.Bottom>0)
 {
  OverlayMaskBitmap->Canvas->Brush->Color=clBlack;
  OverlayMaskBitmap->Canvas->Brush->Style=bsSolid;
  OverlayMaskBitmap->Canvas->FillRect(Rect);
  return true;
 }                 */
 OverlayMaskBitmap->Assign(value);
// OverlayMaskBitmap->PixelFormat=pf32bit;
 ReleaseMutex(OSDMutex);
 return true;
}
// --------------------------

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
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
// Управление параметрами
// --------------------------
/// Имя канала общей памяти
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
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberAvi::ExecuteCaptureInit(void)
{
 TVideoCaptureThreadVideoGrabber::ExecuteCaptureInit();
 if(VideoGrabber)
 {
  VideoGrabber->FrameGrabber=fg_BothStreams;
  VideoGrabber->VideoSource=vs_VideoFileOrURL;
  VideoGrabber->ClosePlayer();
  VideoGrabber->PlayerFileName=FileName.Get().c_str();
  VideoGrabber->FrameGrabberRGBFormat=fgf_RGB24;
  VideoGrabber->OpenPlayer();
 }
}

// Меняет временную метку с блокировкой
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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadVideoGrabberAvi::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadVideoGrabberAvi(frame,create_suspended);
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadVideoGrabberAvi::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ASaveParameters(xml))
  return false;

 xml.WriteString("FileName", FileName);
 xml.WriteBool("ProcessAllFramesFlag",ProcessAllFramesFlag);

 return true;
}

/// Загрузка и применение настроек из xml
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
// Скрытые методы управления потоком
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
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberCamera::TVideoCaptureThreadVideoGrabberCamera(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThreadVideoGrabber(frame, CreateSuspended)
{
 SourceMode=2;
 TVTunerMode=-1;
 TVTunerInputType=-1;
 TVTunerChannel=1;
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

int TVideoCaptureThreadVideoGrabberCamera::GetTVTunerChannel(void) const
{
 return TVTunerChannel;
}

int TVideoCaptureThreadVideoGrabberCamera::GetTVTunerCountryCode(void) const
{
 return TVTunerCountryCode;
}

int TVideoCaptureThreadVideoGrabberCamera::GetTVTunerMode(void) const
{
 return TVTunerMode;
}

int TVideoCaptureThreadVideoGrabberCamera::GetTVTunerInputType(void) const
{
 return TVTunerInputType;
}


bool TVideoCaptureThreadVideoGrabberCamera::Init(int camera_index, int input_index, int size_index, int subtype_index, int analog_index,
			int tv_tuner_channel, int tv_tuner_country_code, int tv_tuner_mode, int tv_tuner_input_type)
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
 VideoGrabber->TunerMode=TTunerMode(tv_tuner_mode);
 VideoGrabber->TVTunerInputType=TTunerInput(tv_tuner_input_type);
 VideoGrabber->TVCountryCode=tv_tuner_country_code;
 VideoGrabber->TVChannel=tv_tuner_channel;
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberCamera::ExecuteCaptureInit(void)
{
 TVideoCaptureThreadVideoGrabber::ExecuteCaptureInit();
 if(VideoGrabber)
 {
  VideoGrabber->VideoSource=vs_VideoCaptureDevice;
  VideoGrabber->BurstType = fc_TBitmap;
  VideoGrabber->BurstMode = True;
  VideoGrabber->BurstCount = 0;
  VideoGrabber->VideoDevice=CameraIndex;
  VideoGrabber->VideoInput=InputIndex;
  VideoGrabber->VideoSize=SizeIndex;
  VideoGrabber->VideoSubtype=SubtypeIndex;
  VideoGrabber->AnalogVideoStandard=AnalogIndex;
  VideoGrabber->TunerMode=TTunerMode(GetTVTunerMode());
  VideoGrabber->TVTunerInputType=TTunerInput(GetTVTunerInputType());
  VideoGrabber->TVCountryCode=TVTunerCountryCode;
  VideoGrabber->TVChannel=TVTunerChannel;
 }
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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadVideoGrabberCamera::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadVideoGrabberCamera(frame,create_suspended);
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadVideoGrabberCamera::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ASaveParameters(xml))
  return false;

 xml.WriteInteger("CameraIndex",CameraIndex);
 xml.WriteInteger("InputIndex",InputIndex);
 xml.WriteInteger("SizeIndex",SizeIndex);
 xml.WriteInteger("SubtypeIndex",SubtypeIndex);
 xml.WriteInteger("AnalogIndex",AnalogIndex);
 xml.WriteInteger("TVTunerMode",TVTunerMode);
 xml.WriteInteger("TVTunerInputType",TVTunerInputType);
 xml.WriteInteger("TVTunerCountryCode",TVTunerCountryCode);
 xml.WriteInteger("TVTunerChannel",TVTunerChannel);

 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThreadVideoGrabberCamera::ALoadParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ALoadParameters(xml))
  return false;

 CameraIndex=xml.ReadInteger("CameraIndex",CameraIndex);
 InputIndex=xml.ReadInteger("InputIndex",InputIndex);
 SizeIndex=xml.ReadInteger("SizeIndex",SizeIndex);
 SubtypeIndex=xml.ReadInteger("SubtypeIndex",SubtypeIndex);
 AnalogIndex=xml.ReadInteger("AnalogIndex",AnalogIndex);
 TVTunerChannel=xml.ReadInteger("TVTunerChannel",TVTunerChannel);
 TVTunerMode=xml.ReadInteger("TVTunerMode",TVTunerMode);
 TVTunerInputType=xml.ReadInteger("TVTunerInputType",TVTunerInputType);
 TVTunerCountryCode=xml.ReadInteger("TVTunerCountryCode",TVTunerCountryCode);
 TVTunerChannel=xml.ReadInteger("TVTunerChannel",TVTunerChannel);

 Init(CameraIndex, InputIndex, SizeIndex, SubtypeIndex, AnalogIndex,
		TVTunerChannel, TVTunerCountryCode, TVTunerMode, TVTunerInputType);

 return true;
}
// --------------------------


// --------------------------
// Скрытые методы управления потоком
// --------------------------
void __fastcall TVideoCaptureThreadVideoGrabberCamera::ARunCapture(void)
{
 if(VideoGrabber)
 {
  VideoGrabber->StartPreview();
  VideoGrabber->TVChannel=TVTunerChannel;
//  VideoGrabber->StartSynchronized();
 }
}

void __fastcall TVideoCaptureThreadVideoGrabberCamera::AStopCapture(void)
{
 if(VideoGrabber)
  VideoGrabber->Stop();
}
// --------------------------

//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
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
// Управление параметрами
// --------------------------
/// Имя камеры
String TVideoCaptureThreadVideoGrabberIpCamera::GetUrl(void) const
{
 return Url;
}

/// Имя пользователя
String TVideoCaptureThreadVideoGrabberIpCamera::GetUserName(void) const
{
 return UserName;
}

/// Пароль
String TVideoCaptureThreadVideoGrabberIpCamera::GetPassword(void) const
{
 return Password;
}


bool TVideoCaptureThreadVideoGrabberIpCamera::Init(const String camera_url, const String user_name, const String user_password)
{
 ExecuteCaptureInit();
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
// Управление потоком
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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadVideoGrabberIpCamera::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadVideoGrabberIpCamera(frame,create_suspended);
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadVideoGrabberIpCamera::ASaveParameters(RDK::USerStorageXML &xml)
{
 if(!TVideoCaptureThreadVideoGrabber::ASaveParameters(xml))
  return false;

 xml.WriteString("Url",AnsiString(Url).c_str());
 xml.WriteString("UserName", AnsiString(UserName).c_str());
 xml.WriteString("Password", AnsiString(Password).c_str());

 return true;
}

/// Загрузка и применение настроек из xml
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
// Скрытые методы управления потоком
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
  VideoGrabber->Stop();

 MEngine_LogMessage(ChannelIndex, RDK_EX_DEBUG, (std::string("TVideoCaptureThreadVideoGrabberIpCamera::AStopCapture ")+AnsiString(Url).c_str()).c_str());
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

 if(!Usm_GetNumPipes || !Usm_SetNumPipes)
  return false;

 if(Usm_GetNumPipes() <= value)
  Usm_SetNumPipes(value+1);

 PipeIndex=value;

 return true;
}

/// Имя канала общей памяти
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
 return GetLastTimeStampSafe()*86400.0*1000.0;
}

bool TVideoCaptureThreadSharedMemory::SetPosition(long long index)
{
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
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
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadSharedMemory::New(TVideoOutputFrame *frame, bool create_suspended)
{
 return new TVideoCaptureThreadSharedMemory(frame,create_suspended);
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadSharedMemory::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("PipeIndex",PipeIndex);
 xml.WriteString("PipeName",PipeName);
 xml.WriteInteger("SharedMemoryPipeSize",SharedMemoryPipeSize);

 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThreadSharedMemory::ALoadParameters(RDK::USerStorageXML &xml)
{
 PipeIndex=xml.ReadInteger("PipeIndex",PipeIndex);
 PipeName=xml.ReadString("PipeName",PipeName);
 SharedMemoryPipeSize=xml.ReadInteger("SharedMemoryPipeSize",SharedMemoryPipeSize);

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
void __fastcall TVideoCaptureThreadSharedMemory::ARunCapture(void)
{
}

void __fastcall TVideoCaptureThreadSharedMemory::AStopCapture(void)
{
}
// --------------------------


//---------------------------------------------------------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadNewVideoGrabber::TVideoCaptureThreadNewVideoGrabber(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThread(frame, CreateSuspended)
{
 TempBitmap=new Graphics::TBitmap;
 VideoGrabberFrame= new TVideoGrabberFrame(0);
 VideoGrabberFrame->SetCallbackThread(this);
// GrabberThread = new TGrabberThread(0,"");
// GrabberThread->GetVideoGrabberFrame()->SetCallbackThread(this);
// ImageFrame = 0;
// ImageFrame->Parent=frame;
// ImageFrame->GetGrabberThread()->GetVideoGrabberFrame()->SetCallbackThread(this);
 Fps=25.0;
 CurrentTimeStamp=0;
 SourceMode=0;
}

__fastcall TVideoCaptureThreadNewVideoGrabber::~TVideoCaptureThreadNewVideoGrabber(void)
{
 if(TempBitmap)
 {
  delete TempBitmap;
  TempBitmap=0;
 }


 if(VideoGrabberFrame)
 {
  delete VideoGrabberFrame;
  VideoGrabberFrame=0;
 }
/*
 if(GrabberThread)
 {
  GrabberThread->Terminate();
  GrabberThread->WaitFor();
  delete GrabberThread;
  GrabberThread=0;
 }*/
 /*
 if(ImageFrame)
 {
  delete ImageFrame;
  ImageFrame=0;
 } */
}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
bool TVideoCaptureThreadNewVideoGrabber::SetSourceMode(int mode)
{
 if(SourceMode == mode)
  return true;

 SourceMode = mode;
 return true;
}

/*
/// Имя файла изображения
std::string TVideoCaptureThreadNewVideoGrabber::GetFileName(void) const
{
 return FileName;
}

bool TVideoCaptureThreadNewVideoGrabber::SetFileName(const std::string& value)
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
          */
/// Возвращает число изображений в последовательности
long long TVideoCaptureThreadNewVideoGrabber::GetNumBitmaps(void) const
{
 return 1;
}

/// Устанавливает текущую позицию в последовательности
long long TVideoCaptureThreadNewVideoGrabber::GetPosition(void) const
{
 return 0;
}

bool TVideoCaptureThreadNewVideoGrabber::SetPosition(long long index)
{
 return true;
}


/// Устанавливает значение FPS
double TVideoCaptureThreadNewVideoGrabber::GetFps(void) const
{
 return Fps;
}

bool TVideoCaptureThreadNewVideoGrabber::SetFps(double fps)
{
 if(Fps == fps)
  return true;

 Fps=fps;
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TVideoCaptureThreadNewVideoGrabber::AStart(double time)
{
 CurrentTimeStamp=0;
}

void __fastcall TVideoCaptureThreadNewVideoGrabber::AStop(double time)
{
}


void __fastcall TVideoCaptureThreadNewVideoGrabber::BeforeCalculate(void)
{

}

void __fastcall TVideoCaptureThreadNewVideoGrabber::AfterCalculate(void)
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

void __fastcall TVideoCaptureThreadNewVideoGrabber::Calculate(void)
{
// double time_stamp=CurrentTimeStamp;//TDateTime::CurrentDateTime().operator double();
// SetLastTimeStampSafe(time_stamp);
}
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Создает копию этого потока
RDK::UEPtr<TVideoCaptureThread> TVideoCaptureThreadNewVideoGrabber::New(TVideoOutputFrame *frame, bool create_suspended)
{
 TVideoCaptureThreadNewVideoGrabber* thread=new TVideoCaptureThreadNewVideoGrabber(frame,create_suspended);
// thread->ImageFrame = new TImageFrame(0);
// thread->ImageFrame->Parent=frame;
 return thread;
}

/// Сохранение настроек в xml
bool TVideoCaptureThreadNewVideoGrabber::ASaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("SourceMode",SourceMode);

/// 1 - avi
/// 2 - camera
/// 3 - ip camera
 switch(SourceMode)
 {
 case 1:
 {
//  xml.WriteString("FileName", AnsiString(ImageFrame->GetGrabberThread()->GetVideoGrabberFrame()->VideoGrabber1->PlayerFileName).c_str());
//  xml.WriteString("FileName", AnsiString(GrabberThread->GetVideoGrabberFrame()->VideoGrabber1->PlayerFileName).c_str());
  xml.WriteString("FileName", AnsiString(VideoGrabberFrame->VideoGrabber1->PlayerFileName).c_str());
  xml.WriteBool("ProcessAllFramesFlag",false);
 }
 break;

 case 2:
 break;

 case 3:
//  xml.WriteString("Url",AnsiString(ImageFrame->GetGrabberThread()->GetVideoGrabberFrame()->VideoGrabber1->IPCameraURL).c_str());
//  xml.WriteString("Url",AnsiString(GrabberThread->GetVideoGrabberFrame()->VideoGrabber1->IPCameraURL).c_str());
  xml.WriteString("Url",AnsiString(VideoGrabberFrame->VideoGrabber1->IPCameraURL).c_str());
  xml.WriteString("UserName", AnsiString(UserName).c_str());
  xml.WriteString("Password", AnsiString(Password).c_str());
 break;
 }

 return true;
}

/// Загрузка и применение настроек из xml
bool TVideoCaptureThreadNewVideoGrabber::ALoadParameters(RDK::USerStorageXML &xml)
{
 int mode=xml.ReadInteger("SourceMode",SourceMode);
 SetSourceMode(mode);
 switch(SourceMode)
 {
 case 1:
 {
  String file_name=xml.ReadString("FileName", "").c_str();
  bool proc_all_frame=xml.ReadBool("ProcessAllFramesFlag",false);
  VideoGrabberFrame->InitByAvi(file_name);
//  GrabberThread->GetVideoGrabberFrame()->InitByAvi(file_name);
//  ImageFrame->Init();//SetData(0,file_name);
//  ImageFrame->GetGrabberThread()->GetVideoGrabberFrame()->InitByAvi(file_name);
//  ImageFrame->GetGrabberThread()->GetVideoGrabberFrame()->SetCallbackThread(this);
 }
 break;

 case 2:
 break;

 case 3:
  String url=xml.ReadString("Url","").c_str();
  UserName=xml.ReadString("UserName", AnsiString(UserName).c_str()).c_str();
  Password=xml.ReadString("Password", AnsiString(Password).c_str()).c_str();
  VideoGrabberFrame->InitByIpCamera(url, UserName, Password);
//  GrabberThread->GetVideoGrabberFrame()->InitByIpCamera(url, UserName, Password);
//  ImageFrame->Init();//SetData(1,url);
//  ImageFrame->GetGrabberThread()->GetVideoGrabberFrame()->InitByIpCamera(url, UserName, Password);
//  ImageFrame->GetGrabberThread()->GetVideoGrabberFrame()->SetCallbackThread(this);
break;

 }

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
void __fastcall TVideoCaptureThreadNewVideoGrabber::ARunCapture(void)
{
 VideoGrabberFrame->Start();
// ImageFrame->StartCaptureThread();
// GrabberThread->StartCaptureThread();
}

void __fastcall TVideoCaptureThreadNewVideoGrabber::AStopCapture(void)
{
 VideoGrabberFrame->Stop();
// ImageFrame->StopCaptureThread();
// GrabberThread->StopCaptureThread();
}

void __fastcall TVideoCaptureThreadNewVideoGrabber::ARecreateCapture(void)
{

}
// --------------------------



//---------------------------------------------------------------------------

