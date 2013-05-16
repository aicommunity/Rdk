//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsControlFormUnit.h"
#include "UComponentLinksFormUnit.h"
#include "UDrawEngineFormUnit.h"
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

// Создание копии этого компонента
TUComponentsControlForm* TUComponentsControlForm::New(TComponent *owner)
{
 return new TUComponentsControlForm(owner);
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
 UDrawEngineForm->SelectComponent(ComponentsControlFrame->ComponentsListFrame->GetSelectedComponentName());
/* if(ComponentsControlFrame->ComponentsListFrame->UpdateInterfaceFlag)
  return;
 //UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
 UDrawEngineForm->SelectComponent(ComponentsControlFrame->ComponentsListFrame->GetSelectedComponentName());
*/
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlForm::ComponentsListFrameStringGridDblClick(TObject *Sender)
{
 ComponentsControlFrame->ComponentsListFrame->StringGridDblClick(Sender);
 if(ComponentsControlFrame->ComponentsListFrame->UpdateInterfaceFlag)
  return;
 UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
 UDrawEngineForm->SetNet(ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlForm::ComponentsListFrameStringGridSelectCell(TObject *Sender,
		  int ACol, int ARow, bool &CanSelect)
{
 ComponentsControlFrame->ComponentsListFrame->StringGridSelectCell(Sender,
		  ACol, ARow, CanSelect);
 if(ComponentsControlFrame->ComponentsListFrame->UpdateInterfaceFlag)
  return;
// UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
 UDrawEngineForm->SelectComponent(ComponentsControlFrame->ComponentsListFrame->GetSelectedComponentName());
}
//---------------------------------------------------------------------------


void __fastcall TUComponentsControlForm::ComponentsListFrameStringGridKeyPress(TObject *Sender,
		  System::WideChar &Key)
{
 ComponentsControlFrame->ComponentsListFrame->StringGridKeyPress(Sender, Key);
 UComponentLinksForm->UComponentLinksFrame->UpdateInterface();
 if(Key == VK_RETURN)
  UDrawEngineForm->SetNet(ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
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


void __fastcall TUComponentsControlForm::ComponentsControlFrameTakeObjectButtonClick(TObject *Sender)

{
  ComponentsControlFrame->TakeObjectButtonClick(Sender);
  UDrawEngineForm->SetNet(ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsControlForm::ComponentsControlFrameReturnObjectButtonClick(TObject *Sender)

{
  ComponentsControlFrame->ReturnObjectButtonClick(Sender);
  UDrawEngineForm->SetNet(ComponentsControlFrame->ComponentsListFrame->GetCurrentComponentName());
}
//---------------------------------------------------------------------------

