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

std::list<std::string> UnsentLog;

//---------------------------------------------------------------------------
void ExceptionHandler(int channel_index)
{
 if(!RdkExceptionHandlerEvent)
  return;

 if(WaitForSingleObject(RdkExceptionHandlerEvent,1000) == WAIT_TIMEOUT)
  return;
 ResetEvent(RdkExceptionHandlerEvent);

 try
 {
 int error_level=-1;
 const char * data=MEngine_GetUnreadLog(channel_index, error_level);
 if(!data)
  return;

 std::string new_log_data=data;
 UnsentLog.push_back(new_log_data);

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
 catch(...)
 {
  SetEvent(RdkExceptionHandlerEvent);
  throw;
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



void __fastcall TUEngineMonitorForm::LogTimerTimer(TObject *Sender)
{
 if(!RdkExceptionHandlerEvent)
  return;

 if(WaitForSingleObject(RdkExceptionHandlerEvent,10) == WAIT_TIMEOUT)
  return;
 ResetEvent(RdkExceptionHandlerEvent);

 try
 {
  while(!UnsentLog.empty())
  {
  /*
   try
   {
	UEngineMonitorForm->EngineMonitorFrame->RichEdit->SetFocus();
   }
   catch(...)
   {

   }*/
   UEngineMonitorForm->EngineMonitorFrame->RichEdit->Lines->Add(UnsentLog.front().c_str());
   UEngineMonitorForm->EngineMonitorFrame->RichEdit->SelStart =
	UEngineMonitorForm->EngineMonitorFrame->RichEdit->Perform(EM_LINEINDEX, UEngineMonitorForm->EngineMonitorFrame->RichEdit->Lines->Count-1, 0);
   UEngineMonitorForm->EngineMonitorFrame->RichEdit->Update();
   UnsentLog.pop_front();
  }
 }
 catch(...)
 {
  SetEvent(RdkExceptionHandlerEvent);
  throw;
 }

 SetEvent(RdkExceptionHandlerEvent);
}
//---------------------------------------------------------------------------

