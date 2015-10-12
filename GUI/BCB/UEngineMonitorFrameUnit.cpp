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

/// Экземпляр класса контроллера расчета
extern UEngineControlVcl RdkEngineControl;


// --------------------------
// Конструкторы и деструкторы
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
// Управление параметрами
// --------------------------
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний источников видеозахвата
std::vector<int> TEngineMonitorThread::ReadVideoCaptureStates(void) const
{
 return VideoCaptureStates;
}
// --------------------------


// --------------------------
// Управление потоком
// --------------------------
/// Возвращает класс-владелец потока
UEngineControlVcl* TEngineMonitorThread::GetEngineControl(void)
{
 return dynamic_cast<UEngineControlVcl*>(EngineControl);
}

void TEngineMonitorThread::AdditionExecute(void)
{
  // Определяем состояние тредов захвата видео
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
// Конструкторы и деструкторы
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
// Управление параметрами
// --------------------------
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
/// Возвращает класс-владелец потока
UEngineControlVcl* TEngineThread::GetEngineControl(void)
{
 return dynamic_cast<UEngineControlVcl*>(EngineControl);
}

void TEngineThread::ABeforeCalculate(void)
{
   #ifdef RDK_VIDEO
   if(GetNumEngines() == 1)
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

void TEngineThread::AAfterCalculate(void)
{
 GetEngineControl()->AddMetadata(EngineIndex, GetEngineControl()->GetEngineThread(EngineIndex)->GetLastCalculationServerTimeStamp());
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineControlVcl::UEngineControlVcl(void)
{

}

UEngineControlVcl::~UEngineControlVcl(void)
{

}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Создание нового треда расчета
RDK::UEngineControlThread* UEngineControlVcl::CreateEngineThread(RDK::UEngineControl* engine_control, int engine_index)
{
 return new TEngineThread(engine_control, engine_index);
}

/// Создание нового треда расчета
RDK::UEngineStateThread* UEngineControlVcl::CreateEngineStateThread(RDK::UEngineControl* engine_control)
{
 return new TEngineMonitorThread(engine_control);
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

 double fps=0;
 double instperf=Model_GetInstantPerformance("");
 if(fabs(instperf)>0.0001)
  fps=Model_GetTimeStep("")/instperf;

 double rt_calc_duration(0.0);
 double rt_model_duration(0.0), rt_performance(0.0);

 if(RdkEngineControl.GetThreadMode() == 1)
 {
  rt_calc_duration=Env_GetRTLastDuration();
  rt_model_duration=Env_GetRTModelCalcTime();
  rt_performance=Env_CalcRTPerformance();
 }

 double model_time=Model_GetDoubleTime();
 double real_time=Model_GetDoubleRealTime();
 double diff=real_time-model_time;
 if(RdkEngineControl.GetThreadMode() == 1)
 {
  string str_model_time=RDK::get_text_time_from_seconds(model_time);
  string str_diff_time=RDK::get_text_time_from_seconds(diff);
  StatusBar->Panels->Items[0]->Text=String("Time: ")+str_model_time.c_str();
  if(diff>0)
   StatusBar->Panels->Items[0]->Text=StatusBar->Panels->Items[0]->Text+String(" (")+String(str_diff_time.c_str())+String(")");
  int width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[0]->Text)+25;
  if(width<100)
   width=100;
  StatusBar->Panels->Items[0]->Width=width;
 }
 else
 {
  string str_real_time=RDK::get_text_time_from_seconds(real_time);
  StatusBar->Panels->Items[0]->Text=String("Time: ")+str_real_time.c_str();
  int width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[0]->Text)+25;
  StatusBar->Panels->Items[0]->Width=width;
 }

 if(RdkEngineControl.GetThreadMode() == 1)
 {
  StatusBar->Panels->Items[1]->Text=String("RT: ")+FloatToStrF(rt_performance,ffFixed,3,3)+String("=")+FloatToStrF(rt_model_duration,ffFixed,3,3)+String("/")+FloatToStrF(rt_calc_duration,ffFixed,3,3);
 }
 else
 {
  double full_step=Model_GetFullStepDuration("")/1000.0;
  double interstep=Model_GetInterstepsInterval("")/1000.0;
  StatusBar->Panels->Items[1]->Text=String("Step: ")+FloatToStrF(full_step,ffFixed,3,3)+"/"+FloatToStrF(full_step+interstep,ffFixed,3,3)+"s";
 }
 int width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[1]->Text)+25;
 StatusBar->Panels->Items[1]->Width=width;

 if(RdkEngineControl.GetThreadMode() != 1)
 {
  if(instperf)
  {
   StatusBar->Panels->Items[2]->Text=String("Perf: ")+
				FloatToStrF(instperf,ffFixed,3,1)+String(" (")+
				FloatToStrF(fps,ffFixed,3,1)+String(" Fps)");
   width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[2]->Text)+25;
   StatusBar->Panels->Items[2]->Width=width;
  }
  else
  {
   StatusBar->Panels->Items[2]->Text="";
  }
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
				String("Logs: [")+IntToStr(Engine_GetNumUnreadLogLines())+String(":")+IntToStr(Engine_GetNumLogLines())+String("]");
 width=StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[4]->Text)+25;
 StatusBar->Panels->Items[4]->Width=width;
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
 xml.WriteBool("AutoupdateProperties",AutoupdatePropertiesCheckBox->Checked);
// xml.WriteInteger("CalculateMode",GetCalculateMode());
}

// Загружает параметры интерфейса из xml
void TUEngineMonitorFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 AutoupdatePropertiesCheckBox->Checked=xml.ReadBool("AutoupdateProperties",AutoupdatePropertiesCheckBox->Checked);
 AutoupdatePropertiesCheckBoxClick(this);
// SetCalculateMode(xml.ReadInteger("CalculateMode",0));
}


/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
void TUEngineMonitorFrame::StartChannel(int channel_index)
{
 RdkEngineControl.StartEngine(channel_index);
 switch(RdkEngineControl.GetThreadMode())
 {
 case 0:
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=1;
  Timer->Enabled=true;
 break;

 case 1:
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=30;
  Timer->Enabled=true;
 break;
 }

}

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
void TUEngineMonitorFrame::PauseChannel(int channel_index)
{
 RdkEngineControl.PauseEngine(channel_index);
 switch(RdkEngineControl.GetThreadMode())
 {
 case 0:
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  Timer->Enabled=false;
 break;

 case 1:
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  Timer->Enabled=false;
 break;
 }
}

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
void TUEngineMonitorFrame::ResetChannel(int channel_index)
{
 RdkEngineControl.ResetEngine(channel_index);
}

//---------------------------------------------------------------------------
/// Проверяет состояние расчета
/// 0 - Не считает
/// 1 - Идет расчет
int TUEngineMonitorFrame::CheckCalcState(int channel_id) const
{
 return RdkEngineControl.CheckCalcState(channel_id);
}

/// Доступ к треду мониторинга состояния модулей сервера
const TEngineMonitorThread* TUEngineMonitorFrame::GetEngineMonitorThread(void) const
{
 return dynamic_cast<TEngineMonitorThread*>(RdkEngineControl.GetEngineStateThread());
}

TEngineThread* TUEngineMonitorFrame::GetThreadChannel(int i)
{
 return dynamic_cast<TEngineThread*>(RdkEngineControl.GetEngineThread(i));
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
 switch(RdkEngineControl.GetThreadMode())
 {
 case 0:
 {
  RDK::UIVisualControllerStorage::BeforeCalculate();

  for(int i=0;i<GetNumEngines();i++)
  {
   RdkEngineControl.GetEngineThread(i)->Calculate();
  }
  RdkEngineControl.SendMetadata();
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 break;

 case 1:
 {
  RdkEngineControl.SendMetadata();
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
 for(int i=0;i<GetNumEngines();i++)
  RdkEngineControl.SetServerTimeStamp(i,RDK::GetCurrentStartupTime());
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
 if(UGEngineControlForm->DebugModeFlag.size() != size)
 {
  ShowDebugMessagesCheckBox->Checked=false;
  return;
 }
 for(int i=0;i<size;i++)
 {
  UGEngineControlForm->DebugModeFlag[i]=ShowDebugMessagesCheckBox->Checked;
  MEnv_SetDebugMode(i,UGEngineControlForm->DebugModeFlag[i]);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::AutoupdatePropertiesCheckBoxClick(TObject *Sender)

{
 if(UGEngineControlForm)
 {
  if(AutoupdatePropertiesCheckBox->Checked == true)
   UGEngineControlForm->UComponentsListFrame1->UpdateInterval=100;
  else
   UGEngineControlForm->UComponentsListFrame1->UpdateInterval=-1;
 }
}
//---------------------------------------------------------------------------

