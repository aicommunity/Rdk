//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "VideoOutputFormUnit.h"
#include "MainFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VideoOutputFrameUnit"
#pragma resource "*.dfm"
TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
__fastcall TVideoOutputForm::TVideoOutputForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TVideoOutputForm::VideoOutputFrame1ImageMouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
  VideoOutputFrame->ImageMouseUp(Sender,Button,
		  Shift, X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TVideoOutputForm::VideoOutputFrameN1Click(TObject *Sender)
{
  VideoOutputFrame->N1Click(Sender);
  VideoOutputFrame->MyVideoOutputToolsForm->Top=Top;
  VideoOutputFrame->MyVideoOutputToolsForm->Left=Left+Width;
}
//---------------------------------------------------------------------------


