//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UClassesListFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UClassesListFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUClassesListForm *UClassesListForm;
//---------------------------------------------------------------------------
__fastcall TUClassesListForm::TUClassesListForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
