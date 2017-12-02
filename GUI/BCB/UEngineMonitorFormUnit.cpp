//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEngineMonitorFormUnit.h"
#include "TUVisualController.h"
#include "UGEngineControlFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UEngineMonitorFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorForm *UEngineMonitorForm;

//HANDLE RdkExceptionHandlerMutex=0;

//std::list<int> UnsentLogChannelIndexes;

//std::list<std::string> UnsentLog;

/// Ёкзепл€р класса приложени€
extern RDK::UApplication RdkApplication;

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TUEngineMonitorForm::TUEngineMonitorForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// —охран€ет параметры интерфейса в xml
void TUEngineMonitorForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// «агружает параметры интерфейса из xml
void TUEngineMonitorForm::ALoadParameters(RDK::USerStorageXML &xml)
{
// EventsLogFlag=true;
}

// —оздание копии этого компонента
TUEngineMonitorForm* TUEngineMonitorForm::New(TComponent *owner)
{
 return new TUEngineMonitorForm(owner);
}

//---------------------------------------------------------------------------





void __fastcall TUEngineMonitorForm::LogTimerTimer(TObject *Sender)
{
 try
 {
  if(!RdkApplication.IsInit())
   return;

  std::list<std::string> log=RdkApplication.GetEngineControl()->GetEngineStateThread()->ReadGuiUnsentLog();

  for(std::list<std::string>::iterator I=log.begin(); I != log.end();++I)
  {
   EngineMonitorFrame->RichEdit->Lines->Add(I->c_str());
  }
  while(EngineMonitorFrame->RichEdit->Lines->Count>1000)
  {
   EngineMonitorFrame->RichEdit->Lines->Delete(0);
  }
  if(!log.empty())
  {
   EngineMonitorFrame->RichEdit->SelStart =
	EngineMonitorFrame->RichEdit->Perform(EM_LINEINDEX, EngineMonitorFrame->RichEdit->Lines->Count-1, (NativeInt)0);
   EngineMonitorFrame->RichEdit->Update();
   EngineMonitorFrame->RichEdit->Repaint();
  }
 }
 catch(std::exception &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("TUEngineMonitorForm::LogTimerTimer - ")+ex.what()).c_str());
 }
 catch(...)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "TUEngineMonitorForm::LogTimerTimer - unhandled exception");
 }
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorForm::EngineMonitorFrameRichEditMouseEnter(TObject *Sender)

{
 if(Active)
  EngineMonitorFrame->RichEditMouseEnter(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorForm::FormDestroy(TObject *Sender)
{
 LogTimer->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorForm::FormCreate(TObject *Sender)
{
 LogTimer->Enabled=true;
}
//---------------------------------------------------------------------------


