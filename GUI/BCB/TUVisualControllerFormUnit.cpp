//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFormUnit.h"
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
 form->Left=xml.ReadInteger("Left",form->Left);
 form->Top=xml.ReadInteger("Top",form->Top);
 form->Width=xml.ReadInteger("Width",form->Width);
 form->Height=xml.ReadInteger("Height",form->Height);
 form->Visible=xml.ReadBool("Visible",form->Visible);
 form->WindowState=xml.ReadInteger("WindowState",form->WindowState);
 form->Repaint();
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
 ABeforeReset();
}

void TUVisualControllerForm::ABeforeReset(void)
{

}

// Метод, вызываемый после сброса модели
void TUVisualControllerForm::AfterReset(void)
{
 LastUpdateTime=0;

 AAfterReset();
}

void TUVisualControllerForm::AAfterReset(void)
{

}

// Метод, вызываемый перед шагом расчета
void TUVisualControllerForm::BeforeCalculate(void)
{
 ABeforeCalculate();
}

void TUVisualControllerForm::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void TUVisualControllerForm::AfterCalculate(void)
{
 AAfterCalculate();
}

void TUVisualControllerForm::AAfterCalculate(void)
{
}

// Обновление интерфейса
void TUVisualControllerForm::UpdateInterface(bool force_update)
{
 if(!force_update)
 {
  if((!AlwaysUpdateFlag && !Visible) || (UpdateInterval<0 && CalculationModeFlag))
   return;
  if(UpdateInterval>0 && CalculationModeFlag)
  {
   DWORD curr_time=GetTickCount();
   if(curr_time-LastUpdateTime<UpdateInterval)
	return;

   LastUpdateTime=curr_time;
  }
 }

 if(ComponentControlName.size()>0)
  Caption=(PureFormCaption+std::string(" [")+ComponentControlName+std::string("]")).c_str();
 else
  Caption=PureFormCaption.c_str();
 UpdateInterfaceFlag=true;
 AUpdateInterface();
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

void TUVisualControllerForm::ASaveParameters(RDK::USerStorageXML &xml)
{

}


// Загружает параметры интерфейса из xml
void TUVisualControllerForm::LoadParameters(RDK::USerStorageXML &xml)
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
// -----------------------------

