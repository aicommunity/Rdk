//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberControlFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TVideoGrabberControlFrameUnit"
#pragma link "TUVisualControllerFormUnit"
#pragma link "TUVisualControllerFrameUnit"
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
void TVideoGrabberControlForm::ASaveParameters(RDK::USerStorageXML &xml)
{
}

// ��������� ��������� ���������� �� xml
void TVideoGrabberControlForm::ALoadParameters(RDK::USerStorageXML &xml)
{
}

// �������� ����� ����� ����������
TVideoGrabberControlForm* TVideoGrabberControlForm::New(TComponent *owner)
{
 return new TVideoGrabberControlForm(owner);
}
// -----------------------------



