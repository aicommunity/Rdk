//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsHttpServerFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsHttpServerFrame *VideoCaptureOptionsHttpServerFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsHttpServerFrame::TVideoCaptureOptionsHttpServerFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsHttpServerFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsHttpServerFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------