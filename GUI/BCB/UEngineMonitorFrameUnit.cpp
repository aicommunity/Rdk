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

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorFrame *UEngineMonitorFrame;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineThread::TEngineThread(int channel_index, int calculate_mode, int min_inerval, bool CreateSuspended)
: ChannelIndex(channel_index), CalculateMode(calculate_mode), MinInterstepsInterval(min_inerval), TThread(CreateSuspended)
{
 CalcEnable=CreateEvent(0,TRUE,0,0);
 CalcStarted=CreateEvent(0,TRUE,0,0);
 CalculationNotInProgress=CreateEvent(0,TRUE,TRUE,0);
 RealLastCalculationTime=0;
}

__fastcall TEngineThread::~TEngineThread(void)
{
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

bool TEngineThread::SetMinInterstepsInterval(int value)
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
 TVideoOutputFrame* video=VideoOutputForm->GetVideoOutputFrame(ChannelIndex);
 if(video)
 {
/*  if(video->CaptureThread)
  {
   long long time_stamp=0;
   video->CaptureThread->ReadSourceSafe(Source,time_stamp,true);
  }*/
 }
 else
  Source.Clear();
#endif

#endif
}

void __fastcall TEngineThread::AfterCalculate(void)
{
 UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]=
  UEngineMonitorForm->EngineMonitorFrame->GetServerTimeStamp(ChannelIndex);

 IdTcpResultBroadcasterForm->AddMetadata(ChannelIndex, UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]);
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



void __fastcall TEngineThread::Execute(void)
{
 while(!Terminated)
 {
//  Synchronize(BeforeCalculate);
//  BeforeCalculate();
  if(WaitForSingleObject(CalcStarted,30) == WAIT_TIMEOUT)
   continue;
  if(CalculateMode == 2)
  {
   if(WaitForSingleObject(CalcEnable,30) == WAIT_TIMEOUT)
	continue;
  }
  else
  if(CalculateMode == 0)
  {
   unsigned long long diff=GetTickCount()-RealLastCalculationTime;
   if(diff<MinInterstepsInterval)
   {
	Sleep(MinInterstepsInterval-diff);
	continue;
   }
  }
  ResetEvent(CalcEnable);
  ResetEvent(CalculationNotInProgress);
  BeforeCalculate();
  if(GetNumEngines()>ChannelIndex)
  {
   TDateTime dt=TDateTime::CurrentDateTime();
   MModel_SetDoubleSourceTime(ChannelIndex,dt.operator double());
   #ifdef RDK_VIDEO
   TVideoOutputFrame* video=VideoOutputForm->GetVideoOutputFrame(ChannelIndex);
   if(video)
	video->BeforeCalculate();
   //   MModel_SetComponentBitmapOutput(ChannelIndex, "", "Output", &Source,false);
   #endif
   MEnv_Calculate(ChannelIndex,0);
  }
  AfterCalculate();
  RealLastCalculationTime=GetTickCount();
  SetEvent(CalculationNotInProgress);
 }
}
// --------------------------


//---------------------------------------------------------------------------
__fastcall TUEngineMonitorFrame::TUEngineMonitorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 CalculateMode.assign(GetNumEngines(),0);
 CalculateSignal.assign(GetNumEngines(),false);
 MinInterstepsInterval.assign(GetNumEngines(),0);
 AlwaysUpdateFlag=true;
 UpdateInterval=100;
 ChannelsMode=0;
}

__fastcall TUEngineMonitorFrame::~TUEngineMonitorFrame(void)
{
// SetNumChannels(0);
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
 if(ThreadChannels.size()>channel_index)
  ThreadChannels[channel_index]->SetCalculateMode(value);
}

void TUEngineMonitorFrame::SetMinInterstepsInterval(int channel_index, int value)
{
 MinInterstepsInterval[channel_index]=value;
 if(ThreadChannels.size()>channel_index)
  ThreadChannels[channel_index]->SetMinInterstepsInterval(value);
}

// Управление временной меткой сервера
long long TUEngineMonitorFrame::GetServerTimeStamp(int channel_index) const
{
 return ServerTimeStamp[channel_index];
}

void TUEngineMonitorFrame::SetServerTimeStamp(int channel_index, long long stamp)
{
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
  ThreadChannels[i]->FreeOnTerminate=false;
 }

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

 StatusBar->Repaint();
 StatusBar->Update();
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
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Start1Click(TObject *Sender)
{
 UShowProgressBarForm->SetBarHeader(1,"Starting Channel Calculation...");
 UShowProgressBarForm->ResetBarStatus(1, 1, int(ThreadChannels.size()));

 switch(ChannelsMode)
 {
 case 0:
  if(CalculateMode[GetSelectedEngineIndex()] == 1)
   Reset1Click(Sender);
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=1;
  Timer->Enabled=true;
 break;

 case 1:
//  for(int i=0;i<GetNumChannels();i++)
//   ThreadChannels[i]->Start();//Resume();
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=30;
  Timer->Enabled=true;
  for(size_t i=0;i<ThreadChannels.size();i++)
  {
   if(ThreadChannels[i])
   {
    SetEvent(ThreadChannels[i]->CalcStarted);
	WaitForSingleObject(ThreadChannels[i]->CalculationNotInProgress,1000);
   }
   UShowProgressBarForm->IncBarStatus(1);
  }
 break;
 }

}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Pause1Click(TObject *Sender)
{
 if(Timer->Enabled == false)
  return;

 UShowProgressBarForm->SetBarHeader(1,"Stopping Channel Calculation...");
 UShowProgressBarForm->ResetBarStatus(1, 1, int(ThreadChannels.size()));

 switch(ChannelsMode)
 {
 case 0:
  Timer->Enabled=false;
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
 break;

 case 1:
  Timer->Enabled=false;
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  for(int i=0;i<GetNumChannels();i++)
  {
//   ThreadChannels[i]->Terminate();//Suspend();
//   ThreadChannels[i]->WaitFor();
  }
   //Suspend();
  for(size_t i=0;i<ThreadChannels.size();i++)
  {
   if(ThreadChannels[i])
   {
    ResetEvent(ThreadChannels[i]->CalcStarted);
	WaitForSingleObject(ThreadChannels[i]->CalculationNotInProgress,1000);
   }
   UShowProgressBarForm->IncBarStatus(1);
  }
 break;
 }

}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Reset1Click(TObject *Sender)
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
	if(GetTickCount()-RealLastCalculationTime[i]<MinInterstepsInterval[i])
	 continue;
   }
   CalculateSignal[i]=false;

   TDateTime dt=TDateTime::CurrentDateTime();
   MModel_SetDoubleSourceTime(i,dt.operator double());
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
   IdTcpResultBroadcasterForm->AddMetadata(i, ServerTimeStamp[i]);
   RealLastCalculationTime[i]=GetTickCount();
  }
  IdTcpResultBroadcasterForm->SendMetadata();
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 break;

 case 1:
 {
  IdTcpResultBroadcasterForm->SendMetadata();
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
 for(size_t i=0;i<GetNumChannels();i++)
  SetServerTimeStamp(i,GetTickCount());
 TimerTimer(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::SaveClassesDescriptions1Click(TObject *Sender)

{
 TRichEdit* RichEdit=new TRichEdit(this);
 RichEdit->Visible=false;
 RichEdit->Parent=this;

 RichEdit->PlainText=true;
 RichEdit->Text=Storage_SaveAllClassesDescription();
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

