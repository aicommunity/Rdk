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
 std::string new_log_data=Engine_GetUnreadLog();
 UEngineMonitorForm->EngineMonitorFrame->RichEdit->Text=UEngineMonitorForm->EngineMonitorFrame->RichEdit->Text+new_log_data.c_str();

 if(!new_log_data.empty())
 {
  UEngineMonitorForm->Show();
  UEngineMonitorForm->WindowState=wsNormal;
  UEngineMonitorForm->EngineMonitorFrame->Pause1Click(UEngineMonitorForm);
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
//---------------------------------------------------------------------------
void __fastcall TUEngineMonitorForm::FormDestroy(TObject *Sender)
{
 EngineMonitorFrame->Timer->Enabled=false;
}
//---------------------------------------------------------------------------


