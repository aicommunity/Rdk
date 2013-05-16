//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsPerformanceFormUnit.h"
#include "UEngineMonitorFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentsPerformanceFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentsPerformanceForm *UComponentsPerformanceForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsPerformanceForm::TUComponentsPerformanceForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------

// Создание копии этого компонента
TUComponentsPerformanceForm* TUComponentsPerformanceForm::New(TComponent *owner)
{
 return new TUComponentsPerformanceForm(owner);
}


