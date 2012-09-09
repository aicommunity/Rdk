//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerFrame *UVisualControllerFrame;
// --------------------------
// ������������ � �����������
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
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������� ������
void TUVisualControllerFrame::BeforeReset(void)
{
 ABeforeReset();
}

void TUVisualControllerFrame::ABeforeReset(void)
{

}

// �����, ���������� ����� ������ ������
void TUVisualControllerFrame::AfterReset(void)
{
 AAfterReset();
}

void TUVisualControllerFrame::AAfterReset(void)
{

}

// �����, ���������� ����� ����� �������
void TUVisualControllerFrame::BeforeCalculate(void)
{
 ABeforeCalculate();
}

void TUVisualControllerFrame::ABeforeCalculate(void)
{
}

// �����, ���������� ����� ���� �������
void TUVisualControllerFrame::AfterCalculate(void)
{
 AAfterCalculate();
}

void TUVisualControllerFrame::AAfterCalculate(void)
{
}

// ���������� ����������
void TUVisualControllerFrame::UpdateInterface(void)
{
// if(!Parent->Visible)
//  return;
 UpdateInterfaceFlag=true;
 AUpdateInterface();
 UpdateInterfaceFlag=false;
}

void TUVisualControllerFrame::AUpdateInterface(void)
{
}

// ���������� ���������� ��� ����������
std::string TUVisualControllerFrame::GetName(void)
{
 return AnsiString(Name).c_str();
}

// ��������� ��������� ���������� � xml
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

// ��������� ��������� ���������� �� xml
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

// ���������� ������� ������ ������������ ����������
// ������� ��� ������������ ���������� ������ (�����������)
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
// -----------------------------


