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
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsPerformanceForm::FormShow(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->AddInterface(UComponentsPerformanceFrame);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsPerformanceForm::FormHide(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->DelInterface(UComponentsPerformanceFrame);
}
//---------------------------------------------------------------------------

