//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsIPCameraFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsIPCameraFrame *VideoCaptureOptionsIPCameraFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsIPCameraFrame::TVideoCaptureOptionsIPCameraFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsIPCameraFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsIPCameraFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------