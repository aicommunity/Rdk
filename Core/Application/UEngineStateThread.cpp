#ifndef UENGINE_STATE_THREAD_CPP
#define UENGINE_STATE_THREAD_CPP

#include "UEngineStateThread.h"
#include "UEngineControlThread.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineStateThread::UEngineStateThread(UEngineControl* engine_control)
: EngineControl(engine_control)
{
 CalcState=UCreateEvent(false);

 CalcEnable=UCreateEvent(false);

 CalcStarted=UCreateEvent(false);

 CalculationNotInProgress=UCreateEvent(true);

 Thread=boost::thread(boost::bind(&UEngineStateThread::Execute, boost::ref(*this)));

 NumAvgIterations=200;
 AvgThreshold=5.0;
}

UEngineStateThread::~UEngineStateThread(void)
{
 CalcStarted->reset();
 CalcState->reset();
 Terminated=true;
 Thread.join();

 UDestroyEvent(CalcState);
 UDestroyEvent(CalcEnable);
 UDestroyEvent(CalcStarted);
 UDestroyEvent(CalculationNotInProgress);
}
// --------------------------

// --------------------------
// Управление параметрами
// --------------------------
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
 int num_engines=GetNumEngines();
 CalcThreads.resize(num_engines,0);
 if(index<0 || index>=num_engines)
  return;

 CalcThreads[index]=calc_thread;
}

void UEngineStateThread::UnRegisterCalcThread(int index)
{
 int num_engines=GetNumEngines();
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
   continue;

  if(CalculationNotInProgress->wait(30) == false)
  {
   continue;
  }
  CalculationNotInProgress->reset();

  try
  {
   // Определяем состояние тредов расчета
   std::vector<int> calc_thread_states;

   int num_channels=GetNumEngines();
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

	 RDK::ULongTime last_calc_time=thread->GetRealLastCalculationTime();
	 if(CalcThreadSuccessTime[i] != last_calc_time)
	 {
	  CalcThreadSuccessTime[i]=last_calc_time;
	  AvgIterations[i].push_back(last_calc_time);
	  if(AvgIterations[i].size()>NumAvgIterations)
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

   CalculationNotInProgress->set();
   Sleep(100);
  }
  catch(UException &ex)
  {
   CalculationNotInProgress->set();
   Engine_LogMessage(RDK_EX_DEBUG, (string("UEngineStateThread Rdk exception: ")+ex.CreateLogMessage()).c_str());
  }
  catch(std::exception &ex)
  {
   CalculationNotInProgress->set();
   Engine_LogMessage(RDK_EX_DEBUG, (string("UEngineStateThread std exception: ")).c_str());
  }
  catch(...)
  {
   CalculationNotInProgress->set();
   Engine_LogMessage(RDK_EX_DEBUG, (string("UEngineStateThread unknown exception")).c_str());
  }


 }
}

void UEngineStateThread::AdditionExecute(void)
{

}

// --------------------------

}

#endif
