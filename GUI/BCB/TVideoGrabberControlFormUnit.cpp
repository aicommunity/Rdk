//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TVideoGrabberControlFrameUnit"
#pragma resource "*.dfm"
TVideoGrabberControlForm *VideoGrabberControlForm;
//---------------------------------------------------------------------------
__fastcall TVideoGrabberControlForm::TVideoGrabberControlForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}
//---------------------------------------------------------------------------
// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ����� �������
void TVideoGrabberControlForm::ABeforeCalculate(void)
{
}

// �����, ���������� ����� ���� �������
void TVideoGrabberControlForm::AAfterCalculate(void)
{
}

// ���������� ����������
void TVideoGrabberControlForm::AUpdateInterface(void)
{
}

// ��������� ��������� ���������� � xml
void TVideoGrabberControlForm::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{
}

// ��������� ��������� ���������� �� xml
void TVideoGrabberControlForm::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
}
// -----------------------------

