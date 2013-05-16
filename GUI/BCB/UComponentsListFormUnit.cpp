//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UComponentsListFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUComponentsListForm *UComponentsListForm;
//---------------------------------------------------------------------------
__fastcall TUComponentsListForm::TUComponentsListForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
// Метод открытия диалога для выбора компонента
int TUComponentsListForm::ShowComponentSelect(void)
{
 Panel1->Visible=true;
 Position=poScreenCenter;
 Mode=0;
 return ShowModal();
}

// Метод открытия диалога для выбора параметра
int TUComponentsListForm::ShowParameterSelect(void)
{
 Panel1->Visible=true;
 Position=poScreenCenter;
 Mode=1;
 ComponentsListFrame1->PageControl1->ActivePageIndex=0;
 return ShowModal();
}

// Метод открытия диалога для выбора переменной состояния
int TUComponentsListForm::ShowStateSelect(void)
{
 Panel1->Visible=true;
 Position=poScreenCenter;
 Mode=2;
 ComponentsListFrame1->PageControl1->ActivePageIndex=1;
 return ShowModal();
}

// Метод открытия диалога для выбора входа или выхода
int TUComponentsListForm::ShowIOSelect(void)
{
 Panel1->Visible;
 Position=poScreenCenter;
 Mode=3;
 ComponentsListFrame1->PageControl1->ActivePageIndex=2;
 return ShowModal();
}

// Создание копии этого компонента
TUComponentsListForm* TUComponentsListForm::New(TComponent *owner)
{
 return new TUComponentsListForm(owner);
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

 if(Mode == 1)
 {
  ModalResult=mrOk;
  return;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListForm::ComponentsListFrame1StatesListStringGridDblClick(TObject *Sender)

{
 ComponentsListFrame1->StatesListStringGridDblClick(Sender);

 if(Mode == 2)
 {
  ModalResult=mrOk;
  return;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListForm::ComponentsListFrame1OutputsStringGridDblClick(TObject *Sender)
{
 ComponentsListFrame1->OutputsStringGridDblClick(Sender);

 if(Mode == 3)
 {
  ModalResult=mrOk;
  return;
 }
}
//---------------------------------------------------------------------------

void __fastcall TUComponentsListForm::ComponentsListFrame1InputsStringGridDblClick(TObject *Sender)
{
 ComponentsListFrame1->InputsStringGridDblClick(Sender);

 if(Mode == 3)
 {
  ModalResult=mrOk;
  return;
 }
}
//---------------------------------------------------------------------------

