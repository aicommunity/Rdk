//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TImportModelFromXmlFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TImportModelFromXmlFrameUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TImportModelFromXmlForm *ImportModelFromXmlForm;
//---------------------------------------------------------------------------
__fastcall TImportModelFromXmlForm::TImportModelFromXmlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
