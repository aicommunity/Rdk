//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentLinksFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentLinksFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentLinksForm *UComponentLinksForm;
//---------------------------------------------------------------------------
__fastcall TUComponentLinksForm::TUComponentLinksForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TUComponentLinksForm::FormShow(TObject *Sender)
{
 UComponentLinksFrame->UpdateInterface();
}
//---------------------------------------------------------------------------

