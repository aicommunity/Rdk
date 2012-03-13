//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsListFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentsListFrameUnit"
#pragma resource "*.dfm"
TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsListForm::TUComponentsListForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
