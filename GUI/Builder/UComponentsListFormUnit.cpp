//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentsListFrameUnit"
#pragma resource "*.dfm"
TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsListForm::TUComponentsListForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
// Метод открытия диалога для выбора компонента
int TUComponentsListForm::ShowComponentSelect(void)
{
 Panel1->Visible;
 Position=poScreenCenter;
 return ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListForm::FormShow(TObject *Sender)
{
 ComponentsListFrame1->UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListForm::ComponentsListFrame1StringGridDblClick(TObject *Sender)

{
 int num_components=Model_GetNumComponents(ComponentsListFrame1->GetSelectedComponentLongId().c_str());
 if(!num_components)
 {
  ModalResult=mrOk;
  return;
 }
 ComponentsListFrame1->StringGridDblClick(Sender);
}
//---------------------------------------------------------------------------

