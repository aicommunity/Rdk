//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentsControlFrameUnit"
#pragma resource "*.dfm"
TUComponentsControlForm *UComponentsControlForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsControlForm::TUComponentsControlForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlForm::FormShow(TObject *Sender)
{
 ComponentsControlFrame->UpdateInterface();
}
//---------------------------------------------------------------------------
