//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TLoaderFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLoaderForm *LoaderForm;
//---------------------------------------------------------------------------
__fastcall TLoaderForm::TLoaderForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoaderForm::FormCreate(TObject *Sender)
{
 Application->ShowMainForm = false;
 SetWindowLong(Handle, GWL_HWNDPARENT, 0);
 SetWindowLong(Application->Handle, GWL_HWNDPARENT, (long)Handle);
}
//---------------------------------------------------------------------------
void __fastcall TLoaderForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 Application->Terminate();
}
//---------------------------------------------------------------------------
