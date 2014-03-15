#ifndef UIVisualController_CPP
#define UIVisualController_CPP

#include "UIVisualController.h"
                 
namespace RDK {

// Глобальная коллекция шрифтов
RDK::UBitmapFontCollection GlobalFonts;

// Очищает коллекцию глобальных шрифтов
void ClearClobalFonts(void)
{
 GlobalFonts.DelAllFonts();
}

// Загружает новый глобальный шрифт
bool AddGlobalFont(const std::string &font_file_name)
{
 RDK::UBitmapFont font;
 std::size_t dir_sep_pos=font_file_name.find_last_of("\\/");
 std::string font_name;
 if(dir_sep_pos != string::npos)
  font_name=font_file_name.substr(dir_sep_pos+1);
 else
  font_name=font_file_name;
 std::size_t _pos=font_name.find_first_of("_");
 std::size_t _pos2=font_name.find_first_not_of("0123456789",_pos+1);
 if(_pos != std::string::npos)
 {
  std::string font_string_size=font_name.substr(_pos+1,_pos2);
  int size=RDK::atoi(font_string_size);
  if(!font.LoadFromFile(font_name.substr(0,_pos),font_file_name,size))
   return false;
  return RDK::GlobalFonts.AddFont(font.GetName(),size,font);
 }
 return false;
}



// Класс хранилище-визуальных интерфейсов
// Список обработчиков, которые должны быть вызваны после расчета
std::vector<RDK::UIVisualController*> UIVisualControllerStorage::InterfaceUpdaters;

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
 std::vector<RDK::UIVisualController*>::iterator I=find(InterfaceUpdaters.begin(),InterfaceUpdaters.end(),value);
 if(I != InterfaceUpdaters.end())
  InterfaceUpdaters.erase(I);
}

// Метод, вызываемый перед сбросом модели
void UIVisualControllerStorage::BeforeReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->BeforeReset();
}

// Метод, вызываемый после сброса модели
void UIVisualControllerStorage::AfterReset(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->AfterReset();
}

// Метод, вызываемый перед шагом расчета
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

// Метод, вызываемый после шага расчета
void UIVisualControllerStorage::AfterCalculate(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->AfterCalculate();
}

// Обновление интерфейса
void UIVisualControllerStorage::UpdateInterface(bool force_update)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->UpdateInterface(force_update);
}

// Возврат интерфейса в исходное состояние
void UIVisualControllerStorage::ClearInterface(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->ClearInterface();
}

// Сохраняет параметры интерфейса в xml
void UIVisualControllerStorage::SaveParameters(RDK::USerStorageXML &xml)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->SaveParameters(xml);
}

// Загружает параметры интерфейса из xml
void UIVisualControllerStorage::LoadParameters(RDK::USerStorageXML &xml)
{
/*
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i] && InterfaceUpdaters[i]->GetName() == "UGEngineControlForm")
  {
   InterfaceUpdaters[i]->LoadParameters(xml);
   break;
  }

 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i] && InterfaceUpdaters[i]->GetName() == "UEngineMonitorForm")
  {
   InterfaceUpdaters[i]->LoadParameters(xml);
   break;
  }

 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i] && InterfaceUpdaters[i]->GetName() == "UEngineMonitorFrame")
  {
   InterfaceUpdaters[i]->LoadParameters(xml);
   break;
  }

 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i] && InterfaceUpdaters[i]->GetName() == "UServerControlForm")
  {
   InterfaceUpdaters[i]->LoadParameters(xml);
   break;
  }

 std::string name;
*/
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
  {
/*   name=InterfaceUpdaters[i]->GetName();
   if(name == "UGEngineControlForm" ||
	  name == "UEngineMonitorForm" ||
	  name == "UEngineMonitorFrame" ||
	  name == "UServerControlForm")
	continue;*/
   InterfaceUpdaters[i]->LoadParameters(xml);
  }
}

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
void UIVisualControllerStorage::ResetCalculationStepUpdatedFlag(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->ResetCalculationStepUpdatedFlag();
}

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
void UIVisualControllerStorage::SetCalculationStepUpdatedFlag(void)
{
 for(size_t i=0;i<InterfaceUpdaters.size();i++)
  if(InterfaceUpdaters[i])
   InterfaceUpdaters[i]->SetCalculationStepUpdatedFlag();
}


}

#endif

