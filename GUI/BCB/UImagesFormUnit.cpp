//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UImagesFormUnit.h"
#include "UEngineMonitorFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UImagesFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUImagesForm *UImagesForm;
//---------------------------------------------------------------------------
__fastcall TUImagesForm::TUImagesForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// Создание копии этого компонента
TUImagesForm* TUImagesForm::New(TComponent *owner)
{
 return new TUImagesForm(owner);
}

