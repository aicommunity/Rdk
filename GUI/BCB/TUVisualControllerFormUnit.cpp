//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFormUnit.h"
#include "rdk_cpp_initdll.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerForm *UVisualControllerForm;

// --------------------------
// Вспомогательные функции сериализации
// --------------------------
// Сохраняет данные положения формы в xml
void SaveFormPosition(RDK::USerStorageXML &xml, TForm *form)
{
 if(!form)
  return;

 xml.SelectNodeForce("FormPosition");
 xml.WriteInteger("Left",form->Left);
 xml.WriteInteger("Top",form->Top);
 xml.WriteInteger("Width",form->Width);
 xml.WriteInteger("Height",form->Height);
 xml.WriteBool("Visible",form->Visible);
 xml.WriteInteger("WindowState",form->WindowState);
 xml.SelectUp();
}

// Загружает данные положения формы из xml
void LoadFormPosition(RDK::USerStorageXML &xml, TForm *form)
{
 if(!form)
  return;

 xml.SelectNodeForce("FormPosition");
 int value=0;
 value=xml.ReadInteger("Left",form->Left);
 if(value<Screen->DesktopLeft || value>= Screen->DesktopLeft+Screen->DesktopWidth)
  value=Screen->DesktopLeft;
 form->Left=value;

 value=xml.ReadInteger("Top",form->Top);
 if(value<Screen->DesktopTop || value>= Screen->DesktopTop+Screen->DesktopHeight)
  value=Screen->DesktopTop;
 form->Top=value;

 form->Width=xml.ReadInteger("Width",form->Width);
 form->Height=xml.ReadInteger("Height",form->Height);
 form->Visible=xml.ReadBool("Visible",form->Visible);
 form->WindowState=xml.ReadInteger("WindowState",(int)form->WindowState);
// form->Repaint();
 xml.SelectUp();
}
// --------------------------


// --------------------------
// Конструкторы и деструкторы
// --------------------------
// Флаг, сообщающий что идет расчет
bool TUVisualControllerForm::CalculationModeFlag=false;


__fastcall TUVisualControllerForm::TUVisualControllerForm(TComponent* Owner)
 : TForm(Owner)
{
 UpdateInterfaceFlag=false;
 AlwaysUpdateFlag=false;
 UpdateInterval=1000;
 PureFormCaption=AnsiString(Caption).c_str();

 CalculationStepUpdatedFlag=0;

 RDK::UIVisualControllerStorage::AddInterface(this);
}

__fastcall TUVisualControllerForm::~TUVisualControllerForm(void)
{
 RDK::UIVisualControllerStorage::DelInterface(this);
}
// --------------------------

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый перед сбросом модели
void TUVisualControllerForm::BeforeReset(void)
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

void TUVisualControllerForm::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void TUVisualControllerForm::AfterReset(void)
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

void TUVisualControllerForm::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TUVisualControllerForm::BeforeCalculate(void)
{
 try
 {
  ABeforeCalculate();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerForm::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void TUVisualControllerForm::AfterCalculate(void)
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

void TUVisualControllerForm::AAfterCalculate(void)
{
}

// Обновление интерфейса
void TUVisualControllerForm::UpdateInterface(bool force_update)
{
 try
 {
 if(!force_update)
 {
  if((!AlwaysUpdateFlag && (!Visible || (Parent && !Parent->Visible))) || (UpdateInterval<0 && CalculationModeFlag))
   return;

  UpdateControlState();
  if(!Showing)
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

 if(ComponentControlName.size()>0)
  Caption=(PureFormCaption+std::string(" [")+ComponentControlName+std::string("]")).c_str();
 else
  Caption=PureFormCaption.c_str();

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

void TUVisualControllerForm::AUpdateInterface(void)
{
}

// Возвращает уникальное имя интерфейса
std::string TUVisualControllerForm::GetName(void)
{
 return AnsiString(Name).c_str();
}

// Сохраняет параметры интерфейса в xml
void TUVisualControllerForm::SaveParameters(RDK::USerStorageXML &xml)
{
 try
 {
 TTabSheet *tab=dynamic_cast<TTabSheet*>(Parent);
 if(tab)
  xml.SelectNodeForce(AnsiString(tab->PageControl->Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ASaveParameters(xml);
 SaveFormPosition(xml, this);
 xml.WriteString("ComponentControlName",ComponentControlName);
 xml.WriteInteger("UpdateInterval",UpdateInterval);
 if(tab)
  xml.SelectUp();

 xml.SelectUp();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }

}

void TUVisualControllerForm::ASaveParameters(RDK::USerStorageXML &xml)
{

}


// Загружает параметры интерфейса из xml
void TUVisualControllerForm::LoadParameters(RDK::USerStorageXML &xml)
{
 try
 {
 TTabSheet *tab=dynamic_cast<TTabSheet*>(Parent);
 if(tab)
  xml.SelectNodeForce(AnsiString(tab->PageControl->Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ComponentControlName=xml.ReadString("ComponentControlName","");
 UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
 LoadFormPosition(xml, this);
 ALoadParameters(xml);

 if(tab)
  xml.SelectUp();
 xml.SelectUp();
 UpdateInterface();
 }
 catch (RDK::UException &exception)
 {
  GetEngine()->ProcessException(exception);
 }
}

void TUVisualControllerForm::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// Управление длинным именем управляемого компонента
// Длинное имя управляемого компонента модели (опционально)
const std::string& TUVisualControllerForm::GetComponentControlName(void) const
{
 return ComponentControlName;
}

bool TUVisualControllerForm::SetComponentControlName(const std::string& name)
{
 if(ComponentControlName == name)
  return true;

 ComponentControlName=name;
 UpdateInterface();
 return true;
}

// Служебные методы управления интерфейсом
/// Сбрасывает флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerForm::ResetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=false;
}

/// Выставляет флаг прошедшей перерисовки в этой итерации счета
void TUVisualControllerForm::SetCalculationStepUpdatedFlag(void)
{
 CalculationStepUpdatedFlag=true;
}

/// Возвращает состояние флага прошедшей перерисовки в этой итерации счета
bool TUVisualControllerForm::GetCalculationStepUpdatedFlag(void)
{
 return CalculationStepUpdatedFlag;
}
// -----------------------------

