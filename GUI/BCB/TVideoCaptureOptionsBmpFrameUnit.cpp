//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsBmpFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsBmpFrame *VideoCaptureOptionsBmpFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsBmpFrame::TVideoCaptureOptionsBmpFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsBmpFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsBmpFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------