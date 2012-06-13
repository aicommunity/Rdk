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
	: TForm(Owner)
{
}

void __fastcall TUImagesForm::FormShow(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->AddInterface(ImagesFrame);
}
//---------------------------------------------------------------------------

void __fastcall TUImagesForm::FormHide(TObject *Sender)
{
 if(UEngineMonitorForm)
  UEngineMonitorForm->DelInterface(ImagesFrame);
}
//---------------------------------------------------------------------------

