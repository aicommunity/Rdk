//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DebugControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "DebugControlFrameUnit"
#pragma resource "*.dfm"
TDebugControlForm *DebugControlForm;
//---------------------------------------------------------------------------
__fastcall TDebugControlForm::TDebugControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
