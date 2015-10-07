﻿#ifndef UENGINE_CONTROL_THREAD_CPP
#define UENGINE_CONTROL_THREAD_CPP

#include "UEngineControlThread.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"
#ifdef WIN32
#include <windows.h>
#endif

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
 ExternalCurrentTime=0.0;
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

/// Источник времени для модели
/// 0 - часы
/// 1 - время внешнего видеоисточника
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
// Управление данными
// --------------------------
/// Внешний источник времени
double UEngineControlThread::GetExternalCurrentTime(void) const
{
 return ExternalCurrentTime;
}

bool UEngineControlThread::SetExternalCurrentTime(double value)
{
 ExternalCurrentTime=value;
 return true;
}

double UEngineControlThread::GetRealLastCalculationTime(void) const
{
 return RealLastCalculationTime;
}
// --------------------------

// --------------------------
// Управление потоком
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
 AAfterCalculate();
}

void UEngineControlThread::AAfterCalculate(void)
{
}

void UEngineControlThread::Execute(void)
{
 while(!Terminated)
 {
  if(CalcStarted->wait(30) == false)
   continue;

  if(CalculateMode == 2)
  {
   if(CalcEnable->wait(30) == false)
	continue;
  }
  CalcEnable->reset();

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
  if(EngineIndex>=GetNumEngines())
  {
   CalculationNotInProgress->set();
   continue;
  }

  BeforeCalculate();
  if(CalculationTimeSource == 0)
  {
   MModel_SetDoubleSourceTime(EngineIndex,GetVariantLocalTime());
  }
  else
  if(CalculationTimeSource == 1)
  {
   MModel_SetDoubleSourceTime(EngineIndex,ExternalCurrentTime);
  }
  MEnv_Calculate(EngineIndex,0);
  AfterCalculate();
  RealLastCalculationTime=GetVariantLocalTime();
  CalculationNotInProgress->set();
 }
}

/// Проверяет состояние расчета по id канала
/// 0 - Не считает
/// 1 - Идет расчет
int UEngineControlThread::CheckCalcState(void) const
{
 if(CalcState->wait(0))
  return 1;

 return 0;
}

/// Возвращает состояния запуска треда
int UEngineControlThread::IsCalcStarted(void) const
{
 if(CalcStarted->wait(0))
  return 1;

 return 0;
}

/// Взводит флаг разрешения расчета
bool UEngineControlThread::EnableCalculation(void)
{
 CalcEnable->set();
 return true;
}

/// Запускает аналитику канала
void UEngineControlThread::Start(void)
{
 if(EngineIndex<0 || EngineIndex>GetNumEngines())
  return;

 CalcStarted->set();
 CalcState->set();
}

/// Останавливает аналитику канала
void UEngineControlThread::Pause(void)
{
 if(EngineIndex<0 || EngineIndex>GetNumEngines())
  return;

 CalcStarted->reset();
 CalcState->reset();
}

/// Сбрасывает аналитику канала
void UEngineControlThread::Reset(void)
{
 if(EngineIndex<0 || EngineIndex>GetNumEngines())
  return;

 MEnv_Reset(EngineIndex,0);
 RealLastCalculationTime=0;
}

/// Устанавливает приоритет потока
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
