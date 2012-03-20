//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TVideoGrabberControlFrameUnit"
#pragma resource "*.dfm"
TVideoGrabberControlForm *VideoGrabberControlForm;
//---------------------------------------------------------------------------
__fastcall TVideoGrabberControlForm::TVideoGrabberControlForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

