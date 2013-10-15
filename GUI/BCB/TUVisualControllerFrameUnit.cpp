//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFrameUnit.h"
#include "rdk_cpp_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerFrame *UVisualControllerFrame;
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Флаг, сообщающий что идет расчет
bool TUVisualControllerFrame::CalculationModeFlag=false;

__fastcall TUVisualControllerFrame::TUVisualControllerFrame(TComponent* Owner)
 : TFrame(Owner)
{
 UpdateInterfaceFlag=false;
 AlwaysUpdateFlag=false;
 UpdateInterval=1000;
 CalculationStepUpdatedFlag=false;

 RDK::UIVisualControllerStorage::AddInterface(this);
}

__fastcall TUVisualControllerFrame::~TUVisualControllerFrame(void)
{
 RDK::UIVisualControllerStorage::DelInterface(this);
}
// --------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом модели
void TUVisualControllerFrame::BeforeReset(void)
{
 try
 {
  ABeforeReset();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerFrame::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void TUVisualControllerFrame::AfterReset(void)
{
 try
 {
  LastUpdateTime=0;
  AAfterReset();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerFrame::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TUVisualControllerFrame::BeforeCalculate(void)
{
 try
 {
  ResetCalculationStepUpdatedFlag();
  ABeforeCalculate();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerFrame::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void TUVisualControllerFrame::AfterCalculate(void)
{
 try
 {
  AAfterCalculate();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerFrame::AAfterCalculate(void)
{
}

// Обновление интерфейса
void TUVisualControllerFrame::UpdateInterface(bool force_update)
{
 try
 {
 if(!force_update)
 {
  UpdateControlState();
  if(!Showing && !AlwaysUpdateFlag)
   return;
  if(!Parent || (!AlwaysUpdateFlag && !Parent->Visible) || (UpdateInterval<0 && CalculationModeFlag))
   return;
  if(UpdateInterval>0 && CalculationModeFlag)
  {
   DWORD curr_time=GetTickCount();
   if(curr_time-LastUpdateTime<DWORD(UpdateInterval))
	return;

   if(GetCalculationStepUpdatedFlag() == true)
	return;
   else
    SetCalculationStepUpdatedFlag();

   LastUpdateTime=curr_time;
  }
 }

 if(!IsEngineInit())
  return;
 if(!Model_Check())
  return;
 UpdateInterfaceFlag=true;
  AUpdateInterface();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
  UpdateInterfaceFlag=false;
 }
 catch(...)
 {
  UpdateInterfaceFlag=false;
  throw;
 }
 UpdateInterfaceFlag=false;
}

void TUVisualControllerFrame::AUpdateInterface(void)
{
}

// Возвращает уникальное имя интерфейса
std::string TUVisualControllerFrame::GetName(void)
{
 return AnsiString(Name).c_str();
}

// Сохраняет параметры интерфейса в xml
void TUVisualControllerFrame::SaveParameters(RDK::USerStorageXML &xml)
{
 try
 {
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
// xml.WriteString("FrameTypeName",AnsiString(ClassName()).c_str());
 ASaveParameters(xml);
 xml.WriteInteger("UpdateInterval",UpdateInterval);
 xml.WriteString("ComponentControlName",ComponentControlName);
 xml.WriteBool("AlwaysUpdateFlag",AlwaysUpdateFlag);

 xml.SelectUp();
 xml.SelectUp();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUVisualControllerFrame::LoadParameters(RDK::USerStorageXML &xml)
{
 try
 {
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ComponentControlName=xml.ReadString("ComponentControlName","");
 UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
 AlwaysUpdateFlag=xml.ReadBool("AlwaysUpdateFlag",false);
 ALoadParameters(xml);
 xml.SelectUp();
 xml.SelectUp();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
const std::string& TUVisualControllerFrame::GetComponentControlName(void) const
{
 return ComponentControlName;
}

bool TUVisualControllerFrame::SetComponentControlName(const std::string& name)
{
 if(ComponentControlName == name)
  return true;

 ComponentControlName=name;
 UpdateInterface();
 return true;
}

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerFrame::ResetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=false;
}

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerFrame::SetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=true;
}

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
bool TUVisualControllerFrame::GetCalculationStepUpdatedFlag(void)
{
 return CalculationStepUpdatedFlag;
}
// -----------------------------


