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
// ������������ � �����������
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
// ������ ���������� �����������
// --------------------------
/// ���������� ������� ������
/// 0 - ������������ (�������������) �����
/// 1 - ������������� �����
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

/// ����� ������ ������������
/// 0 - ��� ���������� ����������
/// 1 - ������������� ��� ������� ������ �� ����� ������� ������� ������
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

// ���������� ������� ��������
int UEngineControl::GetCalculateMode(int channel_index) const
{
 if(int(EngineControlThreads.size())>channel_index)
  return EngineControlThreads[channel_index]->GetCalculateMode();
 return 0; // TODO: ����� ����������
}

void UEngineControl::SetCalculateMode(int channel_index,int value)
{
 if(int(EngineControlThreads.size())>channel_index)
  EngineControlThreads[channel_index]->SetCalculateMode(value);
}

/// ����� ������������� ������� ��� �������
/// 0 - ��������� �����
/// 1 - ����� ��������� ������
int UEngineControl::GetCalculationTimeSource(int channel_index) const
{
 if(channel_index>=int(EngineControlThreads.size()))
  return 0; // TODO: ��� ����������
 return EngineControlThreads[channel_index]->GetCalculationTimeSource();
}

bool UEngineControl::SetCalculationTimeSource(int channel_index, int value)
{
 if(channel_index>=int(EngineControlThreads.size()))
  return false; // TODO: ��� ����������

 EngineControlThreads[channel_index]->SetCalculationTimeSource(value);
 return true;
}

RDK::UTime UEngineControl::GetMinInterstepsInterval(int channel_index) const
{
 if(channel_index>=int(EngineControlThreads.size()))
  return 0; // TODO: ��� ����������
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
// ������ ������� � ������
// --------------------------
/// ���������� ��������� �� ��������� ����������
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

/// ������ � ����� ����������� ��������� ������� �������
UEngineStateThread* UEngineControl::GetEngineStateThread(void)
{
 return EngineStateThread;
}

/// ���������� ������ � ������������������ ������
UChannelProfiler* UEngineControl::GetChannelProfiler(int i)
{
 if(i<0 || i>= int(EngineControlThreads.size()))
  return 0;

 return EngineControlThreads[i]->GetProfiler();
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// ������������� (����������� ������)
void UEngineControl::Init(void)
{
 if(IsInit())
  return;

 ThreadCalcCompleteEvent=UCreateEvent(false);

 EngineStateThread=0;

 EngineStateThread=CreateEngineStateThread(this);
 InitFlag=true;
}

/// ��������������� (����������� ���������)
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

/// �������� ��������� �������������
bool UEngineControl::IsInit(void) const
{
 return InitFlag;
}

/// �������� ������ ����� �������
UEngineControlThread* UEngineControl::CreateEngineThread(UEngineControl* engine_control, int channel_index)
{
 return new UEngineControlThread(engine_control, channel_index);
}

/// �������� ������ ����� �������
UEngineStateThread* UEngineControl::CreateEngineStateThread(UEngineControl* engine_control)
{
 return new UEngineStateThread(engine_control);
}

// ���������� ��������� ������ �������
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

/// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
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

/// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
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
	}
   }
  }
  else
  {
	if(EngineControlThreads[channel_index])
	{
	 EngineControlThreads[channel_index]->Pause();
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

/// ���������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
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

/// ������ ��� ������� ���������� ������, ��� ����, ���� channel_index == -1
void UEngineControl::StepChannel(int channel_index)
{
 if(channel_index>=GetNumChannels())
  return;

 RDK::UIVisualControllerStorage::BeforeCalculate();
 int use_controllers_mode=UseControllersMode;
 if(use_controllers_mode == 0)
  RDK::UIControllerStorage::BeforeCalculate(channel_index);

 if(channel_index <0)
 {
  for(int i=0;i<GetNumChannels();i++)
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
 SendMetadata();
 RDK::UIVisualControllerStorage::AfterCalculate();
 RDK::UIVisualControllerStorage::ResetCalculationStepUpdatedFlag();
 RDK::UIVisualControllerStorage::UpdateInterface();

 if(channel_index <0)
 {
  for(int i=0;i<GetNumChannels();i++)
  {
   EngineControlThreads[i]->GetProfiler()->Calculate();
  }
 }
 else
 {
  EngineControlThreads[channel_index]->GetProfiler()->Calculate();
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
  SendMetadata();
  RDK::UIVisualControllerStorage::AfterCalculate();
  RDK::UIVisualControllerStorage::ResetCalculationStepUpdatedFlag();
  for(int i=0;i<GetNumChannels();i++)
   EngineControlThreads[i]->GetProfiler()->CalcProfilerOutputData();
  RDK::UIVisualControllerStorage::UpdateInterface();

  for(int i=0;i<GetNumChannels();i++)
  {
   EngineControlThreads[i]->GetProfiler()->CalculateGui();
   EngineControlThreads[i]->GetProfiler()->CalcProfilerOutputData();
  }
 }
 break;
 }
}

/// ��������� ��������� �������
/// 0 - �� �������
/// 1 - ���� ������
int UEngineControl::CheckCalcState(int channel_id) const
{
 if(channel_id<0 || channel_id>GetNumChannels())
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

/// ������� ���������� �������
void UEngineControl::StartEngineStateThread(void)
{
 if(EngineStateThread)
  EngineStateThread->CalcStarted->set();
}

/// ������������� ���������� �������
void UEngineControl::StopEngineStateThread(void)
{
 if(EngineStateThread)
 {
  EngineStateThread->CalcStarted->reset();
  EngineStateThread->CalculationNotInProgress->wait(10000);
 }
}

/// ������������ �������� ����������
void UEngineControl::RegisterMetadataBroadcaster(UBroadcasterInterface *broadcaster)
{
 for(size_t i=0;i<BroadcastersList.size();i++)
 {
  if(BroadcastersList[i] == broadcaster)
   return;
 }
 BroadcastersList.push_back(broadcaster);
}

/// ������� ����������� �������� ����������
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

/// ���������� ���������� �� ��� ������������������ ��������
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

/// ���������� ��������� �������� ���������� ����� ������������������� ����������
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
// ����� ������ ���������� ������������
// --------------------------
// ��������� ��������� ���������� � xml
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

  // ������ ��������������
  if(!xml.SelectNode("Profiling"))
   xml.AddNode("Profiling");
  EngineControlThreads[i]->GetProfiler()->SaveParameters(xml);
  xml.SelectUp();
  // ����� ������ ��������������

  xml.SelectUp();
 }
 xml.SelectUp();


}

// ��������� ��������� ���������� �� xml
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
/// ���������� ������ �������
// --------------------------
/// ���������� ������ �������
int UEngineControl::GetNumChannels(void) const
{
 return ::Core_GetNumChannels();
}

bool UEngineControl::SetNumChannels(int num)
{
 if(num == int(EngineControlThreads.size()))
  return true;
 int old_num=GetNumChannels();
 ::Core_SetNumChannels(num);

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

bool UEngineControl::InsertChannel(int index)
{
 int old_num=GetNumChannels();
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
// --------------------------

}

#endif
