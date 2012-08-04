//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentsControlFrameUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma resource "*.dfm"
TUComponentsControlForm *UComponentsControlForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsControlForm::TUComponentsControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlForm::FormShow(TObject *Sender)
{
 ComponentsControlFrame->UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUComponentsControlForm::ComponentsListFrameStringGridClick(TObject *Sender)
{
 ComponentsControlFrame->ComponentsListFrame->StringGridClick(Sender);
 if(ComponentsControlFrame->ComponentsListFrame->UpdateInterfaceFlag)
  return;
 UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlForm::ComponentsListFrameStringGridDblClick(TObject *Sender)
{
 ComponentsControlFrame->ComponentsListFrame->StringGridDblClick(Sender);
 if(ComponentsControlFrame->ComponentsListFrame->UpdateInterfaceFlag)
  return;
 UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlForm::ComponentsListFrameStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 ComponentsControlFrame->ComponentsListFrame->StringGridSelectCell(Sender,
		  ACol, ARow, CanSelect);
 if(ComponentsControlFrame->ComponentsListFrame->UpdateInterfaceFlag)
  return;
 UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsControlForm::ComponentsListFrameStringGridKeyPress(TObject *Sender,
		  System::WideChar &Key)
{
 ComponentsControlFrame->ComponentsListFrame->StringGridKeyPress(Sender, Key);
 UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlForm::ClassesListFrameStringGridDblClick(TObject *Sender)

{
 ComponentsControlFrame->ClassesListFrameStringGridDblClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlForm::ClassesListFrameStringGridKeyPress(TObject *Sender,
		  System::WideChar &Key)
{
 ComponentsControlFrame->ClassesListFrameStringGridKeyPress(Sender,Key);
}
//---------------------------------------------------------------------------

