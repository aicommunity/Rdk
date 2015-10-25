#ifndef UIVisualController_CPP
#define UIVisualController_CPP

#include <algorithm>
#include "UIVisualController.h"
#include "../../Core/Utilities/USupport.h"
#include "../../Core/System/rdk_system.h"

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
   InterfaceUpdaters[i]->AfterLoadProject();
}


// �����, ���������� ����� ������� ������
void UIVisualControllerStorage::BeforeReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->BeforeReset();
}

// �����, ���������� ����� ������ ������
void UIVisualControllerStorage::AfterReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->AfterReset();
}

// �����, ���������� ����� ����� �������
void UIVisualControllerStorage::BeforeCalculate(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
 {
  if(InterfaceUpdaters[i])
  {
   InterfaceUpdaters[i]->ResetCalculationStepUpdatedFlag();
   InterfaceUpdaters[i]->BeforeCalculate();
  }
 }
}

// �����, ���������� ����� ���� �������
void UIVisualControllerStorage::AfterCalculate(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->AfterCalculate();
}

// ���������� ����������
void UIVisualControllerStorage::UpdateInterface(bool force_update)
{
 unsigned long long begin_time=RDK::GetCurrentStartupTime();
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->UpdateInterface(force_update);
 UpdateTime=RDK::CalcDiffTime(RDK::GetCurrentStartupTime(),begin_time);
}

// ������� ���������� � �������� ���������
void UIVisualControllerStorage::ClearInterface(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->ClearInterface();
}

// ��������� ��������� ���������� � xml
void UIVisualControllerStorage::SaveParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->SaveParameters(xml);
}

// ��������� ��������� ���������� �� xml
void UIVisualControllerStorage::LoadParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
   InterfaceUpdaters[i]->LoadParameters(xml);
  }
}

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
void UIVisualControllerStorage::ResetCalculationStepUpdatedFlag(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->ResetCalculationStepUpdatedFlag();
}

/// ���������� ���� ��������� ����������� � ���� �������� �����
void UIVisualControllerStorage::SetCalculationStepUpdatedFlag(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->SetCalculationStepUpdatedFlag();
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
   GetControllers()[i]->AfterLoadProject();
}


// �����, ���������� ����� ������� ������
void UIControllerStorage::BeforeReset(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
   GetControllers()[i]->BeforeReset();
}

// �����, ���������� ����� ������ ������
void UIControllerStorage::AfterReset(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
   GetControllers()[i]->AfterReset();
}

// �����, ���������� ����� ����� �������
void UIControllerStorage::BeforeCalculate(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
 {
  if(GetControllers()[i])
  {
   GetControllers()[i]->BeforeCalculate();
  }
 }
}

// �����, ���������� ����� ���� �������
void UIControllerStorage::AfterCalculate(int channel_index)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
   GetControllers()[i]->AfterCalculate();
}

// ��������� ��������� ���������� � xml
void UIControllerStorage::SaveParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
   GetControllers()[i]->SaveParameters(xml);
}

// ��������� ��������� ���������� �� xml
void UIControllerStorage::LoadParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<GetControllers().size();i++)
  if(GetControllers()[i])
  {
   GetControllers()[i]->LoadParameters(xml);
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

