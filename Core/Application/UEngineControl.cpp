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

 PauseEngine(-1);
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

 PauseEngine(-1);
 UseControllersMode=value;
 return;
}

// Управление режимом расчетов
int UEngineControl::GetCalculateMode(int engine_index) const
{
 if(int(EngineControlThreads.size())>engine_index)
  return EngineControlThreads[engine_index]->GetCalculateMode();
 return 0; // TODO: здесь исключение
}

void UEngineControl::SetCalculateMode(int engine_index,int value)
{
 if(int(EngineControlThreads.size())>engine_index)
  EngineControlThreads[engine_index]->SetCalculateMode(value);
}

/// Режим использования времени для расчета
/// 0 - системное время
/// 1 - время источника данных
int UEngineControl::GetCalculationTimeSource(int engine_index) const
{
 if(engine_index>=int(EngineControlThreads.size()))
  return 0; // TODO: тут исключение
 return EngineControlThreads[engine_index]->GetCalculationTimeSource();
}

bool UEngineControl::SetCalculationTimeSource(int engine_index, int value)
{
 if(engine_index>=int(EngineControlThreads.size()))
  return false; // TODO: тут исключение

 EngineControlThreads[engine_index]->SetCalculationTimeSource(value);
 return true;
}

RDK::UTime UEngineControl::GetMinInterstepsInterval(int engine_index) const
{
 if(engine_index>=int(EngineControlThreads.size()))
  return 0; // TODO: тут исключение
 return EngineControlThreads[engine_index]->GetMinInterstepsInterval();
}

bool UEngineControl::SetMinInterstepsInterval(int engine_index, RDK::UTime value)
{
 if(engine_index>=int(EngineControlThreads.size()))
  return false;
 EngineControlThreads[engine_index]->SetMinInterstepsInterval(value);
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
UEngineControlThread* UEngineControl::CreateEngineThread(UEngineControl* engine_control, int engine_index)
{
 return new UEngineControlThread(engine_control, engine_index);
}

/// Создание нового треда расчета
UEngineStateThread* UEngineControl::CreateEngineStateThread(UEngineControl* engine_control)
{
 return new UEngineStateThread(engine_control);
}

// Управление временной меткой сервера
double UEngineControl::GetServerTimeStamp(int engine_index) const
{
 if(engine_index>=GetNumEngines())
  return 0.0;

 return EngineControlThreads[engine_index]->GetServerTimeStamp();
}

void UEngineControl::SetServerTimeStamp(int engine_index, double stamp)
{
 if(engine_index>=GetNumEngines())
  return;
 EngineControlThreads[engine_index]->SetServerTimeStamp(stamp);
 EngineControlThreads[engine_index]->EnableCalculation();
}

/// Запускает аналитику выбранного канала, или всех, если engine_index == -1
void UEngineControl::StartEngine(int engine_index)
{
 if(engine_index>=GetNumEngines())
  return;

 EngineStateThread->RecreateEventsLogFile();
 switch(ThreadMode)
 {
 case 0:
  if(EngineControlThreads[GetSelectedEngineIndex()]->GetCalculateMode() == 1)
   ResetEngine(-1);
//  Timer->Interval=1;
//  Timer->Enabled=true;
//  for(size_t i=0; i<EngineControlThreads.size(); i++)
//   CalculateState[i]=true;
 break;

 case 1:
//  Timer->Interval=30;
//  Timer->Enabled=true;
  if(engine_index<0)
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
	if(EngineControlThreads[engine_index])
	{
	 EngineControlThreads[engine_index]->Start();
	}
  }
 break;
 }

}

/// Останавливает аналитику выбранного канала, или всех, если engine_index == -1
void UEngineControl::PauseEngine(int engine_index)
{
 if(engine_index>=GetNumEngines())
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
  if(engine_index == -1)
  {
//   Timer->Enabled=false;

   for(size_t i=0;i<EngineControlThreads.size();i++)
   {
	if(EngineControlThreads[i])
	{
	 EngineControlThreads[i]->Pause();
	}
   }
  }
  else
  {
	if(EngineControlThreads[engine_index])
	{
	 EngineControlThreads[engine_index]->Pause();
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

 EngineStateThread->RecreateEventsLogFile();
}

/// Сбрасывает аналитику выбранного канала, или всех, если engine_index == -1
void UEngineControl::ResetEngine(int engine_index)
{
 if(engine_index>=GetNumEngines())
  return;

 if(engine_index == -1)
 {
  if(!Model_Check())
  {
   RDK::UIVisualControllerStorage::UpdateInterface();
   return;
  }

  RDK::UIVisualControllerStorage::BeforeReset();
  for(int i=0;i<GetNumEngines();i++)
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
  if(!MModel_Check(engine_index))
  {
   RDK::UIVisualControllerStorage::UpdateInterface();
   return;
  }

  RDK::UIVisualControllerStorage::BeforeReset();
  RDK::UIControllerStorage::BeforeReset(engine_index);
  EngineControlThreads[engine_index]->Reset();
  RDK::UIControllerStorage::AfterReset(engine_index);
  RDK::UIVisualControllerStorage::AfterReset();
  RDK::UIVisualControllerStorage::UpdateInterface();
 }
}

/// Делает шаг расчета выбранного канала, или всех, если engine_index == -1
void UEngineControl::StepEngine(int engine_index)
{
 if(engine_index>=GetNumEngines())
  return;

 RDK::UIVisualControllerStorage::BeforeCalculate();
 int use_controllers_mode=UseControllersMode;
 if(use_controllers_mode == 0)
  RDK::UIControllerStorage::BeforeCalculate(engine_index);

 if(engine_index <0)
 {
  for(int i=0;i<GetNumEngines();i++)
  {
   if(MIsEngineInit(i) && MModel_Check(i))
   {
	EngineControlThreads[i]->EnableCalculation();
	EngineControlThreads[i]->Calculate();
   }
  }
 }
 else
 {
  if(MIsEngineInit(engine_index) && MModel_Check(engine_index))
  {
   EngineControlThreads[engine_index]->EnableCalculation();
   EngineControlThreads[engine_index]->Calculate();
  }
 }

 if(use_controllers_mode == 0)
  RDK::UIControllerStorage::AfterCalculate(engine_index);
 SendMetadata();
 RDK::UIVisualControllerStorage::AfterCalculate();
 RDK::UIVisualControllerStorage::ResetCalculationStepUpdatedFlag();
 RDK::UIVisualControllerStorage::UpdateInterface();

 if(engine_index <0)
 {
  for(int i=0;i<GetNumEngines();i++)
  {
   EngineControlThreads[i]->GetProfiler()->Calculate();
  }
 }
 else
 {
  EngineControlThreads[engine_index]->GetProfiler()->Calculate();
 }
}


void UEngineControl::TimerExecute(void)
{
 switch(ThreadMode)
 {
 case 0:
 {
  StepEngine(-1);
 }
 break;

 case 1:
 {
  if(UseControllersMode == 0)
   RDK::UIControllerStorage::AfterCalculate(-1);
  SendMetadata();
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::ResetCalculationStepUpdatedFlag();
  for(int i=0;i<GetNumEngines();i++)
   EngineControlThreads[i]->GetProfiler()->CalcProfilerOutputData();
  RDK::UIVisualControllerStorage::UpdateInterface();

  for(int i=0;i<GetNumEngines();i++)
  {
   EngineControlThreads[i]->GetProfiler()->CalculateGui();
   EngineControlThreads[i]->GetProfiler()->CalcProfilerOutputData();
  }
 }
 break;
 }
}

/// Проверяет состояние расчета
/// 0 - Не считает
/// 1 - Идет расчет
int UEngineControl::CheckCalcState(int channel_id) const
{
 if(channel_id<0 || channel_id>GetNumEngines())
  return 0;

 switch(ThreadMode)
 {
 case 0:
  return 0;
 break;

 case 1:
  return EngineControlThreads[channel_id]->CheckCalcState();
 break;
 }
 return 0;
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
int UEngineControl::GetNumEngines(void) const
{
 return ::GetNumEngines();
}

bool UEngineControl::SetNumEngines(int num)
{
 if(num == int(EngineControlThreads.size()))
  return true;
 int old_num=GetNumEngines();
 ::SetNumEngines(num);

 int old_size=int(EngineControlThreads.size());
 for(int i=num;i<old_size;i++)
 {
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

bool UEngineControl::InsertEngine(int index)
{
 int old_num=GetNumEngines();
 Engine_Add(index);
 int new_num=GetNumEngines();

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

bool UEngineControl::DeleteEngine(int index)
{
 int old_num=GetNumEngines();

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

 int del_res=Engine_Del(index);
 int new_num=GetNumEngines();

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
// --------------------------

}

#endif
