//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UClassesDescriptionsFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma link "UClassesDescriptionsFrameUnit"
#pragma resource "*.dfm"
TUClassesDescriptionsForm *UClassesDescriptionsForm;
//---------------------------------------------------------------------------
__fastcall TUClassesDescriptionsForm::TUClassesDescriptionsForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
void TUClassesDescriptionsForm::AAfterReset(void)
{

}

// ���������� ����������
void TUClassesDescriptionsForm::AUpdateInterface(void)
{

}

// ��������� ��������� ���������� � xml
void TUClassesDescriptionsForm::ASaveParameters(RDK::USerStorageXML &xml)
{

}

// ��������� ��������� ���������� �� xml
void TUClassesDescriptionsForm::ALoadParameters(RDK::USerStorageXML &xml)
{

}

// �������� ����� ����� ����������
TUClassesDescriptionsForm* TUClassesDescriptionsForm::New(TComponent *owner)
{
 return new TUClassesDescriptionsForm(owner);
}
// -----------------------------
//---------------------------------------------------------------------------
