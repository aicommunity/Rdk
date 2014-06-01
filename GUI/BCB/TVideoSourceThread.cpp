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
// Конструкторы и деструкторы
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
 RestartMode=0;

 FreeOnTerminate=false;
 ConnectionState=0;
 CommandMutex=new TMutex(false);
 ThreadState=0;
 RestartInterval=10000;
 LastStartTime=0;
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
 delete CommandMutex;
 CommandMutex=0;
 CloseHandle(CaptureEnabled);
 CloseHandle(SourceUnlock);
 CloseHandle(SourceWriteUnlock);
 CloseHandle(FrameNotInProgress);
 CloseHandle(CalcCompleteEvent);
}
// --------------------------

// --------------------------
// Управление командами
// --------------------------
/// Добавляет команду в очередь
void TVideoCaptureThread::AddCommand(TVideoCaptureThreadCommands value)
{
 CommandMutex->Acquire();
 CommandQueue[TDateTime::CurrentDateTime().operator double()]=value;
 CommandMutex->Release();
}

/// Очищает очередь
void TVideoCaptureThread::ClearCommandQueue(void)
{
 CommandMutex->Acquire();
 CommandQueue.clear();
 CommandMutex->Release();
}

/// Осуществляет обработку очередной команды из очереди
void TVideoCaptureThread::ProcessCommandQueue(void)
{
 double cmd_time=0;
 TVideoCaptureThreadCommands cmd=tvcNone;
 CommandMutex->Acquire();
 std::map<double,TVideoCaptureThreadCommands>::iterator I=CommandQueue.begin();
 if(I != CommandQueue.end())
 {
  cmd_time=I->first;
  cmd=I->second;
  CommandQueue.erase(I);
 }
 CommandMutex->Release();

 switch(cmd)
 {
 case tvcNone:
 break;

 case tvcStart:
  RunCapture();
  SetThreadState(1);
  SetEvent(CaptureEnabled);
 break;

 case tvcStop:
  ResetEvent(CaptureEnabled);
  SetThreadState(0);
  ConnectionState=1;
  PauseCapture();
 break;

 case tvcTerminate:
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
// --------------------------

// --------------------------
// Управление данными
// --------------------------
/// Флаг состояния треда
/// 0 - остановлен
/// 1 - Запущен
int TVideoCaptureThread::GetThreadState(void) const
{
// ThreadStateMutex->Acquire();
// int res=ThreadState;
// ThreadStateMutex->Release();
 return ThreadState;
}

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

/// Сохранение настроек в xml
bool TVideoCaptureThread::SaveParameters(RDK::USerStorageXML &xml)
{
 xml.SelectNodeRoot("VideoSourceThread");
 return SaveParametersEx(xml);
}

bool TVideoCaptureThread::SaveParametersEx(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("SyncMode",SyncMode);
 xml.WriteBool("RepeatFlag",RepeatFlag);
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
 AddCommand(tvcStart);
 AStart();
}

void __fastcall TVideoCaptureThread::Stop(void)
{
 AStop();
 AddCommand(tvcStop);
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
 while(!Terminated)
 {
  if(WaitForSingleObject(CaptureEnabled,30) == WAIT_TIMEOUT)
  {
   ProcessCommandQueue();
   continue;
  }
  ProcessCommandQueue();

  if(CheckConnection() != 2 && GetThreadState() == 1)
  {
   double curr_time=TDateTime::CurrentDateTime().operator double();
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
	  Sleep(30);
	  continue;
	 }
	 AddCommand(tvcStop);
	 AddCommand(tvcStart);
	 continue;
 //	 PauseCapture();
//	 RunCapture();
	 break;
	}

	case 2:
	{
	 AddCommand(tvcStop);
	 continue;
//	 PauseCapture();
	 break;
	}
   };

   Sleep(30);
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
}

/// Возвращает копию изображения с блокировкой
bool TVideoCaptureThread::ReadSourceSafe(RDK::UBitmap& dest, double &time_stamp, bool reflect)
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

/// Записывает изображение в тред с блокировкой
bool TVideoCaptureThread::WriteSourceSafe(const RDK::UBitmap& src, double time_stamp, bool reflect)
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

bool TVideoCaptureThread::WriteSourceSafe(Graphics::TBitmap *src, double time_stamp, bool reflect)
{
 if(WaitForSingleObject(SourceWriteUnlock,100) == WAIT_TIMEOUT)
  return false;

 ResetEvent(SourceWriteUnlock);
 TBitmapToUBitmap(*WriteSource, src, reflect);
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

// Меняет временную метку с блокировкой
bool TVideoCaptureThread::SetLastTimeStampSafe(double time_stamp)
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return false;
 ResetEvent(SourceUnlock);

 LastTimeStamp=time_stamp;
 SetEvent(SourceUnlock);
 return true;
}

// Считывает временную метку с блокировкой
double TVideoCaptureThread::GetLastTimeStampSafe(void) const
{
 if(WaitForSingleObject(SourceUnlock,30) == WAIT_TIMEOUT)
  return 0.0;
 ResetEvent(SourceUnlock);

 double res=LastTimeStamp;
 SetEvent(SourceUnlock);
 return res;
}
// --------------------------

// --------------------------
// Скрытые методы управления потоком
// --------------------------
bool __fastcall TVideoCaptureThread::RunCapture(void)
{
 double curr_time=TDateTime::CurrentDateTime().operator double();
 LastStartTime=curr_time;
 Synchronize(ARunCapture);
 return true;
}

bool __fastcall TVideoCaptureThread::PauseCapture(void)
{
 LastStartTime=0;
 Synchronize(APauseCapture);
 return true;
}

bool TVideoCaptureThread::SetThreadState(int value)
{
 ThreadState=value;
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
void __fastcall TVideoCaptureThreadBmp::AStart(void)
{
 CurrentTimeStamp=0;
}

void __fastcall TVideoCaptureThreadBmp::AStop(void)
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

void __fastcall TVideoCaptureThreadBmp::APauseCapture(void)
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
void __fastcall TVideoCaptureThreadBmpSequence::AStart(void)
{
 CurrentTimeStamp=0;
 SetSyncMode(true);
// TVideoCaptureThread::Start();
}

void __fastcall TVideoCaptureThreadBmpSequence::AStop(void)
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

void __fastcall TVideoCaptureThreadBmpSequence::APauseCapture(void)
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
void __fastcall TVideoCaptureThreadHttpServer::AStart(void)
{
// TVideoCaptureThread::Start();
}

void __fastcall TVideoCaptureThreadHttpServer::AStop(void)
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

void __fastcall TVideoCaptureThreadHttpServer::APauseCapture(void)
{
 UHttpServerFrame->IdHTTPServer->Active=false;
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
 VideoGrabber->OnLog=VideoGrabberLog;
 VideoGrabber->OnDeviceLost=VideoGrabberDeviceLost;

 VideoGrabber->Display_AutoSize = false;
 VideoGrabber->PlayerRefreshPausedDisplay = false;
 VideoGrabber->AutoStartPlayer = false;
 VideoGrabber->BurstCount = 0;
 VideoGrabber->BurstInterval = 0;
 VideoGrabber->BurstMode = true;
 VideoGrabber->BurstType = fc_TBitmap;
 VideoGrabber->Synchronized=false;
 VideoGrabber->SetIPCameraSetting(ips_ConnectionTimeout, 1000);
 VideoGrabber->SetIPCameraSetting(ips_ReceiveTimeout, 2000);

 ConvertBitmap=new Graphics::TBitmap;

 VideoGrabberCompleted=CreateEvent(0,TRUE,0,0);

 RestartMode=1;
 ConnectionState=0;
}

__fastcall TVideoCaptureThreadVideoGrabber::~TVideoCaptureThreadVideoGrabber(void)
{
 if(VideoGrabber)
 {
  Terminate();
  WaitForSingleObject(GetFrameNotInProgress(),INFINITE);
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
// --------------------------
// Управление потоком
// --------------------------
TVideoGrabber* TVideoCaptureThreadVideoGrabber::GetVideoGrabber(void)
{
 return VideoGrabber;
}

void __fastcall TVideoCaptureThreadVideoGrabber::OnFrameCaptureCompleted(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId)
{
 if(GetThreadState())
  ConnectionState=2;

 if(Fps > 0)
 {
  double diffTime=double(FrameTime)/(10000000.0*86400)-GetLastTimeStampSafe();
  if(diffTime<(1.0/Fps)/86400.0)
  {
	return;
  }
 }

 Graphics::TBitmap *Frame_Bitmap;

 Frame_Bitmap = (Graphics::TBitmap*) FrameBitmap;

 switch (DestType)
 {
 case fc_TBitmap:
  if(Frame_Bitmap->PixelFormat == pf24bit)
  {
//   LastTimeStamp=FrameTime/10000;
//   *WriteSource<<Frame_Bitmap;
   WriteSourceSafe(Frame_Bitmap, double(FrameTime)/(10000000.0*86400), false);
  }
  else
  {
//   LastTimeStamp=FrameTime/10000;
   ConvertBitmap->Assign(Frame_Bitmap);
   ConvertBitmap->PixelFormat=pf24bit;
//   *WriteSource<<ConvertBitmap;
   WriteSourceSafe(ConvertBitmap, double(FrameTime)/(10000000.0*86400), false);
  }
  if(GetNumEngines() > ChannelIndex)
   UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,GetLastTimeStampSafe()*86400.0*1000.0);

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

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberLog(TObject *Sender,
	  TLogType LogType, String Severity, String InfoMsg)
{
 MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, (std::string("VideoGrabber [")+std::string(AnsiString(Severity).c_str())+std::string("] ")+AnsiString(InfoMsg).c_str() ).c_str());
 if(Severity == "ERROR")
  ConnectionState=1;
}

void __fastcall TVideoCaptureThreadVideoGrabber::VideoGrabberDeviceLost(TObject *Sender)
{
 MEngine_LogMessage(ChannelIndex, RDK_EX_INFO, "VideoGrabber Device lost");
 ConnectionState=1;
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
 if(WaitForSingleObject(VideoGrabberCompleted, wait_time) == WAIT_TIMEOUT)
 {
/*  if(WaitForSingleObject(CaptureEnabled,10) != WAIT_TIMEOUT)
  {
   LastTimeStamp+=(1.0/fps)/86400.0;
   if(MIsEngineInit(Frame->FrameIndex))
	MEnv_CallSourceController(Frame->FrameIndex);
   TVideoCaptureThread::AfterCalculate();
  }

  ResetEvent(VideoGrabberCompleted);*/
  return;
 }
/* if(WaitForSingleObject(VideoGrabberCompleted, wait_time) != WAIT_TIMEOUT)
 {
  UEngineMonitorForm->EngineMonitorFrame->SetServerTimeStamp(ChannelIndex,LastTimeStamp*86400.0*1000.0);
 }*/

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

/// Возвращает 0 если если состояние не определено
/// Возвращает 1 если если нет подключения к источнику
/// Возвращает 2 если если есть подключение к источнику
int TVideoCaptureThreadVideoGrabber::CheckConnection(void) const
{
 return ConnectionState;
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

 return true;
}

/// Загрузка и применение настроек из xml
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
// Конструкторы и деструкторы
// --------------------------
__fastcall TVideoCaptureThreadVideoGrabberAvi::TVideoCaptureThreadVideoGrabberAvi(TVideoOutputFrame *frame, bool CreateSuspended)
 : TVideoCaptureThreadVideoGrabber(frame, CreateSuspended)
{
 SourceMode=1;
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
std::string TVideoCaptureThreadVideoGrabberAvi::GetFileName(void) const
{
 return FileName;
}

bool TVideoCaptureThreadVideoGrabberAvi::SetFileName(const std::string& value)
{
 Stop();
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 FileName=value;
 VideoGrabber->PlayerFileName=FileName.Get().c_str();
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
// Меняет временную метку с блокировкой
bool TVideoCaptureThreadVideoGrabberAvi::SetLastTimeStampSafe(double time_stamp)
{
 TVideoCaptureThread::SetLastTimeStampSafe(time_stamp);
 if(WaitForSingleObject(FrameNotInProgress,1000) == WAIT_TIMEOUT)
  return false;
 VideoGrabber->PlayerTimePosition=time_stamp*10000000.0*86400.0;
 return true;
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AStart(void)
{
// TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AStop(void)
{
// TVideoCaptureThreadVideoGrabber::Stop();
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::AfterCalculate(void)
{
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
void __fastcall TVideoCaptureThreadVideoGrabberAvi::ARunCapture(void)
{
 if(VideoGrabber)
 {
  VideoGrabber->StartSynchronized();
  VideoGrabber->RunPlayer();
 }
}

void __fastcall TVideoCaptureThreadVideoGrabberAvi::APauseCapture(void)
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
void __fastcall TVideoCaptureThreadVideoGrabberCamera::AStart(void)
{
// TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberCamera::AStop(void)
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

 Init(CameraIndex, InputIndex, SizeIndex, SubtypeIndex, AnalogIndex);

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
  VideoGrabber->StartSynchronized();
 }
}

void __fastcall TVideoCaptureThreadVideoGrabberCamera::APauseCapture(void)
{
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
void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::AStart(void)
{
// TVideoCaptureThreadVideoGrabber::Start();
}

void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::AStop(void)
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
  VideoGrabber->StartSynchronized();
 }
}

void __fastcall TVideoCaptureThreadVideoGrabberIpCamera::APauseCapture(void)
{
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
void __fastcall TVideoCaptureThreadSharedMemory::AStart(void)
{

}

void __fastcall TVideoCaptureThreadSharedMemory::AStop(void)
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

void __fastcall TVideoCaptureThreadSharedMemory::APauseCapture(void)
{
}
// --------------------------
