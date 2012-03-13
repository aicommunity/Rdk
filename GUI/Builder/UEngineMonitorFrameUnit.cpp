//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEngineMonitorFrameUnit.h"
#include "nmsdk_initdll.h"
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
				+String("; Real Time=")+FloatToStrF(Model_GetDoubleRealTime(),ffFixed,3,3);
 UpdateInterfaceFlag=false;
}
