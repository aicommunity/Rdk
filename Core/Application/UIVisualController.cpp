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

// Класс хранилище-визуальных интерфейсов
// Список обработчиков, которые должны быть вызваны после расчета
std::vector<RDK::UIVisualController*> UIVisualControllerStorage::InterfaceUpdaters;

/// Общее время обновления интерфейса
unsigned long long UIVisualControllerStorage::UpdateTime=0;

// Добавляет обработчик в список
void UIVisualControllerStorage::AddInterface(RDK::UIVisualController *value)
{
 std::vector<RDK::UIVisualController*>::iterator I=find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I == InterfaceUpdaters.end())
  InterfaceUpdaters.push_back(value);
}

// Удаляет обработчик из списка
void UIVisualControllerStorage::DelInterface(RDK::UIVisualController *value)
{
 std::vector<RDK::UIVisualController*>::iterator I=std::find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I != InterfaceUpdaters.end())
  InterfaceUpdaters.erase(I);
}

// Метод, вызываемый после загрузки проекта
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


// Метод, вызываемый перед сбросом модели
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

// Метод, вызываемый после сброса модели
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

// Метод, вызываемый перед шагом расчета
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

// Метод, вызываемый после шага расчета
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

// Обновление интерфейса
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

// Возврат интерфейса в исходное состояние
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

// Сохраняет параметры интерфейса в xml
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

// Загружает параметры интерфейса из xml
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

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
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

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
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

/// Возвращает время обновления интерфейса (мс)
unsigned long long UIVisualControllerStorage::GetUpdateTime(void)
{
 return UpdateTime;
}


// Класс-хранилище внутренних модулей
// Список обработчиков, которые должны быть вызваны после расчета
//std::vector<RDK::UIController*> UIControllerStorage::Controllers;

std::vector<RDK::UIController*>& UIControllerStorage::GetControllers(void)
{
 // Список обработчиков, которые должны быть вызваны после расчета
 static std::vector<RDK::UIController*> Controllers;

 return Controllers;
}

// Добавляет обработчик в список
void UIControllerStorage::AddController(RDK::UIController *value)
{
 std::vector<RDK::UIController*>::iterator I=find(GetControllers().begin(),GetControllers().end(),value);
 if(I == GetControllers().end())
  GetControllers().push_back(value);
}

// Удаляет обработчик из списка
void UIControllerStorage::DelController(RDK::UIController *value)
{
 std::vector<RDK::UIController*>::iterator I=std::find(GetControllers().begin(),GetControllers().end(),value);
 if(I != GetControllers().end())
  GetControllers().erase(I);
}

// Метод, вызываемый после загрузки проекта
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


// Метод, вызываемый перед сбросом модели
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

// Метод, вызываемый после сброса модели
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

// Метод, вызываемый перед шагом расчета
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

// Метод, вызываемый после шага расчета
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

// Сохраняет параметры интерфейса в xml
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

// Загружает параметры интерфейса из xml
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

// Класс дефолтный прототип интерфейсов
// Метод, вызываемый после загрузки проекта
void UAppController::AfterLoadProject(void)
{

}

// Метод, вызываемый перед сбросом модели
void UAppController::BeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void UAppController::AfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void UAppController::BeforeCalculate(void)
{

}

// Метод, вызываемый после шага расчета
void UAppController::AfterCalculate(void)
{

}

// Возвращает уникальное имя интерфейса
std::string UAppController::GetName(void)
{
 return Name;
}

// Возвращает имя класса интерфейса
std::string UAppController::GetClassName(void)
{
 return typeid(*this).name();
}

// Сохраняет параметры интерфейса в xml
void UAppController::SaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void UAppController::LoadParameters(RDK::USerStorageXML &xml)
{

}

}

#endif

