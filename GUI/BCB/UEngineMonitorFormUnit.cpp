//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEngineMonitorFormUnit.h"

#include "../../Core/Engine/UGlogGuiSink.h"

#ifdef RDK_USE_GLOG
#include <glog/logging.h>
#endif
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

/// Экзепляр класса приложения
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

// Сохраняет параметры интерфейса в xml
void TUEngineMonitorForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// Загружает параметры интерфейса из xml
void TUEngineMonitorForm::ALoadParameters(RDK::USerStorageXML &xml)
{
// EventsLogFlag=true;
}

// Создание копии этого компонента
TUEngineMonitorForm* TUEngineMonitorForm::New(TComponent *owner)
{
 return new TUEngineMonitorForm(owner);
}

//---------------------------------------------------------------------------





namespace
{
int MapGuiSeverity(const RDK::UGlogGuiMessage &message)
{
 int level=RDK_EX_INFO;
#ifdef RDK_USE_GLOG
 switch(message.Severity)
 {
 case google::GLOG_FATAL:
  level=RDK_EX_FATAL;
  break;
 case google::GLOG_ERROR:
  level=RDK_EX_ERROR;
  break;
 case google::GLOG_WARNING:
  level=RDK_EX_WARNING;
  break;
 default:
  level=RDK_EX_INFO;
  break;
 }
#endif
 if(message.Text.find("[APP]") != std::string::npos)
  level=RDK_EX_APP;
 else
 if(message.Text.find("[DEBUG]") != std::string::npos)
  level=RDK_EX_DEBUG;
 return level;
}
}

void __fastcall TUEngineMonitorForm::LogTimerTimer(TObject *Sender)
{
 try
 {
  if(!RdkApplication.IsInit())
   return;

 const std::vector<RDK::UGlogGuiMessage> log = RDK::UGlogGuiSink::Instance().ReadMessages(512);
 for(std::vector<RDK::UGlogGuiMessage>::const_iterator I=log.begin(); I != log.end();++I)
  {
  int log_level=MapGuiSeverity(*I);

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

  AddColorText(EngineMonitorFrame->RichEdit, I->Text.c_str(), color);
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
  RDK::Logging::ChannelLog(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("TUEngineMonitorForm::LogTimerTimer - ")+ex.what()).c_str());
 }
 catch(...)
 {
  RDK::Logging::ChannelLog(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "TUEngineMonitorForm::LogTimerTimer - unhandled exception");
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


