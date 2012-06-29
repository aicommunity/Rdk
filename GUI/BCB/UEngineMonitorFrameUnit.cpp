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
 StatusBar->SimpleText=String("Model Time=")+FloatToStrF(Model_GetDoubleTime(),ffFixed,3,3)
				+String("; Real Time=")+FloatToStrF(Model_GetDoubleRealTime(),ffFixed,3,3)
				+String("; Model Duration Time=")+FloatToStrF(Model_GetFullStepDuration("")/1000.0,ffFixed,3,3)
				+String("; Model Performance=")+FloatToStrF(Model_GetInstantPerformance(""),ffFixed,3,3);
 StatusBar->Repaint();
 StatusBar->Update();
}

// Сохраняет параметры интерфейса в xml
void TUEngineMonitorFrame::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.WriteInteger("CalculateMode",GetCalculateMode());
}

// Загружает параметры интерфейса из xml
void TUEngineMonitorFrame::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 SetCalculateMode(xml.ReadInteger("CalculateMode",0));
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Start1Click(TObject *Sender)
{
 if(CalculateMode == 1)
  Reset1Click(Sender);
 Timer->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Pause1Click(TObject *Sender)
{
 Timer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::Reset1Click(TObject *Sender)
{
 Env_Reset(0);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorFrame::TimerTimer(TObject *Sender)
{
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

