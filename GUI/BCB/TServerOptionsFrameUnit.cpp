//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TServerOptionsFrameUnit.h"
#include "UGEngineControlFormUnit.h"
#include "UServerControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TServerOptionsFrame *ServerOptionsFrame;

/// Экзепляр класса приложения
extern RDK::UApplication RdkApplication;
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

 BindAddressLabeledEdit->Text=RdkApplication.GetProjectConfig().ServerInterfaceAddress.c_str();
 try
 {
  BindPortLabeledEdit->Text=IntToStr(RdkApplication.GetProjectConfig().ServerInterfacePort);
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

 if(UServerControlForm && UGEngineControlForm)
 {
  RDK::TProjectConfig config=RdkApplication.GetProjectConfig();
  config.ServerInterfaceAddress=AnsiString(BindAddressLabeledEdit->Text).c_str();
  config.ServerInterfacePort=StrToInt(BindPortLabeledEdit->Text);
  RdkApplication.SetProjectConfig(config);

  UServerControlForm->SetServerBinding(config.ServerInterfaceAddress,config.ServerInterfacePort);
  UServerControlForm->UpdateInterface();
 }
}
// -----------------------------
//---------------------------------------------------------------------------
