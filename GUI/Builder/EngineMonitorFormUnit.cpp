//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EngineMonitorFormUnit.h"
#include "nmsdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "EngineMonitorFrameUnit"
#pragma resource "*.dfm"
TEngineMonitorForm *EngineMonitorForm;

//---------------------------------------------------------------------------

void ExceptionHandler(void)
{
 EngineMonitorForm->EngineMonitorFrame->RichEdit->Text=Engine_GetLog();
}
//---------------------------------------------------------------------------
__fastcall TEngineMonitorForm::TEngineMonitorForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
