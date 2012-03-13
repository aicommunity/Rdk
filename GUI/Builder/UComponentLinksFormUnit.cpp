//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentLinksFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentLinksFrameUnit"
#pragma resource "*.dfm"
TUComponentLinksForm *UComponentLinksForm;
//---------------------------------------------------------------------------
__fastcall TUComponentLinksForm::TUComponentLinksForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
