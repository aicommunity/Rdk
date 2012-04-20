//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEngineMonitorFormUnit.h"
#include "rdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UEngineMonitorFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorForm *UEngineMonitorForm;

//---------------------------------------------------------------------------

void ExceptionHandler(void)
{
 UEngineMonitorForm->EngineMonitorFrame->RichEdit->Text=Engine_GetLog();
}
//---------------------------------------------------------------------------
__fastcall TUEngineMonitorForm::TUEngineMonitorForm(TComponent* Owner)
	: TForm(Owner)
{
}

// Добавляет обработчик в список
void TUEngineMonitorForm::AddInterface(RDK::IVisualInterface *value)
{
 if(EngineMonitorFrame)
  EngineMonitorFrame->AddInterface(value);
}

// Удаляет обработчик из списка
void TUEngineMonitorForm::DelInterface(RDK::IVisualInterface *value)
{
 if(EngineMonitorFrame)
  EngineMonitorFrame->DelInterface(value);
}
//---------------------------------------------------------------------------
void __fastcall TUEngineMonitorForm::FormDestroy(TObject *Sender)
{
 EngineMonitorFrame->Timer->Enabled=false;
}
//---------------------------------------------------------------------------

