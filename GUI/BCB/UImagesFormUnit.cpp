//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UImagesFormUnit.h"
#include "UEngineMonitorFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UImagesFrameUnit"
#pragma resource "*.dfm"
TUImagesForm *UImagesForm;
//---------------------------------------------------------------------------
__fastcall TUImagesForm::TUImagesForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}



