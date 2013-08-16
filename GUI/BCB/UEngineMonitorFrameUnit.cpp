//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UEngineMonitorFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "rdk_initdll.h"
#ifdef RDK_VIDEO
#include "VideoOutputFormUnit.h"
#endif

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorFrame *UEngineMonitorFrame;

// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TEngineThread::TEngineThread(int channel_index, bool CreateSuspended)
: ChannelIndex(channel_index), TThread(CreateSuspended)
{
 CalcEnable=CreateEvent(0,TRUE,0,0);
}

__fastcall TEngineThread::~TEngineThread(void)
{
 CloseHandle(CalcEnable);
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
  Source=video->BmpSource;
 else
  Source.Clear();
#endif

#endif
}

void __fastcall TEngineThread::AfterCalculate(void)
{
 UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]=
 UEngineMonitorForm->EngineMonitorFrame->GetServerTimeStamp(ChannelIndex);
 RDK::UIVisualControllerStorage::AfterCalculate();
}



void __fastcall TEngineThread::Execute(void)
{
 while(!Terminated)
 {
//  Synchronize(BeforeCalculate);
  if(WaitForSingleObject(CalcEnable,30) == WAIT_TIMEOUT)
   continue;
  ResetEvent(CalcEnable);

  Synchronize(BeforeCalculate);
  MModel_SetComponentBitmapOutput(ChannelIndex, "", 0, &Source,true);
  MEnv_Calculate(ChannelIndex,0);
  Synchronize(AfterCalculate);
 }
}
// --------------------------


//---------------------------------------------------------------------------
__fastcall TUEngineMonitorFrame::TUEngineMonitorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 CalculateMode.assign(GetNumEngines(),0);
 CalculateSignal.assign(GetNumEngines(),false);
 AlwaysUpdateFlag=true;
 UpdateInterval=100;
 ChannelsMode=0;
}

__fastcall TUEngineMonitorFrame::~TUEngineMonitorFrame(void)
{
 SetNumChannels(0);
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

 SetNumEngines(num);
 CalculateMode.resize(num,0);
 CalculateSignal.resize(num,false);
 ServerTimeStamp.resize(num,0);
 LastCalculatedServerTimeStamp.resize(num,0);

 int old_size=int(ThreadChannels.size());
 for(int i=num;i<old_size;i++)
 {
  ThreadChannels[i]->Terminate();
//  ThreadChannels[i]->WaitFor();
  delete ThreadChannels[i];
 }

 ThreadChannels.resize(num);
 for(int i=old_size;i<num;i++)
  ThreadChannels[i]=new TEngineThread(i,true);

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
  for(int i=0;i<GetNumChannels();i++)
   ThreadChannels[i]->Resume();
  Timer->Interval=30;
  Timer->Enabled=true;
 break;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Pause1Click(TObject *Sender)
{
 switch(ChannelsMode)
 {
 case 0:
  Timer->Enabled=false;
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
 break;

 case 1:
  Timer->Enabled=false;
  for(int i=0;i<GetNumChannels();i++)
   ThreadChannels[i]->Suspend();
   //Suspend();
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
  MEnv_Reset(i,0);
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
   CalculateSignal[i]=false;

   RDK::UIVisualControllerStorage::BeforeCalculate();

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
  }
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 break;

 case 1:
 {
//  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 break;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Step1Click(TObject *Sender)
{
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

