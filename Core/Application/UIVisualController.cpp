#ifndef UIVisualController_CPP
#define UIVisualController_CPP

#include <algorithm>
#include <typeinfo>
#include "UIVisualController.h"
#include "../../Core/Utilities/USupport.h"
#include "../../Core/System/rdk_system.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

#ifdef WIN32
#undef GetClassName
#endif

namespace RDK {

// ����� ���������-���������� �����������
// ������ ������������, ������� ������ ���� ������� ����� �������
std::vector<RDK::UIVisualController*> UIVisualControllerStorage::InterfaceUpdaters;

/// ����� ����� ���������� ����������
unsigned long long UIVisualControllerStorage::UpdateTime=0;

// ��������� ���������� � ������
void UIVisualControllerStorage::AddInterface(RDK::UIVisualController *value)
{
 std::vector<RDK::UIVisualController*>::iterator I=find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I == InterfaceUpdaters.end())
  InterfaceUpdaters.push_back(value);
}

// ������� ���������� �� ������
void UIVisualControllerStorage::DelInterface(RDK::UIVisualController *value)
{
 std::vector<RDK::UIVisualController*>::iterator I=std::find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I != InterfaceUpdaters.end())
  InterfaceUpdaters.erase(I);
}

// �����, ���������� ����� �������� �������
void UIVisualControllerStorage::AfterLoadProject(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->AfterLoadProject();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterLoadProject - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterLoadProject - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterLoadProject - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}


// �����, ���������� ����� ������� ������
void UIVisualControllerStorage::BeforeReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->BeforeReset();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::BeforeReset - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::BeforeReset - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::BeforeReset - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

// �����, ���������� ����� ������ ������
void UIVisualControllerStorage::AfterReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->AfterReset();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterReset - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterReset - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterReset - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

// �����, ���������� ����� ����� �������
void UIVisualControllerStorage::BeforeCalculate(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
 {
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->ResetCalculationStepUpdatedFlag();
    InterfaceUpdaters[i]->BeforeCalculate();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::BeforeCalculate - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::BeforeCalculate - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::BeforeCalculate - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
 }
}

// �����, ���������� ����� ���� �������
void UIVisualControllerStorage::AfterCalculate(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->AfterCalculate();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterCalculate - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterCalculate - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::AfterCalculate - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

// ���������� ����������
void UIVisualControllerStorage::UpdateInterface(bool force_update)
{
 unsigned long long begin_time=RDK::GetCurrentStartupTime();
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->UpdateInterface(force_update);
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::UpdateInterface - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::UpdateInterface - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::UpdateInterface - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
 UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),begin_time);
}

// ������� ���������� � �������� ���������
void UIVisualControllerStorage::ClearInterface(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->ClearInterface();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::ClearInterface - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::ClearInterface - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::ClearInterface - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

// ��������� ��������� ���������� � xml
void UIVisualControllerStorage::SaveParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->SaveParameters(xml);
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::SaveParameters - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::SaveParameters - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::SaveParameters - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

// ��������� ��������� ���������� �� xml
void UIVisualControllerStorage::LoadParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->LoadParameters(xml);
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::LoadParameters - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::LoadParameters - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::LoadParameters - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
void UIVisualControllerStorage::ResetCalculationStepUpdatedFlag(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->ResetCalculationStepUpdatedFlag();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::ResetCalculationStepUpdatedFlag - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::ResetCalculationStepUpdatedFlag - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::ResetCalculationStepUpdatedFlag - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

/// ���������� ���� ��������� ����������� � ���� �������� �����
void UIVisualControllerStorage::SetCalculationStepUpdatedFlag(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   try
   {
	InterfaceUpdaters[i]->SetCalculationStepUpdatedFlag();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::SetCalculationStepUpdatedFlag - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::SetCalculationStepUpdatedFlag - ")+ex.what()+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIVisualControllerStorage::SetCalculationStepUpdatedFlag - unhandled exception")+std::string(" in ")+InterfaceUpdaters[i]->GetName()).c_str());
   }
  }
}

/// ���������� ����� ���������� ���������� (��)
unsigned long long UIVisualControllerStorage::GetUpdateTime(void)
{
 return UpdateTime;
}


// �����-��������� ���������� �������
// ������ ������������, ������� ������ ���� ������� ����� �������
//std::vector<RDK::UIController*> UIControllerStorage::Controllers;

std::vector<RDK::UIController*>& UIControllerStorage::GetControllers(void)
{
 // ������ ������������, ������� ������ ���� ������� ����� �������
 static std::vector<RDK::UIController*> Controllers;

 return Controllers;
}

// ��������� ���������� � ������
void UIControllerStorage::AddController(RDK::UIController *value)
{
 std::vector<RDK::UIController*>::iterator I=find(GetControllers().begin(),GetControllers().end(),value);
 if(I == GetControllers().end())
  GetControllers().push_back(value);
}

// ������� ���������� �� ������
void UIControllerStorage::DelController(RDK::UIController *value)
{
 std::vector<RDK::UIController*>::iterator I=std::find(GetControllers().begin(),GetControllers().end(),value);
 if(I != GetControllers().end())
  GetControllers().erase(I);
}

// �����, ���������� ����� �������� �������
void UIControllerStorage::AfterLoadProject(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
  {
   try
   {
	GetControllers()[i]->AfterLoadProject();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterLoadProject - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterLoadProject - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterLoadProject - unhandled exception")+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
  }
}


// �����, ���������� ����� ������� ������
void UIControllerStorage::BeforeReset(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
  {
   try
   {
	GetControllers()[i]->BeforeReset();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::BeforeReset - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::BeforeReset - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::BeforeReset - unhandled exception")+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
  }
}

// �����, ���������� ����� ������ ������
void UIControllerStorage::AfterReset(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
  {
   try
   {
	GetControllers()[i]->AfterReset();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterReset - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterReset - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterReset - unhandled exception")+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
  }
}

// �����, ���������� ����� ����� �������
void UIControllerStorage::BeforeCalculate(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
 {
  if(GetControllers()[i])
  {
   try
   {
	GetControllers()[i]->BeforeCalculate();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::BeforeCalculate - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::BeforeCalculate - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::BeforeCalculate - unhandled exception")+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
  }
 }
}

// �����, ���������� ����� ���� �������
void UIControllerStorage::AfterCalculate(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
  {
   try
   {
	GetControllers()[i]->AfterCalculate();
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterCalculate - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterCalculate - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::AfterCalculate - unhandled exception")+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
  }
}

// ��������� ��������� ���������� � xml
void UIControllerStorage::SaveParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
  {
   try
   {
    GetControllers()[i]->SaveParameters(xml);
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::SaveParameters - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::SaveParameters - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::SaveParameters - unhandled exception")+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
  }
}

// ��������� ��������� ���������� �� xml
void UIControllerStorage::LoadParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
  {
   try
   {
	GetControllers()[i]->LoadParameters(xml);
   }
   catch(RDK::UException &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::LoadParameters - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(std::exception &ex)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::LoadParameters - ")+ex.what()+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
   catch(...)
   {
	MLog_LogMessage(RDK_GLOB_MESSAGE, RDK_EX_FATAL, (std::string("UIControllerStorage::LoadParameters - unhandled exception")+std::string(" in ")+GetControllers()[i]->GetName()).c_str());
   }
  }
}


UAppController::UAppController(void)
{
 UIControllerStorage::AddController(this);
}

UAppController::~UAppController(void)
{
 UIControllerStorage::DelController(this);
}

// ����� ��������� �������� �����������
// �����, ���������� ����� �������� �������
void UAppController::AfterLoadProject(void)
{

}

// �����, ���������� ����� ������� ������
void UAppController::BeforeReset(void)
{

}

// �����, ���������� ����� ������ ������
void UAppController::AfterReset(void)
{

}

// �����, ���������� ����� ����� �������
void UAppController::BeforeCalculate(void)
{

}

// �����, ���������� ����� ���� �������
void UAppController::AfterCalculate(void)
{

}

// ���������� ���������� ��� ����������
std::string UAppController::GetName(void)
{
 return Name;
}

// ���������� ��� ������ ����������
std::string UAppController::GetClassName(void)
{
 return typeid(*this).name();
}

// ��������� ��������� ���������� � xml
void UAppController::SaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void UAppController::LoadParameters(RDK::USerStorageXML &xml)
{

}

}

#endif

