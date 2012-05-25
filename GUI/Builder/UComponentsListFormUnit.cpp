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
 Mode=0;
 return ShowModal();
}

// Метод открытия диалога для выбора параметра
int TUComponentsListForm::ShowParameterSelect(void)
{
 Panel1->Visible;
 Position=poScreenCenter;
 Mode=1;
 ComponentsListFrame1->PageControl1->ActivePageIndex=3;
 return ShowModal();
}

// Метод открытия диалога для выбора переменной состояния
int TUComponentsListForm::ShowStateSelect(void)
{
 Panel1->Visible;
 Position=poScreenCenter;
 Mode=2;
 ComponentsListFrame1->PageControl1->ActivePageIndex=4;
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
 if(!num_components && Mode == 0)
 {
  ModalResult=mrOk;
  return;
 }
 ComponentsListFrame1->StringGridDblClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListForm::ComponentsListFrame1ParametersListStringGridDblClick(TObject *Sender)
{
 ComponentsListFrame1->ParametersListStringGridDblClick(Sender);

 ModalResult=mrOk;
 return;
}
//---------------------------------------------------------------------------

