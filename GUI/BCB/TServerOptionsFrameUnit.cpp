//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TServerOptionsFrameUnit.h"
#include "UGEngineControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TServerOptionsFrame *ServerOptionsFrame;
//---------------------------------------------------------------------------
__fastcall TServerOptionsFrame::TServerOptionsFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 UpdateInterval=-1;
}

// -----------------------------
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TServerOptionsFrame::AAfterReset(void)
{

}

// Обновление интерфейса
void TServerOptionsFrame::AUpdateInterface(void)
{
 if(!UGEngineControlForm)
  return;

 BindAddressLabeledEdit->Text=UGEngineControlForm->ServerInterfaceAddress.c_str();
 try
 {
  BindPortLabeledEdit->Text=IntToStr(UGEngineControlForm->ServerInterfacePort);
 }
 catch(EConvertError &exception)
 {
  BindPortLabeledEdit->Text="45045";
 }
}

// Возврат интерфейса в исходное состояние
void TServerOptionsFrame::AClearInterface(void)
{

}

// Сохраняет параметры интерфейса в xml
void TServerOptionsFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void TServerOptionsFrame::ALoadParameters(RDK::USerStorageXML &xml)
{

}

void TServerOptionsFrame::ApplyOptions(void)
{
 if(!UGEngineControlForm)
  return;

 UGEngineControlForm->ServerInterfaceAddress=AnsiString(BindAddressLabeledEdit->Text).c_str();
 UGEngineControlForm->ServerInterfacePort=StrToInt(BindPortLabeledEdit->Text);
}
// -----------------------------
//---------------------------------------------------------------------------
