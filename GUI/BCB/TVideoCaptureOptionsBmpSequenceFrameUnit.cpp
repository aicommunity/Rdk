//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsBmpSequenceFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsBmpSequenceFrame *VideoCaptureOptionsBmpSequenceFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsBmpSequenceFrame::TVideoCaptureOptionsBmpSequenceFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsBmpSequenceFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsBmpSequenceFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------