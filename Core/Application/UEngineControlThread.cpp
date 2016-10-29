#ifndef UENGINE_CONTROL_THREAD_CPP
#define UENGINE_CONTROL_THREAD_CPP

#include "UEngineControlThread.h"
#include "UEngineControl.h"
#include "UChannelProfiler.h"
#include "UBroadcasterInterface.h"

#include "../../Deploy/Include/rdk_cpp_initdll.h"
#ifdef WIN32
#include <windows.h>
#endif

namespace RDK {

// --------------------------
// ������������ � �����������
// --------------------------
UEngineControlThread::UEngineControlThread(UEngineControl* engine_control, int channel_index)
: EngineControl(engine_control), EngineIndex(channel_index), Terminated(false)
{
 CalcState=UCreateEvent(false);

 CalcEnable=UCreateEvent(false);

 CalcStarted=UCreateEvent(false);

 CalculationNotInProgress=UCreateEvent(true);

 RealLastCalculationTime=0.0;
 CalculationTime=0.0;
 ServerTimeStamp=0.0;
 CalculationTimeSource=0;
 CalculateMode=0;
 MinInterstepsInterval=0;
 Thread=boost::thread(boost::bind(&UEngineControlThread::Execute, boost::ref(*this)));
 Profiler=new UChannelProfiler;
 Profiler->SetChannelIndex(channel_index);
 Profiler->AddAllGui();
}

UEngineControlThread::~UEngineControlThread(void)
{
 Terminated=true;
 Thread.join();

 UDestroyEvent(CalcState);
 UDestroyEvent(CalcEnable);
 UDestroyEvent(CalcStarted);
 UDestroyEvent(CalculationNotInProgress);

 if(Profiler)
 {
  delete Profiler;
  Profiler=0;
 }
}
// --------------------------

// --------------------------
// ���������� �����������
// --------------------------
/// ����� �����
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

/// ����������� �������� ������� ����� ���������� ������� � ������ 0 � 2, ��
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

/// �������� ������� ��� ������
/// 0 - ����
/// 1 - ����� �������� ��������������
int UEngineControlThread::GetCalculationTimeSource(void) const
{
 return CalculationTimeSource;
}

bool UEngineControlThread::SetCalculationTimeSource(int value)
{
 if(CalculationTimeSource == value)
  return true;

 CalculationTimeSource=value;
 return true;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
/// ���������� ����� �������, ���������� � ������ ��������� ���
/// � ������������ � ������� CalculationTimeSource
double UEngineControlThread::GetCalculationTime(void) const
{
 return CalculationTime;
}

/// ������� �������� �������
double UEngineControlThread::GetServerTimeStamp(void) const
{
 return ServerTimeStamp;
}

bool UEngineControlThread::SetServerTimeStamp(double value)
{
 ServerTimeStamp=value;
 return true;
}

/// ����� �������� ��������� ������� ����� ��� ���������� ��������� ������
double UEngineControlThread::GetLastCalculationServerTimeStamp(void) const
{
 return LastCalculationServerTimeStamp;
}

double UEngineControlThread::GetRealLastCalculationTime(void) const
{
 return RealLastCalculationTime;
}

/// ���������� �����-�������� ������
UEngineControl* UEngineControlThread::GetEngineControl(void)
{
 return EngineControl;
}

/// ��������� ����������� ������ ������������������
UChannelProfiler* UEngineControlThread::GetProfiler(void)
{
 return Profiler;
}
// --------------------------

// --------------------------
// ���������� �������
// --------------------------
void UEngineControlThread::BeforeCalculate(void)
{
 ABeforeCalculate();
}

void UEngineControlThread::ABeforeCalculate(void)
{
}

void UEngineControlThread::AfterCalculate(void)
{
 GetEngineControl()->AddMetadata(EngineIndex, GetEngineControl()->GetEngineThread(EngineIndex)->GetLastCalculationServerTimeStamp());
 AAfterCalculate();
 if(GetEnvironmentLock(EngineIndex)->IsCalcFinished())
  GetEngineControl()->PauseChannel(EngineIndex);
}

void UEngineControlThread::AAfterCalculate(void)
{
}

void UEngineControlThread::Calculate(void)
{
  if(CalculateMode == 2)
  {
   if(CalcEnable->wait(30) == false)
	return;
  }
  CalcEnable->reset();

  if(CalculationNotInProgress->wait(30) == false)
  {
   return;
  }

  double diff=(RDK::GetVariantLocalTime()-RealLastCalculationTime)*86400*1000.0;
  if(diff<MinInterstepsInterval)
  {
   Sleep(MinInterstepsInterval-diff);
   return;
  }

  CalculationNotInProgress->reset();
  if(EngineIndex>=Core_GetNumChannels())
  {
   CalculationNotInProgress->set();
   return;
  }

  int use_controllers_mode=EngineControl->GetUseControllersMode();
  if(use_controllers_mode == 1)
   RDK::UIControllerStorage::BeforeCalculate(EngineIndex);
  BeforeCalculate();
  double current_time(0.0);
  if(CalculationTimeSource == 0)
  {
   current_time=GetVariantLocalTime();
  }
  else
  if(CalculationTimeSource == 1)
  {
   current_time=ServerTimeStamp;
  }
  CalculationTime=current_time;
  MModel_SetDoubleSourceTime(EngineIndex,current_time);

  if(CalculateMode == 1)
  {
   MEnv_RTCalculate(EngineIndex);
  }
  else
  if(CalculateMode == 3)
  {
   MEnv_FastCalculate(EngineIndex, 1.0); // TODO: magic constant
  }
  else
  {
   MEnv_Calculate(EngineIndex,0);
  }
  AfterCalculate();
  LastCalculationServerTimeStamp=ServerTimeStamp; // TODO: �������� ��� current_time?
  RealLastCalculationTime=GetVariantLocalTime();
//  ServerLastCalculationTime=current_time;
  if(use_controllers_mode == 1)
   RDK::UIControllerStorage::AfterCalculate(EngineIndex);
  CalculationNotInProgress->set();
  try
  {
   Profiler->CalculateCore();
  }
  catch(...)
  {

  }
}

void UEngineControlThread::Execute(void)
{
 while(!Terminated)
 {
  if(CalcStarted->wait(30) == false)
   continue;

  Calculate();
 }
}

/// ��������� ��������� ������� �� id ������
/// 0 - �� �������
/// 1 - ���� ������
int UEngineControlThread::CheckCalcState(void) const
{
 if(CalcState->wait(0))
  return 1;

 return 0;
}

/// ���������� ��������� ������� �����
int UEngineControlThread::IsCalcStarted(void) const
{
 if(CalcStarted->wait(0))
  return 1;

 return 0;
}

/// ������� ���� ���������� �������
bool UEngineControlThread::EnableCalculation(void)
{
 CalcEnable->set();
 return true;
}

/// ��������� ��������� ������
void UEngineControlThread::Start(void)
{
 if(EngineIndex<0 || EngineIndex>Core_GetNumChannels())
  return;

 CalcStarted->set();
 CalcState->set();
}

/// ������������� ��������� ������
void UEngineControlThread::Pause(void)
{
 if(EngineIndex<0 || EngineIndex>Core_GetNumChannels())
  return;

 CalcStarted->reset();
 CalcState->reset();
}

/// ���������� ��������� ������
void UEngineControlThread::Reset(void)
{
 if(EngineIndex<0 || EngineIndex>Core_GetNumChannels())
  return;

 MEnv_Reset(EngineIndex,0);
 RealLastCalculationTime=0;
 CalculationTime=0.0;
 ServerTimeStamp=0.0;
}

/// ������������� ��������� ������
bool UEngineControlThread::SetPriority(int priority_id)
{
#ifdef WIN32
 BOOL res;
 HANDLE th = Thread.native_handle();

 switch (priority_id)
	{
	case RDK_THREAD_PRIORITY_REALTIME:
	 res = SetThreadPriority(th, THREAD_PRIORITY_TIME_CRITICAL);
	break;
	case RDK_THREAD_PRIORITY_HIGH:
	 res = SetThreadPriority(th, THREAD_PRIORITY_HIGHEST);
	break;
	case RDK_THREAD_PRIORITY_ABOVE_NORMAL:
	 res = SetThreadPriority(th, THREAD_PRIORITY_ABOVE_NORMAL);
	break;
	case RDK_THREAD_PRIORITY_NORMAL:
	 res = SetThreadPriority(th, THREAD_PRIORITY_NORMAL);
	break;
	case RDK_THREAD_PRIORITY_BELOW_NORMAL:
	 res = SetThreadPriority(th, THREAD_PRIORITY_BELOW_NORMAL);
	break;
	case RDK_THREAD_PRIORITY_IDLE:
	 res = SetThreadPriority(th, THREAD_PRIORITY_LOWEST);
	break;
	}

 return res;
#else
 return false;
#endif
}
// --------------------------


}

#endif
