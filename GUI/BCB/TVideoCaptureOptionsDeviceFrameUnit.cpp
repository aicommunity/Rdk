//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoCaptureOptionsDeviceFrameUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TVideoCaptureOptionsDeviceFrame *VideoCaptureOptionsDeviceFrame;
//---------------------------------------------------------------------------
__fastcall TVideoCaptureOptionsDeviceFrame::TVideoCaptureOptionsDeviceFrame(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

/// -------------------------------------
/// ������ ��������/���������� ����������
/// -------------------------------------
/// ��������� ��������� � ���� ����������
bool TVideoCaptureOptionsDeviceFrame::LoadParamters(RDK::USerStorageXML &xml)
{

}

/// ���������� ��������� �� ����� ���������� � xml
bool TVideoCaptureOptionsDeviceFrame::SaveParamters(RDK::USerStorageXML &xml)
{

}
/// -------------------------------------
