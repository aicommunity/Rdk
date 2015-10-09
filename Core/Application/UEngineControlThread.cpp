#ifndef UENGINE_CONTROL_THREAD_CPP
#define UENGINE_CONTROL_THREAD_CPP

#include "UEngineControlThread.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineControlThread::UEngineControlThread(int engine_index)
: EngineIndex(engine_index), Terminated(false)
{
 CalcState=UCreateEvent(false);

 CalcEnable=UCreateEvent(false);

 CalcStarted=UCreateEvent(false);

 CalculationNotInProgress=UCreateEvent(true);

 RealLastCalculationTime=0.0;
 Thread=boost::thread(boost::bind(&UEngineControlThread::Execute, boost::ref(*this)));
}

UEngineControlThread::~UEngineControlThread(void)
{
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
/// Режим счета
int UEngineControlThread::GetCalculateMode(void) const
{
 return CalculateMode;
}

bool UEngineControlThread::SetCalculateMode(int value)
{
 if(CalculateMode == value)
  return true;

 if(CalculationNotInProgress->wait(1000) == false)
  return false;

 CalculateMode=value;

 return true;
}

/// Минимальный интервал времени между итерациями расчета в режиме 0 и 2, мс
int UEngineControlThread::GetMinInterstepsInterval(void) const
{
 return MinInterstepsInterval;
}

bool UEngineControlThread::SetMinInterstepsInterval(RDK::UTime value)
{
 if(MinInterstepsInterval == value)
  return true;

 if(CalculationNotInProgress->wait(1000) == false)
  return false;

 MinInterstepsInterval=value;
 return true;
}
// --------------------------

// --------------------------
// Управление потоком
// --------------------------
void UEngineControlThread::BeforeCalculate(void)
{
}

void UEngineControlThread::AfterCalculate(void)
{
/* UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]=
  UEngineMonitorForm->EngineMonitorFrame->GetServerTimeStamp(ChannelIndex);

 UEngineMonitorForm->EngineMonitorFrame->AddMetadata(ChannelIndex, UEngineMonitorForm->EngineMonitorFrame->LastCalculatedServerTimeStamp[ChannelIndex]);
*/
}


void UEngineControlThread::Execute(void)
{
 while(!Terminated)
 {
  if(CalcStarted->wait(30) == false)
   continue;

  if(CalculationNotInProgress->wait(30) == false)
  {
   continue;
  }

  RDK::ULongTime diff=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),RealLastCalculationTime);
  if(diff<MinInterstepsInterval)
  {
   Sleep(MinInterstepsInterval-diff);
   continue;
  }

  CalculationNotInProgress->reset();
  if(EngineIndex<GetNumEngines())
  {
   CalculationNotInProgress->set();
   continue;
  }

  BeforeCalculate();
  MModel_SetDoubleSourceTime(EngineIndex,GetVariantLocalTime());
  MEnv_Calculate(EngineIndex,0);
  AfterCalculate();
  RealLastCalculationTime=GetVariantLocalTime();
  CalculationNotInProgress->set();
 }
}

double UEngineControlThread::GetRealLastCalculationTime(void) const
{
 return RealLastCalculationTime;
}

// --------------------------


}

#endif
