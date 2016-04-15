//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TImportModelFromXmlFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TImportModelFromXmlFrame *ImportModelFromXmlFrame;
//---------------------------------------------------------------------------
__fastcall TImportModelFromXmlFrame::TImportModelFromXmlFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
}
//---------------------------------------------------------------------------
