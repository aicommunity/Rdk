#ifndef UENGINE_CONTROL_CPP
#define UENGINE_CONTROL_CPP

#include "UEngineControl.h"
#include "UEngineStateThread.h"
#include "UEngineControlThread.h"
#include "UChannelProfiler.h"
#include "UBroadcasterInterface.h"
#include "UApplication.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineControl::UEngineControl(void)
{
    ThreadCalcCompleteEvent=0;//UCreateEvent(false);
    
    EngineStateThread=0;
    
    UseControllersMode=0;

    UpdateInterval = 100;
    
    InitFlag=false;
    
    Name="EngineControl";
}

UEngineControl::~UEngineControl(void)
{
 UnInit();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Управление режимом работы
/// 0 - однопоточный (одноканальный) режим
/// 1 - многопоточный режим
int UEngineControl::GetThreadMode(void) const
{
 return ThreadMode;
}

void UEngineControl::SetThreadMode(int mode)
{
 if(ThreadMode == mode)
  return;

 PauseChannel(-1);
 ThreadMode=mode;
}

/// Режим вызова контроллеров
/// 0 - при обновлении интерфейса
/// 1 - индивидуально для каждого канала во время каждого расчета канала
int UEngineControl::GetUseControllersMode(void) const
{
 return UseControllersMode;
}

void UEngineControl::SetUseControllersMode(int value)
{
 if(UseControllersMode == value)
  return;

 PauseChannel(-1);
 UseControllersMode=value;
 return;
}

// Управление режимом расчетов
int UEngineControl::GetCalculateMode(int channel_index) const
{
 if(int(EngineControlThreads.size())>channel_index)
  return EngineControlThreads[channel_index]->GetCalculateMode();
 return 0; // TODO: здесь исключение
}

void UEngineControl::SetCalculateMode(int channel_index,int value)
{
 if(int(EngineControlThreads.size())>channel_index)
  EngineControlThreads[channel_index]->SetCalculateMode(value);
}

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
int UEngineControl::GetCalculationTimeSource(int channel_index) const
{
 if(channel_index>=int(EngineControlThreads.size()))
  return 0; // TODO: тут исключение
 return EngineControlThreads[channel_index]->GetCalculationTimeSource();
}

bool UEngineControl::SetCalculationTimeSource(int channel_index, int value)
{
 if(channel_index>=int(EngineControlThreads.size()))
  return false; // TODO: тут исключение

 EngineControlThreads[channel_index]->SetCalculationTimeSource(value);
 return true;
}

RDK::UTime UEngineControl::GetMinInterstepsInterval(int channel_index) const
{
 if(channel_index>=int(EngineControlThreads.size()))
  return 0; // TODO: тут исключение
 return EngineControlThreads[channel_index]->GetMinInterstepsInterval();
}

bool UEngineControl::SetMinInterstepsInterval(int channel_index, RDK::UTime value)
{
 if(channel_index>=int(EngineControlThreads.size()))
  return false;
 EngineControlThreads[channel_index]->SetMinInterstepsInterval(value);
 return true;
}
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> UEngineControl::GetApplication(void)
{
 return Application;
}

bool UEngineControl::SetApplication(UEPtr<UApplication> value)
{
 if(Application == value)
  return true;

 Application=value;
 return true;
}

UEngineControlThread* UEngineControl::GetEngineThread(int i)
{
 if(i<0 || i>= int(EngineControlThreads.size()))
  return 0;

 return EngineControlThreads[i];
}

/// Доступ к треду мониторинга состояния модулей сервера
UEngineStateThread* UEngineControl::GetEngineStateThread(void)
{
 return EngineStateThread;
}

/// Возвращает данные о производительности канала
UChannelProfiler* UEngineControl::GetChannelProfiler(int i)
{
 if(i<0 || i>= int(EngineControlThreads.size()))
  return 0;

 return EngineControlThreads[i]->GetProfiler();
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
/// Инициализация (выполняется первой)
void UEngineControl::Init(void)
{
 if(IsInit())
  return;

 ThreadCalcCompleteEvent=UCreateEvent(false);

 EngineStateThread=0;

 EngineStateThread=CreateEngineStateThread(this);
 InitFlag=true;
}

/// Деинициализация (выполняется последней)
void UEngineControl::UnInit(void)
{
 if(!IsInit())
  return;

 UDestroyEvent(ThreadCalcCompleteEvent);
 ThreadCalcCompleteEvent=0;

 if(EngineStateThread)
 {
  EngineStateThread->Terminate();
  delete EngineStateThread;
  EngineStateThread=0;
 }
 InitFlag=false;
}

/// Проверка состояния инициализации
bool UEngineControl::IsInit(void) const
{
 return InitFlag;
}

/// Создание нового треда расчета
UEngineControlThread* UEngineControl::CreateEngineThread(UEngineControl* engine_control, int channel_index)
{
 return new UEngineControlThread(engine_control, channel_index);
}

/// Создание нового треда расчета
UEngineStateThread* UEngineControl::CreateEngineStateThread(UEngineControl* engine_control)
{
 return new UEngineStateThread(engine_control);
}

// Управление временной меткой сервера
double UEngineControl::GetServerTimeStamp(int channel_index) const
{
 if(channel_index>=GetNumChannels())
  return 0.0;

 return EngineControlThreads[channel_index]->GetServerTimeStamp();
}

void UEngineControl::SetServerTimeStamp(int channel_index, double stamp)
{
 if(channel_index>=GetNumChannels())
  return;
 EngineControlThreads[channel_index]->SetServerTimeStamp(stamp);
 EngineControlThreads[channel_index]->EnableCalculation();
}

/// Запускает аналитику выбранного канала, или всех, если channel_index == -1
void UEngineControl::StartChannel(int channel_index)
{
 if(channel_index>=GetNumChannels())
  return;

 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->RecreateEventsLogFile();
// EngineStateThread->RecreateEventsLogFile();
 switch(ThreadMode)
 {
 case 0:
  if(EngineControlThreads[Core_GetSelectedChannelIndex()]->GetCalculateMode() == 1)
   ResetChannel(-1);
//  Timer->Interval=1;
//  Timer->Enabled=true;
//  for(size_t i=0; i<EngineControlThreads.size(); i++)
//   CalculateState[i]=true;
 break;

 case 1:
//  Timer->Interval=30;
//  Timer->Enabled=true;
  if(channel_index<0)
  {
   for(size_t i=0;i<EngineControlThreads.size();i++)
   {
	if(EngineControlThreads[i])
	{
	 EngineControlThreads[i]->Start();
	}
   }
  }
  else
  {
	if(EngineControlThreads[channel_index])
	{
	 EngineControlThreads[channel_index]->Start();
	}
  }
 break;
 }

}

/// Останавливает аналитику выбранного канала, или всех, если channel_index == -1
void UEngineControl::PauseChannel(int channel_index)
{
 if(channel_index>=GetNumChannels())
  return;

// if(Timer->Enabled == false)
//  return;

 switch(ThreadMode)
 {
 case 0:
//  Timer->Enabled=false;
//  for(size_t i=0; i<EngineControlThreads.size(); i++)
//   CalculateState[i]=false;
 break;

 case 1:
  if(channel_index == -1)
  {
//   Timer->Enabled=false;

   for(size_t i=0;i<EngineControlThreads.size();i++)
   {
	if(EngineControlThreads[i])
	{
	 EngineControlThreads[i]->Pause();
	 if(!EngineControlThreads[i]->WaitForCalculationComplete())
	 {
	  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_WARNING, (std::string("UEngineControl::PauseChannel - Calculation complete waiting timed out. Channel: ")+sntoa(i)).c_str());
	 }
	}
   }
  }
  else
  {
	if(EngineControlThreads[channel_index])
	{
	 EngineControlThreads[channel_index]->Pause();
	}

	if(!EngineControlThreads[channel_index]->WaitForCalculationComplete())
	{
	 MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_WARNING, (std::string("UEngineControl::PauseChannel - Calculation complete waiting timed out. Channel: ")+sntoa(channel_index)).c_str());
	}

   bool all_stopped=true;
   for(size_t i=0;i<EngineControlThreads.size();i++)
   {
	if(EngineControlThreads[i])
	{
	 if(!EngineControlThreads[i]->IsCalcStarted())
	  all_stopped&=true;
	 else
	  all_stopped&=false;
	}
   }

   if(all_stopped)
   {
//	Timer->Enabled=false;
   }
  }
 break;
 }

 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->Clear();
// EngineStateThread->CloseEventsLogFile();
}

/// Сбрасывает аналитику выбранного канала, или всех, если channel_index == -1
void UEngineControl::ResetChannel(int channel_index)
{
 if(channel_index>=GetNumChannels())
  return;

 GetCore()->GetLogger(RDK_GLOB_MESSAGE)->RecreateEventsLogFile();
// EngineStateThread->RecreateEventsLogFile();
 if(channel_index == -1)
 {
  if(!Model_Check())
  {
   RDK::UIVisualControllerStorage::UpdateInterface();
   return;
  }

  RDK::UIVisualControllerStorage::BeforeReset();
  for(int i=0;i<GetNumChannels();i++)
  {
   RDK::UIControllerStorage::BeforeReset(i);
   EngineControlThreads[i]->Reset();
   RDK::UIControllerStorage::AfterReset(i);
  }
  RDK::UIVisualControllerStorage::AfterReset();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
 else
 {
  if(!MModel_Check(channel_index))
  {
   RDK::UIVisualControllerStorage::UpdateInterface();
   return;
  }

  RDK::UIVisualControllerStorage::BeforeReset();
  RDK::UIControllerStorage::BeforeReset(channel_index);
  EngineControlThreads[channel_index]->Reset();
  RDK::UIControllerStorage::AfterReset(channel_index);
  RDK::UIVisualControllerStorage::AfterReset();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
}

/// Делает шаг расчета выбранного канала, или всех, если channel_index == -1
void UEngineControl::StepChannel(int channel_index)
{
 int num_channels=GetNumChannels();
 if(channel_index>=num_channels)
  return;

 RDK::UIVisualControllerStorage::BeforeCalculate();
 int use_controllers_mode=UseControllersMode;
 if(use_controllers_mode == 0)
  RDK::UIControllerStorage::BeforeCalculate(channel_index);

 if(channel_index <0)
 {
  for(int i=0;i<num_channels;i++)
  {
   if(MCore_IsChannelInit(i) && MModel_Check(i))
   {
	EngineControlThreads[i]->EnableCalculation();
	EngineControlThreads[i]->Calculate();
   }
  }
 }
 else
 {
  if(MCore_IsChannelInit(channel_index) && MModel_Check(channel_index))
  {
   EngineControlThreads[channel_index]->EnableCalculation();
   EngineControlThreads[channel_index]->Calculate();
  }
 }

 if(use_controllers_mode == 0)
  RDK::UIControllerStorage::AfterCalculate(channel_index);

 try
 {
  SendMetadata();
 }
 catch(std::exception &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControl::StepChannel:SendMetadata - ")+ex.what()).c_str());
  throw;
 }
 catch(...)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "UEngineControl::StepChannel:SendMetadata - unhandled exception");
  throw;
 }
 RDK::UIVisualControllerStorage::AfterCalculate();
 RDK::UIVisualControllerStorage::ResetCalculationStepUpdatedFlag();
 RDK::UIVisualControllerStorage::UpdateInterface();

 try
 {
  if(channel_index <0)
  {
   for(int i=0;i<num_channels;i++)
	EngineControlThreads[i]->GetProfiler()->Calculate();
  }
  else
  {
   EngineControlThreads[channel_index]->GetProfiler()->Calculate();
  }
 }
 catch(std::exception &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControl::TimerExecute:Calculate profiler stats - ")+ex.what()).c_str());
 }
 catch(...)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "UEngineControl::TimerExecute:Calculate profiler stats - unhandled exception");
  throw;
 }
}


void UEngineControl::TimerExecute(void)
{
 switch(ThreadMode)
 {
 case 0:
 {
  StepChannel(-1);
 }
 break;

 case 1:
 {
  if(UseControllersMode == 0)
   RDK::UIControllerStorage::AfterCalculate(-1);

  try
  {
   SendMetadata();
  }
  catch(std::exception &ex)
  {
   MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControl::TimerExecute:SendMetadata - ")+ex.what()).c_str());
   throw;
  }
  catch(...)
  {
   MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "UEngineControl::TimerExecute:SendMetadata - unhandled exception");
   throw;
  }
//  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::ResetCalculationStepUpdatedFlag();
//  for(int i=0;i<GetNumChannels();i++)
//   EngineControlThreads[i]->GetProfiler()->CalcProfilerOutputData();
  RDK::UIVisualControllerStorage::UpdateInterface();

  try
  {
   int num_channels=GetNumChannels();
   for(int i=0;i<num_channels;i++)
   {
	EngineControlThreads[i]->GetProfiler()->CalculateGui();
	EngineControlThreads[i]->GetProfiler()->CalcProfilerOutputData();
   }
  }
  catch(std::exception &ex)
  {
   MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControl::TimerExecute:Calculate profiler stats - ")+ex.what()).c_str());
  }
  catch(...)
  {
   MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "UEngineControl::TimerExecute:Calculate profiler stats - unhandled exception");
   throw;
  }
 }
 break;
 }
}

/// Проверяет состояние расчета
UEngineControl::UCalcState UEngineControl::CheckCalcState(int channel_id) const
{
 if(channel_id<0 || channel_id>GetNumChannels())
  return csStopped;

 switch(ThreadMode)
 {
 case 0:
  return csStopped;
 break;

 case 1:
 {
  UEngineControlThread::UCalcState state=EngineControlThreads[channel_id]->CheckCalcState();
  if(state == UEngineControlThread::UCalcState::csStopped)
   return csStopped;
  else
  if(state == UEngineControlThread::UCalcState::csRunning)
  {
   if(EngineStateThread)
   {
	UEngineStateThread::UCalcState thread_state=EngineStateThread->ReadCalcThreadState(channel_id);
	if(thread_state == UEngineStateThread::UCalcState::csRunning)
	 return csRunning;
	else
	if(thread_state == UEngineStateThread::UCalcState::csStopped)
	 return csStopped;
	else
	if(thread_state == UEngineStateThread::UCalcState::csHanging)
	 return csHanging;
	else
	 return csUnknown;
   }
   else
	return csUnknown;
  }
  else
   return csUnknown;
 }
 break;
 }
 return csUnknown;
}

/// Вклчает мониторинг сервера
void UEngineControl::StartEngineStateThread(void)
{
 if(EngineStateThread)
  EngineStateThread->CalcStarted->set();
}

/// Останавливает мониторинг сервера
void UEngineControl::StopEngineStateThread(void)
{
 if(EngineStateThread)
 {
  EngineStateThread->CalcStarted->reset();
  EngineStateThread->CalculationNotInProgress->wait(10000);
 }
}

/// Регистрирует вещатель метаданных
void UEngineControl::RegisterMetadataBroadcaster(UBroadcasterInterface *broadcaster)
{
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i] == broadcaster)
   return;
 }
 BroadcastersList.push_back(broadcaster);
}

/// Снимает регистрацию вещателя метаданных
void UEngineControl::UnRegisterMetadataBroadcaster(UBroadcasterInterface *broadcaster)
{
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i] == broadcaster)
  {
   BroadcastersList.erase(BroadcastersList.begin()+i);
   return;
  }
 }
}

/// Отправляет метаданные во все зарегистрированные вещатели
bool UEngineControl::AddMetadata(int channel_index, double time_stamp)
{
 bool res=true;
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i])
   res&=BroadcastersList[i]->AddMetadata(channel_index,time_stamp);
 }
 return res;
}

/// Инициирует процедуру отправки метаданных всеми зарегистрированными вещателями
bool UEngineControl::SendMetadata(void)
{
 bool res=true;
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i])
   res&=BroadcastersList[i]->SendMetadata();
 }
 return res;
}
//---------------------------------------------------------------------------

// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
void UEngineControl::SaveParameters(RDK::USerStorageXML &xml)
{
 xml.WriteInteger("ThreadMode",ThreadMode);
 xml.WriteInteger("UseControllersMode",UseControllersMode);

 if(!xml.SelectNode("EngineControlThreads"))
  xml.AddNode("EngineControlThreads");
 for(size_t i=0;i<EngineControlThreads.size();i++)
 {
  if(!xml.SelectNode(sntoa(i)))
   xml.AddNode(sntoa(i));
  xml.WriteInteger("CalculateMode",EngineControlThreads[i]->GetCalculateMode());
  xml.WriteInteger("MinInterstepsInterval",EngineControlThreads[i]->GetMinInterstepsInterval());
  xml.WriteInteger("CalculationTimeSource",EngineControlThreads[i]->GetCalculationTimeSource());

  // Данные профилирования
  if(!xml.SelectNode("Profiling"))
   xml.AddNode("Profiling");
  EngineControlThreads[i]->GetProfiler()->SaveParameters(xml);
  xml.SelectUp();
  // Конец данных профилирования

  xml.SelectUp();
 }
 xml.SelectUp();


}

// Загружает параметры интерфейса из xml
void UEngineControl::LoadParameters(RDK::USerStorageXML &xml)
{
 SetThreadMode(xml.ReadInteger("ThreadMode",ThreadMode));
 SetUseControllersMode(xml.ReadInteger("UseControllersMode",UseControllersMode));

 if(xml.SelectNode("EngineControlThreads"))
 {
  for(size_t i=0;i<EngineControlThreads.size();i++)
  {
   if(xml.SelectNode(sntoa(i)))
   {
	EngineControlThreads[i]->SetCalculateMode(xml.ReadInteger("CalculateMode",EngineControlThreads[i]->GetCalculateMode()));
	EngineControlThreads[i]->SetMinInterstepsInterval(xml.ReadInteger("MinInterstepsInterval",EngineControlThreads[i]->GetMinInterstepsInterval()));
	EngineControlThreads[i]->SetCalculationTimeSource(xml.ReadInteger("CalculationTimeSource",EngineControlThreads[i]->GetCalculationTimeSource()));

	if(xml.SelectNode("Profiling"))
	{
	 EngineControlThreads[i]->GetProfiler()->LoadParameters(xml);
	 xml.SelectUp();
	}

    xml.SelectUp();
   }
  }
  xml.SelectUp();
 }
}
// --------------------------

// --------------------------
/// Управление числом каналов
// --------------------------
/// Управление числом каналов
int UEngineControl::GetNumChannels(void) const
{
 return ::Core_GetNumChannels();
}

bool UEngineControl::SetNumChannels(int num)
{
 if(num == int(EngineControlThreads.size()))
  return true;
 ::Core_SetNumChannels(num);

 int old_size=int(EngineControlThreads.size());
 for(int i=num;i<old_size;i++)
 {
  PauseChannel(i);
  delete EngineControlThreads[i];
 }

 EngineControlThreads.resize(num);
 for(int i=old_size;i<num;i++)
 {
  EngineControlThreads[i]=CreateEngineThread(this, i);
  #ifdef RDK_MUTEX_DEADLOCK_DEBUG
  TUThreadInfo info;
  info.Pid=ThreadChannels[i]->ThreadID;
  info.Name=string("ThreadChannels ")+RDK::sntoa(i);
  GlobalThreadInfoMap[info.Pid]=info;
  #endif
  EngineControlThreads[i]->SetPriority(RDK_DEFAULT_THREAD_PRIORITY);
  if(EngineStateThread)
   EngineStateThread->RegisterCalcThread(i,EngineControlThreads[i]);

//  ThreadChannels[i]->FreeOnTerminate=false;
 }

 return true;
}

bool UEngineControl::InsertChannel(int index)
{
 Core_AddChannel(index);
 int new_num=GetNumChannels();

 EngineControlThreads.resize(new_num);

 for(int i=new_num-1;i>index;i--)
 {
  if(EngineStateThread)
   EngineStateThread->UnRegisterCalcThread(i-1);
  EngineControlThreads[i]=EngineControlThreads[i-1];
  if(EngineStateThread)
   EngineStateThread->RegisterCalcThread(i,EngineControlThreads[i]);
 }

 EngineControlThreads[index]=CreateEngineThread(this, index);
 EngineControlThreads[index]->SetPriority(RDK_DEFAULT_THREAD_PRIORITY);
 if(EngineStateThread)
  EngineStateThread->RegisterCalcThread(index,EngineControlThreads[index]);
 return true;
}

bool UEngineControl::DeleteChannel(int index)
{
 int old_num=GetNumChannels();

// if(index<0 || index>=int(ThreadChannels.size()))
//  return true;

 if(EngineControlThreads[index])
 {
//  ThreadChannels[index]->Terminate();
  if(EngineStateThread)
   EngineStateThread->UnRegisterCalcThread(index);
  delete EngineControlThreads[index];
  EngineControlThreads[index]=0;
 }

 int del_res=Core_DelChannel(index);
 int new_num=Core_GetNumChannels();

 if(new_num == old_num)
  return true;

 for(int i=index;i<new_num;i++)
 {
  if(EngineStateThread)
   EngineStateThread->UnRegisterCalcThread(i+1);
  EngineControlThreads[i]=EngineControlThreads[i+1];
  if(EngineStateThread)
   EngineStateThread->RegisterCalcThread(i,EngineControlThreads[i]);
 }


 EngineControlThreads.resize(new_num);

 return true;
}

int UEngineControl::getUpdateInterval() const
{
    return UpdateInterval;
}

void UEngineControl::setUpdateInterval(const int value)
{
    UpdateInterval = value;
}

// --------------------------

}

#endif
