//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsVideoFileFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsVideoFileFrame *VideoCaptureOptionsVideoFileFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsVideoFileFrame::TVideoCaptureOptionsVideoFileFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsVideoFileFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsVideoFileFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------