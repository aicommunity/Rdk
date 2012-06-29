//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerFrame *UVisualControllerFrame;
// --------------------------
// Конструкторы и деструкторы
// --------------------------
__fastcall TUVisualControllerFrame::TUVisualControllerFrame(TComponent* Owner)
 : TFrame(Owner)
{
 UpdateInterfaceFlag=false;
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
// Метод, вызываемый перед шагом расчета
void TUVisualControllerFrame::BeforeCalculate(void)
{
 ABeforeCalculate();
}

void TUVisualControllerFrame::ABeforeCalculate(void)
{
}

// Метод, вызываемый после шага расчета
void TUVisualControllerFrame::AfterCalculate(void)
{
 AAfterCalculate();
}

void TUVisualControllerFrame::AAfterCalculate(void)
{
}

// Обновление интерфейса
void TUVisualControllerFrame::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;
 AUpdateInterface();
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
void TUVisualControllerFrame::SaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ASaveParameters(xml);
 xml.SelectUp();
 xml.SelectUp();
}

void TUVisualControllerFrame::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TUVisualControllerFrame::LoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ALoadParameters(xml);
 xml.SelectUp();
 xml.SelectUp();
}

void TUVisualControllerFrame::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
}
// -----------------------------


