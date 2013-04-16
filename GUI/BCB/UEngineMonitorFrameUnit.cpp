//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <algorithm>
#include "UEngineMonitorFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorFrame *UEngineMonitorFrame;

//---------------------------------------------------------------------------
__fastcall TUEngineMonitorFrame::TUEngineMonitorFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 CalculateMode=0;
 AlwaysUpdateFlag=true;
 UpdateInterval=100;
}

// Управление режимом расчетов
int TUEngineMonitorFrame::GetCalculateMode(void) const
{
 return CalculateMode;
}

void TUEngineMonitorFrame::SetCalculateMode(int value)
{
 CalculateMode=value;
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
 if(CalculateMode == 1)
  Reset1Click(Sender);
 TUVisualControllerFrame::CalculationModeFlag=true;
 TUVisualControllerForm::CalculationModeFlag=true;
 Timer->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Pause1Click(TObject *Sender)
{
 Timer->Enabled=false;
 TUVisualControllerFrame::CalculationModeFlag=false;
 TUVisualControllerForm::CalculationModeFlag=false;
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
 Env_Reset(0);
 RDK::UIVisualControllerStorage::AfterReset();
 RDK::UIVisualControllerStorage::UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::TimerTimer(TObject *Sender)
{
 if(!Model_Check())
 {
  Timer->Enabled=false;
  RDK::UIVisualControllerStorage::UpdateInterface();
  return;
 }

 RDK::UIVisualControllerStorage::BeforeCalculate();

 switch(CalculateMode)
 {
 case 0:
  Env_Calculate(0);
 break;

 case 1:
  Env_RTCalculate();
 break;
 }

 RDK::UIVisualControllerStorage::AfterCalculate();
 RDK::UIVisualControllerStorage::UpdateInterface();
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

