#include <algorithm>
#include "UEngineControlQt.h"



// --------------------------
// ������������ � �����������
// --------------------------
UEngineControlQt::UEngineControlQt(void)
{
 Name="EngineControlVcl";
}

UEngineControlQt::~UEngineControlQt(void)
{

}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
/// �������� ������ ����� �������
//RDK::UEngineControlThread* UEngineControlQt::CreateEngineThread(RDK::UEngineControl* engine_control, int channel_index)
//{
// return new TEngineThread(engine_control, channel_index);
//}


/// �������� ������ ����� �������
//RDK::UEngineStateThread* UEngineControlQt::CreateEngineStateThread(RDK::UEngineControl* engine_control)
//{
// return new TEngineMonitorThread(engine_control);
//}

/// ��������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
void UEngineControlQt::StartChannel(int channel_index)
{
 RDK::UEngineControl::StartChannel(channel_index);
 switch(GetThreadMode())
 {
 case 0:
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=1;
  Timer->Enabled=true;
 break;

 case 1:
  TUVisualControllerFrame::CalculationModeFlag=true;
  TUVisualControllerForm::CalculationModeFlag=true;
  Timer->Interval=GetApplication()->GetProjectConfig().MTUpdateInterfaceInterval;
  Timer->Enabled=true;
 break;
 }

}

/// ������������� ��������� ���������� ������, ��� ����, ���� channel_index == -1
void UEngineControlQt::PauseChannel(int channel_index)
{
 RDK::UEngineControl::PauseChannel(channel_index);
 switch(GetThreadMode())
 {
 case 0:
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  Timer->Enabled=false;
 break;

 case 1:
  TUVisualControllerFrame::CalculationModeFlag=false;
  TUVisualControllerForm::CalculationModeFlag=false;
  Timer->Enabled=false;
 break;
 }
}
// --------------------------

void UEngineControlQt::TimerTimer(void)
{
 try
 {
  RdkApplication.GetEngineControl()->TimerExecute();
 }
 catch(RDK::UException &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControlQt::TimerTimer - ")+ex.what()).c_str());
 }
 catch(std::exception &ex)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UEngineControlQt::TimerTimer - ")+ex.what()).c_str());
 }
 catch(...)
 {
  MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, "UEngineControlQt::TimerTimer - unhandled exception");
 }
}

