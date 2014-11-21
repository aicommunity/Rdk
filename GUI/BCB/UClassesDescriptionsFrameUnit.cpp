//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UClassesDescriptionsFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UClassesListFrameUnit"
#pragma resource "*.dfm"
TUClassesDescriptionsFrame *UClassesDescriptionsFrame;
//---------------------------------------------------------------------------
__fastcall TUClassesDescriptionsFrame::TUClassesDescriptionsFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
}
//---------------------------------------------------------------------------
