//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEngineMonitorFrameUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUEngineMonitorFrame *UEngineMonitorFrame;

//---------------------------------------------------------------------------
__fastcall TUEngineMonitorFrame::TUEngineMonitorFrame(TComponent* Owner)
	: TFrame(Owner)
{
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------

void TUEngineMonitorFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 StatusBar->SimpleText=String("Model Time=")+FloatToStrF(Model_GetDoubleTime(),ffFixed,3,3)
				+String("; Real Time=")+FloatToStrF(Model_GetDoubleRealTime(),ffFixed,3,3)
				+String("; Model Duration Time=")+FloatToStrF(Model_GetFullStepDuration("")/1000.0,ffFixed,3,3)
				+String("; Model Performance=")+FloatToStrF(Model_GetInstantPerformance(""),ffFixed,3,3);
 UpdateInterfaceFlag=false;
}
void __fastcall TUEngineMonitorFrame::Start1Click(TObject *Sender)
{
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
 Env_Calculate(0);
 UpdateInterface();
}
//---------------------------------------------------------------------------

