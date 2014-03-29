//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEngineMonitorFormUnit.h"
#include "rdk_initdll.h"
#include "TUVisualController.h"
#include "UGEngineControlFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UEngineMonitorFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUEngineMonitorForm *UEngineMonitorForm;

HANDLE RdkExceptionHandlerEvent=0;

std::list<int> UnsentLogChannelIndexes;

std::list<std::string> UnsentLog;

//---------------------------------------------------------------------------
void ExceptionHandler(int channel_index)
{
 if(!RdkExceptionHandlerEvent)
  return;

 if(WaitForSingleObject(RdkExceptionHandlerEvent,1000) == WAIT_TIMEOUT)
  return;
 ResetEvent(RdkExceptionHandlerEvent);

 UnsentLogChannelIndexes.push_back(channel_index);

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

/// Функция обеспечивает закрытие текущего файла логов и создание нового
void TUEngineMonitorForm::RecreateEventsLogFile(void)
{
 if(EventsLogFile)
 {
  delete EventsLogFile;
  EventsLogFile=0;
 }
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

 int global_error_level=-1;
 try
 {
  std::list<int>::iterator I=UnsentLogChannelIndexes.begin();
  for(;I!=UnsentLogChannelIndexes.end();++I)
  {
   int error_level=-1;
   const char * data=MEngine_GetUnreadLog(*I, error_level);
   if(!data)
	continue;
   if(global_error_level>error_level)
	global_error_level=error_level;


   std::string new_log_data=data;
   MEngine_FreeBufString(*I,data);
   UnsentLog.push_back(new_log_data);
  }
 }
 catch(...)
 {
  UnsentLogChannelIndexes.clear();
  SetEvent(RdkExceptionHandlerEvent);
  throw;
 }
 UnsentLogChannelIndexes.clear();
 SetEvent(RdkExceptionHandlerEvent);

 try
 {
  if(global_error_level>=0 && global_error_level<3)
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


  /// Сохраняем лог в файл если это необходимо
  if(UGEngineControlForm->EventsLogEnabled && UGEngineControlForm->ProjectPath.Length()>0)
  {
   ForceDirectories(UGEngineControlForm->ProjectPath+"EventsLog");
   EventsLogFilePath=AnsiString(UGEngineControlForm->ProjectPath+"EventsLog/").c_str();
  }

  while(!UnsentLog.empty())
  {
   if(UGEngineControlForm->EventsLogEnabled)
   {
	if(!EventsLogFile)
	{
	 std::string file_name;
	 time_t time_data;
	 time(&time_data);
	 file_name=RDK::get_text_time(time_data, '.', '_');
	 EventsLogFile= new std::ofstream((EventsLogFilePath+file_name+".txt").c_str(),std::ios_base::out | std::ios_base::app);
	}

	if(*EventsLogFile)
	{
	 *EventsLogFile<<UnsentLog.front()<<std::endl;
	 EventsLogFile->flush();
	}
   }

   UEngineMonitorForm->EngineMonitorFrame->RichEdit->Lines->Add(UnsentLog.front().c_str());
   UEngineMonitorForm->EngineMonitorFrame->RichEdit->SelStart =
	UEngineMonitorForm->EngineMonitorFrame->RichEdit->Perform(EM_LINEINDEX, UEngineMonitorForm->EngineMonitorFrame->RichEdit->Lines->Count-1, 0);
   UEngineMonitorForm->EngineMonitorFrame->RichEdit->Update();
   UEngineMonitorForm->EngineMonitorFrame->RichEdit->Repaint();
   UnsentLog.pop_front();
  }
 }
 catch(...)
 {
  throw;
 }

}
//---------------------------------------------------------------------------

void __fastcall TUEngineMonitorForm::EngineMonitorFrameRichEditMouseEnter(TObject *Sender)

{
 if(Active)
  EngineMonitorFrame->RichEditMouseEnter(Sender);
}
//---------------------------------------------------------------------------

