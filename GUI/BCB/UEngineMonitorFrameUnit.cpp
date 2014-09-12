//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UEngineMonitorFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "TIdTcpResultBroadcasterFormUnit.h"
#include "TIdHttpResultBroadcasterFormUnit.h"
#include "UServerControlFormUnit.h"
#include "rdk_initdll.h"
#ifdef RDK_VIDEO
#include "VideoOutputFormUnit.h"
#endif
#include "UShowProgressBarUnit.h"
#include "TBcbSupport.h"
#include "UGEngineControlFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorFrame *UEngineMonitorFrame;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineMonitorThread::TEngineMonitorThread(TUEngineMonitorFrame *engine_monitor_frame, bool CreateSuspended)
: EngineMonitorFrame(engine_monitor_frame), TThread(CreateSuspended)
{
 CalcState=CreateEvent(0,TRUE,0,0);
 CalcEnable=CreateEvent(0,TRUE,0,0);
 CalcStarted=CreateEvent(0,TRUE,0,0);
 CalculationNotInProgress=CreateEvent(0,TRUE,TRUE,0);
}

__fastcall TEngineMonitorThread::~TEngineMonitorThread(void)
{
 ResetEvent(CalcStarted);
 ResetEvent(CalcState);
 Terminate();
 WaitForSingleObject(CalculationNotInProgress,INFINITE);
 WaitFor();
 CloseHandle(CalcState);
 CloseHandle(CalcStarted);
 CloseHandle(CalcEnable);
 CloseHandle(CalculationNotInProgress);
}
// --------------------------


// --------------------------
// Управление параметрами
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний тредов
std::vector<int> TEngineMonitorThread::ReadCalcThreadStates(void) const
{
 return CalcThreadStates;
}

/// Возвращает вектор состояний источников видеозахвата
std::vector<int> TEngineMonitorThread::ReadVideoCaptureStates(void) const
{
 return VideoCaptureStates;
}
// --------------------------


// --------------------------
// Управление потоком
// --------------------------
void __fastcall TEngineMonitorThread::BeforeCalculate(void)
{
}

void __fastcall TEngineMonitorThread::AfterCalculate(void)
{
}



void __fastcall TEngineMonitorThread::Execute(void)
{
 while(!Terminated)
 {
  if(WaitForSingleObject(CalcStarted,30) == WAIT_TIMEOUT)
   continue;

  if(WaitForSingleObject(CalculationNotInProgress,30) == WAIT_TIMEOUT)
  {
   continue;
  }
  ResetEvent(CalculationNotInProgress);

  // Определяем состояние тредов расчета
  std::vector<int> calc_thread_states;

  int num_channels=EngineMonitorFrame->GetNumChannels();
  calc_thread_states.assign(num_channels,1);
  CalcThreadStateTime.resize(num_channels,0);
  CalcThreadSuccessTime.resize(num_channels,0);

  for(int i=0;i<num_channels;i++)
  {
   TEngineThread *thread=EngineMonitorFrame->GetThreadChannel(i);
   if(thread)
   {
	if(WaitForSingleObject(thread->CalcStarted,0) == WAIT_TIMEOUT)
	{
	 calc_thread_states[i]=1;
	 continue;
	}

	RDK::ULongTime last_calc_time=thread->GetRealLastCalculationTime();
	if(CalcThreadSuccessTime[i] != last_calc_time)
	{
	 CalcThreadSuccessTime[i]=last_calc_time;

	 TDateTime dt=TDateTime::CurrentDateTime();
	 CalcThreadStateTime[i]=dt.operator double();
	 calc_thread_states[i]=0;
	}
	else
	{
	 TDateTime dt=TDateTime::CurrentDateTime();
	 if((dt.operator double()-CalcThreadStateTime[i])*86400.0>2.0)
	  calc_thread_states[i]=2;
	 else
	  calc_thread_states[i]=0;
	}
   }
   else
   {
	calc_thread_states[i]=1;
   }
  }

  CalcThreadStates=calc_thread_states;

  // Определяем состояние тредов захвата видео
#ifdef RDK_VIDEO
  std::vector<int> video_capture_states;

  int num_captures=VideoOutputForm->GetNumSources();
  video_capture_states.assign(num_captures,1);
  VideoCaptureStateTime.resize(num_captures,0);
  VideoCaptureSuccessTime.resize(num_captures,0);

  for(int i=0;i<num_captures;i++)
  {
   TVideoCaptureThread *thread=0;

   if(VideoOutputForm && VideoOutputForm->GetVideoOutputFrame(i) && VideoOutputForm->GetVideoOutputFrame(i)->CaptureThread)
	thread=VideoOutputForm->GetVideoOutputFrame(i)->CaptureThread;

   if(thread)
   {
	if(WaitForSingleObject(thread->GetCaptureEnabled(),0) == WAIT_TIMEOUT)
	{
	 video_capture_states[i]=1;
	 continue;
	}

	double last_calc_time=thread->GetLastTimeStampSafe();
	if(VideoCaptureSuccessTime[i] != last_calc_time)
	{
	 VideoCaptureSuccessTime[i]=last_calc_time;

	 TDateTime dt=TDateTime::CurrentDateTime();
	 VideoCaptureStateTime[i]=dt.operator double();
	 video_capture_states[i]=0;
	}
	else
	{
	 TDateTime dt=TDateTime::CurrentDateTime();
	 if((dt.operator double()-VideoCaptureStateTime[i])*86400.0>2.0)
	  video_capture_states[i]=2;
	 else
	  video_capture_states[i]=0;
	}
   }
   else
   {
	video_capture_states[i]=1;
   }
  }


  VideoCaptureStates=video_capture_states;
#endif

  Sleep(1);

  SetEvent(CalculationNotInProgress);
 }
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineThread::TEngineThread(int channel_index, int calculate_mode, RDK::UTime min_inerval, bool CreateSuspended)
: ChannelIndex(channel_index), CalculateMode(calculate_mode), MinInterstepsInterval(min_inerval), TThread(CreateSuspended)
{
 CalcState=CreateEvent(0,TRUE,0,0);
 CalcEnable=CreateEvent(0,TRUE,0,0);
 CalcStarted=CreateEvent(0,TRUE,0,0);
 CalculationNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 RealLastCalculationTime=0;
}

__fastcall TEngineThread::~TEngineThread(void)
{
 ResetEvent(CalcStarted);
 ResetEvent(CalcState);
 Terminate();
 WaitForSingleObject(CalculationNotInProgress,INFINITE);
 WaitFor();
 CloseHandle(CalcState);
 CloseHandle(CalcStarted);
 CloseHandle(CalcEnable);
 CloseHandle(CalculationNotInProgress);
}
// --------------------------


// --------------------------
// Управление параметрами
// --------------------------
/// Режим счета
int TEngineThread::GetCalculateMode(void) const
{
 return CalculateMode;
}

bool TEngineThread::SetCalculateMode(int value)
{
 if(CalculateMode == value)
  return true;

 if(WaitForSingleObject(CalculationNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 CalculateMode=value;

 return true;
}

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
int TEngineThread::GetMinInterstepsInterval(void) const
{
 return MinInterstepsInterval;
}

bool TEngineThread::SetMinInterstepsInterval(RDK::UTime value)
{
 if(MinInterstepsInterval == value)
  return true;

 if(WaitForSingleObject(CalculationNotInProgress,1000) == WAIT_TIMEOUT)
  return false;

 MinInterstepsInterval=value;
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void __fastcall TEngineThread::BeforeCalculate(void)
{
#ifdef RDK_VIDEO
#ifdef RDK_VIDEO
/* TVideoOutputFrame* video=VideoOutputForm->GetVideoOutputFrame(ChannelIndex);
 if(video)
 {
  if(video->CaptureThread)
  {
   double time_stamp=0;
   video->CaptureThread->ReadSourceSafe(Source,time_stamp,true);
  }
 }
 else
  Source.Clear();
   */
#endif

#endif
}

void __fastcall TEngineThread::AfterCalculate(void)
{
 UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]=
  UEngineMonitorForm->EngineMonitorFrame->GetServerTimeStamp(ChannelIndex);

 UEngineMonitorForm->EngineMonitorFrame->AddMetadata(ChannelIndex, UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]);
}



void __fastcall TEngineThread::Execute(void)
{
 while(!Terminated)
 {
  if(WaitForSingleObject(CalcStarted,30) == WAIT_TIMEOUT)
   continue;
  if(CalculateMode == 2)
  {
   if(WaitForSingleObject(CalcEnable,30) == WAIT_TIMEOUT)
	continue;
  }
  ResetEvent(CalcEnable);
  if(WaitForSingleObject(CalculationNotInProgress,30) == WAIT_TIMEOUT)
  {
   continue;
  }

  RDK::ULongTime diff=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),RealLastCalculationTime);
  if(diff<MinInterstepsInterval)
  {
   Sleep(MinInterstepsInterval-diff);
   continue;
  }

  ResetEvent(CalculationNotInProgress);
  BeforeCalculate();
  if(GetNumEngines()>ChannelIndex)
  {
   if(UEngineMonitorForm->EngineMonitorFrame->GetCalculationTimeSourceMode() == 0)
   {
    TDateTime dt=TDateTime::CurrentDateTime();
	MModel_SetDoubleSourceTime(ChannelIndex,dt.operator double());
   }
   else
   if(UEngineMonitorForm->EngineMonitorFrame->GetCalculationTimeSourceMode() == 1)
	MModel_SetDoubleSourceTime(ChannelIndex,UEngineMonitorForm->EngineMonitorFrame->GetServerTimeStamp(ChannelIndex)/(86400.0*1000.0)/*dt.operator double()*/);
   #ifdef RDK_VIDEO
   TVideoOutputFrame* video=VideoOutputForm->GetVideoOutputFrame(ChannelIndex);
   if(video)
	video->BeforeCalculate();
   #endif
   MEnv_Calculate(ChannelIndex,0);
  }
  AfterCalculate();
  RealLastCalculationTime=RDK::GetCurrentStartupTime();
  SetEvent(CalculationNotInProgress);
 }
}

RDK::ULongTime TEngineThread::GetRealLastCalculationTime(void) const
{
 return RealLastCalculationTime;
}

// --------------------------


//---------------------------------------------------------------------------
__fastcall TUEngineMonitorFrame::TUEngineMonitorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 CalculateMode.assign(GetNumEngines(),0);
 CalculateSignal.assign(GetNumEngines(),false);
 CalculateState.assign(GetNumEngines(),false);
 MinInterstepsInterval.assign(GetNumEngines(),20);
 AlwaysUpdateFlag=true;
 UpdateInterval=100;
 ChannelsMode=0;
 CalculationTimeSourceMode=0;
 ThreadCalcCompleteEvent=CreateEvent(0,TRUE,FALSE,0);

 EngineMonitorThread=new TEngineMonitorThread(this,false);
}

__fastcall TUEngineMonitorFrame::~TUEngineMonitorFrame(void)
{
// SetNumChannels(0);
 CloseHandle(ThreadCalcCompleteEvent);

 if(EngineMonitorThread)
 {
  ResetEvent(EngineMonitorThread->CalcStarted);
  WaitForSingleObject(EngineMonitorThread->CalculationNotInProgress,INFINITE);
  EngineMonitorThread->Terminate();
  EngineMonitorThread->WaitFor();
  delete EngineMonitorThread;
  EngineMonitorThread=0;
 }
}


/// Управление режимом работы
/// 0 - однопоточный (одноканальный) режим
/// 1 - многопоточный режим
int TUEngineMonitorFrame::GetChannelsMode(void) const
{
 return ChannelsMode;
}

void TUEngineMonitorFrame::SetChannelsMode(int mode)
{
 if(ChannelsMode == mode)
  return;

 Pause1Click(this);
 ChannelsMode=mode;
}


// Управление режимом расчетов
int TUEngineMonitorFrame::GetCalculateMode(int channel_index) const
{
 return CalculateMode[channel_index];
}

void TUEngineMonitorFrame::SetCalculateMode(int channel_index,int value)
{
 CalculateMode[channel_index]=value;
 if(int(ThreadChannels.size())>channel_index)
  ThreadChannels[channel_index]->SetCalculateMode(value);
}

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
int TUEngineMonitorFrame::GetCalculationTimeSourceMode(void) const
{
 return CalculationTimeSourceMode;
}

bool TUEngineMonitorFrame::SetCalculationTimeSourceMode(int value)
{
 if(CalculationTimeSourceMode == value)
  return true;

 CalculationTimeSourceMode=value;
 return true;
}


void TUEngineMonitorFrame::SetMinInterstepsInterval(int channel_index, RDK::UTime value)
{
 MinInterstepsInterval[channel_index]=value;
 if(int(ThreadChannels.size())>channel_index)
  ThreadChannels[channel_index]->SetMinInterstepsInterval(value);
}

// Управление временной меткой сервера
RDK::ULongTime TUEngineMonitorFrame::GetServerTimeStamp(int channel_index) const
{
 return ServerTimeStamp[channel_index];
}

void TUEngineMonitorFrame::SetServerTimeStamp(int channel_index, RDK::ULongTime stamp)
{
 if(int(ServerTimeStamp.size()) <= channel_index)
  return;

 if(ServerTimeStamp[channel_index] == stamp)
  return;

 ServerTimeStamp[channel_index]=stamp;
 CalculateSignal[channel_index]=true;
 if(ChannelsMode == 1)
  SetEvent(ThreadChannels[channel_index]->CalcEnable);
}



/// Управление числом каналов
int TUEngineMonitorFrame::GetNumChannels(void) const
{
 return GetNumEngines();
}

bool TUEngineMonitorFrame::SetNumChannels(int num)
{
 if(num == int(ThreadChannels.size()))
  return true;
 int old_num=GetNumEngines();
 SetNumEngines(num);
 if(old_num>0)
 {
  CalculateMode.resize(num,CalculateMode[old_num-1]);
  MinInterstepsInterval.resize(num,MinInterstepsInterval[old_num-1]);
 }
 else
 {
  CalculateMode.resize(num,0);
  MinInterstepsInterval.resize(num,0);
 }

 CalculateSignal.resize(num,false);
 CalculateState.resize(num,false);
 ServerTimeStamp.resize(num,0);
 LastCalculatedServerTimeStamp.resize(num,0);
 RealLastCalculationTime.resize(num,0);

 int old_size=int(ThreadChannels.size());
 for(int i=num;i<old_size;i++)
 {
//  ThreadChannels[i]->Suspend();
//  ThreadChannels[i]->WaitFor();
  ThreadChannels[i]->Terminate();
  delete ThreadChannels[i];
 }

 ThreadChannels.resize(num);
 for(int i=old_size;i<num;i++)
 {
  ThreadChannels[i]=new TEngineThread(i,CalculateMode[i],MinInterstepsInterval[i],false);
  ThreadChannels[i]->Priority=RDK_DEFAULT_THREAD_PRIORITY;

  ThreadChannels[i]->FreeOnTerminate=false;
 }

 return true;
}

bool TUEngineMonitorFrame::InsertChannel(int index)
{
 int old_num=GetNumEngines();
 Engine_Add(index);
 int new_num=GetNumEngines();

 if(old_num>0)
 {
  CalculateMode.resize(new_num,CalculateMode[old_num-1]);
  MinInterstepsInterval.resize(new_num,MinInterstepsInterval[old_num-1]);
 }
 else
 {
  CalculateMode.resize(new_num,0);
  MinInterstepsInterval.resize(new_num,0);
 }

 CalculateSignal.resize(new_num,false);
 CalculateState.resize(new_num,false);
 ServerTimeStamp.resize(new_num,0);
 LastCalculatedServerTimeStamp.resize(new_num,0);
 RealLastCalculationTime.resize(new_num,0);

 ThreadChannels.resize(new_num);
 for(int i=new_num-1;i>index;i--)
 {
  ThreadChannels[i]=ThreadChannels[i-1];
  CalculateSignal[i]=CalculateSignal[i-1];
  CalculateState[i]=CalculateState[i-1];
  ServerTimeStamp[i]=ServerTimeStamp[i-1];
  LastCalculatedServerTimeStamp[i]=LastCalculatedServerTimeStamp[i-1];
  RealLastCalculationTime[i]=RealLastCalculationTime[i-1];
  CalculateMode[i]=CalculateMode[i-1];
  MinInterstepsInterval[i]=MinInterstepsInterval[i-1];
 }

 ThreadChannels[index]=new TEngineThread(index,CalculateMode[index],MinInterstepsInterval[index],false);
 ThreadChannels[index]->Priority=RDK_DEFAULT_THREAD_PRIORITY;
 ThreadChannels[index]->FreeOnTerminate=false;
 return true;
}

bool TUEngineMonitorFrame::DeleteChannel(int index)
{
 int old_num=GetNumEngines();

// if(index<0 || index>=int(ThreadChannels.size()))
//  return true;

 if(ThreadChannels[index])
 {
  ThreadChannels[index]->Terminate();
  delete ThreadChannels[index];
  ThreadChannels[index]=0;
 }

 int del_res=Engine_Del(index);
 int new_num=GetNumEngines();

 if(new_num == old_num)
  return true;

 for(int i=index;i<new_num;i++)
 {
  ThreadChannels[i]=ThreadChannels[i+1];
  CalculateSignal[i]=CalculateSignal[i+1];
  CalculateState[i]=CalculateState[i+1];
  ServerTimeStamp[i]=ServerTimeStamp[i+1];
  LastCalculatedServerTimeStamp[i]=LastCalculatedServerTimeStamp[i+1];
  RealLastCalculationTime[i]=RealLastCalculationTime[i+1];
  CalculateMode[i]=CalculateMode[i+1];
  MinInterstepsInterval[i]=MinInterstepsInterval[i+1];
 }


 CalculateMode.resize(new_num);
 MinInterstepsInterval.resize(new_num);

 CalculateSignal.resize(new_num);
 CalculateState.resize(new_num);
 ServerTimeStamp.resize(new_num);
 LastCalculatedServerTimeStamp.resize(new_num);
 RealLastCalculationTime.resize(new_num);
 ThreadChannels.resize(new_num);

 return true;
}


void TUEngineMonitorFrame::AUpdateInterface(void)
{
 if(!Model_Check())
 {
  StatusBar->SimpleText="Model not exist!";
  StatusBar->Repaint();
  StatusBar->Update();
  return;
 }

 double fps=0;
 double instperf=Model_GetInstantPerformance("");
 if(fabs(instperf)>0.0001)
  fps=Model_GetTimeStep("")/instperf;

 StatusBar->SimpleText=String("Model Time=")+FloatToStrF(Model_GetDoubleTime(),ffFixed,3,3)
				+String("; Real Time=")+FloatToStrF(Model_GetDoubleRealTime(),ffFixed,3,3)
				+String("; Model Duration Time=")+FloatToStrF(Model_GetFullStepDuration("")/1000.0,ffFixed,3,3);
 if(instperf)
  StatusBar->SimpleText=StatusBar->SimpleText+
				String("; Model Performance=")+FloatToStrF(instperf,ffFixed,3,3)+
				StatusBar->SimpleText=StatusBar->SimpleText+String(" (")+
				FloatToStrF(fps,ffFixed,3,3)+String(" FPS)");

 StatusBar->SimpleText=StatusBar->SimpleText
				+String(" NumBfs=")+IntToStr(Engine_GetNumBufStrings());
 StatusBar->Repaint();
 StatusBar->Update();

 ShowDebugMessagesCheckBox->Checked=MEnv_GetDebugMode(0);
}

// Возврат интерфейса в исходное состояние
void TUEngineMonitorFrame::AClearInterface(void)
{


}

// Сохраняет параметры интерфейса в xml
void TUEngineMonitorFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
// xml.WriteInteger("CalculateMode",GetCalculateMode());
}

// Загружает параметры интерфейса из xml
void TUEngineMonitorFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
// SetCalculateMode(xml.ReadInteger("CalculateMode",0));
}

/// Регистрирует вещатель метаданных
void TUEngineMonitorFrame::RegisterMetadataBroadcaster(TBroadcasterForm *broadcaster)
{
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i] == broadcaster)
   return;
 }
 BroadcastersList.push_back(broadcaster);
}

/// Снимает регистрацию вещателя метаданных
void TUEngineMonitorFrame::UnRegisterMetadataBroadcaster(TBroadcasterForm *broadcaster)
{
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i] == broadcaster)
  {
   BroadcastersList.erase(BroadcastersList.begin()+i);
   return;
  }
 }
}

/// Отправляет метаданные во все зарегистрированные вещатели
bool TUEngineMonitorFrame::AddMetadata(int channel_index, RDK::ULongTime time_stamp)
{
 bool res=true;
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i])
   res&=BroadcastersList[i]->AddMetadata(channel_index,time_stamp);
 }
 return res;
}

/// Инициирует процедуру отправки метаданных всеми зарегистрированными вещателями
bool TUEngineMonitorFrame::SendMetadata(void)
{
 bool res=true;
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i])
   res&=BroadcastersList[i]->SendMetadata();
 }
 return res;
}


/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
void TUEngineMonitorFrame::StartChannel(int channel_index)
{
 if(channel_index>=GetNumChannels())
  return;
 UShowProgressBarForm->SetBarHeader(1,"Starting Channel Calculation...");
 UShowProgressBarForm->ResetBarStatus(1, 1, (channel_index<0)?int(ThreadChannels.size()):1);

 switch(ChannelsMode)
 {
 case 0:
  if(CalculateMode[GetSelectedEngineIndex()] == 1)
   Reset1Click(this);
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=1;
  Timer->Enabled=true;
  for(size_t i=0; i<ThreadChannels.size(); i++)
   CalculateState[i]=true;
 break;

 case 1:
//  for(int i=0;i<GetNumChannels();i++)
//   ThreadChannels[i]->Start();//Resume();
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=30;
  Timer->Enabled=true;
  if(channel_index<0)
  {
   for(size_t i=0;i<ThreadChannels.size();i++)
   {
	if(ThreadChannels[i])
	{
		SetEvent(ThreadChannels[i]->CalcStarted);
		SetEvent(ThreadChannels[i]->CalcState);
//		WaitForSingleObject(ThreadChannels[i]->CalculationNotInProgress,1000);
	}
	UShowProgressBarForm->IncBarStatus(1);
   }
  }
  else
  {
	if(ThreadChannels[channel_index])
	{
		SetEvent(ThreadChannels[channel_index]->CalcStarted);
		SetEvent(ThreadChannels[channel_index]->CalcState);
//		WaitForSingleObject(ThreadChannels[channel_index]->CalculationNotInProgress,1000);
	}
	UShowProgressBarForm->IncBarStatus(1);
  }
 break;
 }

}

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
void TUEngineMonitorFrame::PauseChannel(int channel_index)
{
 if(channel_index>=GetNumChannels())
  return;

 if(Timer->Enabled == false)
  return;

 UShowProgressBarForm->SetBarHeader(1,"Stopping Channel Calculation...");
 UShowProgressBarForm->ResetBarStatus(1, 1, (channel_index<0)?int(ThreadChannels.size()):1);

 switch(ChannelsMode)
 {
 case 0:
  Timer->Enabled=false;
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  for(size_t i=0; i<ThreadChannels.size(); i++)
   CalculateState[i]=false;
 break;

 case 1:
  if(channel_index == -1)
  {
   Timer->Enabled=false;
   TUVisualControllerFrame::CalculationModeFlag=false;
   TUVisualControllerForm::CalculationModeFlag=false;

   for(size_t i=0;i<ThreadChannels.size();i++)
   {
	if(ThreadChannels[i])
	{
	 ResetEvent(ThreadChannels[i]->CalcStarted);
	 ResetEvent(ThreadChannels[i]->CalcState);
//	 WaitForSingleObject(ThreadChannels[i]->CalculationNotInProgress,1000);
	}
	UShowProgressBarForm->IncBarStatus(1);
   }
  }
  else
  {
	if(ThreadChannels[channel_index])
	{
	 ResetEvent(ThreadChannels[channel_index]->CalcStarted);
	 ResetEvent(ThreadChannels[channel_index]->CalcState);
//	 WaitForSingleObject(ThreadChannels[channel_index]->CalculationNotInProgress,1000);
	}
	UShowProgressBarForm->IncBarStatus(1);

   bool all_stopped=true;
   for(size_t i=0;i<ThreadChannels.size();i++)
   {
	if(ThreadChannels[i])
	{
	 if(WaitForSingleObject(ThreadChannels[i]->CalcStarted,0) == WAIT_TIMEOUT)
	  all_stopped&=true;
	 else
	  all_stopped&=false;
	}
   }

   if(all_stopped)
   {
	Timer->Enabled=false;
	TUVisualControllerFrame::CalculationModeFlag=false;
	TUVisualControllerForm::CalculationModeFlag=false;
   }
  }
 break;
 }


}

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
void TUEngineMonitorFrame::ResetChannel(int channel_index)
{
 if(channel_index>=GetNumChannels())
  return;

 if(channel_index == -1)
 {
  if(!Model_Check())
  {
   RDK::UIVisualControllerStorage::UpdateInterface();
   return;
  }

  RDK::UIVisualControllerStorage::BeforeReset();
  for(int i=0;i<GetNumEngines();i++)
  {
   MEnv_Reset(i,0);
   RealLastCalculationTime[i]=0;
  }
  RDK::UIVisualControllerStorage::AfterReset();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 else
 {
  if(!MModel_Check(channel_index))
  {
   RDK::UIVisualControllerStorage::UpdateInterface();
   return;
  }

  RDK::UIVisualControllerStorage::BeforeReset();
  MEnv_Reset(channel_index,0);
  RealLastCalculationTime[channel_index]=0;
  RDK::UIVisualControllerStorage::AfterReset();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
}

//---------------------------------------------------------------------------
/// Проверяет состояние расчета
/// 0 - Не считает
/// 1 - Идет расчет
int TUEngineMonitorFrame::CheckCalcState(int channel_id) const
{
 if(channel_id<0 || channel_id>GetNumEngines())
  return 0;

 switch(ChannelsMode)
 {
  case 0:
  {
   if(CalculateState[channel_id])
	return 1;

   return 0;
  }
  case 1:
  {
   if(WaitForSingleObject(ThreadChannels[channel_id]->CalcState, 0) != WAIT_TIMEOUT)
	return 1;

   return 0;
  }
 }
 return 0;
}

/// Доступ к треду мониторинга состояния модулей сервера
const TEngineMonitorThread* TUEngineMonitorFrame::GetEngineMonitorThread(void) const
{
 return EngineMonitorThread;
}

/// Вклчает мониторинг сервера
void TUEngineMonitorFrame::StartEngineMonitorThread(void)
{
 if(EngineMonitorThread)
  SetEvent(EngineMonitorThread->CalcStarted);
}

/// Останавливает мониторинг сервера
void TUEngineMonitorFrame::StopEngineMonitorThread(void)
{
 if(EngineMonitorThread)
 {
  ResetEvent(EngineMonitorThread->CalcStarted);
  WaitForSingleObject(EngineMonitorThread->CalculationNotInProgress,INFINITE);
 }
}

TEngineThread* TUEngineMonitorFrame::GetThreadChannel(int i)
{
 if(i<0 || i>= int(ThreadChannels.size()))
  return 0;

 return ThreadChannels[i];
}

//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Start1Click(TObject *Sender)
{
 StartChannel(-1);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Pause1Click(TObject *Sender)
{
 PauseChannel(-1);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Reset1Click(TObject *Sender)
{
 ResetChannel(-1);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::TimerTimer(TObject *Sender)
{
 switch(ChannelsMode)
 {
 case 0:
 {
  RDK::UIVisualControllerStorage::BeforeCalculate();

  for(int i=0;i<GetNumEngines();i++)
  {
   if(!MIsEngineInit(i) || !MModel_Check(i))
   {
	continue;
   }

   if(CalculateMode[i] == 2)
   {
	if(!CalculateSignal[i])
	 continue;
   }
   else
   if(CalculateMode[i] == 0)
   {
	if(RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),RealLastCalculationTime[i])<MinInterstepsInterval[i])
	 continue;
   }
   CalculateSignal[i]=false;

   if(CalculationTimeSourceMode == 0)
   {
	TDateTime dt=TDateTime::CurrentDateTime();
	MModel_SetDoubleSourceTime(i,dt.operator double());
   }
   else
   if(CalculationTimeSourceMode == 1)
	MModel_SetDoubleSourceTime(i,ServerTimeStamp[i]/(86400.0*1000.0));
   switch(CalculateMode[i])
   {
   case 0:
	MEnv_Calculate(i,0);
   break;

   case 1:
	MEnv_RTCalculate(i);
   break;

   case 2:
	MEnv_Calculate(i,0);
   break;
   }
   LastCalculatedServerTimeStamp[i]=ServerTimeStamp[i];
//   IdTcpResultBroadcasterForm->AddMetadata(i, ServerTimeStamp[i]);
   AddMetadata(i, ServerTimeStamp[i]);
   RealLastCalculationTime[i]=RDK::GetCurrentStartupTime();
  }
//  IdTcpResultBroadcasterForm->SendMetadata();
  SendMetadata();
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 break;

 case 1:
 {
  SendMetadata();
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::ResetCalculationStepUpdatedFlag();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 break;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Step1Click(TObject *Sender)
{
 for(int i=0;i<GetNumChannels();i++)
  SetServerTimeStamp(i,RDK::GetCurrentStartupTime());
 TimerTimer(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::SaveClassesDescriptions1Click(TObject *Sender)

{
 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Visible=false;
 RichEdit->Parent=this;

 RichEdit->PlainText=true;
 const char *p=Storage_SaveAllClassesDescription();
 if(p)
  RichEdit->Text=p;
 Engine_FreeBufString(p);
 RichEdit->Lines->SaveToFile("ClassesDescription.xml");
}
//---------------------------------------------------------------------------


void __fastcall TUEngineMonitorFrame::LoadAllClassesDescriptions1Click(TObject *Sender)

{
 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Parent=this;
 RichEdit->PlainText=true;
 RichEdit->Visible=false;
 RichEdit->Lines->LoadFromFile("ClassesDescription.xml");

 Storage_LoadClassesDescription(AnsiString(RichEdit->Text).c_str());
 Storage_LoadCommonClassesDescription(AnsiString(RichEdit->Text).c_str());

 delete RichEdit;
}
//---------------------------------------------------------------------------



void __fastcall TUEngineMonitorFrame::RichEditMouseEnter(TObject *Sender)
{
 RichEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::ShowDebugMessagesCheckBoxClick(TObject *Sender)

{
 if(UpdateInterfaceFlag)
  return;
 int size=GetNumEngines();
 for(int i=0;i<size;i++)
 {
  UGEngineControlForm->DebugModeFlag[i]=ShowDebugMessagesCheckBox->Checked;
  MEnv_SetDebugMode(i,UGEngineControlForm->DebugModeFlag[i]);
 }
}
//---------------------------------------------------------------------------

