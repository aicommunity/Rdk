//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UClassesDescriptionsFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UClassesDescriptionsFrameUnit"
#pragma resource "*.dfm"
TUClassesDescriptionsForm *UClassesDescriptionsForm;
//---------------------------------------------------------------------------
__fastcall TUClassesDescriptionsForm::TUClassesDescriptionsForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
