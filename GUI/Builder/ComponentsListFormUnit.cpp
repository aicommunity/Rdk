//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ComponentsListFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ComponentsListFrameUnit"
#pragma resource "*.dfm"
TComponentsListForm *ComponentsListForm;
//---------------------------------------------------------------------------
__fastcall TComponentsListForm::TComponentsListForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
