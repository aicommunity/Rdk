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

HANDLE RdkExceptionHandlerMutex=0;

std::list<int> UnsentLogChannelIndexes;

std::list<std::string> UnsentLog;

//---------------------------------------------------------------------------
void ExceptionHandler(int channel_index)
{
 if(!RdkExceptionHandlerMutex)
  return;

 if(WaitForSingleObject(RdkExceptionHandlerMutex,1000) != WAIT_OBJECT_0)
// if(WaitForSingleObject(RdkExceptionHandlerMutex,1000) == WAIT_TIMEOUT)
  return;
// ResetEvent(RdkExceptionHandlerMutex);

 if(find(UnsentLogChannelIndexes.begin(), UnsentLogChannelIndexes.end(),channel_index) == UnsentLogChannelIndexes.end())
  UnsentLogChannelIndexes.push_back(channel_index);

 ReleaseMutex(RdkExceptionHandlerMutex);
// SetEvent(RdkExceptionHandlerMutex);
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
 if(RdkExceptionHandlerMutex)
 {
  CloseHandle(RdkExceptionHandlerMutex);
  RdkExceptionHandlerMutex=0;
 }
// EngineMonitorFrame->Timer->Enabled=false;
}
//---------------------------------------------------------------------------


void __fastcall TUEngineMonitorForm::FormCreate(TObject *Sender)
{
 if(!RdkExceptionHandlerMutex)
  RdkExceptionHandlerMutex=CreateMutex(0,FALSE,0);//CreateEvent(0,TRUE,TRUE,0);////
 LogTimer->Enabled=true;
 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 TUThreadInfo info;
 info.Pid=EngineMonitorFrame->GetEngineMonitorThread()->ThreadID;
 info.Name="EngineMonitorThread";
 GlobalThreadInfoMap[info.Pid]=info;
 #endif
}
//---------------------------------------------------------------------------



void __fastcall TUEngineMonitorForm::LogTimerTimer(TObject *Sender)
{
 if(!RdkExceptionHandlerMutex)
  return;

 if(WaitForSingleObject(RdkExceptionHandlerMutex,10) != WAIT_OBJECT_0)
// if(WaitForSingleObject(RdkExceptionHandlerMutex,10) == WAIT_TIMEOUT)
  return;
// ResetEvent(RdkExceptionHandlerMutex);

 std::list<int> ch_indexes=UnsentLogChannelIndexes;
 UnsentLogChannelIndexes.clear();
 ReleaseMutex(RdkExceptionHandlerMutex);
// SetEvent(RdkExceptionHandlerMutex);

 int global_error_level=-1;
 try
 {
  std::list<int>::iterator I=ch_indexes.begin();
  for(;I!=ch_indexes.end();++I)
  {
   int error_level=-1;
   int num_log_lines=MEngine_GetNumUnreadLogLines(*I);
   for(int k=0;k<num_log_lines;k++)
   {
	const char * data=MEngine_GetUnreadLog(*I, error_level);
	if(!data)
	 continue;
	if(global_error_level>error_level)
	 global_error_level=error_level;

	std::string new_log_data=data;
	MEngine_FreeBufString(*I,data);
	if(!new_log_data.empty())
	 UnsentLog.push_back(new_log_data);
   }
   MEngine_ClearReadLog(*I);
  }
 }
 catch(...)
 {
  throw;
 }

 try
 {
  if(global_error_level>=0 && global_error_level<3)
  {
   EngineMonitorFrame->Pause1Click(this);
   TTabSheet *tab=dynamic_cast<TTabSheet*>(Parent);
   if(tab)
   {
	tab->PageControl->ActivePage=tab;
   }
   else
   {
	Show();
	WindowState=wsNormal;
   }
  }


  /// Сохраняем лог в файл если это необходимо
  if(!UnsentLog.empty() && UGEngineControlForm->EventsLogEnabled && UGEngineControlForm->ProjectPath.Length()>0)
  {
   try
   {
	ForceDirectories(UGEngineControlForm->ProjectPath+"EventsLog");
   }
   catch(EInOutError &exception)
   {
	UGEngineControlForm->EventsLogEnabled=false;
   }
   EventsLogFilePath=AnsiString(UGEngineControlForm->ProjectPath+"EventsLog/").c_str();
  }

  bool is_logged=false;
  while(!UnsentLog.empty())
  {
   is_logged=true;
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
	}
   }

   EngineMonitorFrame->RichEdit->Lines->Add(UnsentLog.front().c_str());
   UnsentLog.pop_front();
  }

  if(is_logged)
  {
   if(UGEngineControlForm->EventsLogEnabled && EventsLogFile && *EventsLogFile)
	EventsLogFile->flush();

   EngineMonitorFrame->RichEdit->SelStart =
	EngineMonitorFrame->RichEdit->Perform(EM_LINEINDEX, EngineMonitorFrame->RichEdit->Lines->Count-1, 0);
   EngineMonitorFrame->RichEdit->Update();
   EngineMonitorFrame->RichEdit->Repaint();
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


