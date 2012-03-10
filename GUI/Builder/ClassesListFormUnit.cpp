//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClassesListFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ClassesListFrameUnit"
#pragma resource "*.dfm"
TClassesListForm *ClassesListForm;
//---------------------------------------------------------------------------
__fastcall TClassesListForm::TClassesListForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
