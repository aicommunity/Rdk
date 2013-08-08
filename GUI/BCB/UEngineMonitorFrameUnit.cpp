//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UEngineMonitorFrameUnit.h"
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
// ������������ � �����������
// --------------------------
__fastcall TEngineThread::TEngineThread(int channel_index, bool CreateSuspended)
: ChannelIndex(channel_index), TThread(CreateSuspended)
{

}

__fastcall TEngineThread::~TEngineThread(void)
{

}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void __fastcall TEngineThread::Execute(void)
{
 while(!Terminated)
 {
#ifdef RDK_VIDEO
 TVideoOutputFrame* video=VideoOutputForm->GetVideoOutputFrame(ChannelIndex);
 if(video)
  video->BeforeCalculate();
 MEnv_Calculate(ChannelIndex,0);
#endif
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

/// ���������� ������� ������
/// 0 - ������������ (�������������) �����
/// 1 - ������������� �����
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


// ���������� ������� ��������
int TUEngineMonitorFrame::GetCalculateMode(int channel_index) const
{
 return CalculateMode[channel_index];
}

void TUEngineMonitorFrame::SetCalculateMode(int channel_index,int value)
{
 CalculateMode[channel_index]=value;
}


/// ���������� ������ �������
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

 for(int i=num;i<int(ThreadChannels.size());i++)
 {
  ThreadChannels[i]->Terminate();
  ThreadChannels[i]->WaitFor();
  delete ThreadChannels[i];
 }

 ThreadChannels.resize(num);
 for(int i=int(ThreadChannels.size());i<num;i++)
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

// ��������� ��������� ���������� � xml
void TUEngineMonitorFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
// xml.WriteInteger("CalculateMode",GetCalculateMode());
}

// ��������� ��������� ���������� �� xml
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
  Timer->Enabled=true;
 break;

 case 1:
  for(int i=0;i<GetNumChannels();i++)
   ThreadChannels[i]->Resume();
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
  for(int i=0;i<GetNumChannels();i++)
   ThreadChannels[i]->Suspend();
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

   if(CalculateMode[GetSelectedEngineIndex()] == 2)
   {
	if(!CalculateSignal[GetSelectedEngineIndex()])
	 continue;

	CalculateSignal[GetSelectedEngineIndex()]=false;
   }

   RDK::UIVisualControllerStorage::BeforeCalculate();

   switch(CalculateMode[GetSelectedEngineIndex()])
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
  }
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 break;

 case 1:
 {
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

