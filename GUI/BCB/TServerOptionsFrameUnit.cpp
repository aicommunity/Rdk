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
//---------------------------------------------------------------------------
__fastcall TServerOptionsFrame::TServerOptionsFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 UpdateInterval=-1;
}

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
void TServerOptionsFrame::AAfterReset(void)
{

}

// ���������� ����������
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

// ������� ���������� � �������� ���������
void TServerOptionsFrame::AClearInterface(void)
{

}

// ��������� ��������� ���������� � xml
void TServerOptionsFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void TServerOptionsFrame::ALoadParameters(RDK::USerStorageXML &xml)
{

}

void TServerOptionsFrame::ApplyOptions(void)
{
 if(!UGEngineControlForm)
  return;

 if(UServerControlForm && UGEngineControlForm)
 {
  UGEngineControlForm->ServerInterfaceAddress=AnsiString(BindAddressLabeledEdit->Text).c_str();
  UGEngineControlForm->ServerInterfacePort=StrToInt(BindPortLabeledEdit->Text);

  UServerControlForm->SetServerBinding(UGEngineControlForm->ServerInterfaceAddress,UGEngineControlForm->ServerInterfacePort);
  UServerControlForm->UpdateInterface();
 }
}
// -----------------------------
//---------------------------------------------------------------------------
