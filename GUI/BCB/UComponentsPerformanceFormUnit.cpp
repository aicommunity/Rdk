//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsPerformanceFormUnit.h"
#include "UEngineMonitorFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentsPerformanceFrameUnit"
#pragma resource "*.dfm"
TUComponentsPerformanceForm *UComponentsPerformanceForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsPerformanceForm::TUComponentsPerformanceForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------


