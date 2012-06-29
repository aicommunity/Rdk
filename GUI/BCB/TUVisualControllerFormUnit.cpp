//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUVisualControllerFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUVisualControllerForm *UVisualControllerForm;

// --------------------------
// ��������������� ������� ������������
// --------------------------
// ��������� ������ ��������� ����� � xml
void SaveFormPosition(RDK::Serialize::USerStorageXML &xml, TForm *form)
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

// ��������� ������ ��������� ����� �� xml
void LoadFormPosition(RDK::Serialize::USerStorageXML &xml, TForm *form)
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
// ������������ � �����������
// --------------------------
__fastcall TUVisualControllerForm::TUVisualControllerForm(TComponent* Owner)
 : TForm(Owner)
{
 UpdateInterfaceFlag=false;
 RDK::UIVisualControllerStorage::AddInterface(this);
}

__fastcall TUVisualControllerForm::~TUVisualControllerForm(void)
{
 RDK::UIVisualControllerStorage::DelInterface(this);
}
// --------------------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ����� �������
void TUVisualControllerForm::BeforeCalculate(void)
{
 ABeforeCalculate();
}

void TUVisualControllerForm::ABeforeCalculate(void)
{
}

// �����, ���������� ����� ���� �������
void TUVisualControllerForm::AfterCalculate(void)
{
 AAfterCalculate();
}

void TUVisualControllerForm::AAfterCalculate(void)
{
}

// ���������� ����������
void TUVisualControllerForm::UpdateInterface(void)
{
 UpdateInterfaceFlag=true;
 AUpdateInterface();
 UpdateInterfaceFlag=false;
}

void TUVisualControllerForm::AUpdateInterface(void)
{
}

// ���������� ���������� ��� ����������
std::string TUVisualControllerForm::GetName(void)
{
 return AnsiString(Name).c_str();
}

// ��������� ��������� ���������� � xml
void TUVisualControllerForm::SaveParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.SelectNodeForce(GetName());
 ASaveParameters(xml);
 SaveFormPosition(xml, this);
 xml.SelectUp();
}

void TUVisualControllerForm::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{

}


// ��������� ��������� ���������� �� xml
void TUVisualControllerForm::LoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 xml.SelectNodeForce(GetName());
 LoadFormPosition(xml, this);
 ALoadParameters(xml);
 xml.SelectUp();
}

void TUVisualControllerForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
}
// -----------------------------
