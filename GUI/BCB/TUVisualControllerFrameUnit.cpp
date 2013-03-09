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
// ����, ���������� ��� ���� ������
bool TUVisualControllerFrame::CalculationModeFlag=false;

__fastcall TUVisualControllerFrame::TUVisualControllerFrame(TComponent* Owner)
 : TFrame(Owner)
{
 UpdateInterfaceFlag=false;
 AlwaysUpdateFlag=false;
 UpdateInterval=1000;

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
 LastUpdateTime=0;
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
void TUVisualControllerFrame::UpdateInterface(bool force_update)
{
 if(!force_update)
 {
  if((!AlwaysUpdateFlag && !Parent->Visible) || (UpdateInterval<0 && CalculationModeFlag))
   return;
  if(UpdateInterval>0 && CalculationModeFlag)
  {
   DWORD curr_time=GetTickCount();
   if(curr_time-LastUpdateTime<UpdateInterval)
	return;

   LastUpdateTime=curr_time;
  }
 }

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
void TUVisualControllerFrame::SaveParameters(RDK::USerStorageXML &xml)
{
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ASaveParameters(xml);
 xml.WriteInteger("UpdateInterval",UpdateInterval);
 xml.WriteString("ComponentControlName",ComponentControlName);

 xml.SelectUp();
 xml.SelectUp();
}

void TUVisualControllerFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void TUVisualControllerFrame::LoadParameters(RDK::USerStorageXML &xml)
{
 xml.SelectNodeForce(AnsiString(Owner->Name).c_str());
 xml.SelectNodeForce(GetName());
 ComponentControlName=xml.ReadString("ComponentControlName","");
 UpdateInterval=xml.ReadInteger("UpdateInterval",UpdateInterval);
 ALoadParameters(xml);
 xml.SelectUp();
 xml.SelectUp();
}

void TUVisualControllerFrame::ALoadParameters(RDK::USerStorageXML &xml)
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


