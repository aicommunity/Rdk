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

HANDLE RdkExceptionHandlerEvent=0;

//---------------------------------------------------------------------------
void ExceptionHandler(int channel_index)
{
 if(!RdkExceptionHandlerEvent)
  return;

 if(WaitForSingleObject(RdkExceptionHandlerEvent,1000) == WAIT_TIMEOUT)
  return;
 ResetEvent(RdkExceptionHandlerEvent);

 int error_level=-1;
 const char * data=MEngine_GetUnreadLog(channel_index, error_level);
 if(!data)
  return;

 std::string new_log_data=data;
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
 SetEvent(RdkExceptionHandlerEvent);
}
//---------------------------------------------------------------------------
__fastcall TUEngineMonitorForm::TUEngineMonitorForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// Сохраняет параметры интерфейса в xml
void TUEngineMonitorForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// Загружает параметры интерфейса из xml
void TUEngineMonitorForm::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// Создание копии этого компонента
TUEngineMonitorForm* TUEngineMonitorForm::New(TComponent *owner)
{
 return new TUEngineMonitorForm(owner);
}

//---------------------------------------------------------------------------
void __fastcall TUEngineMonitorForm::FormDestroy(TObject *Sender)
{
 if(RdkExceptionHandlerEvent)
 {
  CloseHandle(RdkExceptionHandlerEvent);
  RdkExceptionHandlerEvent=0;
 }
// EngineMonitorFrame->Timer->Enabled=false;
}
//---------------------------------------------------------------------------


void __fastcall TUEngineMonitorForm::FormCreate(TObject *Sender)
{
 if(!RdkExceptionHandlerEvent)
  RdkExceptionHandlerEvent=CreateEvent(0,TRUE,TRUE,0);
}
//---------------------------------------------------------------------------

