//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEngineMonitorFormUnit.h"
#include "rdk_initdll.h"
#include "TUVisualController.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UEngineMonitorFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorForm *UEngineMonitorForm;

//---------------------------------------------------------------------------
void ExceptionHandler(void)
{
 int error_level=-1;
 std::string new_log_data=Engine_GetUnreadLog(error_level);
 UEngineMonitorForm->EngineMonitorFrame->RichEdit->Text=UEngineMonitorForm->EngineMonitorFrame->RichEdit->Text+new_log_data.c_str();

 if(!new_log_data.empty())
 {
  if(error_level<3)
  {
   UEngineMonitorForm->EngineMonitorFrame->Pause1Click(UEngineMonitorForm);
   TTabSheet *tab=dynamic_cast<TTabSheet*>(UEngineMonitorForm->Parent);
   if(tab)
   {
	tab->PageControl->ActivePage=tab;
   }
   else
   {
	UEngineMonitorForm->Show();
	UEngineMonitorForm->WindowState=wsNormal;
   }
  }
 }
}
//---------------------------------------------------------------------------
__fastcall TUEngineMonitorForm::TUEngineMonitorForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// ��������� ��������� ���������� � xml
void TUEngineMonitorForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// ��������� ��������� ���������� �� xml
void TUEngineMonitorForm::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// �������� ����� ����� ����������
TUEngineMonitorForm* TUEngineMonitorForm::New(TComponent *owner)
{
 return new TUEngineMonitorForm(owner);
}

//---------------------------------------------------------------------------
void __fastcall TUEngineMonitorForm::FormDestroy(TObject *Sender)
{
// EngineMonitorFrame->Timer->Enabled=false;
}
//---------------------------------------------------------------------------


