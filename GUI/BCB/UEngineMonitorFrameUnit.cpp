//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UEngineMonitorFrameUnit.h"
#include "UEngineMonitorFormUnit.h"
#include "TIdTcpResultBroadcasterFormUnit.h"
#include "TIdHttpResultBroadcasterFormUnit.h"
#include "UServerControlFormUnit.h"
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

/// �������� ������ ����������
extern RDK::UApplication RdkApplication;


// --------------------------
// ������������ � �����������
// --------------------------
TEngineMonitorThread::TEngineMonitorThread(RDK::UEngineControl* engine_control)
 : RDK::UEngineStateThread(engine_control)
{
}

TEngineMonitorThread::~TEngineMonitorThread(void)
{
}
// --------------------------


// --------------------------
// ���������� �����������
// --------------------------
// --------------------------

// --------------------------
// ������ ������� � ������ ��������� �������
// --------------------------
/// ���������� ������ ��������� ���������� ������������
std::vector<int> TEngineMonitorThread::ReadVideoCaptureStates(void) const
{
 return VideoCaptureStates;
}
// --------------------------


// --------------------------
// ���������� �������
// --------------------------
/// ���������� �����-�������� ������
UEngineControlVcl* TEngineMonitorThread::GetEngineControl(void)
{
 return dynamic_cast<UEngineControlVcl*>(EngineControl);
}

void TEngineMonitorThread::AdditionExecute(void)
{
  // ���������� ��������� ������ ������� �����
#ifdef RDK_VIDEO
  std::vector<int> video_capture_states;

  int num_captures=(VideoOutputForm)?VideoOutputForm->GetNumSources():0;
  video_capture_states.assign(num_captures,1);
  VideoCaptureStateTime.resize(num_captures,0);
  VideoCaptureSuccessTime.resize(num_captures,0);
  AvgCaptureIterations.resize(num_captures);

  for(int i=0;i<num_captures;i++)
  {
   TVideoCaptureThread *thread=0;

   if(VideoOutputForm && VideoOutputForm->GetVideoOutputFrame(i) && VideoOutputForm->GetVideoOutputFrame(i)->CaptureThread)
	thread=VideoOutputForm->GetVideoOutputFrame(i)->CaptureThread;

   if(thread)
   {
	int connection_state=thread->CheckConnection();
	switch(connection_state)
	{
	case 0:
	 video_capture_states[i]=1;
	break;

	case 1:
	 video_capture_states[i]=1;
	break;

	case 2:
	 video_capture_states[i]=0;
	break;

	case 10:
	 video_capture_states[i]=2;
	break;
	}
   }
   else
   {
	video_capture_states[i]=1;
   }
  }


  VideoCaptureStates=video_capture_states;
#endif

}
// --------------------------


// --------------------------
// ������������ � �����������
// --------------------------
TEngineThread::TEngineThread(RDK::UEngineControl* engine_control, int channel_index)
 : RDK::UEngineControlThread(engine_control, channel_index)
{
}

TEngineThread::~TEngineThread(void)
{
}
// --------------------------


// --------------------------
// ���������� �����������
// --------------------------
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ���������� �����-�������� ������
UEngineControlVcl* TEngineThread::GetEngineControl(void)
{
 return dynamic_cast<UEngineControlVcl*>(EngineControl);
}

void TEngineThread::ABeforeCalculate(void)
{
   #ifdef RDK_VIDEO
   if(Core_GetNumChannels() == 1)
   {
	for(int i=0;i<VideoOutputForm->GetNumSources();i++)
	{
	 TVideoOutputFrame* video=VideoOutputForm->GetVideoOutputFrame(i);
	 if(video)
	 {
	  video->BeforeCalculate();
	 }
    }
   }
   else
   if(EngineIndex<VideoOutputForm->GetNumSources())
   {
	TVideoOutputFrame* video=VideoOutputForm->GetVideoOutputFrame(EngineIndex);
	if(video)
	{
	 video->BeforeCalculate();
	}
   }
   #endif
}
// --------------------------


// --------------------------
// ������������ � �����������
// --------------------------
UEngineControlVcl::UEngineControlVcl(void)
{
 Name="EngineControlVcl";
}

UEngineControlVcl::~UEngineControlVcl(void)
{

}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// �������� ������ ����� �������
RDK::UEngineControlThread* UEngineControlVcl::CreateEngineThread(RDK::UEngineControl* engine_control, int channel_index)
{
 return new TEngineThread(engine_control, channel_index);
}

/// �������� ������ ����� �������
RDK::UEngineStateThread* UEngineControlVcl::CreateEngineStateThread(RDK::UEngineControl* engine_control)
{
 return new TEngineMonitorThread(engine_control);
}

/// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
void UEngineControlVcl::StartChannel(int channel_index)
{
 RDK::UEngineControl::StartChannel(channel_index);
 switch(GetThreadMode())
 {
 case 0:
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  UEngineMonitorForm->EngineMonitorFrame->Timer->Interval=1;
  UEngineMonitorForm->EngineMonitorFrame->Timer->Enabled=true;
 break;

 case 1:
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  UEngineMonitorForm->EngineMonitorFrame->Timer->Interval=GetApplication()->GetProjectConfig().MTUpdateInterfaceInterval;
  UEngineMonitorForm->EngineMonitorFrame->Timer->Enabled=true;
 break;
 }

}

/// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
void UEngineControlVcl::PauseChannel(int channel_index)
{
 RDK::UEngineControl::PauseChannel(channel_index);
 switch(GetThreadMode())
 {
 case 0:
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  UEngineMonitorForm->EngineMonitorFrame->Timer->Enabled=false;
 break;

 case 1:
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  UEngineMonitorForm->EngineMonitorFrame->Timer->Enabled=false;
 break;
 }
}
// --------------------------



//---------------------------------------------------------------------------
__fastcall TUEngineMonitorFrame::TUEngineMonitorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 AlwaysUpdateFlag=true;
 UpdateInterval=100;
}

__fastcall TUEngineMonitorFrame::~TUEngineMonitorFrame(void)
{
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

 int sel_index=Core_GetSelectedChannelIndex();
 RDK::UChannelProfiler * profiler=RdkApplication.GetEngineControl()->GetChannelProfiler(sel_index);
 if(!profiler)
  return;

 if(RdkApplication.GetEngineControl()->GetCalculateMode(sel_index) == 1)
 {
  StatusBar->Panels->Items[0]->Text=profiler->CalcRtTimeText().c_str();
  int width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[0]->Text)+25;
  if(width<100)
   width=100;
  StatusBar->Panels->Items[0]->Width=width;

  StatusBar->Panels->Items[1]->Text=profiler->CalcRtPerfomanceText().c_str();
 }
 else
 {
  StatusBar->Panels->Items[0]->Text=profiler->CalcNormalTimeText().c_str();
  int width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[0]->Text)+25;
  StatusBar->Panels->Items[0]->Width=width;

  StatusBar->Panels->Items[1]->Text=profiler->CalcNormalStepDurationText().c_str();
 }

 int width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[1]->Text)+25;
 StatusBar->Panels->Items[1]->Width=width;


 if(RdkApplication.GetEngineControl()->GetCalculateMode(sel_index) != 1)
 {
   StatusBar->Panels->Items[2]->Text=profiler->CalcNormalFpsText().c_str();
   width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[2]->Text)+25;
   StatusBar->Panels->Items[2]->Width=width;
 }
 else
 {
  StatusBar->Panels->Items[2]->Text="";
  StatusBar->Panels->Items[2]->Width=0;
 }
 StatusBar->Panels->Items[3]->Text=
				String("Bufs: ")+IntToStr(Engine_GetNumBufStrings());
 width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[3]->Text)+25;
 StatusBar->Panels->Items[3]->Width=width;

 StatusBar->Panels->Items[4]->Text=
				String("Logs: [")+IntToStr(MLog_GetNumUnreadLogLines(RDK_GLOB_MESSAGE))+String(":")+IntToStr(MLog_GetNumLogLines(RDK_GLOB_MESSAGE))+String("]");
//				String("Logs: [")+IntToStr(Log_GetNumUnreadLogLines())+String(":")+IntToStr(Log_GetNumLogLines())+String("]");
 width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[4]->Text)+25;
 StatusBar->Panels->Items[4]->Width=width;
 StatusBar->Repaint();
 StatusBar->Update();
}

// ������� ���������� � �������� ���������
void TUEngineMonitorFrame::AClearInterface(void)
{


}

// ��������� ��������� ���������� � xml
void TUEngineMonitorFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// ��������� ��������� ���������� �� xml
void TUEngineMonitorFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
}



//---------------------------------------------------------------------------
/// ������ � ����� ����������� ��������� ������� �������
const TEngineMonitorThread* TUEngineMonitorFrame::GetEngineMonitorThread(void) const
{
 return dynamic_cast<TEngineMonitorThread*>(RdkApplication.GetEngineControl()->GetEngineStateThread());
}

TEngineThread* TUEngineMonitorFrame::GetThreadChannel(int i)
{
 return dynamic_cast<TEngineThread*>(RdkApplication.GetEngineControl()->GetEngineThread(i));
}

//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::TimerTimer(TObject *Sender)
{
 try
 {
  RdkApplication.GetEngineControl()->TimerExecute();
 }
 catch(RDK::UException &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("TUEngineMonitorFrame::TimerTimer - ")+ex.what()).c_str());
 }
 catch(std::exception &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("TUEngineMonitorFrame::TimerTimer - ")+ex.what()).c_str());
 }
 catch(...)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "TUEngineMonitorFrame::TimerTimer - unhandled exception");
 }
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



