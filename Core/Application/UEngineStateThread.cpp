#ifndef UENGINE_STATE_THREAD_CPP
#define UENGINE_STATE_THREAD_CPP

#include "UEngineStateThread.h"
#include "UEngineControlThread.h"
#include "UApplication.h"
#include "UEngineControl.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

void ExceptionHandler(int channel_index)
{
 using namespace RDK;
 if(!UEngineStateThread::GetRdkExceptionHandlerMutex())
  return;

 UGenericMutexExclusiveLocker locker(UEngineStateThread::GetRdkExceptionHandlerMutex());

 if(find(UEngineStateThread::GetUnsentLogChannelIndexes().begin(), UEngineStateThread::GetUnsentLogChannelIndexes().end(),channel_index) == UEngineStateThread::GetUnsentLogChannelIndexes().end())
  UEngineStateThread::GetUnsentLogChannelIndexes().push_back(channel_index);
}

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineStateThread::UEngineStateThread(UEngineControl* engine_control)
: EngineControl(engine_control)
{
 if(!GetRdkExceptionHandlerMutex())
  GetRdkExceptionHandlerMutex()=UCreateMutex();
 #ifdef RDK_MUTEX_DEADLOCK_DEBUG
 TUThreadInfo info;
 info.Name="UEngineStateThread";
 GlobalThreadInfoMap[info.Pid]=info;
 #endif

 CalcState=UCreateEvent(false);

 CalcEnable=UCreateEvent(false);

 CalcStarted=UCreateEvent(false);

 CalculationNotInProgress=UCreateEvent(true);

 Terminated=false;
 Thread=boost::thread(boost::bind(&UEngineStateThread::Execute, boost::ref(*this)));

 NumAvgIterations=200;
 AvgThreshold=5.0;
// EventsLogFlag=true;
// LogFlag=true;
}

UEngineStateThread::~UEngineStateThread(void)
{
 Terminate();

 UDestroyEvent(CalcState);
 UDestroyEvent(CalcEnable);
 UDestroyEvent(CalcStarted);
 UDestroyEvent(CalculationNotInProgress);

 if(GetRdkExceptionHandlerMutex())
 {
  UDestroyMutex(GetRdkExceptionHandlerMutex());
  GetRdkExceptionHandlerMutex()=0;
 }

}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
/// Флаг разрешения логгирования
//bool UEngineStateThread::GetLogFlag(void) const
//{
// return LogFlag;
//}
//
//bool UEngineStateThread::SetLogFlag(bool value)
//{
// if(LogFlag == value)
//  return true;
//
// LogFlag=value;
// RecreateEventsLogFile();
// return true;
//}
// --------------------------

// --------------------------
// Методы доступа к данным состояния модулей
// --------------------------
/// Возвращает вектор состояний тредов
std::vector<int> UEngineStateThread::ReadCalcThreadStates(void) const
{
 return CalcThreadStates;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
/// Возвращает класс-владелец потока
UEngineControl* UEngineStateThread::GetEngineControl(void)
{
 return EngineControl;
}


/// Регистрация потока расчета
void UEngineStateThread::RegisterCalcThread(int index, UEngineControlThread *calc_thread)
{
 int num_engines=Core_GetNumChannels();
 CalcThreads.resize(num_engines,0);
 if(index<0 || index>=num_engines)
  return;

 CalcThreads[index]=calc_thread;
}

void UEngineStateThread::UnRegisterCalcThread(int index)
{
 int num_engines=Core_GetNumChannels();
 CalcThreads.resize(num_engines,0);
 if(index<0 || index>=num_engines)
  return;

 CalcThreads[index]=0;
}

void UEngineStateThread::Execute(void)
{
 while(!Terminated)
 {
  if(CalcStarted->wait(30) == false)
  {
   ProcessLog();
   continue;
  }

  if(CalculationNotInProgress->wait(30) == false)
  {
   ProcessLog();
   continue;
  }
  CalculationNotInProgress->reset();

  try
  {
   // Определяем состояние тредов расчета
   std::vector<int> calc_thread_states;

   int num_channels=Core_GetNumChannels();
   calc_thread_states.assign(num_channels,1);
   CalcThreadStateTime.resize(num_channels,0);
   CalcThreadSuccessTime.resize(num_channels,0);
   AvgIterations.resize(num_channels);

   for(int i=0;i<num_channels;i++)
   {
	if(int(CalcThreads.size())<=i)
     continue;
	UEngineControlThread *thread=CalcThreads[i];
	if(thread)
	{
	 if(!thread->IsCalcStarted())
	 {
	  calc_thread_states[i]=1;
	  continue;
	 }

	 double last_calc_time=thread->GetRealLastCalculationTime();
	 if(CalcThreadSuccessTime[i] != last_calc_time)
	 {
	  CalcThreadSuccessTime[i]=last_calc_time;
	  AvgIterations[i].push_back(last_calc_time);
	  if(int(AvgIterations[i].size())>NumAvgIterations)
	   AvgIterations[i].erase(AvgIterations[i].begin());

	  CalcThreadStateTime[i]=GetVariantLocalTime();
	  calc_thread_states[i]=0;
	 }
	 else
	 {
	  double avg_diff(0.0);
	  for(size_t j=1;j<AvgIterations[i].size();j++)
	  {
	   if(AvgIterations[i][j]-AvgIterations[i][j-1]>avg_diff)
		avg_diff=AvgIterations[i][j]-AvgIterations[i][j-1];
	  }
	  avg_diff/=1000;

	  if(fabs(avg_diff) < 1e-8 || (GetVariantLocalTime()-CalcThreadStateTime[i])*86400.0>AvgThreshold*avg_diff)
	   calc_thread_states[i]=2;
	  else
	   calc_thread_states[i]=0;
	 }
	}
	else
	{
	 calc_thread_states[i]=1;
	}
   }

   CalcThreadStates=calc_thread_states;

   AdditionExecute();
  }
  catch(UException &ex)
  {
   CalculationNotInProgress->set();
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_DEBUG, (string("UEngineStateThread Rdk exception: ")+ex.what()).c_str());
  }
  catch(std::exception &ex)
  {
   CalculationNotInProgress->set();
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_DEBUG, (string("UEngineStateThread std exception: ")+ex.what()).c_str());
  }
  catch(...)
  {
   CalculationNotInProgress->set();
   MLog_LogMessage(RDK_SYS_MESSAGE, RDK_EX_DEBUG, (string("UEngineStateThread unknown exception")).c_str());
  }

  ProcessLog();
  CalculationNotInProgress->set();
  Sleep(100);
 }
}

void UEngineStateThread::AdditionExecute(void)
{

}

/// Функция обеспечивает закрытие текущего файла логов и создание нового
//void UEngineStateThread::RecreateEventsLogFile(void)
//{
// if(!CalculationNotInProgress)
//  return;
// if(!CalculationNotInProgress->wait(100))
//  return;
// CalculationNotInProgress->reset();
//
// Logger.Clear();
// std::string log_dir;
// if(EngineControl && EngineControl->GetApplication())
//  log_dir=EngineControl->GetApplication()->CalcCurrentLogDir();
// else
//  log_dir="EventsLog/";
//
// Logger.SetLogDir(log_dir);
// if(Logger.InitLog() != RDK_SUCCESS)
// {
//  EventsLogFlag=false;
//  return;
// }
// else
// {
//  EventsLogFilePath=log_dir;
// }
//
// /// Сохраняем лог в файл если это необходимо
// if(!LogFlag)
//  EventsLogFlag=false;
// else
//  EventsLogFlag=true;
//
// CalculationNotInProgress->set();
//}
//
///// Закрывает текущий лог
//void UEngineStateThread::CloseEventsLogFile(void)
//{
// if(!CalculationNotInProgress)
//  return;
// if(!CalculationNotInProgress->wait(100))
//  return;
// CalculationNotInProgress->reset();
//
// Logger.Clear();
// CalculationNotInProgress->set();
//}

/// Временная переменная в которой хранится весь еще не отображенный в интерфейсе лог
/// Очищается каждый раз при запросе этой переменной
std::list<std::string> UEngineStateThread::ReadGuiUnsentLog(void)
{
 if(!CalculationNotInProgress)
  return std::list<std::string>();
 if(!CalculationNotInProgress->wait(100))
  return std::list<std::string>();
 CalculationNotInProgress->reset();

 std::list<std::string> buffer=GuiUnsentLog;
 GuiUnsentLog.clear();
 CalculationNotInProgress->set();
 return buffer;
}

/// Прерывает исполнение потока
void UEngineStateThread::Terminate(void)
{
 CalcStarted->reset();

 if(!CalculationNotInProgress->wait(0) || !CalculationNotInProgress->wait(10000))
  return;
 CalculationNotInProgress->reset();
 CalcState->reset();
 Terminated=true;
 Thread.join();
}

// Общедоступные данные логгирования
UGenericMutex*& UEngineStateThread::GetRdkExceptionHandlerMutex(void)
{
 static UGenericMutex* RdkExceptionHandlerMutex=0;
 return RdkExceptionHandlerMutex;
}

std::list<int>& UEngineStateThread::GetUnsentLogChannelIndexes(void)
{
 static std::list<int> UnsentLogChannelIndexes;
 return UnsentLogChannelIndexes;
}
// --------------------------

// --------------------------
// Вспомогательные методы
// --------------------------
void UEngineStateThread::ProcessLog(void)
{
 if(!GetRdkExceptionHandlerMutex())
  return;
 std::list<int> ch_indexes;
 {
  UGenericMutexExclusiveLocker locker(GetRdkExceptionHandlerMutex());
  ch_indexes=GetUnsentLogChannelIndexes();
  GetUnsentLogChannelIndexes().clear();
 }

 if(ch_indexes.empty())
  return;

 if(find(ch_indexes.begin(),ch_indexes.end(),RDK_GLOB_MESSAGE) == ch_indexes.end())
  return;

 int global_error_level=-1;
 try
 {
   int error_level=-1;
   int number=0;
   unsigned long long time=0;
   int num_log_lines=MLog_GetNumUnreadLogLines(RDK_GLOB_MESSAGE);
   for(int k=0;k<num_log_lines;k++)
   {
	const char * data=MLog_GetUnreadLog(RDK_GLOB_MESSAGE, error_level,number,time);
	if(!data)
	 continue;
	if(global_error_level>error_level)
	 global_error_level=error_level;

	std::string new_log_data=data;
	if(!new_log_data.empty())
	{
//	 UnsentLog.push_back(new_log_data);
     GuiUnsentLog.push_back(new_log_data);
	}
   }
   MLog_ClearReadLog(RDK_GLOB_MESSAGE);
 }
 catch(...)
 {
  throw;
 }
/*
 try
 {
  while(!UnsentLog.empty())
  {
   if(EventsLogFlag)
   {
	Logger.WriteMessageToFile(UnsentLog.front());// TODO: Проверить на RDK_SUCCESS
   }

   UnsentLog.pop_front();
  }
 }
 catch(...)
 {
  throw;
 }      */
}
// --------------------------


}

#endif
