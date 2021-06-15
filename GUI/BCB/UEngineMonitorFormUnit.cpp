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

void AddColorText(TRichEdit* richedit, const String &text, TColor color)
{
 //richedit->SelStart=richedit->Text.Length();
 richedit->SelAttributes->Color = color;
 //richedit->SelText= text+"\r\n";
 richedit->Lines->Add(text);
}

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
   int log_level=-1;
   std::string::size_type pos1=I->find_first_of('>');
   if(pos1 != std::string::npos)
   {
	std::string::size_type pos2=I->find_first_of('>',pos1+1);
	if(pos2 != std::string::npos && pos2 > 0)
	{
	 log_level=RDK::atoi(I->substr(pos2-1,1));
	}
   }

   TColor color;
   switch(log_level)
   {
   case RDK_EX_APP:
	color=clBlue;
   break;

   case RDK_EX_INFO:
	color=clGreen;
   break;

   case RDK_EX_DEBUG:
	color=clNavy;
   break;

   case RDK_EX_WARNING:
	color=clOlive;
   break;

   case RDK_EX_ERROR:
	color=clMaroon;
   break;

   case RDK_EX_FATAL:
	color=clRed;
   break;

   case RDK_EX_UNKNOWN:
	color=clFuchsia;
   break;

   default:
    color=clBlack;
   }

   AddColorText(EngineMonitorFrame->RichEdit, I->c_str(), color);
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


