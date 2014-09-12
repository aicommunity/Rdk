//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TApplicationOptionsFormUnit.h"
#include "UGEngineControlFormUnit.h"
#include "UServerControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TApplicationOptionsFrameUnit"
#pragma link "TServerOptionsFrameUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TApplicationOptionsForm *ApplicationOptionsForm;
//---------------------------------------------------------------------------
__fastcall TApplicationOptionsForm::TApplicationOptionsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TApplicationOptionsForm::RestoreButtonClick(TObject *Sender)
{
 ApplicationOptionsFrame->AUpdateInterface();
 ServerOptionsFrame->AUpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TApplicationOptionsForm::OKButtonClick(TObject *Sender)
{
 ApplicationOptionsFrame->ApplyOptions();
 if(UServerControlForm && UGEngineControlForm)
 {
  UServerControlForm->SetServerBinding(UGEngineControlForm->ServerInterfaceAddress,UGEngineControlForm->ServerInterfacePort);
  UServerControlForm->UpdateInterface();
 }

 ModalResult=mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TApplicationOptionsForm::CancelButtonClick(TObject *Sender)
{
 ModalResult=mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TApplicationOptionsForm::FormShow(TObject *Sender)
{
 RestoreButtonClick(Sender);
}
//---------------------------------------------------------------------------

