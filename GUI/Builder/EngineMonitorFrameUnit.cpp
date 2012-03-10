//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EngineMonitorFrameUnit.h"
#include "nmsdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEngineMonitorFrame *EngineMonitorFrame;

//---------------------------------------------------------------------------
__fastcall TEngineMonitorFrame::TEngineMonitorFrame(TComponent* Owner)
	: TFrame(Owner)
{
 UpdateInterfaceFlag=false;
}
//---------------------------------------------------------------------------

void TEngineMonitorFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;

 StatusBar->SimpleText=String("Model Time=")+FloatToStrF(Model_GetDoubleTime(),ffFixed,3,3)
				+String("; Real Time=")+FloatToStrF(Model_GetDoubleRealTime(),ffFixed,3,3);
 UpdateInterfaceFlag=false;
}
